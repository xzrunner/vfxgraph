#ifndef PARAM_INFO
#error "You must define PARAM_INFO macro before include this file"
#endif

PARAM_INFO(Filepath, std::string, filepath, m_filepath, ())

PARAM_INFO(Type, vfxgraph::node::Texture::Type, type, m_type, (vfxgraph::node::Texture::Type::Tex2D))

PARAM_INFO(Width,  int, width,  m_width,  (0))
PARAM_INFO(Height, int, height, m_height, (0))
PARAM_INFO(Format, vfxgraph::node::Texture::Format, format, m_format, (vfxgraph::node::Texture::Format::RGBA8))

PARAM_INFO(Wrapping,  vfxgraph::node::Texture::Wrapping,  wrap,   m_wrap,   (vfxgraph::node::Texture::Wrapping::Repeat))
PARAM_INFO(Filtering, vfxgraph::node::Texture::Filtering, filter, m_filter, (vfxgraph::node::Texture::Filtering::Linear))
