#include "vfxgraph/node/BuoyantForce.h"
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
	float kappa;
	float sigma;
	float t0;
};

layout(rgba32f) uniform image2D velocities_READ_WRITE;
uniform sampler2D temperature;
uniform sampler2D density;

void main()
{
  ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

  float t = texelFetch(temperature, pixelCoords, 0).x;
  float d = texelFetch(density, pixelCoords, 0).x;

  vec2 force = (- kappa * d + sigma * (t - t0)) * vec2(0.0f, 1.0f);
  vec4 oldVel = imageLoad(velocities_READ_WRITE, pixelCoords);

  imageStore(velocities_READ_WRITE, pixelCoords, oldVel + dt * vec4(force, 0.0f, 0.0f));
}

)";

}

namespace vfxgraph
{
namespace node
{

void BuoyantForce::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto velocities_tex  = NodeHelper::GetInputTex(*this, I_VELOCITIES);
	auto temperature_tex = NodeHelper::GetInputTex(*this, I_TEMPERATURE);
	auto density_tex     = NodeHelper::GetInputTex(*this, I_DENSITY);
	if (!velocities_tex || !temperature_tex || !density_tex) {
		return;
	}

	float dt = NodeHelper::GetInputFloat(*this, I_DT);
	auto u_dt = m_shader->QueryUniform("dt");
	assert(u_dt);
	u_dt->SetValue(&dt, 1);

	auto u_kappa = m_shader->QueryUniform("kappa");
	assert(u_kappa);
	u_kappa->SetValue(&m_kappa, 1);

	auto u_sigma = m_shader->QueryUniform("sigma");
	assert(u_sigma);
	u_sigma->SetValue(&m_sigma, 1);

	auto u_t0 = m_shader->QueryUniform("t0");
	assert(u_t0);
	u_t0->SetValue(&m_t0, 1);

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("velocities_READ_WRITE"), velocities_tex, ur::AccessType::ReadWrite);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("temperature"), temperature_tex);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("density"), density_tex);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}