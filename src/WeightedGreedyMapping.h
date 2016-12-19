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

	const size_t numberOfNodes;
	
	const std::vector<unsigned int>& cgMemoryMap;
	const std::vector<unsigned int>& pgMemoryMap;
	
	CommTime commTimes;

	std::vector<double> sum_c;
	std::vector<double> sum_p;

	std::vector<NetworKit::index> mapping;
	std::vector<NetworKit::index> invmapping;
	
	std::vector<bool> isMapped;

	size_t numberOfMappedNodes;
	
	bool hasrun;

	void setMapping(NetworKit::node, NetworKit::node);

	void removeCommNodeMapping(NetworKit::node);
	void removeProcNodeMapping(NetworKit::node);
	
	void pickInitialNodes(NetworKit::node*, NetworKit::node*);

	NetworKit::node findReplacement(NetworKit::node);
	
	NetworKit::node pickNextCommNode(NetworKit::node);
	NetworKit::node pickNextProcNode(NetworKit::node, bool*);

protected:
	void assertRequirements(const NetworKit::Graph&, const NetworKit::Graph&) override;
};


} // namespace


#endif
