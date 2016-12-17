#include "WeightedGreedyMapping.h"

#include <math.h>

namespace StaticMapping {

#define MAX_INT std::numeric_limits<int>::max()
#define MAX_DOUBLE std::numeric_limits<double>::max()

#define PROCESSOR_ASSIGNED MAX_INT
#define COMM_NODE_ASSIGNED -1

WeightedGreedyMapping::WeightedGreedyMapping(const NetworKit::Graph& cg,
											 const NetworKit::Graph& pg,
											 const std::vector<unsigned int>& cgMemMap,
											 const std::vector<unsigned int>& pgMemMap):
	communicationGraph(cg), processorGraph(pg),
	cgMemoryMap(cgMemMap), pgMemoryMap(pgMemMap),
	commTimes(CommTime(pg)),
	sum_c(std::vector<int>(cg.numberOfNodes(), 0)),
	sum_p(std::vector<int>(pg.numberOfNodes(), 1)),
	mapping(std::vector<NetworKit::index>(cg.numberOfNodes(), 0)),
	hasrun(false)
{}

inline double totalCommVolume(const NetworKit::Graph& cg, NetworKit::node u) {
	double total = 0;
	for (auto& v: cg.neighbors(u))
		total += cg.weight(u, v);
	return total;
}

inline double totalCommTime(const NetworKit::Graph& pg, const CommTime& ct,
							NetworKit::node u)
{
	double total = 0;
	for (auto& v: pg.neighbors(u))
		total += ct.time(u, v);
	return total;
}

#define FITTINGPARAM 2


double fittingRating(unsigned int commNodeMemoryRequired,
					 unsigned int commNodeNormParam, 
					 unsigned int procNodeMemoryAvailable,
					 unsigned int procNodeNormParam)
{

	double c = (double) commNodeMemoryRequired / (double) commNodeNormParam;
	double p = (double) procNodeMemoryAvailable / (double) procNodeNormParam;

	double delta = p - c;

	if (delta < 0)
		return -1;
	return std::pow(delta, FITTINGPARAM);
}

void WeightedGreedyMapping::pickInitialNodes(NetworKit::node& vc0, NetworKit::node& vp0) {
	NetworKit::edgeweight maxCommVolume = 0;
	for (auto& v: communicationGraph.nodes()) {
		auto cv = totalCommVolume(communicationGraph, v);
		if (cv > maxCommVolume) {
			maxCommVolume = cv;
			vc0 = v;
		}
	}
	// vc0 is the communication node with maximal communication volume

	unsigned int maxMemRequired = *std::max(cgMemoryMap.begin(), cgMemoryMap.end());
	unsigned int maxMemAvailable = *std::max(pgMemoryMap.begin(), pgMemoryMap.end());
	
	unsigned int memoryRequired = cgMemoryMap[vc0];

	double minRating = MAX_DOUBLE;
	for (auto& v: processorGraph.nodes()) {
		auto ct = totalCommTime(processorGraph, commTimes, v);
		
		unsigned int memoryAvaiable = pgMemoryMap[v];
		double fitting = fittingRating(memoryRequired, maxMemRequired,
									   memoryAvaiable, maxMemAvailable);

		if (fitting <= 0) {
			continue;
		}
			
		double rating = ct * (1 / fitting);

		if (rating < minRating) {
			minRating = rating;
			vp0 = v;
		}
	}
}

void WeightedGreedyMapping::run() {
	assertRequirements(communicationGraph, processorGraph);

	if (cgMemoryMap.size() != communicationGraph.numberOfNodes())
		throw std::runtime_error("cgMemoryMap needs to have one entry for each communication node");

	if (pgMemoryMap.size() != processorGraph.numberOfNodes())
		throw std::runtime_error("pgMemory needs to have one entry for each processor node");

	NetworKit::node vc0 = 0;
	NetworKit::node vp0 = 0;
	pickInitialNodes(vc0, vp0);


}


} // namespace
