#include "WeightedGreedyMapping.h"

#include <math.h>
#include <stack>
#include <tuple>
#include <string>

#define NOLOG 1
#include "Log.h"

namespace StaticMapping {

#define MAX_INT std::numeric_limits<int>::max()
#define MAX_DOUBLE std::numeric_limits<double>::max()

#define PROCESSOR_ASSIGNED MAX_INT
#define COMM_NODE_ASSIGNED -1

WeightedGreedyMapping::WeightedGreedyMapping(const NetworKit::Graph& cg,
											 const NetworKit::Graph& pg,
											 const std::vector<unsigned int>& cgMemMap,
											 const std::vector<unsigned int>& pgMemMap):
	communicationGraph(cg), processorGraph(pg),
	numberOfNodes(cg.numberOfNodes()),
	cgMemoryMap(cgMemMap), pgMemoryMap(pgMemMap),
	commTimes(CommTime(pg)),
	sum_c(std::vector<double>(numberOfNodes, 0.0)),
	sum_p(std::vector<double>(numberOfNodes, 0.0)),
	mapping(std::vector<NetworKit::index>(numberOfNodes, 0)),
	invmapping(std::vector<NetworKit::index>(numberOfNodes, 0)),
	isMapped(std::vector<bool>(numberOfNodes, false)),
	invIsMapped(std::vector<bool>(numberOfNodes, false)),
	numberOfMappedNodes(0),
	hasrun(false)
{}

#define FITTINGPARAM 2

void WeightedGreedyMapping::pickInitialNodes(NetworKit::node* vc0, NetworKit::node* vp0) {
	double maxCommvol = -1.0;
	for (auto v: communicationGraph.nodes()) {
		double commvol = 0.0;
		for (auto w: communicationGraph.neighbors(v)) {
			commvol += communicationGraph.weight(v, w);
		}
		
		if (commvol > maxCommvol) {
			maxCommvol = commvol;
			*vc0 = v;
		}
	}

	double minTime = MAX_DOUBLE;
	for (auto v: processorGraph.nodes()) {
		double time = 0.0;
		for (auto w: processorGraph.nodes()) {
			time += commTimes.time(v, w);
		}

		if (time < minTime) {
			minTime = time;
			*vp0 = v;
		}
	}
}

NetworKit::node WeightedGreedyMapping::pickNextCommNode(NetworKit::node vci) {
	for (auto w: communicationGraph.neighbors(vci)) {
		if (sum_c[w] > -1.0) {
			// w is not assigned
			sum_c[w] += communicationGraph.weight(vci, w);
		}	
	}

	NetworKit::node vci_next = 0;
	double max_val = -1.0;
	for (size_t i = 0; i < sum_c.size(); ++i) {
		if (sum_c[i] > max_val) {
			max_val = sum_c[i];
			vci_next = i;
		}
	}
	return vci_next;
}

NetworKit::node WeightedGreedyMapping::pickNextProcNode(NetworKit::node vci_next,
														bool* nodeAvailable)
{
	NetworKit::node vpi_next = 0;

	*nodeAvailable = false;

    #pragma omp parallel for
	for (NetworKit::node vpi_candidate = 0; vpi_candidate < numberOfNodes; ++vpi_candidate) {
		if (sum_p[vpi_candidate] < MAX_DOUBLE) {
			// j is not assigned
			sum_p[vpi_candidate] = 0;

			for (auto w: communicationGraph.neighbors(vci_next)) {
				if (sum_c[w] < 0) {
					// w has already been assigned
					double weight = communicationGraph.weight(vci_next, w);
					double time = commTimes.time(vpi_candidate, mapping[w]); 

					sum_p[vpi_candidate] += weight * time;
				}
			}

			unsigned int memoryNeeded = cgMemoryMap[vci_next];
			unsigned int memoryAvailable = pgMemoryMap[vpi_candidate];

			if (memoryNeeded > memoryAvailable) {
				// can't use this processor node
				sum_p[vpi_candidate] = MAX_DOUBLE * 0.99; // TODO: fix this insane shit
			} else {
				// fitting processor node exists
				*nodeAvailable = true;

				double rating = computeRating(vci_next, vpi_candidate);
				sum_p[vpi_candidate] = (1 + rating) * sum_p[vpi_candidate];
			}
		}
	}

	double min_val = MAX_DOUBLE;
	for (size_t i = 0; i < sum_p.size(); ++i) {
		if (sum_p[i] < min_val) {
			min_val = sum_p[i];
			vpi_next = i;
		}
	}
	return vpi_next;
}

double WeightedGreedyMapping::computeRating(NetworKit::node vc, NetworKit::node vp) {
	// rating(a, n) = (a-n)^2
	// rating = 0 <=> a == n
	// a - n small => (a - n)^2 --> 0

	auto memoryAvailable = pgMemoryMap[vp];
	auto memoryNeeded = cgMemoryMap[vc];
	
	double rating = std::pow(memoryAvailable - memoryNeeded, 2);
	return rating;
}

NetworKit::node WeightedGreedyMapping::findReplacement(NetworKit::node vci_next) {
	auto memNeeded = cgMemoryMap[vci_next];

	LOG("search a node to place vci_next = " << vci_next << " in");

	for (size_t i = 0; i < mapping.size(); ++i) {
		if (isMapped[i]) {
			auto vp = mapping[i];
			auto memAvailable = pgMemoryMap[vp];

			LOG("test vp = " << vp << " mem needed = " << memNeeded << " mem available = " << memAvailable);
			
			if (memNeeded <= memAvailable) {

				LOG("found a processor node: " << vp);
				
				removeProcNodeMapping(vp);
				return vp;
			}
		}
	}

	throw std::runtime_error("there is not enough space to map node to any processor");
}


void WeightedGreedyMapping::run() { 
	assertRequirements(communicationGraph, processorGraph);
	
	// compute communication time between processor nodes
	commTimes.run();
	
	NetworKit::node vci;
	NetworKit::node vpi;

	// initial mapping
	pickInitialNodes(&vci, &vpi);
	setMapping(vci, vpi);
	
	LOG("Map CommNode " << vci << " -> ProcessorNode " << vpi << " || (INITIAL)");

	while (numberOfMappedNodes < numberOfNodes) {
		// mark nodes as assigned
		sum_c[vci] = -1.0;
		sum_p[vpi] = MAX_DOUBLE;

		// pick the comm node with the heighest communication volume
		// with the already mapped nodes as the next comm node to be mapped
		NetworKit::node vci_next = pickNextCommNode(vci);

		LOG("Next CommNode is vci_next = " << vci_next);
		
		// try to find a processor to fit this node in ..
		bool foundNode = false;
		NetworKit::node vpi_next = pickNextProcNode(vci_next, &foundNode);

		LOG("Next ProcNode is vpi_next = " << vpi_next);
		
		if (!foundNode) {
			// couldn't find a processor node to proceed
			// execute replacement strategy
			vpi_next = findReplacement(vci_next);
		}

		setMapping(vci_next, vpi_next);
		vci = vci_next;
		vpi = vpi_next;
	}

	hasrun = true;
}

void WeightedGreedyMapping::setMapping(NetworKit::node vc, NetworKit::node vp) {
	mapping[vc] = vp;
	invmapping[vp] = vc;
	isMapped[vc] = true;
	invIsMapped[vp] = true;
	++numberOfMappedNodes;
}

void WeightedGreedyMapping::removeCommNodeMapping(NetworKit::node vc) {
	if (isMapped[vc]) {
		auto vp = mapping[vc];
		mapping[vc] = 0;
		invmapping[vp] = 0;
		isMapped[vc] = false;
		invIsMapped[vp] = false;
		--numberOfMappedNodes;
	}
}

void WeightedGreedyMapping::removeProcNodeMapping(NetworKit::node vp) {
	if (invIsMapped[vp]) {
		auto vc = invmapping[vp];
		mapping[vc] = 0;
		invmapping[vp] = 0;
		isMapped[vc] = false;
		invIsMapped[vp] = false;
		--numberOfMappedNodes;
	}
}

std::vector<NetworKit::index> WeightedGreedyMapping::getMapping() const {
	return mapping;
}

CommTime WeightedGreedyMapping::getCommTime() const {
	return commTimes;
}

bool WeightedGreedyMapping::hasRun() const {
	return hasrun;
}

void WeightedGreedyMapping::assertRequirements(const NetworKit::Graph& communicationGraph,
											   const NetworKit::Graph& processorGraph)
{
	StaticMapping::assertRequirements(communicationGraph, processorGraph);
	
	if (cgMemoryMap.size() != communicationGraph.numberOfNodes())
		throw std::runtime_error("each comm node has to have exactly one memory map entry");
	if (pgMemoryMap.size() != processorGraph.numberOfNodes())
		throw std::runtime_error("each proc node has to have exactly one memory map entry");
	if (invmapping.size() != mapping.size())
		throw std::runtime_error("mapping arrays have different lengths");
	if (numberOfNodes != communicationGraph.numberOfNodes() || numberOfNodes != processorGraph.numberOfNodes())
		throw std::runtime_error("numberOfNodes wrongly initialized");
}

} // namespace
