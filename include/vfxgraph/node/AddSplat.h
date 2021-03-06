#pragma once

#include "vfxgraph/Node.h"

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class AddSplat : public Node
{
public:
    AddSplat()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "tex" }},
            {{ VarType::Float2,  "pos" }},
            {{ VarType::Float3,  "color" }},
            {{ VarType::Float,   "intensity" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        I_TEX = 1,
        I_POS,
        I_COLOR,
        I_INTENSITY,
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

}; // AddSplat

}
}