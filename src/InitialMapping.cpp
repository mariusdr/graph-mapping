#include "InitialMapping.h"

#include <vector>
#include <algorithm>

namespace StaticMapping {

InitialMapping::InitialMapping(const NetworKit::Graph& cg, const NetworKit::Graph& pg):
    communicationGraph(cg), processorGraph(pg),
    mapping(std::vector<NetworKit::index>(cg.numberOfNodes())),
	hasrun(false)
{}

InitialMapping::InitialMapping(const InitialMapping& rhs):
    communicationGraph(rhs.communicationGraph),
	processorGraph(rhs.communicationGraph),
    mapping(rhs.mapping),
	hasrun(rhs.hasrun)
{}	

void InitialMapping::run() {
	assertRequirements(communicationGraph, processorGraph);
	
    std::iota(mapping.begin(), mapping.end(), 0);
	hasrun = true;
}

std::vector<NetworKit::index> InitialMapping::getMapping() const {
	return mapping;
}

bool InitialMapping::hasRun() const {
	return hasrun;
}

CommTime InitialMapping::getCommTime() const {
	auto ct = CommTime(processorGraph);
	if (!ct.hasRun())
		ct.run();
	return ct;
}

} // namespace
