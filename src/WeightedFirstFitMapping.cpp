#include "WeightedFirstFitMapping.h"

namespace StaticMapping {

WeightedFirstFitMapping::WeightedFirstFitMapping(const NetworKit::Graph& cg,
												 const NetworKit::Graph& pg,
												 const std::vector<unsigned int>& cgMemoryMap,
												 const std::vector<unsigned int>& pgMemoryMap):
	communicationGraph(cg), processorGraph(pg), numberOfNodes(cg.numberOfNodes()),
	cgMemoryMap(cgMemoryMap), pgMemoryMap(pgMemoryMap),
	commTimes(CommTime(pg)),
	mapping(std::vector<NetworKit::index>(numberOfNodes, 0)),
	invmapping(std::vector<NetworKit::index>(numberOfNodes, 0)),
	isMapped(std::vector<bool>(numberOfNodes, false)),
	invIsMapped(std::vector<bool>(numberOfNodes, false)),
	numberOfMappedNodes(0), hasrun(false)
{}

NetworKit::node WeightedFirstFitMapping::findReplacement(NetworKit::node vc_next) {
	auto memNeeded = cgMemoryMap[vc_next];
	
	for (auto vp: processorGraph.nodes()) {
		if (invIsMapped[vp]) {
			auto memAvailable = pgMemoryMap[vp];

			if (memAvailable >= memNeeded) {
				removeProcNodeMapping(vp);
				return vp;
			}
		}
	}
	throw std::runtime_error("can't find replacement for node");
}

void WeightedFirstFitMapping::run() {
	NetworKit::node vc_next;
	while (numberOfMappedNodes < numberOfNodes) {
		
	}
}
	
std::vector<NetworKit::index> WeightedFirstFitMapping::getMapping() const {
	return mapping;
}

CommTime WeightedFirstFitMapping::getCommTime() const {
	return commTimes;
}

bool WeightedFirstFitMapping::hasRun() const {
	return hasrun;
}

void WeightedFirstFitMapping::setMapping(NetworKit::node vc, NetworKit::node vp) {
	mapping[vc] = vp;
	invmapping[vp] = vc;
	isMapped[vc] = true;
	invIsMapped[vp] = true;
	++numberOfMappedNodes;
}

void WeightedFirstFitMapping::removeCommNodeMapping(NetworKit::node vc) {
	if (isMapped[vc]) {
		auto vp = mapping[vc];
		invmapping[vp] = 0;
		mapping[vc] = 0;
		isMapped[vc] = false;
		invIsMapped[vp] = false;
		--numberOfMappedNodes;
	}
}

void WeightedFirstFitMapping::removeProcNodeMapping(NetworKit::node vp) {
	if (invIsMapped[vp]) {
		auto vc = invmapping[vp];
		mapping[vc] = 0;
		invmapping[vp] = 0;
		isMapped[vc] = false;
		invIsMapped[vp] = false;
		--numberOfMappedNodes;
	}
}

} // namespace
