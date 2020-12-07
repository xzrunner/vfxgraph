#pragma once

#include "vfxgraph/Node.h"

#include <unirender/typedef.h>

namespace ur { class TextureSampler; }

namespace vfxgraph
{

class RenderContext;

namespace node
{

class Texture : public Node
{
public:
    enum InputID
    {
        I_SIZE = 0,
    };

    enum OutputID
    {
        O_TEX = 0,
        O_SIZE,
    };

public:
    Texture()
    {
        m_imports = {
            {{ VarType::Float2, "size" }},
        };
        m_exports = {
            {{ VarType::Texture, "out" }},
            {{ VarType::Float2, "size" }},
        };
    }

    virtual void Execute(const std::shared_ptr<dag::Context>& ctx = nullptr) override;

    void Init(const RenderContext& rc) const;

    void Draw(const RenderContext& rc) const;

    auto GetTexture() const { return m_tex; }
    void SetTexture(const ur::TexturePtr& tex) { m_tex = tex; }

    auto GetSampler() const { return m_sampler; }
    void SetSampler(const std::shared_ptr<ur::TextureSampler>& sampler) {
        m_sampler = sampler;
    }

    enum class Type
    {
        Tex2D,
        TexCube,
    };

    enum class Format
    {
        RGBA8,
        RGBA4,
        RGB,
        RGB565,
        RGBA16F,
        RGBA32F,
		RGB16F,
		RG16F,
        A8,
        RED,
        Depth,
    };

    enum class Wrapping
    {
        Repeat,
        MirroredRepeat,
        ClampToEdge,
        ClampToBorder,
    };

    enum class Filtering
    {
        Nearest,
        Linear,
    };

private:
    mutable ur::TexturePtr m_tex = nullptr;
    mutable std::shared_ptr<ur::TextureSampler> m_sampler = nullptr;

    RTTR_ENABLE(Node)

#define PARM_FILEPATH "vfxgraph/node/Texture.parm.h"
#include <dag/node_parms_gen.h>
#undef PARM_FILEPATH

}; // Texture

}
}