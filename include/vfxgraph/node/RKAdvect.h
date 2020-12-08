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
            {{ VarType::Texture, "read" }},
            {{ VarType::Texture, "write" }},
            {{ VarType::Float,   "dt" }},
        };
        m_exports = {
            {{ VarType::Port,    "next" }},
        };
    }

    enum InputID
    {
        ID_VELOCITIES = 1,
        ID_READ,
        ID_WRITE,
        ID_DT
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    static void Execute(const std::shared_ptr<dag::Context>& ctx, const ur::TexturePtr& v, 
        const ur::TexturePtr& read, const ur::TexturePtr& write, float dt);

private:
    static std::shared_ptr<ur::ShaderProgram> m_shader;

    RTTR_ENABLE(Node)

}; // RKAdvect

}
}