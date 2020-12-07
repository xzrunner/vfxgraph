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
            {{ VarType::Texture, "read" }},
            {{ VarType::Texture, "write" }},
        };
        m_exports = {
            {{ VarType::Texture, "out" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

}; // Copy

}
}