#include <iostream>
#include <string>

#include <graph/Graph.h>
#include <io/METISGraphReader.h>

#include "boost/program_options.hpp"

#include "GreedyMapping.h"
#include "RandomMapping.h"
#include "InitialMapping.h"

#include "IOUtil.h"

namespace { 

namespace po = boost::program_options;

/**  
 *  simple helper class to parse commandline options
 */
class Options {
public:
	explicit Options() = default;
	Options(const Options&) = delete;
	Options(Options&&) = delete;
	~Options() = default;
	
	bool parse(int argc, char** argv) {
		po::options_description desc("General Options");
		desc.add_options()
			("help", "show help menu")
			("cg,c", po::value<std::string>(&cgFilename)->required(),
			 "communication graph file, required")
			("pg,p", po::value<std::string>(&pgFilename)->required(),
			 "processor graph file, required")
			("benchmark_output,b", po::value<std::string>(&benchmarkOutputFilename),
			 "output filename for benchmark data")
			("mapping_output,m", po::value<std::string>(&mappingOutputFilename),
			 "output filename for mapping data")
			("initial,i", po::value<bool>(&initialFlag)->implicit_value(true), "switch for initial mapping")
			("greedy,g", po::value<bool>(&greedyFlag)->implicit_value(true), "switch for greedy mapping")
			("random,r", po::value<bool>(&randomFlag)->implicit_value(true), "switch for random mapping")
			("verbose", po::value<bool>(&verbose)->implicit_value(true), "show results on stdout")
			("no_bm", po::value<bool>(&noBenchmark)->implicit_value(true), "run algorithms without benchmarking");

		po::variables_map vm;
		try {
			po::store(po::parse_command_line(argc, argv, desc), vm);
			po::notify(vm);

			if (vm.count("help")) {
				std::cout << desc;
				return 0;
			}

			if (vm.count("benchmark_output")) {
				saveBenchmark = true;
			}

			if (vm.count("mapping_output")) {
				saveMapping = true;
			}
		
		} catch (po::error& e) {
			std::cerr << "[ERROR]:" <<  e.what() << std::endl;
			std::cerr << desc << std::endl;
			return false;
		}

		return true;
	}
	
	// filenames
	
	std::string cgFilename;
	std::string pgFilename;
	std::string benchmarkOutputFilename;
	std::string mappingOutputFilename;

	// algorithm flags
	bool initialFlag;
	bool greedyFlag;
	bool randomFlag;

	// config flags
	bool noBenchmark;
	bool saveBenchmark;
	bool saveMapping;
	bool verbose;
};

/** helper class that holds application state 
 */
class App {
public:
	explicit App(const Options& _options, const NetworKit::Graph& _cg,
				 const NetworKit::Graph& _pg):
		options(_options),
		mappingWriter(_options.mappingOutputFilename),
		benchmarkWriter(_options.benchmarkOutputFilename),
		cg(_cg), pg(_pg)
	{}

	~App() = default;
	App(const App&) = delete;
	App(App&&) = delete;
	
	void infolog(std::string&& s) {
		if (options.verbose) {
			std::cout << s << std::endl;
		}
	}

	void infolog(const StaticMapping::BenchmarkResults& results) {
		if (options.verbose) {
			std::cout << "[INFO] runtime: " << results.runtime << "ms \n";
			std::cout << "[INFO] average dilation: " << results.avgDilation << "\n";
			std::cout << "[INFO] max dilation: " << results.maxDilation << "\n";
			std::cout << "[INFO] max congestion: " << results.maxCongestion << "\n";
		}
	}

	void runAlgorithm(StaticMapping::StaticMapping& algo) {
		if (algo.hasRun())
			throw std::runtime_error("can't run algo twice");

		if (options.noBenchmark) {
			infolog("[INFO]: ... without benchmark");
			algo.run();
		} else {
			infolog("[INFO]: ... with benchmark");
			StaticMapping::Benchmark bm(cg, pg, algo);
			bm.run();

			auto results = bm.getResults();

			if (options.saveBenchmark) {
				infolog("[INFO]: saving benchmark results");
				benchmarkWriter.bufferItem(results, typeid(algo).name());
			} else {
				infolog("[INFO]: discarding benchmark results");
			}
		}

		if (options.saveMapping) {
			infolog("[INFO]: saving mapping");
			mappingWriter.bufferItem(algo.getMapping());
		} else {
			infolog("[INFO]: discarding mapping");
		}
	}	

	bool run() {
		if (options.verbose) {
			std::cout << "[INFO]: Comm Graph #nodes: " << cg.numberOfNodes()
					  << "  mem used: " << sizeof(NetworKit::node) * cg.numberOfNodes() << " byte \n"
					  << "[INFO]: Comm Graph #edges: " << cg.numberOfEdges()
					  << "  mem used: " << sizeof(cg.edges()[0]) * cg.numberOfEdges() << " byte \n";

			std::cout << "[INFO]: Proc Graph #nodes: " << pg.numberOfNodes()
					  << "  mem used: " << sizeof(NetworKit::node) * pg.numberOfNodes() << " byte \n"
					  << "[INFO]: Proc Graph #edges: " << cg.numberOfEdges()
					  << "  mem used: " << sizeof(pg.edges()[0]) * pg.numberOfEdges() << " byte \n";
		}
	
		if (!options.initialFlag && !options.randomFlag && !options.greedyFlag) {
			std::cerr << "[ERROR]: no algorithms in use" << std::endl;
			return false;
		}

		if (options.initialFlag) {
			infolog("[INFO]: Running InitialMapping");
			StaticMapping::InitialMapping algo(cg, pg);
			runAlgorithm(algo);
			infolog("[DONE]");
		}

		if (options.randomFlag) {
			infolog("[INFO]: Running RandomMapping");
			StaticMapping::RandomMapping algo(cg, pg);
			runAlgorithm(algo);
			infolog("[DONE]");
		}

		if (options.greedyFlag) {
			infolog("[INFO]: Running GreedyMapping");
			StaticMapping::GreedyMapping algo(cg, pg);
			runAlgorithm(algo);
			infolog("[DONE]");
		}

		if (options.saveMapping)
			mappingWriter.write();

		if (options.saveBenchmark)
			benchmarkWriter.write();

		return true;
	}	
private:
	const Options& options;
	
	StaticMapping::Util::MappingWriter mappingWriter;
	StaticMapping::Util::BenchmarkWriter benchmarkWriter;

	const NetworKit::Graph& cg;
	const NetworKit::Graph& pg;
};

} // namespace

#include <cstdio.h>

int main(int argc, char** argv) {
	Options options;
	options.parse(argc, argv);

	NetworKit::METISGraphReader reader;
	NetworKit::Graph cg = reader.read(options.cgFilename);
	NetworKit::Graph pg = reader.read(options.pgFilename);
	pg.indexEdges();

	App app(options, cg, pg);
	app.run();

	
	return 0;
}

