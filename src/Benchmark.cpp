#include "Benchmark.h"

#include <graph/Dijkstra.h>

#include <chrono>
#include <algorithm>
#include <functional>

#ifndef PARALLELBENCHMARK
#define PARALLELBENCHMARK 1
#endif

namespace StaticMapping {

double computeMaximumDilation(const NetworKit::Graph& commGraph, 
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

double computeAverageDilation(const NetworKit::Graph& commGraph, 
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

std::vector<NetworKit::index> inverseMapping(const std::vector<NetworKit::index>& mapping) {
	std::vector<NetworKit::index> inverse(mapping.size());

	for (size_t cn = 0; cn < mapping.size(); cn++) {
		auto pn = mapping[cn];
		inverse[pn] = cn;
	}
	return inverse;
}


double computeMaximumCongestion(const NetworKit::Graph& commGraph,
								const NetworKit::Graph& procGraph,
								const std::vector<NetworKit::index>& mapping)
{
	std::vector<double> congestion(procGraph.numberOfEdges(), 0.0);

	auto commEdges(commGraph.edges());
	auto procEdges(procGraph.edges());
	
#if PARALLELBENCHMARK
	#pragma omp parallel for
#endif
	for (auto it = commEdges.begin(); it < commEdges.end(); ++it) {
		auto cEdge = *it;
		auto w = commGraph.weight(cEdge.first, cEdge.second);
		auto px = mapping[cEdge.first];
		auto py = mapping[cEdge.second];

		// assume that the communication along cEdge
		// is routed through the shortest path between
		// processor nodes px and py 

		NetworKit::Dijkstra dijk(procGraph, px, true, false, py);
		dijk.run();

		// get one shortest path from px to py 
		auto shortest_path = dijk.getPath(py, true);

		//assert(shortest_path[0] == px);

		for (size_t i = 1; i < shortest_path.size(); ++i) {
			auto pEdgeId = procGraph.edgeId(px, shortest_path[i]);

#if PARALLELBENCHMARK
			#pragma omp atomic
#endif
			congestion[pEdgeId] += w;

			px = shortest_path[i];
		}
	}

#if PARALLELBENCHMARK
	#pragma omp parallel for
#endif
	for (auto it = procEdges.begin(); it < procEdges.end(); ++it) {
		auto pEdge = *it;
		auto bandwith = procGraph.weight(pEdge.first, pEdge.second);
		auto pEdgeId = procGraph.edgeId(pEdge.first, pEdge.second);
		congestion[pEdgeId] /= bandwith;
	}

	return *std::max_element(congestion.begin(), congestion.end());
}


Benchmark::Benchmark(const NetworKit::Graph& cg, const NetworKit::Graph& pg,
					 StaticMapping& ma):
	communicationGraph(cg), processorGraph(pg),
	mappingAlgorithm(ma)
{}

void Benchmark::run() {
	using ms = std::chrono::milliseconds;
	using std::chrono::steady_clock;
	using std::chrono::duration_cast;

	if (mappingAlgorithm.hasRun())
		throw std::runtime_error("mapping algorithm has already run");

	auto startTime = steady_clock::now();
	mappingAlgorithm.run();
	auto endTime = steady_clock::now();
	auto duration = duration_cast<ms>(endTime - startTime);

	results.runtime = duration.count();
	
	CommTime ct = mappingAlgorithm.getCommTime();
	if (!ct.hasRun())
		throw std::runtime_error("communication time object hasn't run yet");

		
	results.maxDilation = computeMaximumDilation(communicationGraph,
												 mappingAlgorithm.getMapping(), ct);
	results.avgDilation = computeAverageDilation(communicationGraph,
												 mappingAlgorithm.getMapping(), ct);
	results.maxCongestion = computeMaximumCongestion(communicationGraph, processorGraph,
													 mappingAlgorithm.getMapping());
}

BenchmarkResults Benchmark::getResults() const {
	return results;
}

} // namespace
