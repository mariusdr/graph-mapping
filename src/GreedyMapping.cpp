#include "GreedyMapping.h"

#include <algorithm>
#include <assert.h>
#include <limits>

#include <graph/SSSP.h>
#include <graph/Dijkstra.h>

#define NOLOG 1
#include "Log.h"

namespace StaticMapping {

#define MAX_INT std::numeric_limits<int>::max()
#define MAX_DOUBLE std::numeric_limits<double>::max()

#define PROCESSOR_ASSIGNED MAX_DOUBLE
#define COMM_NODE_ASSIGNED -1

GreedyMapping::GreedyMapping(const NetworKit::Graph& cg, const NetworKit::Graph& pg): 
	communicationGraph(cg), processorGraph(pg),
	commTimes(CommTime(pg)),
	sum_c(std::vector<double>(cg.numberOfNodes(), 0.0)),
	sum_p(std::vector<double>(pg.numberOfNodes(), 0.0)),
	mapping(std::vector<NetworKit::index>(cg.numberOfNodes(), 0)),
	hasrun(false)
{}

GreedyMapping::GreedyMapping(const GreedyMapping& rhs):
	communicationGraph(rhs.communicationGraph), processorGraph(rhs.processorGraph),
	commTimes(rhs.commTimes),
	sum_c(rhs.sum_c),
	sum_p(rhs.sum_p),
	mapping(rhs.mapping),
	hasrun(rhs.hasrun)
{}

void GreedyMapping::pickInitialNodes(NetworKit::node* vc0, NetworKit::node* vp0) {
	double maxCommvol = -1.0;
	for (auto v: communicationGraph.nodes()) {
		double commvol = 0.0;
		for (auto w: communicationGraph.neighbors(v)) {
			commvol += communicationGraph.weight(v, w);
		}

		LOG("CommNode " << v << " has total communication volume " << commvol);
		
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

		LOG("ProcNode " << v << " has total communication time " << time);

		if (time < minTime) {
			minTime = time;
			*vp0 = v;
		}
	}

	LOG("Choose vc0 = " << *vc0 << " and vp0 = " << *vp0);
}

inline NetworKit::node pickMaxNode(const std::vector<double>& sum) {
	NetworKit::node max_node = 0;
	int max_val = -1.0;

	for (size_t i = 0; i < sum.size(); i++) {
		if (sum[i] > max_val) {
			max_val = sum[i];
			max_node = i;
		}
	}
	return max_node;
}

inline NetworKit::node pickMinNode(const std::vector<double>& sum) {
	NetworKit::node min_node = 0;
	int min_val = MAX_DOUBLE;

	for (size_t i = 0; i < sum.size(); i++) {
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
	
	NetworKit::node vci;
	NetworKit::node vpi;

	pickInitialNodes(&vci, &vpi);
	
	// initial mapping
	mapping[vci] = vpi;
	
	LOG("Map CommNode " << vci << " -> ProcessorNode " << vpi << " || (INITIAL)");

	for (size_t i = 0; i < communicationGraph.numberOfNodes() - 1; ++i) {
		// mark nodes as assigned
		sum_c[vci] = -1.0;
		sum_p[vpi] = MAX_DOUBLE;

		for (auto w: communicationGraph.neighbors(vci)) {
			if (sum_c[w] > -1.0) {
				// w is not assigned
				sum_c[w] += communicationGraph.weight(vci, w);
			}	
		}
		NetworKit::node vci_next = pickMaxNode(sum_c);

		#pragma omp parallel for
		for (size_t j = 0; j < processorGraph.numberOfNodes(); ++j) {
			if (sum_p[j] < MAX_DOUBLE) {
				// j is not assigned
				sum_p[j] = 0;

				for (auto w: communicationGraph.neighbors(vci_next)) {
					if (sum_c[w] < 0) {
						// w has already been assigned
						double weight = communicationGraph.weight(vci_next, w);
						double time = commTimes.time(j, mapping[w]); 

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
