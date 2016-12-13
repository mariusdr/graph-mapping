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
	MappingRunner() = delete;
	MappingRunner(const MappingRunner&) = delete;
	MappingRunner(MappingRunner&&) = delete;
	
	MappingRunner(const NetworKit::Graph& cg,
				  const NetworKit::Graph& pg,
				  std::string mappingOutputFilename,
				  std::string benchmarkOutputFilename,
				  bool saveMapping, bool saveBenchmark,
				  bool noBenchmark, bool verbose,
				  bool useInitialMapping, bool useRandomMapping,
				  bool useGreedyMapping);

	void infolog(std::string&& s);
	void infolog(const BenchmarkResults& results);
	void runAlgorithm(StaticMapping& algo);
	
	void run();
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
