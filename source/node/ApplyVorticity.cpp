#include "vfxgraph/node/ApplyVorticity.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Context.h>
#include <unirender/ShaderProgram.h>

namespace
{

const char* cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

uniform UBO
{
	float dt;
};

layout(rgba32f) uniform image2D velocities_READ_WRITE;
uniform sampler2D curl;

void main()
{
	const ivec2 tSize = textureSize(curl, 0);
	const ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	const ivec2 dx = ivec2(1, 0); 
	const ivec2 dy = ivec2(0, 1);

	float vL = texelFetchOffset(curl, pixelCoords, 0, - dx).y;
	float vR = texelFetchOffset(curl, pixelCoords, 0,   dx).y;
	float vB = texelFetchOffset(curl, pixelCoords, 0, - dy).y;
	float vT = texelFetchOffset(curl, pixelCoords, 0,   dy).y;

	float vC = texelFetch(curl, pixelCoords, 0).y;
	if(pixelCoords.x == 0) vL = vC;
	if(pixelCoords.y == 0) vB = vC;
	if(pixelCoords.x >= tSize.x - 1) vR = vC;
	if(pixelCoords.y >= tSize.y - 1) vT = vC;

	vec2 force = 0.5f * vec2(abs(vT) - abs(vB), abs(vR) - abs(vL));
	force /= 1e-10 + length(force);
	force *= vC * vec2(1.0f, -1.0f);

	imageStore(velocities_READ_WRITE, pixelCoords, imageLoad(velocities_READ_WRITE, pixelCoords) + dt * vec4(force, 0.0f, 0.0f));
}

)";

}

namespace vfxgraph
{
namespace node
{

void ApplyVorticity::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto v_tex = NodeHelper::GetInputTex(*this, I_VELOCITIES);
	auto c_tex = NodeHelper::GetInputTex(*this, I_CURL);
	if (!v_tex || !c_tex) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetTexture(m_shader->QueryTexSlot("curl"), c_tex);
	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("velocities_READ_WRITE"), v_tex, ur::AccessType::ReadWrite);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}