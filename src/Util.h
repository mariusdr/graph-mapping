#ifndef STATIC_MAPPING_UTIL_H
#define STATIC_MAPPING_UTIL_H

#include <string>

#include <graph/Graph.h>

#include "IOUtil.h"
#include "Benchmark.h"
#include "InitialMapping.h"
#include "RandomMapping.h"
#include "GreedyMapping.h"

namespace StaticMapping {
namespace Util {

class MappingRunner {
public:

	MappingRunner(const NetworKit::Graph& cg,
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

	void infolog(std::string&& s) {
		if (verbose) {
			std::cout << s << std::endl;
		}
	}

	void infolog(const BenchmarkResults& results) {
		if (verbose) {
			std::cout << "[INFO] runtime: " << results.runtime << "ms \n";
			std::cout << "[INFO] average dilation: " << results.avgDilation << "\n";
			std::cout << "[INFO] max dilation: " << results.maxDilation << "\n";
			std::cout << "[INFO] max congestion: " << results.maxCongestion << "\n";
		}
	}
	
	void runAlgorithm(StaticMapping& algo) {
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
	
	void run() {
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
	
	
private:
	const NetworKit::Graph& cg;
	const NetworKit::Graph& pg;

	MappingWriter mappingWriter;
	BenchmarkWriter benchmarkWriter;
	
	bool saveMapping;
	bool saveBenchmark;

	bool noBenchmark;
	bool verbose;

	bool useInitialMapping;
	bool useRandomMapping;
	bool useGreedyMapping;
	
};


}} // namespace



#endif
