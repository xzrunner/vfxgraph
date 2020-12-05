#pragma once

#include "vfxgraph/Node.h"
#include "vfxgraph/ValueImpl.h"

#include <SM_Vector.h>

namespace vfxgraph
{
namespace node
{

class Float : public Node
{
public:
    Float()
    {
        m_exports = {
            {{ VarType::Float, "out" }}
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override
    {
        m_exports[0].var.type.val = std::make_shared<FloatVal>();
    }

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/Float.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Float

class Float2 : public Node
{
public:
    Float2()
    {
        m_exports = {
            {{ VarType::Float2, "out" }}
        };
    }

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/Float2.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Float2

class Float3 : public Node
{
public:
    Float3()
    {
        m_exports = {
            {{ VarType::Float3, "out" }}
        };
    }

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/Float3.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Float3

class Float4 : public Node
{
public:
    Float4()
    {
        m_exports = {
            {{ VarType::Float4, "out" }}
        };
    }

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/Float4.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Float4

}
}