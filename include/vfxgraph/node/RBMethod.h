#pragma once

#include "vfxgraph/Node.h"

#include <unirender/typedef.h>

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class RBMethod : public Node
{
public:
    RBMethod()
    {
        m_imports = {
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "velocities" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        ID_VELOCITIES = 1,
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_div_rb_shader           = nullptr;
    std::shared_ptr<ur::ShaderProgram> m_jacobi_black_shader     = nullptr;
    std::shared_ptr<ur::ShaderProgram> m_jacobi_red_shader       = nullptr;
    std::shared_ptr<ur::ShaderProgram> m_pressure_proj_rb_shader = nullptr;

    ur::TexturePtr m_divergence_tex = nullptr;
    ur::TexturePtr m_pressure_tex = nullptr;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/RBMethod.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // RBMethod

}
}