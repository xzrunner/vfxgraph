#include "vfxgraph/node/Copy.h"
#include "vfxgraph/RenderContext.h"
#include "vfxgraph/ValueImpl.h"

#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Texture.h>
#include <shadertrans/ShaderTrans.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba8, binding = 0) uniform image2D tex_READ;
layout(rgba32f, binding = 1) uniform image2D tex_WRITE;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec4 pixel = imageLoad(tex_READ, pixelCoords);
	imageStore(tex_WRITE, pixelCoords, pixel);
}

)";

}

namespace vfxgraph
{
namespace node
{

void Copy::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!IsDirty()) {
		return;
	}

	if (!m_shader)
	{
		std::vector<unsigned int> _cs;
		shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::ComputeShader, cs, _cs);

		auto rc = std::static_pointer_cast<RenderContext>(ctx);
		m_shader = rc->ur_dev->CreateShaderProgram(_cs);
	}

	if (!m_shader) {
		return;
	}

	if (m_imports[0].conns.empty() ||
		m_imports[1].conns.empty()) {
		return;
	}

	assert(m_imports[0].conns.size() == 1 
		&& m_imports[1].conns.size() == 1);
	auto& read_conn = m_imports[0].conns[0];
	auto& write_conn = m_imports[1].conns[0];
	auto read_node = read_conn.node.lock();
	auto write_node = write_conn.node.lock();
	if (!read_node || !write_node) {
		return;
	}

	auto& read_val = read_node->GetExports()[read_conn.idx].var.type;
	auto& write_val = write_node->GetExports()[write_conn.idx].var.type;
	if (read_val.type != VarType::Texture ||
		write_val.type != VarType::Texture) {
		return;
	}

	auto read_tex = std::static_pointer_cast<TextureVal>(read_val.val)->texture;
	auto write_tex = std::static_pointer_cast<TextureVal>(write_val.val)->texture;
	if (!read_tex || !write_tex) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("tex_READ"), read_tex, ur::AccessType::ReadOnly);
	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("tex_WRITE"), write_tex, ur::AccessType::WriteOnly);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });

	SetDirty(false);
}

}
}