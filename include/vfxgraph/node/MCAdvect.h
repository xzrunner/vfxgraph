#pragma once

#include "vfxgraph/Node.h"

#include <unirender/typedef.h>

namespace ur { class ShaderProgram; }

namespace vfxgraph
{
namespace node
{

class MCAdvect : public Node
{
public:
    MCAdvect()
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
        ID_VELOCITIES = 1,
        ID_FIELD,
        ID_DT
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    std::array<ur::TexturePtr, 2> m_temp_texs;

    RTTR_ENABLE(Node)

}; // MCAdvect

}
}