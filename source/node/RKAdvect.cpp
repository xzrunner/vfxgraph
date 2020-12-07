#include "vfxgraph/node/RKAdvect.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>
#include <unirender/Uniform.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

uniform UBO
{
	float dt;
};

uniform sampler2D velocities_READ;
uniform sampler2D field_READ;
layout(rgba32f) uniform image2D field_WRITE;

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

void main()
{
	vec2 tSize = textureSize(field_READ, 0);
	vec2 pixelCoords = gl_GlobalInvocationID.xy;

	vec2 v = RK(velocities_READ, pixelCoords, dt);
	vec2 pos = pixelCoords - dt * vec2(1,1);
	vec4 val = texture(field_READ, pixelToTexel(pos, tSize));

	imageStore(field_WRITE, ivec2(pixelCoords), val);
}

)";

}

namespace vfxgraph
{
namespace node
{

void RKAdvect::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	m_shader = NodeHelper::CreateShader(ctx, cs);
	if (!m_shader) {
		return;
	}

	auto velocities_tex = NodeHelper::GetInputTex(*this, ID_VELOCITIES);
	auto read_tex = NodeHelper::GetInputTex(*this, ID_READ);
	auto write_tex = NodeHelper::GetInputTex(*this, ID_WRITE);
	if (!read_tex || !write_tex) {
		return;
	}

	auto u_dt = m_shader->QueryUniform("dt");
	assert(u_dt);
	float dt = NodeHelper::GetInputFloat(*this, ID_DT);
	u_dt->SetValue(&dt, 1);

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("velocities_READ"), velocities_tex);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("field_READ"), read_tex);
	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("field_WRITE"), write_tex, ur::AccessType::WriteOnly);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}