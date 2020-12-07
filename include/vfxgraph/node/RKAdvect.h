#pragma once

#include "vfxgraph/Node.h"

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class RKAdvect : public Node
{
public:
    RKAdvect()
    {
        m_imports = {
            {{ VarType::Texture, "velocities" }},
            {{ VarType::Texture, "read" }},
            {{ VarType::Texture, "write" }},
            {{ VarType::Float,   "dt" }},
        };
        //m_exports = {
        //    {{ VarType::Texture, "out" }},
        //};
    }

    enum InputID
    {
        ID_VELOCITIES = 0,
        ID_READ,
        ID_WRITE,
        ID_DT
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

}; // RKAdvect

}
}