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
        ID_READ = 1,
        ID_WRITE,
        ID_DT
    };

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

private:
    std::shared_ptr<ur::ShaderProgram> m_shader = nullptr;

    std::array<ur::TexturePtr, 3> m_temp_texs;

    RTTR_ENABLE(Node)

}; // MCAdvect

}
}