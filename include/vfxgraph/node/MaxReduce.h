#pragma once

#include "vfxgraph/Node.h"

#include <unirender/typedef.h>

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class MaxReduce : public Node
{
public:
    MaxReduce()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "field" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
            {{ VarType::Float,   "max" }},
        };
    }

    enum InputID
    {
        I_FIELD = 1,
    };

    enum OutputID
    {
        O_MAX = 1,
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    std::vector<ur::TexturePtr> m_reduce_texs;

    RTTR_ENABLE(Node)

}; // MaxReduce

}
}