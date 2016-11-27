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

namespace StaticMapping {

double maximumDilation(const NetworKit::Graph& commGraph, 
					   const std::vector<NetworKit::index>& mapping,
					   const CommTime& commTime);

double averageDilation(const NetworKit::Graph& commGraph, 
					   const std::vector<NetworKit::index>& mapping,
					   const CommTime& commTime);

template <class StaticMappingAlgo> 
class Benchmark {
public:
	Benchmark(NetworKit::Graph& cg, NetworKit::Graph& pg,
			  CommTime& ct,
			  unsigned int numberOfRuns):
		commGraph(cg), processorGraph(pg),
		commTime(ct), numberOfRuns(numberOfRuns),
		runtime(std::vector<double>(numberOfRuns, 0)),
		maxDilation(std::vector<double>(numberOfRuns, 0)),
		avgDilation(std::vector<double>(numberOfRuns, 0))
	{}
	
	void run() {
		using ms = std::chrono::milliseconds;
		using std::chrono::system_clock;
		using std::chrono::duration_cast;

		if (!commTime.hasRun())
			commTime.run();

		for (unsigned int run = 0; run < numberOfRuns; run++) {
			// reset / set up mapping Algo
			StaticMappingAlgo mappingAlgo = StaticMappingAlgo(commGraph, processorGraph);

			// execute and measure runtime
			auto startTime = system_clock::now();
			mappingAlgo.run();
			auto endTime = system_clock::now();

			auto duration = duration_cast<ms>(endTime - startTime);

			auto mapping = mappingAlgo.getMapping();
			runtime[run] = duration.count();
			maxDilation[run] = maximumDilation(commGraph, mapping, commTime);
			avgDilation[run] = averageDilation(commGraph, mapping, commTime);
		}
	}

	double getRuntimeMax() const { return *std::max_element(runtime.begin(), runtime.end()); }
	double getRuntimeMin() const { return *std::min_element(runtime.begin(), runtime.end()); }
	
	double getMaxDilationMax() const { return *std::max_element(maxDilation.begin(), maxDilation.end()); }
	double getMaxDilationMin() const { return *std::min_element(maxDilation.begin(), maxDilation.end()); }
	
	double getAvgDilationMax() const { return *std::max_element(avgDilation.begin(), avgDilation.end()); }
	double getAvgDilationMin() const { return *std::min_element(avgDilation.begin(), avgDilation.end()); }

	double getRuntimeArithMean() const {
		double sum = 0;
		for (auto x: runtime) sum += x;
		return sum / numberOfRuns;
	}

	double getMaxDilationArithMean() const {
		double sum = 0;
		for (auto x: maxDilation) sum += x;
		return sum / numberOfRuns;
	}

	double getAvgDilationArtihMean() const {
		double sum = 0;
		for (auto x: avgDilation) sum += x;
		return sum / numberOfRuns;
	}

	double getRuntimeGeomMean() const {
		double prod = 1;
		for (auto x: runtime) prod *= x;
		return std::pow(prod, 1.0 / numberOfRuns);
	}

	double getMaxDilationGeomMean() const {
		double prod = 1;
		for (auto x: maxDilation) prod *= x;
		return std::pow(prod, 1.0 / numberOfRuns);
	}
	
	double getAvgDilationGeomMean() const {
		double prod = 1;
		for (auto x: avgDilation) prod *= x;
		return std::pow(prod, 1.0 / numberOfRuns);
	}

private:
	NetworKit::Graph& commGraph;
	NetworKit::Graph& processorGraph;
	CommTime& commTime;

	unsigned int numberOfRuns;

	std::vector<double> runtime;
	std::vector<double> maxDilation;
	std::vector<double> avgDilation;
};



template <class T>
void write_benchmark_results(Benchmark<T> benchmark, const char* filename) {
	std::ofstream results_fs;
	results_fs.open(filename);
	results_fs << filename << std::endl
		       << "---------------------------------" << std::endl
		       << "runtime max:  " << benchmark.getRuntimeMax() << " ms" << std::endl
		       << "runtime min:  " << benchmark.getRuntimeMin() << " ms" << std::endl
		       << "runtime arith. mean: " << benchmark.getRuntimeArithMean()<< " ms" << std::endl
		       << "runtime geom. mean:  " << benchmark.getRuntimeGeomMean()<< " ms" << std::endl
		       << std::endl
		       << "maxdilation max:  " << benchmark.getMaxDilationMax() << std::endl
		       << "maxdilation min:  " << benchmark.getMaxDilationMin() << std::endl
		       << "maxdilation arith. mean: " << benchmark.getMaxDilationArithMean() << std::endl
		       << "maxdilation geom. mean:  " << benchmark.getMaxDilationGeomMean() << std::endl
		       << std::endl
		       << "avgdilation max:  " << benchmark.getAvgDilationMax() << std::endl
		       << "avgdilation min:  " << benchmark.getAvgDilationMin() << std::endl
		       << "avgdilation arith. mean: " << benchmark.getAvgDilationArtihMean() << std::endl
		       << "avgdilation geom. mean:  " << benchmark.getAvgDilationGeomMean() << std::endl
		       << "---------------------------------" << std::endl;
	results_fs.close();
}

} // namespace
#endif
