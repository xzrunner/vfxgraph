#pragma once

#include "vfxgraph/Value.h"

#include <unirender/typedef.h>

namespace ur { class TextureSampler; }

namespace vfxgraph
{

struct FloatVal : public Value
{
    FloatVal() {}
    FloatVal(float x) : x(x) {}

    float x = 0;
};

struct Float2Val : public Value
{
    Float2Val() {}
    Float2Val(float x, float y) {
        xy[0] = x;
        xy[1] = y;
    }

    float xy[2] = { 0, 0 };
};

struct Float3Val : public Value
{
    Float3Val() {}
    Float3Val(float x, float y, float z) {
        xyz[0] = x;
        xyz[1] = y;
        xyz[2] = z;
    }

    float xyz[3] = { 0, 0, 0 };
};

struct Float4Val : public Value
{
    Float4Val() {}
    Float4Val(float x, float y, float z, float w) {
        xyzw[0] = x;
        xyzw[1] = y;
        xyzw[2] = z;
        xyzw[3] = w;
    }

    float xyzw[4] = { 0, 0, 0, 0 };
};

struct TextureVal : public Value
{
    TextureVal(const ur::TexturePtr& tex, const std::shared_ptr<ur::TextureSampler>& samp = nullptr)
        : texture(tex), sampler(samp) {}

    ur::TexturePtr                      texture = nullptr;
    std::shared_ptr<ur::TextureSampler> sampler = nullptr;
};

}