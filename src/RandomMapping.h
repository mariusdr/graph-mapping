#ifndef RANDOM_MAPPING_H
#define RANDOM_MAPPING_H 

#include "StaticMappingAlgorithm.h"

#include <graph/Graph.h>

#include <vector>

namespace StaticMapping {

class RandomMapping: public StaticMapping {
public:
    RandomMapping(const NetworKit::Graph&, const NetworKit::Graph&); 
    void run();
    
	std::vector<NetworKit::index> getMapping() const;

private:
    const NetworKit::Graph& communicationGraph;
    const NetworKit::Graph& processorGraph;
    
	std::vector<NetworKit::index> mapping;
};


} // namespace

#endif
