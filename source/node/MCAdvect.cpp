#include "vfxgraph/node/MCAdvect.h"
#include "vfxgraph/node/RKAdvect.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Device.h>
#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Uniform.h>
#include <unirender/TextureDescription.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

uniform UBO
{
	float dt;
	float revert;
};

layout(rgba32f) uniform image2D field_WRITE;

uniform sampler2D field_n;
uniform sampler2D field_n_hat_READ;
uniform sampler2D field_n_1_READ;
uniform sampler2D velocities_READ;

vec2 pixelToTexel(in vec2 p, in vec2 tSize)
{
	return (p + 0.5) / tSize;
}

vec4 texture2D_bilinear(in sampler2D t, in vec2 uv)
{
	vec2 res = textureSize(t, 0);

	vec2 st = uv * res - 0.5;

	vec2 iuv = floor(st);
	vec2 fuv = fract(st);

	vec4 a = texture(t, (iuv + vec2(0.5, 0.5)) / res);
	vec4 b = texture(t, (iuv + vec2(1.5, 0.5)) / res);
	vec4 c = texture(t, (iuv + vec2(0.5, 1.5)) / res);
	vec4 d = texture(t, (iuv + vec2(1.5, 1.5)) / res);

	return mix(mix(a, b, fuv.x), mix(c, d, fuv.x), fuv.y);
}

vec2 RK(in sampler2D t, in vec2 pos, in float dt)
{
	vec2 tSize = textureSize(t, 0);

	vec2 v1 = texture2D_bilinear(t, pixelToTexel(pos, tSize)).xy;
	vec2 v2 = texture2D_bilinear(t, pixelToTexel(pos + 0.5 * v1 * dt, tSize)).xy;
	vec2 v3 = texture2D_bilinear(t, pixelToTexel(pos + 0.5 * v2 * dt, tSize)).xy;
	vec2 v4 = texture2D_bilinear(t, pixelToTexel(pos + v3 * dt, tSize)).xy;

	return (v1 + 2.0 * (v2 + v3) + v4) * (1.0 / 6.0);
}

vec4 clampValue(in sampler2D t, in vec4 value, in vec2 uv)
{
	vec2 tSize = textureSize(t, 0);

	vec2 npos = floor(uv * tSize - 0.5);

	vec4 a = texture2D_bilinear(t, pixelToTexel(npos + vec2(0.0, 0.0), tSize));
	vec4 b = texture2D_bilinear(t, pixelToTexel(npos + vec2(1.0, 0.0), tSize));
	vec4 c = texture2D_bilinear(t, pixelToTexel(npos + vec2(0.0, 1.0), tSize));
	vec4 d = texture2D_bilinear(t, pixelToTexel(npos + vec2(1.0, 1.0), tSize));

	vec4 vMin = min(min(min(a, b), c), d);
	vec4 vMax = max(max(max(a, b), c), d);

	return clamp(value, vMin, vMax);
}

void main()
{
	vec2 tSize = textureSize(field_n_hat_READ, 0);
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

	vec4 qAdv = texelFetch(field_n_1_READ, pixelCoords, 0);

	vec4 r = qAdv + 0.5 * texelFetch(field_n, pixelCoords, 0) - 0.5 * texelFetch(field_n_hat_READ, pixelCoords, 0);

	vec2 v = RK(velocities_READ, pixelCoords, dt);
	vec2 pos = pixelCoords - dt * v;
	vec4 rClamped = clampValue(field_n, r, pixelToTexel(pos, tSize));

	r = length(rClamped - r) > revert ? qAdv : rClamped;

	imageStore(field_WRITE, pixelCoords, r);
}

)";

}

namespace vfxgraph
{
namespace node
{

void MCAdvect::Execute(const std::shared_ptr<dag::Context>& ctx)
{ 
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}
	if (!m_shader) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);
	if (!m_temp_texs[0]) 
	{
		ur::TextureDescription desc;
		desc.target = ur::TextureTarget::Texture2D;
		desc.width = desc.height = 1024;
		desc.format = ur::TextureFormat::RGBA32F;

		for (auto& tex : m_temp_texs) {
			tex = rc->ur_dev->CreateTexture(desc);
		}
	}

	auto velocities_tex = NodeHelper::GetInputTex(*this, ID_VELOCITIES);
	auto read_tex = NodeHelper::GetInputTex(*this, ID_READ);
	auto write_tex = NodeHelper::GetInputTex(*this, ID_WRITE);
	if (!velocities_tex || !read_tex || !write_tex) {
		return;
	}

	float dt = NodeHelper::GetInputFloat(*this, ID_DT);

	RKAdvect::Execute(ctx, velocities_tex, read_tex, m_temp_texs[0], dt);
	RKAdvect::Execute(ctx, velocities_tex, m_temp_texs[0], m_temp_texs[1], -dt);

	auto u_dt = m_shader->QueryUniform("dt");
	assert(u_dt);
	u_dt->SetValue(&dt, 1);

	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("field_n"), read_tex);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("field_n_hat_READ"), m_temp_texs[1]);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("field_n_1_READ"), m_temp_texs[0]);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("velocities_READ"), velocities_tex);

	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("field_WRITE"), write_tex, ur::AccessType::WriteOnly);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
	
}

}
}