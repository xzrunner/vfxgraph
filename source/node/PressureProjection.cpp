#include "vfxgraph/node/PressureProjection.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D velocities_WRITE;
uniform sampler2D velocities_READ;
uniform sampler2D pressure_READ;

void main()
{
	ivec2 tSize = textureSize(velocities_READ, 0);
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	const ivec2 dx = ivec2(1, 0); 
	const ivec2 dy = ivec2(0, 1);

	float pL = texelFetchOffset(pressure_READ, pixelCoords, 0, - dx).x;
	float pR = texelFetchOffset(pressure_READ, pixelCoords, 0,   dx).x;
	float pB = texelFetchOffset(pressure_READ, pixelCoords, 0, - dy).x;
	float pT = texelFetchOffset(pressure_READ, pixelCoords, 0,   dy).x;

	float pC = texelFetch(pressure_READ, pixelCoords, 0).x;
	if(pixelCoords.x == 0) pL = pC;
	if(pixelCoords.y == 0) pB = pC;
	if(pixelCoords.x >= tSize.x - 1) pR = pC;
	if(pixelCoords.y >= tSize.y - 1) pT = pC;

	vec2 gradP = 0.5f * vec2(pR - pL, pT - pB);

	vec2 oldVel = texelFetch(velocities_READ, pixelCoords, 0).xy;

	imageStore(velocities_WRITE, pixelCoords, vec4(oldVel - gradP, 0.0f, 0.0f));
}

)";

}

namespace vfxgraph
{
namespace node
{

void PressureProjection::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto v_tex = NodeHelper::GetInputTex(*this, I_VELOCITIES);
	auto p_tex = NodeHelper::GetInputTex(*this, I_PRESSURE);
	if (!v_tex || !p_tex) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("velocities_WRITE"), v_tex, ur::AccessType::WriteOnly);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("velocities_READ"), v_tex);
	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("pressure_READ"), p_tex);

	rc->ur_ds.program = m_shader;

	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);

	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}