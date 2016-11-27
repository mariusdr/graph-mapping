#include <iostream>
#include <fstream>
#include <random>

#include <graph/Graph.h>
#include <generators/LFRGenerator.h>
#include <generators/RegularRingLatticeGenerator.h>
#include <generators/StaticDegreeSequenceGenerator.h>
#include <generators/PubWebGenerator.h>

#define NOLOG 1
#include "Log.h"
#include "Benchmark.h"
#include "GreedyMapping.h"
#include "StaticMappingAlgorithm.h"
#include "InitialMapping.h"

void benchmark1() {
	NetworKit::Graph communicationGraph = NetworKit::Graph(5, true, false);

	communicationGraph.addEdge(0, 1, 1);
	communicationGraph.addEdge(0, 2, 3);
	communicationGraph.addEdge(0, 3, 2);
	communicationGraph.addEdge(2, 3, 4);
	communicationGraph.addEdge(2, 4, 2);

	NetworKit::Graph processorGraph = NetworKit::Graph(5, true, false);

	processorGraph.addEdge(0, 1, 2);
	processorGraph.addEdge(0, 3, 4);
	processorGraph.addEdge(1, 2, 2);
	processorGraph.addEdge(1, 4, 5);
	processorGraph.addEdge(3, 4, 4);
	processorGraph.addEdge(4, 2, 5);

	StaticMapping::GreedyMapping mappingAlgo(communicationGraph, processorGraph);
	StaticMapping::CommTime commTimes = mappingAlgo.getCommTime();
	StaticMapping::Benchmark<StaticMapping::GreedyMapping> benchmark(communicationGraph, processorGraph, commTimes, 10);

	benchmark.run();

	StaticMapping::write_benchmark_results<>(benchmark, "benchmark1_results.txt");
}


void benchmark2(NetworKit::Graph& communicationGraph, NetworKit::Graph& processorGraph) {
	using StaticMapping::GreedyMapping;
	
	StaticMapping::CommTime commTimes(processorGraph);
	commTimes.run();
	StaticMapping::Benchmark<GreedyMapping> benchmark(communicationGraph, processorGraph, commTimes, 10);

	benchmark.run();
	StaticMapping::write_benchmark_results<>(benchmark, "benchmark2_results.txt");
}

void benchmark3(NetworKit::Graph& communicationGraph, NetworKit::Graph& processorGraph) {
	using StaticMapping::InitialMapping;
	
	StaticMapping::CommTime commTimes(processorGraph);
	commTimes.run();
	StaticMapping::Benchmark<InitialMapping> benchmark(communicationGraph, processorGraph, commTimes, 10);

	benchmark.run();
	StaticMapping::write_benchmark_results<>(benchmark, "benchmark3_results.txt");
}

int main() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(1, 10);

	NetworKit::Graph processorGraph(1024, true);
	{
		NetworKit::RegularRingLatticeGenerator regRingLatticeGen(1024, 16);
		auto temp = regRingLatticeGen.generate();
	
		for (auto edge: temp.edges())
			processorGraph.addEdge(edge.first, edge.second, dis(gen));
	}
	
	NetworKit::Graph communicationGraph(1024, true);
	{
		NetworKit::PubWebGenerator pubWebGen(1024, 32, 16, 8);
		auto temp = pubWebGen.generate();

		for (auto edge: temp.edges())
			communicationGraph.addEdge(edge.first, edge.second, dis(gen));
	}

	benchmark2(communicationGraph, processorGraph);
	benchmark3(communicationGraph, processorGraph);
}
