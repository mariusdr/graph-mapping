#ifndef STATIC_MAPPING_ALGORITHM_H
#define STATIC_MAPPING_ALGORITHM_H 

#include <vector>

#include <graph/Graph.h>
#include <Globals.h>

#include <iostream>

#include "CommTime.h"

namespace StaticMapping {


class StaticMapping {
public:
    virtual void run() = 0; 
    virtual std::vector<NetworKit::index> getMapping() const = 0;

	virtual CommTime getCommTime() const = 0;

	virtual bool hasRun() const = 0;

protected:

	virtual void assertRequirements(const NetworKit::Graph& cg, const NetworKit::Graph& pg) {
		if (cg.numberOfNodes() != pg.numberOfNodes()) 
			throw std::runtime_error("#nodes of processorGraph != #nodes of communicationGraph");
		if (cg.isDirected() || pg.isDirected()) 
			throw std::runtime_error("graphs need to be undirected");
		if (!cg.isWeighted() || !pg.isWeighted())
			throw std::runtime_error("graphs need to be weigthed");
	}
};


} // namespace
#endif
