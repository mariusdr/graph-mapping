#include "HopByteMapping.h"

#include <limits>

namespace StaticMapping {

HopByteMapping::HopByteMapping(const NetworKit::Graph& cg,
							   const NetworKit::Graph& pg):
	communicationGraph(cg), processorGraph(pg),
	commTimes(CommTime(cg)), apsp(NetworKit::APSP(pg)),
	mapping(std::vector<NetworKit::index>(cg.numberOfNodes())),
	hasrun(false),
	cNodeIsMapped(std::vector<bool>(cg.numberOfNodes(), false)),
	pNodeIsMapped(std::vector<bool>(pg.numberOfNodes(), false))
{}

double HopByteMapping::estimation(NetworKit::node commNode,
								  NetworKit::node procNode)
{
	double est = 0.0;

	for (auto v: communicationGraph.nodes()) {
		if (!cNodeIsMapped[v]) {
			auto commVol = communicationGraph.weight(commNode, v);
			auto pv = mapping[v];
			auto dist = apsp.getDistance(procNode, pv);

			est += commVol * dist;
		}
	}
	return est;
}

void HopByteMapping::run() {
	apsp.run();
	
	double maxCriticality = -1*std::numeric_limits<double>::infinity();

	for (size_t k = 0; k < communicationGraph.numberOfNodes(); ++k) {
		double pEst = 0.0;
		for (auto p: processorGraph.nodes()) {
			if (!pNodeIsMapped[p]) {
				pEst += estimation(k, p);
			}
		}
	}
}

} // namespace



