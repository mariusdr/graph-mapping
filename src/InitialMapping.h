#ifndef INITIAL_MAPPING_H
#define INITIAL_MAPPING_H 

#include "StaticMappingAlgorithm.h" 
#include "CommTime.h"

#include <graph/Graph.h>

namespace StaticMapping {

class InitialMapping : public StaticMapping {
 public: 
	InitialMapping(const NetworKit::Graph&, const NetworKit::Graph&);
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
