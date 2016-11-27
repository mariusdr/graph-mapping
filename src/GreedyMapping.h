#ifndef GREEDY_MAPPING_H
#define GREEDY_MAPPING_H 

#include "StaticMappingAlgorithm.h"
#include "CommTime.h"

#include <graph/Graph.h>

#include <vector>
#include <limits>

namespace StaticMapping {


class GreedyMapping : public StaticMapping {
public: 
    GreedyMapping(const NetworKit::Graph&, const NetworKit::Graph&);
    void run();
	
	std::vector<NetworKit::index> getMapping() const;
	CommTime getCommTime() const;

	bool hasRun() const;
	
private:
    const NetworKit::Graph& communicationGraph;
    const NetworKit::Graph& processorGraph;

	CommTime commTimes;
	
    std::vector<int> sum_c;
    std::vector<int> sum_p;

	std::vector<NetworKit::index> mapping;

	bool hasrun;
};

} // namespace
#endif
