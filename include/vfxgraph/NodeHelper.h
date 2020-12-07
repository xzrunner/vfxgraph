#pragma once

#include <unirender/typedef.h>

#include <memory>

namespace ur { class ShaderProgram; }
namespace dag { class Context; }

namespace vfxgraph
{

class Node;
struct Variant;

class NodeHelper
{
public:
    static std::shared_ptr<ur::ShaderProgram> 
        CreateShader(const std::shared_ptr<dag::Context>& ctx, const char* cs);

    static const Variant* GetInputVar(const Node& node, size_t idx);

    static ur::TexturePtr GetInputTex(const Node& node, size_t idx);
    static float GetInputFloat(const Node& node, size_t idx);

}; // NodeHelper

}