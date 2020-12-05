#pragma once

#include "vfxgraph/VarType.h"

#include <string>

namespace vfxgraph
{

struct Value;

struct Variant
{
    VarType     type = VarType::Unknown;
    std::string name;

    std::shared_ptr<Value> val = nullptr;

}; // Variant

}