#include "vfxgraph/node/MaxReduce.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"
#include "vfxgraph/ValueImpl.h"

#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Texture.h>
#include <unirender/TextureDescription.h>
#include <unirender/ReadPixelBuffer.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D field_WRITE;
uniform sampler2D field_READ;

vec2 pixelToTexel(in vec2 p, in vec2 tSize)
{
	return (p + 0.5) / tSize;
}

void main()
{
	ivec2 tSize = textureSize(field_READ, 0);
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
 
	vec4 a = texture(field_READ, pixelToTexel(2 * pixelCoords              , tSize)); 
	vec4 b = texture(field_READ, pixelToTexel(2 * pixelCoords + ivec2(1, 0), tSize)); 
	vec4 c = texture(field_READ, pixelToTexel(2 * pixelCoords + ivec2(0, 1), tSize)); 
	vec4 d = texture(field_READ, pixelToTexel(2 * pixelCoords + ivec2(1, 1), tSize)); 

	imageStore(field_WRITE, pixelCoords, max(max(max(a, b), c), d));
}

)";

}

namespace vfxgraph
{
namespace node
{

void MaxReduce::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto src_tex = NodeHelper::GetInputTex(*this, I_FIELD);
	if (!src_tex) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	const int w = src_tex->GetWidth();
	const int h = src_tex->GetHeight();
	int nb = static_cast<int>(std::log(static_cast<double>(std::min(w, h))) / std::log(2.0));
	int tw = w / 2;
	int th = h / 2;
	if (m_reduce_texs.size() != nb || (!m_reduce_texs.empty() && (m_reduce_texs[0]->GetWidth() != tw || m_reduce_texs[0]->GetHeight() != th)))
	{
		m_reduce_texs.clear();

		ur::TextureDescription desc;
		for (int i = 0; i < nb; ++i)
		{
			desc.width = tw;
			desc.height = th;
			desc.format = src_tex->GetFormat();
			m_reduce_texs.emplace_back(rc->ur_dev->CreateTexture(desc));

			tw /= 2;
			th /= 2;
		}
	}
	if (m_reduce_texs.empty()) {
		return;
	}

	auto r_util = [&](const ur::TexturePtr& tex_in, const ur::TexturePtr& tex_out, const unsigned size_x, const unsigned size_y)
	{
		rc->ur_ctx->SetImage(m_shader->QueryImgSlot("field_WRITE"), tex_out, ur::AccessType::WriteOnly);
		rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("field_READ"), tex_in);

		rc->ur_ds.program = m_shader;

		rc->ur_ctx->Compute(rc->ur_ds, std::max(size_x / 32, 1u), std::max(size_y / 32, 1u), 1);

		rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
	};

	tw = w / 2;
	th = h / 2;
	r_util(src_tex, m_reduce_texs[0], tw, th);
	for (unsigned i = 0; i < m_reduce_texs.size() - 1; ++i)
	{
		tw /= 2;
		th /= 2;
		r_util(m_reduce_texs[i], m_reduce_texs[i + 1], tw, th);
	}

	float* data = static_cast<float*>(m_reduce_texs.back()->WriteToMemory(sizeof(float) * 4));
	float m = std::max(std::max(abs(data[0]), abs(data[1])), abs(data[2]));
	delete[] data;

	m_exports[O_MAX].var.type.type = VarType::Float;
	m_exports[O_MAX].var.type.val = std::make_shared<FloatVal>(m);
}

}
}