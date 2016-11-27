#include "InitialMapping.h"

#include <vector>
#include <algorithm>

namespace StaticMapping {

InitialMapping::InitialMapping(const NetworKit::Graph& cg, const NetworKit::Graph& pg):
    communicationGraph(cg), processorGraph(pg),
    mapping(std::vector<NetworKit::index>(cg.numberOfNodes()))
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
	return CommTime(processorGraph);
}

} // namespace
