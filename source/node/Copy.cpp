#include "vfxgraph/node/Copy.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f, binding = 0) uniform image2D tex_READ;
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

	m_shader = NodeHelper::CreateShader(ctx, cs);
	if (!m_shader) {
		return;
	}

	auto read_tex = NodeHelper::GetInputTex(*this, ID_READ);
	auto write_tex = NodeHelper::GetInputTex(*this, ID_WRITE);
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