#ifndef GREEDY_MAPPING_H
#define GREEDY_MAPPING_H 

#include "StaticMappingAlgorithm.h"
#include "CommTime.h"

#include <graph/Graph.h>

#include <vector>

namespace StaticMapping {


class GreedyMapping : public StaticMapping {
public: 
    GreedyMapping(const NetworKit::Graph&, const NetworKit::Graph&);
	GreedyMapping(const GreedyMapping&);
	GreedyMapping(GreedyMapping&&) = default;
	GreedyMapping& operator=(const GreedyMapping&) = default;
	GreedyMapping& operator=(GreedyMapping&&) = default;
	~GreedyMapping() = default;
	
	void run();
	
	std::vector<NetworKit::index> getMapping() const;
	CommTime getCommTime() const;

	bool hasRun() const;
	
private:
    const NetworKit::Graph& communicationGraph;
    const NetworKit::Graph& processorGraph;

	CommTime commTimes;
	
    std::vector<double> sum_c;
    std::vector<double> sum_p;

	std::vector<NetworKit::index> mapping;

	bool hasrun;

	void pickInitialNodes(NetworKit::node*, NetworKit::node*);
};

} // namespace
#endif
