#pragma once

#include "vfxgraph/Node.h"

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class UpdateQAndTheta : public Node
{
public:
    UpdateQAndTheta()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "qTex" }},
            {{ VarType::Texture, "pTemp" }},
            {{ VarType::Texture, "pAdvectedTemp" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        I_Q_TEX = 1,
        I_P_TEMP,
        I_P_ADVECTED_TEMP,
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

}; // UpdateQAndTheta

}
}