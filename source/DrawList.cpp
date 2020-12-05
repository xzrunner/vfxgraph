#include "vfxgraph/DrawList.h"
#include "vfxgraph/Node.h"

#include <dag/Graph.h>

#include <assert.h>

#include <stack>
#include <queue>

namespace vfxgraph
{

DrawList::DrawList(const std::vector<NodePtr>& all_nodes)
    : m_nodes(all_nodes)
{
	Sort();
}

void DrawList::GetAntecedentNodes(const NodePtr& src, std::vector<NodePtr>& nodes)
{
    std::queue<NodePtr> buf;
    buf.push(src);

    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        nodes.push_back(n);
        for (auto& port : n->GetImports()) 
        {
            for (auto& conn : port.conns) {
                buf.push(std::static_pointer_cast<Node>(conn.node.lock()));
            }
        }
    }
}

void DrawList::GetSubsequentNodes(const Node::Port& src, std::vector<NodePtr>& nodes)
{
    std::queue<NodePtr> buf;
    for (auto& conn : src.conns) {
        buf.push(std::static_pointer_cast<Node>(conn.node.lock()));
    }

    while (!buf.empty())
    {
        auto n = buf.front(); buf.pop();
        nodes.push_back(n);
        for (auto& port : n->GetExports()) 
        {
            for (auto& conn : port.conns) {
                buf.push(std::static_pointer_cast<Node>(conn.node.lock()));
            }
        }
    }
}

bool DrawList::Draw(const std::shared_ptr<dag::Context>& ctx, const Node* end) const
{
    bool finished = false;
    for (auto& n : m_nodes)
    {
        //if (n->IsEnable()) {
            n->Execute(ctx);
        //}
        if (n.get() == end) {
            finished = true;
            break;
        }
    }
    return finished;
}

void DrawList::Sort()
{
    std::vector<std::shared_ptr<dag::Node<Variant>>> nodes(m_nodes.size());
    for (size_t i = 0, n = m_nodes.size(); i < n; ++i) {
        nodes[i] = m_nodes[i];
    }
    auto orders = dag::Graph<Variant>::TopologicalSorting(nodes);
	m_nodes.resize(orders.size());
    for (size_t i = 0, n = orders.size(); i < n; ++i) {
		m_nodes[i] = std::static_pointer_cast<Node>(nodes[orders[i]]);
    }
}

}