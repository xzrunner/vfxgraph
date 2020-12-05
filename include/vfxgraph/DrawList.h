#pragma once

#include "vfxgraph/Node.h"
#include "vfxgraph/typedef.h"

#include <unirender/typedef.h>

#include <vector>

namespace dag { class Context; }

namespace vfxgraph
{

class DrawList
{
public:
    DrawList(const std::vector<NodePtr>& all_nodes);

    bool Draw(const std::shared_ptr<dag::Context>& ctx,
        const Node* end = nullptr) const;

    static void GetAntecedentNodes(const NodePtr& src, std::vector<NodePtr>& nodes);
    static void GetSubsequentNodes(const Node::Port& src, std::vector<NodePtr>& nodes);

private:
	void Sort();

private:
    std::vector<NodePtr> m_nodes;

}; // DrawList

}