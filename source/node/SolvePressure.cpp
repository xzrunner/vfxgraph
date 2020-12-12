#include "vfxgraph/node/SolvePressure.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D pressure_WRITE;
uniform sampler2D pressure_READ;
uniform sampler2D divergence;

void main()
{
	ivec2 tSize = textureSize(pressure_READ, 0);
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	const ivec2 dx = ivec2(1, 0); 
	const ivec2 dy = ivec2(0, 1);

	float dC = texelFetch(divergence, pixelCoords, 0).x;

	float pL = texelFetchOffset(pressure_READ, pixelCoords, 0, - dx).x;
	float pR = texelFetchOffset(pressure_READ, pixelCoords, 0,   dx).x;
	float pB = texelFetchOffset(pressure_READ, pixelCoords, 0, - dy).x;
	float pT = texelFetchOffset(pressure_READ, pixelCoords, 0,   dy).x;

	float pC = texelFetch(pressure_READ, pixelCoords, 0).x;
	if(pixelCoords.x == 0) pL = pC;
	if(pixelCoords.y == 0) pB = pC;
	if(pixelCoords.x == tSize.x - 1) pR = pC;
	if(pixelCoords.y == tSize.y - 1) pT = pC;

	imageStore(pressure_WRITE, pixelCoords, vec4(0.25f * (pL + pR + pB + pT - dC), 0.0f, 0.0f, 1.0f));
}

)";

}

namespace vfxgraph
{
namespace node
{

void SolvePressure::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto p_tex = NodeHelper::GetInputTex(*this, I_PRESSURE);
	auto d_tex = NodeHelper::GetInputTex(*this, I_DIVERGENCE);
	if (!p_tex || !d_tex) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("pressure_WRITE"), p_tex, ur::AccessType::WriteOnly);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("pressure_READ"), p_tex);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("divergence"), d_tex);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);

	for (int i = 0; i < m_jacobi_iterations; ++i) {
		rc->ur_ctx->Compute(rc->ur_ds, x, y, z);
	}

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}