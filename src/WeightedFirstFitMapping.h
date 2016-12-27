#ifndef WEIGHTED_FIRST_FIT_MAPPING_H
#define WEIGHTED_FIRST_FIT_MAPPING_H

#include <graph/Graph.h>

#include "StaticMappingAlgorithm.h"
#include "CommTime.h"

namespace StaticMapping {

class WeightedFirstFitMapping : public StaticMapping {
public:
	WeightedFirstFitMapping(const NetworKit::Graph& cg,
							const NetworKit::Graph& pg,
							const std::vector<unsigned int>& cgMemoryMap,
							const std::vector<unsigned int>& pgMemoryMap);

	WeightedFirstFitMapping(const WeightedFirstFitMapping&) = default;
	WeightedFirstFitMapping(WeightedFirstFitMapping&&) = default;

	WeightedFirstFitMapping& operator=(const WeightedFirstFitMapping&) = default;
	WeightedFirstFitMapping& operator=(WeightedFirstFitMapping&&) = default;
	
	~WeightedFirstFitMapping() = default;

	void run() override;
	
	std::vector<NetworKit::index> getMapping() const override;
	CommTime getCommTime() const override;
	bool hasRun() const override;
	
private:
	const NetworKit::Graph& communicationGraph;
	const NetworKit::Graph& processorGraph;

	const size_t numberOfNodes;
	
	const std::vector<unsigned int>& cgMemoryMap;
	const std::vector<unsigned int>& pgMemoryMap;
	
	CommTime commTimes;

	std::vector<NetworKit::index> mapping;
	std::vector<NetworKit::index> invmapping;
	
	std::vector<bool> isMapped;
	std::vector<bool> invIsMapped;
	
	size_t numberOfMappedNodes;
	
	bool hasrun;

	void setMapping(NetworKit::node, NetworKit::node);

	void removeCommNodeMapping(NetworKit::node);
	void removeProcNodeMapping(NetworKit::node);
	
	NetworKit::node findReplacement(NetworKit::node);
};


} // namespace
#endif
