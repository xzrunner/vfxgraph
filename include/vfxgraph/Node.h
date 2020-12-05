#pragma once

#include "vfxgraph/Variant.h"

#include <dag/Node.h>

#include <rttr/registration>

namespace vfxgraph
{

class Node : public dag::Node<Variant>
{
public:
    Node() {}
    virtual ~Node() {}

    RTTR_ENABLE(dag::Node<Variant>)

}; // Node

}