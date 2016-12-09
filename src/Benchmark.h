#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "StaticMappingAlgorithm.h"
#include "CommTime.h"

#include <chrono>
#include <algorithm>
#include <functional>
#include <fstream>
#include <vector>
#include <math.h>
#include <ostream>

namespace StaticMapping {

double computeMaximumDilation(const NetworKit::Graph& commGraph, 
							  const std::vector<NetworKit::index>& mapping,
							  const CommTime& commTime);

double computeAverageDilation(const NetworKit::Graph& commGraph, 
							  const std::vector<NetworKit::index>& mapping,
							  const CommTime& commTime);

std::vector<NetworKit::index> inverseMapping(const std::vector<NetworKit::index>&);

double computeMaximumCongestion(const NetworKit::Graph& commGraph,
								const NetworKit::Graph& procGraph,
								const std::vector<NetworKit::index>& mapping);

struct BenchmarkResults {
	double runtime;
	double maxDilation;
	double avgDilation;
	double maxCongestion;
};

class Benchmark {
public:
	Benchmark(const NetworKit::Graph&, const NetworKit::Graph&, StaticMapping&);
	void run();
	BenchmarkResults getResults() const;
private:
	const NetworKit::Graph& communicationGraph;
	const NetworKit::Graph& processorGraph;
	
	StaticMapping& mappingAlgorithm;

	BenchmarkResults results;
};
	
} // namespace

#endif
