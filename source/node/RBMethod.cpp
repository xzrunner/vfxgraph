#include "vfxgraph/node/RBMethod.h"
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

const char* div_rb_cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D divergence;
uniform sampler2D velocities_READ;

// The grid point 11 corresponds to the pixel coords 2 * pixelCoords
// We then fetch various neighboors to compute the divergence of points 11, 21, 12, 22
// and store them in a single point on the divergence texture with r -> 11, g -> 21, b -> 22, a -> 12

// 03 ------ 13 ------ 23 ------ 33
// |         |         |         |
// |         |         |         |
// 02 ------ 12 ------ 22 ------ 32
// |         |         |         |
// |         |         |         |
// 01 ------ 11 ------ 21 ------ 31
// |         |         |         |
// |         |         |         |
// 00 ------ 10 ------ 20 ------ 30


void main()
{
	const ivec2 tSize = textureSize(velocities_READ, 0);
	const ivec2 pixelCoords = ivec2(gl_GlobalInvocationID.xy);

	vec2 field11 = texelFetch(velocities_READ      , 2 * pixelCoords, 0               ).xy;
	vec2 field01 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2(-1,  0)).xy;
	vec2 field21 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 1,  0)).xy;
	vec2 field10 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 0, -1)).xy;
	vec2 field12 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 0,  1)).xy;
	vec2 field20 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 1, -1)).xy;
	vec2 field02 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2(-1,  1)).xy;
	vec2 field13 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 0,  2)).xy;
	vec2 field23 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 1,  2)).xy;
	vec2 field22 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 1,  1)).xy;
	vec2 field31 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 2,  0)).xy;
	vec2 field32 = texelFetchOffset(velocities_READ, 2 * pixelCoords, 0, ivec2( 2,  1)).xy;

	if(pixelCoords.x == 0) 
	{
		field01.x = - field11.x;
		field02.x = - field12.x;
	}
	if(pixelCoords.y == 0)
	{
		field10.y = - field11.y;
		field20.y = - field21.y;
	}
	if(2 * pixelCoords.x + 1 >= tSize.x - 1)
	{
		field31.x = - field21.x;
		field32.x = - field22.x;
	}
	if(2 * pixelCoords.y + 1 >= tSize.y - 1)
	{
		field13.y = - field12.y;
		field23.y = - field22.y;
	}

	const float r = 0.5 * (field21.x - field01.x + field12.y - field10.y);
	const float g = 0.5 * (field31.x - field11.x + field22.y - field20.y);
	const float b = 0.5 * (field32.x - field12.x + field23.y - field21.y);
	const float a = 0.5 * (field22.x - field02.x + field13.y - field11.y);

	imageStore(divergence, pixelCoords, vec4(r, g, b, a));
}

)";

const char* jacobi_black_cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D pressure_WRITE;
uniform sampler2D pressure_READ;
uniform sampler2D divergence;

vec2 pixelToTexel(in vec2 p, in vec2 tSize)
{
	return (p + 0.5) / tSize;
}

// The divergence and pressure is packed in an half-size texture.
// This black pass updates the bottom left and top right points (black points)
// using neighbooring red points.
// For example result(22) = p(12) + p(32) + p(23) + (21) - div(22) 
//                        = p(0, 1).g + p(1, 1).a + p(1, 1).g + p(1, 0).a - div(1, 1).g;

// 03 ------ 13        23 ------ 33
// |          |        |          |
// |  (0, 1)  |        |  (1, 1)  |
// |          |        |          |
// 02 ------ 12        22 ------ 32
//                            
//                            
// 01 ------ 11        21 ------ 31
// |          |        |          |
// |  (0, 0)  |        |  (1, 0)  |
// |          |        |          |
// 00 ------ 10        20 ------ 30

void main()
{
	const ivec2 tSize = textureSize(pressure_READ, 0);
	const vec2 pixelCoords = gl_GlobalInvocationID.xy;
	const vec2 dx = vec2(1, 0); 
	const vec2 dy = vec2(0, 1);

	const vec4 dC = texelFetch(divergence, ivec2(pixelCoords), 0);

	const vec4 pC = texelFetch(pressure_READ, ivec2(pixelCoords), 0);
	const vec4 pL = texture(pressure_READ, pixelToTexel(pixelCoords - dx, tSize));
	const vec4 pR = texture(pressure_READ, pixelToTexel(pixelCoords + dx, tSize));
	const vec4 pB = texture(pressure_READ, pixelToTexel(pixelCoords - dy, tSize));
	const vec4 pT = texture(pressure_READ, pixelToTexel(pixelCoords + dy, tSize));

	const float r = 0.25 * (pL.y + pC.y + pB.w + pC.w - dC.x);
	const float b = 0.25 * (pC.w + pR.w + pC.y + pT.y - dC.z);

	imageStore(pressure_WRITE, ivec2(pixelCoords), vec4(r, pC.y, b, pC.w));
}

)";

