#pragma once

#include "vfxgraph/Node.h"
#include "vfxgraph/NodeHelper.h"
#include "vfxgraph/ValueImpl.h"

namespace vfxgraph
{
namespace node
{

class Add : public Node
{
public:
    enum InputID
    {
        I_A = 1,
        I_B,
    };

    enum OutputID
    {
        O_OUT = 1,
    };

public:
    Add()
    {
        m_imports = {
            {{ VarType::Port, "prev" }},
            {{ VarType::Any,  "a" }},
            {{ VarType::Any,  "b" }}
        };
        m_exports = {
            {{ VarType::Port, "next" }},
            {{ VarType::Any,  "out" }}
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override
    {
        auto a = NodeHelper::GetInputVar(*this, I_A);
        auto b = NodeHelper::GetInputVar(*this, I_B);
        if (!a || !b || !a->val || !b->val) {
            return;
        }

        if (a->type == VarType::Float && b->type == VarType::Float) 
        {
            const float x = std::static_pointer_cast<FloatVal>(a->val)->x + std::static_pointer_cast<FloatVal>(b->val)->x;
            m_exports[O_OUT].var.type.type = VarType::Float;
            m_exports[O_OUT].var.type.val = std::make_shared<FloatVal>(x);
        }
        else if (a->type == VarType::Float2 && b->type == VarType::Float2)
        {
            float xy[2];
            for (int i = 0; i < 2; ++i) {
                xy[i] = std::static_pointer_cast<Float2Val>(a->val)->xy[i] + std::static_pointer_cast<Float2Val>(b->val)->xy[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float2;
            m_exports[O_OUT].var.type.val = std::make_shared<Float2Val>(xy[0], xy[1]);
        }
        else if (a->type == VarType::Float3 && b->type == VarType::Float3)
        {
            float xyz[3];
            for (int i = 0; i < 3; ++i) {
                xyz[i] = std::static_pointer_cast<Float3Val>(a->val)->xyz[i] + std::static_pointer_cast<Float3Val>(b->val)->xyz[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float3;
            m_exports[O_OUT].var.type.val = std::make_shared<Float3Val>(xyz[0], xyz[1], xyz[2]);
        }
        else if (a->type == VarType::Float4 && b->type == VarType::Float4)
        {
            float xyzw[4];
            for (int i = 0; i < 4; ++i) {
                xyzw[i] = std::static_pointer_cast<Float4Val>(a->val)->xyzw[i] + std::static_pointer_cast<Float4Val>(b->val)->xyzw[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float4;
            m_exports[O_OUT].var.type.val = std::make_shared<Float4Val>(xyzw[0], xyzw[1], xyzw[2], xyzw[3]);
        }
    }

    RTTR_ENABLE(Node)

}; // Add

class Sub : public Node
{
public:
    enum InputID
    {
        I_A = 1,
        I_B,
    };

    enum OutputID
    {
        O_OUT = 1,
    };

public:
    Sub()
    {
        m_imports = {
            {{ VarType::Port, "prev" }},
            {{ VarType::Any,  "a" }},
            {{ VarType::Any,  "b" }}
        };
        m_exports = {
            {{ VarType::Port, "next" }},
            {{ VarType::Any,  "out" }}
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override
    {
        auto a = NodeHelper::GetInputVar(*this, I_A);
        auto b = NodeHelper::GetInputVar(*this, I_B);
        if (!a || !b || !a->val || !b->val) {
            return;
        }

        if (a->type == VarType::Float && b->type == VarType::Float) 
        {
            const float x = std::static_pointer_cast<FloatVal>(a->val)->x - std::static_pointer_cast<FloatVal>(b->val)->x;
            m_exports[O_OUT].var.type.type = VarType::Float;
            m_exports[O_OUT].var.type.val = std::make_shared<FloatVal>(x);
        }
        else if (a->type == VarType::Float2 && b->type == VarType::Float2)
        {
            float xy[2];
            for (int i = 0; i < 2; ++i) {
                xy[i] = std::static_pointer_cast<Float2Val>(a->val)->xy[i] - std::static_pointer_cast<Float2Val>(b->val)->xy[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float2;
            m_exports[O_OUT].var.type.val = std::make_shared<Float2Val>(xy[0], xy[1]);
        }
        else if (a->type == VarType::Float3 && b->type == VarType::Float3)
        {
            float xyz[3];
            for (int i = 0; i < 3; ++i) {
                xyz[i] = std::static_pointer_cast<Float3Val>(a->val)->xyz[i] - std::static_pointer_cast<Float3Val>(b->val)->xyz[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float3;
            m_exports[O_OUT].var.type.val = std::make_shared<Float3Val>(xyz[0], xyz[1], xyz[2]);
        }
        else if (a->type == VarType::Float4 && b->type == VarType::Float4)
        {
            float xyzw[4];
            for (int i = 0; i < 4; ++i) {
                xyzw[i] = std::static_pointer_cast<Float4Val>(a->val)->xyzw[i] - std::static_pointer_cast<Float4Val>(b->val)->xyzw[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float4;
            m_exports[O_OUT].var.type.val = std::make_shared<Float4Val>(xyzw[0], xyzw[1], xyzw[2], xyzw[3]);
        }
    }

    RTTR_ENABLE(Node)

}; // Sub

class Mul : public Node
{
public:
    enum InputID
    {
        I_A = 1,
        I_B,
    };

    enum OutputID
    {
        O_OUT = 1,
    };

public:
    Mul()
    {
        m_imports = {
            {{ VarType::Port, "prev" }},
            {{ VarType::Any,  "a" }},
            {{ VarType::Any,  "b" }}
        };
        m_exports = {
            {{ VarType::Port, "next" }},
            {{ VarType::Any,  "out" }}
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override
    {
        auto a = NodeHelper::GetInputVar(*this, I_A);
        auto b = NodeHelper::GetInputVar(*this, I_B);
        if (!a || !b || !a->val || !b->val) {
            return;
        }

        if (a->type == VarType::Float && b->type == VarType::Float) 
        {
            const float x = std::static_pointer_cast<FloatVal>(a->val)->x * std::static_pointer_cast<FloatVal>(b->val)->x;
            m_exports[O_OUT].var.type.type = VarType::Float;
            m_exports[O_OUT].var.type.val = std::make_shared<FloatVal>(x);
        }
        else if (a->type == VarType::Float2 && b->type == VarType::Float2)
        {
            float xy[2];
            for (int i = 0; i < 2; ++i) {
                xy[i] = std::static_pointer_cast<Float2Val>(a->val)->xy[i] * std::static_pointer_cast<Float2Val>(b->val)->xy[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float2;
            m_exports[O_OUT].var.type.val = std::make_shared<Float2Val>(xy[0], xy[1]);
        }
        else if (a->type == VarType::Float3 && b->type == VarType::Float3)
        {
            float xyz[3];
            for (int i = 0; i < 3; ++i) {
                xyz[i] = std::static_pointer_cast<Float3Val>(a->val)->xyz[i] * std::static_pointer_cast<Float3Val>(b->val)->xyz[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float3;
            m_exports[O_OUT].var.type.val = std::make_shared<Float3Val>(xyz[0], xyz[1], xyz[2]);
        }
        else if (a->type == VarType::Float4 && b->type == VarType::Float4)
        {
            float xyzw[4];
            for (int i = 0; i < 4; ++i) {
                xyzw[i] = std::static_pointer_cast<Float4Val>(a->val)->xyzw[i] * std::static_pointer_cast<Float4Val>(b->val)->xyzw[i];
            }
            m_exports[O_OUT].var.type.type = VarType::Float4;
            m_exports[O_OUT].var.type.val = std::make_shared<Float4Val>(xyzw[0], xyzw[1], xyzw[2], xyzw[3]);
        }
    }

    RTTR_ENABLE(Node)

}; // Mul

class Div : public Node
{
public:
    enum InputID
    {
        I_A = 1,
        I_B,
    };

    enum OutputID
    {
        O_OUT = 1,
    };

public:
    Div()
    {
        m_imports = {
            {{ VarType::Port, "prev" }},
            {{ VarType::Any,  "a" }},
            {{ VarType::Any,  "b" }}
        };
        m_exports = {
            {{ VarType::Port, "next" }},
            {{ VarType::Any,  "out" }}
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override
    {
        auto a = NodeHelper::GetInputVar(*this, I_A);
        auto b = NodeHelper::GetInputVar(*this, I_B);
        if (!a || !b || !a->val || !b->val) {
            return;
        }

        if (a->type == VarType::Float && b->type == VarType::Float) 
        {
            if (std::static_pointer_cast<FloatVal>(b->val)->x != 0) {
                const float x = std::static_pointer_cast<FloatVal>(a->val)->x / std::static_pointer_cast<FloatVal>(b->val)->x;
                m_exports[O_OUT].var.type.type = VarType::Float;
                m_exports[O_OUT].var.type.val = std::make_shared<FloatVal>(x);
            }
        }
    }

    RTTR_ENABLE(Node)

}; // Div

}
}