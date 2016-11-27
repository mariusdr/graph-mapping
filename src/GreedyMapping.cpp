#include "GreedyMapping.h"
#include "Log.h"

#include <algorithm>
#include <assert.h>

#include <graph/SSSP.h>
#include <graph/Dijkstra.h>

namespace StaticMapping {

#define MAX_INT std::numeric_limits<int>::max()
#define MAX_DOUBLE std::numeric_limits<double>::max()

#define PROCESSOR_ASSIGNED MAX_INT
#define COMM_NODE_ASSIGNED -1

GreedyMapping::GreedyMapping(const NetworKit::Graph& cg, const NetworKit::Graph& pg): 
    communicationGraph(cg), processorGraph(pg),
	commTimes(CommTime(pg)),
    sum_c(std::vector<int>(cg.numberOfNodes(), 0)),
    sum_p(std::vector<int>(pg.numberOfNodes(), 1)),
	mapping(std::vector<NetworKit::index>(cg.numberOfNodes(), 0)),
	hasrun(false)
{}

inline NetworKit::edgeweight totalCommVolume(const NetworKit::Graph& communicationGraph, NetworKit::node u) {
	NetworKit::edgeweight total = 0;
	for (auto& v: communicationGraph.neighbors(u))
		total += communicationGraph.weight(u, v);
	return total;
}

inline double totalCommTime(const NetworKit::Graph& processorGraph, const CommTime& commTime, NetworKit::node u) {
	double total = 0;
	for (auto& v: processorGraph.neighbors(u))
		total += commTime.time(u, v);
	return total;
}

inline NetworKit::node initialCommNode(const NetworKit::Graph& communicationGraph) {
	NetworKit::node vc0;
	NetworKit::edgeweight maxCommVolume = 0;
	for (auto& v: communicationGraph.nodes()) {
		auto cv = totalCommVolume(communicationGraph, v);
		if (cv > maxCommVolume) {
			maxCommVolume = cv;
			vc0 = v;
		}
	}
	return vc0;
}

inline NetworKit::node initialProcessorNode(const NetworKit::Graph& processorGraph, const CommTime& commTimes) {
	NetworKit::node vp0;
	double minCommTime = MAX_DOUBLE;
	for (auto& v: processorGraph.nodes()) {
		auto ct = totalCommTime(processorGraph, commTimes, v);
		if (ct < minCommTime) {
			minCommTime = ct;
			vp0 = v;
		}
	}
	return vp0;
}

inline NetworKit::node pickMaxNode(const std::vector<int>& sum) {
	NetworKit::node max_node = 0;
	int max_val = -1;

	for (NetworKit::index i = 0; i < sum.size(); i++) {
		if (sum[i] > max_val) {
			max_val = sum[i];
			max_node = i;
		}
	}
	return max_node;
}

inline NetworKit::node pickMinNode(const std::vector<int>& sum) {
	NetworKit::node min_node = 0;
	int min_val = MAX_INT;

	for (NetworKit::index i = 0; i < sum.size(); i++) {
		if (sum[i] < min_val) {
			min_val = sum[i];
			min_node = i;
		}
	}
	return min_node;
}

void GreedyMapping::run() { 
	assertRequirements(communicationGraph, processorGraph);
	// compute communication time between processor nodes
	commTimes.run();

	NetworKit::node vci = initialCommNode(communicationGraph);
	NetworKit::node vpi = initialProcessorNode(processorGraph, commTimes);

	// initial mapping
	mapping[vci] = vpi;
	
	LOG("Map CommNode " << vci << " -> ProcessorNode " << vpi << " || (INITIAL)");

	for (NetworKit::index i = 0; i < communicationGraph.numberOfNodes() - 1; i++) {
		// mark nodes as assigned
		sum_c[vci] = COMM_NODE_ASSIGNED;
		sum_p[vpi] = PROCESSOR_ASSIGNED;

		for (auto w: communicationGraph.neighbors(vci)) {
			if (sum_c[w] != COMM_NODE_ASSIGNED) {
				// w is not assigned
				sum_c[w] += communicationGraph.weight(vci, w);
			}	
		}
		NetworKit::node vci_next = pickMaxNode(sum_c);

		for (NetworKit::index j = 0; j < processorGraph.numberOfNodes(); j++) {
			if (sum_p[j] != PROCESSOR_ASSIGNED) {
				// j is not assigned
				sum_p[j] = 0;

				for (auto w: communicationGraph.neighbors(vci_next)) {
					if (sum_c[w] < 0) {
						// w has already been assigned
						auto weight = communicationGraph.weight(vci_next, w);
						auto time = commTimes.time(j, mapping[w]); 

						sum_p[j] += weight * time;
					}
				}	
			}
		}
		NetworKit::node vpi_next = pickMinNode(sum_p);
		
		// set mapping 
		mapping[vci_next] = vpi_next;

		LOG("Map CommNode " << vci_next << " -> ProcessorNode " << vpi_next
			<< " || " << "sum_c = " << sum_c[vci_next] << " ; sum_p = " << sum_p[vpi_next]);

		//assert(sum_c[vci_next] == *std::max_element(sum_c.begin(), sum_c.end()));
		//assert(sum_p[vpi_next] == *std::min_element(sum_p.begin(), sum_p.end()));

		vci = vci_next;
		vpi = vpi_next;
	}
	hasrun = true;
}

std::vector<NetworKit::index> GreedyMapping::getMapping() const {
	return mapping;
}

CommTime GreedyMapping::getCommTime() const {
	return commTimes;
}

bool GreedyMapping::hasRun() const {
	return hasrun;
}


} // namespace