const char* jacobi_red_cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D pressure_WRITE;
uniform sampler2D pressure_READ;
uniform sampler2D divergence;

vec2 pixelToTexel(in vec2 p, in vec2 tSize)
{
	return (p + 0.5) / tSize;
}

// The divergence and pressure is packed in an half-size texture.
// This red pass updates the top left and bottom right points (red points)
// using neighbooring black points.

// 03 ------ 13        23 ------ 33
// |          |        |          |
// |  (0, 1)  |        |  (1, 1)  |
// |          |        |          |
// 02 ------ 12        22 ------ 32
//                            
//                            
// 01 ------ 11        21 ------ 31
// |          |        |          |
// |  (0, 0)  |        |  (1, 0)  |
// |          |        |          |
// 00 ------ 10        20 ------ 30

void main()
{
	const ivec2 tSize = textureSize(pressure_READ, 0);
	const vec2 pixelCoords = gl_GlobalInvocationID.xy;
	const vec2 dx = vec2(1, 0); 
	const vec2 dy = vec2(0, 1);

	const vec4 dC = texelFetch(divergence, ivec2(pixelCoords), 0);

	const vec4 pC = texelFetch(pressure_READ, ivec2(pixelCoords), 0);
	const vec4 pL = texture(pressure_READ, pixelToTexel(pixelCoords - dx, tSize));
	const vec4 pR = texture(pressure_READ, pixelToTexel(pixelCoords + dx, tSize));
	const vec4 pB = texture(pressure_READ, pixelToTexel(pixelCoords - dy, tSize));
	const vec4 pT = texture(pressure_READ, pixelToTexel(pixelCoords + dy, tSize));

	const float g = 0.25 * (pC.x + pR.x + pB.z + pC.z - dC.y);
	const float a = 0.25 * (pL.z + pC.z + pT.x + pC.x - dC.w);

	imageStore(pressure_WRITE, ivec2(pixelCoords), vec4(pC.x, g, pC.z, a));
}

)";

const char* pressure_proj_rb_cs = R"(

#version 430

layout(local_size_x = 32, local_size_y = 32) in;

layout(rgba32f) uniform image2D velocities_WRITE;
uniform sampler2D velocities_READ;
uniform sampler2D pressure_READ;

vec2 pixelToTexel(in vec2 p, in vec2 tSize)
{
	return (p + 0.5) / tSize;
}

// 01 ------ 11
// |          |
// |  (0, 0)  |
// |          |
// 00 ------ 10

void main()
{
	const ivec2 tSize = textureSize(pressure_READ, 0);
	const vec2 pixelCoords = gl_GlobalInvocationID.xy;
	const vec2 dx = vec2(1, 0); 
	const vec2 dy = vec2(0, 1);

	const vec4 pC = texelFetch(pressure_READ, ivec2(pixelCoords), 0);
	const vec4 pL = texture(pressure_READ, pixelToTexel(pixelCoords - dx, tSize));
	const vec4 pR = texture(pressure_READ, pixelToTexel(pixelCoords + dx, tSize));
	const vec4 pB = texture(pressure_READ, pixelToTexel(pixelCoords - dy, tSize));
	const vec4 pT = texture(pressure_READ, pixelToTexel(pixelCoords + dy, tSize));

	const vec2 rGrad = 0.5 * vec2(pC.y - pL.y, pC.w - pB.w);
	const vec2 gGrad = 0.5 * vec2(pR.x - pC.x, pC.z - pB.z);
	const vec2 bGrad = 0.5 * vec2(pR.w - pC.w, pT.y - pC.y);
	const vec2 aGrad = 0.5 * vec2(pC.z - pL.z, pT.x - pC.x);

	const ivec2 pCoords = 2 * ivec2(pixelCoords);
	const vec2 rVel = texelFetch(      velocities_READ, pCoords, 0             ).xy;
	const vec2 gVel = texelFetchOffset(velocities_READ, pCoords, 0, ivec2(1, 0)).xy;
	const vec2 bVel = texelFetchOffset(velocities_READ, pCoords, 0, ivec2(1, 1)).xy;
	const vec2 aVel = texelFetchOffset(velocities_READ, pCoords, 0, ivec2(0, 1)).xy;

	imageStore(velocities_WRITE, pCoords              , vec4(rVel - rGrad, 0.0, 1.0));
	imageStore(velocities_WRITE, pCoords + ivec2(1, 0), vec4(gVel - gGrad, 0.0, 1.0));
	imageStore(velocities_WRITE, pCoords + ivec2(1, 1), vec4(bVel - bGrad, 0.0, 1.0));
	imageStore(velocities_WRITE, pCoords + ivec2(0, 1), vec4(aVel - aGrad, 0.0, 1.0));
}

)";

}

