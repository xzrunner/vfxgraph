#pragma once

#include "vfxgraph/Node.h"

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class DivCurl : public Node
{
public:
    DivCurl()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "velocities" }},
            {{ VarType::Texture, "divergence" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        I_VELOCITIES = 1,
        I_DIVERGENCE
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

}; // DivCurl

}
}