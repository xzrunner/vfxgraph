#include "vfxgraph/VfxGraph.h"

#include <shadergraph/ShaderGraph.h>

namespace vfxgraph
{

CU_SINGLETON_DEFINITION(VfxGraph);

extern void regist_rttr();

VfxGraph::VfxGraph()
{
	regist_rttr();
}

}