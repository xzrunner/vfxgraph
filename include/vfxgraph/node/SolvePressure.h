#pragma once

#include "vfxgraph/Node.h"

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class SolvePressure : public Node
{
public:
    SolvePressure()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "pressure" }},
            {{ VarType::Texture, "divergence" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        I_PRESSURE = 1,
        I_DIVERGENCE
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/SolvePressure.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // SolvePressure

}
}