#include "RandomMapping.h"

#include <random>

namespace StaticMapping {

RandomMapping::RandomMapping(const NetworKit::Graph& cg, const NetworKit::Graph& pg):
    communicationGraph(cg), processorGraph(pg), 
    mapping(std::vector<NetworKit::index>(cg.numberOfNodes()))
{}

void RandomMapping::run() {
	assertRequirements(communicationGraph, processorGraph);

    std::iota(mapping.begin(), mapping.end(), 0);
    std::random_shuffle(mapping.begin(), mapping.end());
}

std::vector<NetworKit::index> RandomMapping::getMapping() const {
    return mapping;
}

} // namespace
