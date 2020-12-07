#include "vfxgraph/node/value_nodes.h"

#define EXE_FILEPATH "vfxgraph/node_include_gen.h"
#include "vfxgraph/node_regist_cfg.h"
#undef EXE_FILEPATH


#include <rttr/registration>

#define REGIST_NODE_TYPE(type, name)                                      \
	rttr::registration::class_<vfxgraph::device::type>("vfxgraph::"#name) \
		.constructor<>()                                                  \
	;

#define REGIST_ENUM_ITEM(type, name, label) \
    rttr::value(name, type),                \
    rttr::metadata(type, label)             \

RTTR_REGISTRATION
{

rttr::registration::class_<dag::Node<vfxgraph::Variant>::Port>("vfxgraph::Node::Port")
	.property("var", &dag::Node<vfxgraph::Variant>::Port::var)
;

rttr::registration::class_<vfxgraph::Node>("vfxgraph::Node")
	.method("GetImports", &vfxgraph::Node::GetImports)
	.method("GetExports", &vfxgraph::Node::GetExports)
;

#define EXE_FILEPATH "vfxgraph/node_rttr_gen.h"
#include "vfxgraph/node_regist_cfg.h"
#undef EXE_FILEPATH


rttr::registration::enumeration<vfxgraph::node::Texture::Type>("rg_texture_type")
(
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Type::Tex2D,   "2d",   "Tex2D"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Type::TexCube, "cube", "TexCube")
);

rttr::registration::enumeration<vfxgraph::node::Texture::Format>("rg_texture_format")
(
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RGBA8,   "rgba8",   "RGBA8"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RGBA4,   "rgba4",   "RGBA4"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RGB,     "rgb",     "RGB"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RGB565,  "rgb565",  "RGB565"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RGBA16F, "rgba16f", "RGBA16F"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RGBA32F, "rgba32f", "RGBA32F"),
	REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RGB16F,  "rgb16f",  "RGB16F"),
	REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RG16F,   "rg16f",   "RG16F"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::A8,      "a8",      "A8"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::RED,     "red",     "RED"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Format::Depth,   "depth",   "Depth")
);

rttr::registration::enumeration<vfxgraph::node::Texture::Wrapping>("rg_texture_wrapping")
(
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Wrapping::Repeat,         "repeat",          "Repeat"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Wrapping::MirroredRepeat, "mirrored_repeat", "MirroredRepeat"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Wrapping::ClampToEdge,    "clamp_to_edge",   "ClampToEdge"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Wrapping::ClampToBorder,  "clamp_to_border", "ClampToBorder")
);

rttr::registration::enumeration<vfxgraph::node::Texture::Filtering>("rg_texture_filtering")
(
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Filtering::Nearest, "nearest", "Nearest"),
    REGIST_ENUM_ITEM(vfxgraph::node::Texture::Filtering::Linear,  "linear",  "Linear")
);

}

namespace vfxgraph
{

void regist_rttr()
{
}

}