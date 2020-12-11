#pragma once

#include "vfxgraph/Node.h"

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class Copy : public Node
{
public:
    Copy()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "src" }},
            {{ VarType::Texture, "dst" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
            {{ VarType::Texture, "out" }},
        };
    }

    enum InputID
    {
        ID_SRC = 1,
        ID_DST
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

}; // Copy

}
}