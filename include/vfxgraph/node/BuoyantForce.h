#pragma once

#include "vfxgraph/Node.h"

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class BuoyantForce : public Node
{
public:
    BuoyantForce()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "velocities" }},
            {{ VarType::Texture, "temperature" }},
            {{ VarType::Texture, "density" }},
            {{ VarType::Float,   "dt" }},

        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        I_VELOCITIES = 1,
        I_TEMPERATURE,
        I_DENSITY,
        I_DT,
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/BuoyantForce.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // BuoyantForce

}
}