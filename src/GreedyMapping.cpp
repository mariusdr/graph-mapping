#include "GreedyMapping.h"

namespace StaticMapping {

GreedyMapping::GreedyMapping(const Graph& cg, const Graph& pg): 
    communicationGraph(cg), processorGraph(pg),
    sum_c(std::vector<node>(cg.numberOfNodes())),
    sum_p(std::vector<node>(pg.numberOfNodes()))
{}


void GreedyMapping::run() { 
    if (communicationGraph.numberOfNodes() != processorGraph.numberOfNodes()) 
        throw std::runtime_error("V_C != V_P");
      
}






} // namespace

