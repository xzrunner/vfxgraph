#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/Node.h"
#include "vfxgraph/ValueImpl.h"
#include "vfxgraph/RenderContext.h"

#include <unirender/Device.h>
#include <shadertrans/ShaderTrans.h>

namespace vfxgraph
{

std::shared_ptr<ur::ShaderProgram> 
NodeHelper::CreateShader(const std::shared_ptr<dag::Context>& ctx, const char* cs)
{
    std::vector<unsigned int> _cs;
    shadertrans::ShaderTrans::GLSL2SpirV(shadertrans::ShaderStage::ComputeShader, cs, _cs);

    auto rc = std::static_pointer_cast<RenderContext>(ctx);
    return rc->ur_dev->CreateShaderProgram(_cs);
}

const Variant* NodeHelper::GetInputVar(const Node& node, size_t idx)
{
    auto& imports = node.GetImports();
    if (idx < 0 || idx >= imports.size()) {
        return nullptr;
    }

    auto& conns = imports[idx].conns;
    if (conns.empty()) {
        return nullptr;
    }

    assert(imports[idx].conns.size() == 1);
    auto& in_conn = imports[idx].conns[0];
    auto in_node = in_conn.node.lock();
    if (!in_node) {
        return nullptr;
    }

    return &in_node->GetExports()[in_conn.idx].var.type;
}

ur::TexturePtr NodeHelper::GetInputTex(const Node& node, size_t idx)
{
    auto in_val = GetInputVar(node, idx);
    if (!in_val || !in_val->val || in_val->type != VarType::Texture) {
        return nullptr;
    }

    return std::static_pointer_cast<TextureVal>(in_val->val)->texture;
}

float NodeHelper::GetInputFloat(const Node& node, size_t idx)
{
    auto in_val = GetInputVar(node, idx);
    if (!in_val || !in_val->val || in_val->type != VarType::Float) {
        return 0.0f;
    }

    return std::static_pointer_cast<FloatVal>(in_val->val)->x;
}

const float* NodeHelper::GetInputFloat2(const Node& node, size_t idx)
{
    auto in_val = GetInputVar(node, idx);
    if (!in_val || !in_val->val || in_val->type != VarType::Float2) {
        return nullptr;
    }

    return std::static_pointer_cast<Float2Val>(in_val->val)->xy;
}

const float* NodeHelper::GetInputFloat3(const Node& node, size_t idx)
{
    auto in_val = GetInputVar(node, idx);
    if (!in_val || !in_val->val || in_val->type != VarType::Float3) {
        return nullptr;
    }

    return std::static_pointer_cast<Float3Val>(in_val->val)->xyz;
}

}