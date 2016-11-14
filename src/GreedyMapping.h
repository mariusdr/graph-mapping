#ifndef GREEDY_MAPPING_H
#define GREEDY_MAPPING_H 

#include "StaticMappingAlgorithm.h"

#include <graph/Graph.h>

#include <vector>

using NetworKit::node;
using NetworKit::Graph;
using NetworKit::index;

namespace StaticMapping {

class GreedyMapping : StaticMapping {
public: 
    GreedyMapping(const Graph&, const Graph&);
    void run();
    

private:
    const Graph& communicationGraph;
    const Graph& processorGraph;
    
    std::vector<node> sum_c;
    std::vector<node> sum_p;
    

};

} // namespace
#endif
