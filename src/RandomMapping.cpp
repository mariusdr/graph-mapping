#include "RandomMapping.h"

#include <random>

namespace StaticMapping {

RandomMapping::RandomMapping(const NetworKit::Graph& cg, const NetworKit::Graph& pg):
    communicationGraph(cg), processorGraph(pg), 
    mapping(std::vector<NetworKit::index>(cg.numberOfNodes())),
	hasrun(false)
{}

RandomMapping::RandomMapping(const RandomMapping& rhs):
    communicationGraph(rhs.communicationGraph),
	processorGraph(rhs.processorGraph),
	mapping(rhs.mapping),
	hasrun(rhs.hasrun)
{}

void RandomMapping::run() {
	assertRequirements(communicationGraph, processorGraph);

    std::iota(mapping.begin(), mapping.end(), 0);
    std::random_shuffle(mapping.begin(), mapping.end());

	hasrun = true;
}

std::vector<NetworKit::index> RandomMapping::getMapping() const {
    return mapping;
}

bool RandomMapping::hasRun() const {
	return hasrun;
}

CommTime RandomMapping::getCommTime() const {
	auto ct = CommTime(processorGraph);
	if (!ct.hasRun())
		ct.run();
	return ct;
}

} // namespace
