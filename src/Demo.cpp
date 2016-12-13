#include "Demo.h"

namespace StaticMapping {
namespace Util {

MappingRunner::MappingRunner(const NetworKit::Graph& cg,
							 const NetworKit::Graph& pg,
							 std::string mappingOutputFilename,
							 std::string benchmarkOutputFilename,
							 bool saveMapping, bool saveBenchmark,
							 bool noBenchmark, bool verbose,
							 bool useInitialMapping, bool useRandomMapping,
							 bool useGreedyMapping):
	cg(cg), pg(pg),
	mappingWriter(MappingWriter(mappingOutputFilename)),
	benchmarkWriter(BenchmarkWriter(benchmarkOutputFilename)),
	saveMapping(saveMapping),
	saveBenchmark(saveBenchmark),
	noBenchmark(noBenchmark),
	verbose(verbose),
	useInitialMapping(useInitialMapping),
	useRandomMapping(useRandomMapping),
	useGreedyMapping(useGreedyMapping)
{}



void MappingRunner::infolog(std::string&& s) {
	if (verbose) {
		std::cout << s << std::endl;
	}
}

void MappingRunner::infolog(const BenchmarkResults& results) {
	if (verbose) {
		std::cout << "[INFO] runtime: " << results.runtime << "ms \n";
		std::cout << "[INFO] average dilation: " << results.avgDilation << "\n";
		std::cout << "[INFO] max dilation: " << results.maxDilation << "\n";
		std::cout << "[INFO] max congestion: " << results.maxCongestion << "\n";
	}
}

void MappingRunner::runAlgorithm(StaticMapping& algo) {
	if (algo.hasRun())
		throw std::runtime_error("cant run algo twice");

	if (noBenchmark) {
		infolog("[INFO]: ... without benchmark");
		algo.run();
	} else {
		infolog("[INFO]: ... with benchmark");
		Benchmark bm(cg, pg, algo);
		bm.run();

		auto results = bm.getResults();

		if (saveBenchmark) {
			infolog("[INFO]: saving benchmark results");
			benchmarkWriter.bufferItem(results, typeid(algo).name());
		} else {
			infolog("[INFO]: discarding benchmark results");
		}
	}

	if (saveMapping) {
		infolog("[INFO]: saving mapping");
		mappingWriter.bufferItem(algo.getMapping());
	} else {
		infolog("[INFO]: discarding mapping");
	}
}	
	
void MappingRunner::run() {
	if (useInitialMapping) {
		infolog("[INFO]: Running InitialMapping");
		InitialMapping algo(cg, pg);
		runAlgorithm(algo);
		infolog("[DONE]");
	}

	if (useRandomMapping) {
		infolog("[INFO]: Running RandomMapping");
		RandomMapping algo(cg, pg);
		runAlgorithm(algo);
		infolog("[DONE]");
	}

	if (useGreedyMapping) {
		infolog("[INFO]: Running GreedyMapping");
		GreedyMapping algo(cg, pg);
		runAlgorithm(algo);
		infolog("[DONE]");
	}
}	
	

}} // namespace
