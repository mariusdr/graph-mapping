#ifndef RANDOM_MAPPING_H
#define RANDOM_MAPPING_H 

#include "StaticMappingAlgorithm.h"
#include "CommTime.h"

#include <graph/Graph.h>

#include <vector>

namespace StaticMapping {

class RandomMapping: public StaticMapping {
public:
    RandomMapping(const NetworKit::Graph&, const NetworKit::Graph&); 
	RandomMapping(const RandomMapping&);
	RandomMapping(RandomMapping&&) = default;
	RandomMapping& operator=(const RandomMapping&) = default;
	RandomMapping& operator=(RandomMapping&&) = default;
	~RandomMapping() = default;

	void run();
    
	std::vector<NetworKit::index> getMapping() const;
	CommTime getCommTime() const;

	bool hasRun() const;

private:
    const NetworKit::Graph& communicationGraph;
    const NetworKit::Graph& processorGraph;
    
	std::vector<NetworKit::index> mapping;

	bool hasrun;
};

} // namespace

#endif
