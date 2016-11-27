#include "Benchmark.h"

#include <chrono>
#include <algorithm>
#include <functional>

namespace StaticMapping {

double maximumDilation(const NetworKit::Graph& commGraph, 
					   const std::vector<NetworKit::index>& mapping,
					   const CommTime& commTime)
{
	double maxDil = 0;
	for (const auto& commEdge: commGraph.edges()) {
		auto uc = commEdge.first;
		auto vc = commEdge.second;
		auto weight = commGraph.weight(uc, vc);

		auto up = mapping[uc];
		auto vp = mapping[vc];
		auto time = commTime.time(up, vp);

		auto dil = weight * time;
		if (dil > maxDil)
			maxDil = dil;
	}
	return maxDil;
}

double averageDilation(const NetworKit::Graph& commGraph, 
					   const std::vector<NetworKit::index>& mapping,
					   const CommTime& commTime)
{
	double dil = 0;
	for (const auto& commEdge: commGraph.edges()) {
		auto uc = commEdge.first;
		auto vc = commEdge.second;
		auto weight = commGraph.weight(uc, vc);

		auto up = mapping[uc];
		auto vp = mapping[vc];
		auto time = commTime.time(up, vp);

		dil += weight * time;
	}
	dil /= commGraph.numberOfEdges();
	return dil;
}

} // namespace
