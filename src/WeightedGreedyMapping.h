#ifndef WEIGHTED_GREEDY_MAPPING_H
#define WEIGHTED_GREEDY_MAPPING_H

#include "StaticMappingAlgorithm.h"
#include "CommTime.h"

#include <graph/Graph.h>

#include <vector>

namespace StaticMapping {

class WeightedGreedyMapping : public StaticMapping {
public:
	WeightedGreedyMapping(const NetworKit::Graph& cg,
						  const NetworKit::Graph& pg,
						  const std::vector<unsigned int>& cgMemoryMap,
						  const std::vector<unsigned int>& pgMemoryMap);

	WeightedGreedyMapping(const WeightedGreedyMapping&) = default;
	WeightedGreedyMapping(WeightedGreedyMapping&&) = default;

	WeightedGreedyMapping& operator=(const WeightedGreedyMapping&) = default;
	WeightedGreedyMapping& operator=(WeightedGreedyMapping&&) = default;
	
	~WeightedGreedyMapping() = default;

	void run();
	
	std::vector<NetworKit::index> getMapping() const;
	CommTime getCommTime() const;
	bool hasRun() const;
	
private:
	const NetworKit::Graph& communicationGraph;
	const NetworKit::Graph& processorGraph;

	const std::vector<unsigned int>& cgMemoryMap;
	const std::vector<unsigned int>& pgMemoryMap;
	
	CommTime commTimes;

	std::vector<int> sum_c;
	std::vector<int> sum_p;

	std::vector<NetworKit::index> mapping;

	bool hasrun;

	void pickInitialNodes(NetworKit::node&, NetworKit::node&);

};






} // namespace


#endif
