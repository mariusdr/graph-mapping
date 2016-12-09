#ifndef HOPBYTEMAPPING_H
#define HOPBYTEMAPPING_H

#include "StaticMappingAlgorithm.h"
#include "CommTime.h"

#include <graph/Graph.h>
#include <graph/APSP.h>

#include <vector>
#include <limits>

namespace StaticMapping {

class HopByteMapping : public StaticMapping {
public: 
	HopByteMapping(const NetworKit::Graph&, const NetworKit::Graph&);
	HopByteMapping(const HopByteMapping&) = default;
	HopByteMapping(HopByteMapping&&) = default;

	HopByteMapping& operator=(const HopByteMapping&) = default;
	HopByteMapping& operator=(HopByteMapping&&) = default;

	~HopByteMapping() = default;
	
	void run();
	
	std::vector<NetworKit::index> getMapping() const;
	CommTime getCommTime() const;

	bool hasRun() const;
	
private:
    const NetworKit::Graph& communicationGraph;
    const NetworKit::Graph& processorGraph;

	CommTime commTimes;

	NetworKit::APSP apsp;
	
	std::vector<NetworKit::index> mapping;
	bool hasrun;

	std::vector<bool> cNodeIsMapped;
	std::vector<bool> pNodeIsMapped;
	
	double estimation(NetworKit::node c, NetworKit::node p);

};

} // namespace
#endif