namespace vfxgraph
{
namespace node
{

void RBMethod::Execute(const std::shared_ptr<dag::Context>& ctx)
{ 
	if (!m_div_rb_shader) {
		m_div_rb_shader = NodeHelper::CreateShader(ctx, div_rb_cs);
	}
	if (!m_jacobi_black_shader) {
		m_jacobi_black_shader = NodeHelper::CreateShader(ctx, jacobi_black_cs);
	}
	if (!m_jacobi_red_shader) {
		m_jacobi_red_shader = NodeHelper::CreateShader(ctx, jacobi_red_cs);
	}
	if (!m_pressure_proj_rb_shader) {
		m_pressure_proj_rb_shader = NodeHelper::CreateShader(ctx, pressure_proj_rb_cs);
	}
	if (!m_div_rb_shader || !m_jacobi_black_shader || !m_jacobi_red_shader || !m_pressure_proj_rb_shader) {
		return;
	}

	auto rc = std::static_pointer_cast<RenderContext>(ctx);
	if (!m_divergence_tex)
	{
		ur::TextureDescription desc;
		desc.target = ur::TextureTarget::Texture2D;
		desc.width = desc.height = 256;
		desc.format = ur::TextureFormat::RGBA32F;

		m_divergence_tex = rc->ur_dev->CreateTexture(desc);
		m_pressure_tex = rc->ur_dev->CreateTexture(desc);
	}

	auto velocities_tex = NodeHelper::GetInputTex(*this, ID_VELOCITIES);
	if (!velocities_tex) {
		return;
	}

	// divergence

	rc->ur_ctx->SetImage(m_div_rb_shader->QueryImgSlot("divergence"), m_divergence_tex, ur::AccessType::WriteOnly);
	rc->ur_ctx->SetTexture(m_div_rb_shader->QueryTexSlot("velocities_READ"), velocities_tex);

	rc->ur_ds.program = m_div_rb_shader;
	int x, y, z;
	m_div_rb_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x / 2, y / 2, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });

	// pressure
	for (int i = 0; i < m_jacobi_iterations; ++i)
	{
		int x, y, z;

		// black

		rc->ur_ctx->SetImage(m_jacobi_black_shader->QueryImgSlot("pressure_WRITE"), m_pressure_tex, ur::AccessType::WriteOnly);
		rc->ur_ctx->SetTexture(m_jacobi_black_shader->QueryTexSlot("pressure_READ"), m_pressure_tex);
		rc->ur_ctx->SetTexture(m_jacobi_black_shader->QueryTexSlot("divergence"), m_divergence_tex);

		rc->ur_ds.program = m_jacobi_black_shader;
		m_jacobi_black_shader->GetComputeWorkGroupSize(x, y, z);
		rc->ur_ctx->Compute(rc->ur_ds, x / 2, y / 2, z);

		rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });

		// red

		rc->ur_ctx->SetImage(m_jacobi_red_shader->QueryImgSlot("pressure_WRITE"), m_pressure_tex, ur::AccessType::WriteOnly);
		rc->ur_ctx->SetTexture(m_jacobi_red_shader->QueryTexSlot("pressure_READ"), m_pressure_tex);
		rc->ur_ctx->SetTexture(m_jacobi_red_shader->QueryTexSlot("divergence"), m_divergence_tex);

		rc->ur_ds.program = m_jacobi_red_shader;
		m_jacobi_red_shader->GetComputeWorkGroupSize(x, y, z);
		rc->ur_ctx->Compute(rc->ur_ds, x / 2, y / 2, z);

		rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
	}

	// pressure projection

	rc->ur_ctx->SetImage(m_pressure_proj_rb_shader->QueryImgSlot("velocities_WRITE"), velocities_tex, ur::AccessType::WriteOnly);
	rc->ur_ctx->SetTexture(m_pressure_proj_rb_shader->QueryTexSlot("velocities_READ"), velocities_tex);
	rc->ur_ctx->SetTexture(m_pressure_proj_rb_shader->QueryTexSlot("pressure_READ"), m_pressure_tex);

	rc->ur_ds.program = m_pressure_proj_rb_shader;
	m_pressure_proj_rb_shader->GetComputeWorkGroupSize(x, y, z);
	rc->ur_ctx->Compute(rc->ur_ds, x / 2, y / 2, z);

	rc->ur_ctx->MemoryBarrier({ ur::BarrierType::ShaderImageAccess });
}

}
}