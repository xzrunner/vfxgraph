#include "vfxgraph/node/DivCurl.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D divergence;
uniform sampler2D velocities_READ;

void main()
{
	const ivec2 tSize = textureSize(velocities_READ, 0);
	const ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

	const ivec2 dx = ivec2(1, 0); 
	const ivec2 dy = ivec2(0, 1);

	vec2 fieldL = texelFetchOffset(velocities_READ, pixelCoords, 0, - dx).xy;
	vec2 fieldR = texelFetchOffset(velocities_READ, pixelCoords, 0,   dx).xy;
	vec2 fieldB = texelFetchOffset(velocities_READ, pixelCoords, 0, - dy).xy;
	vec2 fieldT = texelFetchOffset(velocities_READ, pixelCoords, 0,   dy).xy;

	vec2 fieldC = texelFetch(velocities_READ, pixelCoords, 0).xy;
	if(pixelCoords.x == 0) fieldL.x = - fieldC.x;
	if(pixelCoords.y == 0) fieldB.y = - fieldC.y;
	if(pixelCoords.x >= tSize.x - 1) fieldR.x = - fieldC.x;
	if(pixelCoords.y >= tSize.y - 1) fieldT.y = - fieldC.y;

	float div = 0.5f * (fieldR.x - fieldL.x + fieldT.y - fieldB.y);
	float curl = 0.5f * (fieldR.y - fieldL.y - fieldT.x + fieldB.x);

	imageStore(divergence, pixelCoords, vec4(div, curl, 0.0f, 1.0f));
}

)";

}

namespace vfxgraph
{
namespace node
{

void DivCurl::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto v_tex = NodeHelper::GetInputTex(*this, I_VELOCITIES);
	auto d_tex = NodeHelper::GetInputTex(*this, I_DIVERGENCE);
	if (!v_tex || !d_tex) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("velocities_READ"), v_tex);
	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("divergence"), d_tex, ur::AccessType::WriteOnly);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}