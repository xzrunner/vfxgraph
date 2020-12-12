#include "vfxgraph/node/UpdateQAndTheta.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>

namespace
{

const char* cs = R"(

#version 430

layout(rgba32f, binding = 0) uniform image2D qTex;
layout(rgba32f, binding = 1) uniform image2D pTemp;
uniform sampler2D pAdvectedTemp;

#define G 9.80665
#define P0 101325.0
#define T0 290.0
#define LAPSE_RATE 10.0
#define RD 287.0
#define kappa 0.286
#define L 2.501

void main()
{
	vec2 tSize = textureSize(pAdvectedTemp, 0);
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

	// Water Continuity
	vec4 theta = imageLoad(pTemp, pixelCoords);
	vec4 q = imageLoad(qTex, pixelCoords);

	float z = float(pixelCoords.y) / tSize.y;
	float p = P0 * pow(1.0 - z *  LAPSE_RATE / T0, G / (LAPSE_RATE / RD));
	float t = pow(P0 / p, kappa) / theta.x;
	float qvs = (380.16 / p) * exp(17.67 * t / (t + 243.5));
	float deltaQ = min(qvs - q.x, q.y);
  
	q.x += deltaQ;
	q.y -= deltaQ;

	// Thermodynamics
	vec4 thetaAdv = texelFetch(pAdvectedTemp, pixelCoords, 0);
	t = pow(P0 / p, kappa) / thetaAdv.x;
	qvs = (380.16 / p) * exp(17.67 * t / (t + 243.5));
	deltaQ = min(qvs - q.x, q.y);
	thetaAdv.x += (RD * L / kappa) * pow(P0 / p, kappa) * deltaQ;

	imageStore(qTex, pixelCoords, q);
	imageStore(pTemp, pixelCoords, thetaAdv);
}

)";

}

namespace vfxgraph
{
namespace node
{

void UpdateQAndTheta::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto q_tex = NodeHelper::GetInputTex(*this, I_Q_TEX);
	auto p_tmp_tex = NodeHelper::GetInputTex(*this, I_P_TEMP);
	auto p_advected_tmp = NodeHelper::GetInputTex(*this, I_P_ADVECTED_TEMP);
	if (!q_tex || !p_tmp_tex || !p_advected_tmp) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("qTex"), q_tex, ur::AccessType::ReadWrite);
	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("pTemp"), p_tmp_tex, ur::AccessType::ReadWrite);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("pAdvectedTemp"), p_advected_tmp);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}