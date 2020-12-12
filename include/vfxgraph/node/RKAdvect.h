#pragma once

#include "vfxgraph/Node.h"

#include <unirender/typedef.h>

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
            {{ VarType::Port,    "prev" }},
            {{ VarType::Texture, "velocities" }},
            {{ VarType::Texture, "field" }},
            {{ VarType::Float,   "dt" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        I_VELOCITIES = 1,
        I_FIELD,
        I_DT
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    static void Execute(const std::shared_ptr<dag::Context>& ctx, const ur::TexturePtr& velocities,
        const ur::TexturePtr& field_read, const ur::TexturePtr& field_write, float dt);

private:
    static std::shared_ptr<ur::ShaderProgram> m_shader;

    RTTR_ENABLE(Node)

}; // RKAdvect

}
}