#include "vfxgraph/node/AddSplat.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/RenderContext.h"

#include <SM_Vector.h>
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
	ivec2 spotPos;
	vec3 color;
	float intensity;
};

layout(rgba32f) uniform image2D field;

void main()
{
	ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);
	vec2 p = vec2(pixelCoords - spotPos);

	vec3 splat = intensity * exp(- dot(p, p) / 200.0f) * color;
	vec3 baseD = imageLoad(field, pixelCoords).xyz;

	imageStore(field, pixelCoords, vec4(baseD + splat, 1.0f));
}

)";

}

namespace vfxgraph
{
namespace node
{

void AddSplat::Execute(const std::shared_ptr<dag::Context>& ctx)
{
	if (!IsDirty()) {
		return;
	}

	if (!m_shader) {
		m_shader = NodeHelper::CreateShader(ctx, cs);
	}

	auto tex = NodeHelper::GetInputTex(*this, I_TEX);
	if (!tex) {
		return;
	}

	auto u_spot_pos = m_shader->QueryUniform("spotPos");
	assert(u_spot_pos);
	auto f2 = NodeHelper::GetInputFloat2(*this, I_POS);
	sm::ivec2 pos;
	if (f2)
	{
		pos.x = static_cast<int>(f2[0]);
		pos.y = static_cast<int>(f2[1]);
	}
	u_spot_pos->SetValue(pos.xy, 2);

	auto u_color = m_shader->QueryUniform("color");
	assert(u_color);
	auto color = NodeHelper::GetInputFloat3(*this, I_COLOR);
	if (color) {
		u_color->SetValue(color, 3);
	}

	auto u_intensity = m_shader->QueryUniform("intensity");
	assert(u_intensity);
	auto intensity = NodeHelper::GetInputFloat(*this, I_INTENSITY);
	if (intensity) {
		u_intensity->SetValue(&intensity, 1);
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);

	rc->ur_ctx->SetImage(m_shader->QueryImgSlot("field"), tex, ur::AccessType::ReadWrite);

	rc->ur_ds.program = m_shader;
	int x, y, z;
	m_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x, y, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });

	SetDirty(false);
}

}
}