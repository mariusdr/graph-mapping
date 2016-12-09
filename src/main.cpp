#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <typeinfo>

#include <graph/Graph.h>
#include <io/METISGraphReader.h>

#include "boost/program_options.hpp"

#include "StaticMappingAlgorithm.h"
#include "RandomMapping.h"
#include "GreedyMapping.h"
#include "InitialMapping.h"
#include "Benchmark.h"


void write_mapping(const std::vector<size_t>& mapping, std::string& path) {
	std::ofstream stream;
	stream.open(path);

	for (size_t i = 0; i < mapping.size(); i++) {
		stream << mapping[i] << "\n";
	}
	stream.close();
}

std::string prettifyAlgoName(std::string oldname) {
	if (oldname == typeid(StaticMapping::GreedyMapping).name()) {
		return "GreedyMapping";
	}
	if (oldname == typeid(StaticMapping::InitialMapping).name()) {
		return "InitialMapping";
	}
	if (oldname == typeid(StaticMapping::RandomMapping).name()) {
		return "RandomMapping";
	}
	return "";
}

void write_benchmark(const StaticMapping::BenchmarkResults& results,
					 std::ofstream& stream, std::string algoName)
{
	stream << "\"Results_" << prettifyAlgoName(algoName) << "\": { \n";
	stream << "    \"runtime\": " << results.runtime << ", \n";
	stream << "    \"avgDilation\": " << results.avgDilation << ", \n";
	stream << "    \"maxDilation\": " << results.maxDilation << ", \n";
	stream << "    \"maxCongestion\": " << results.maxCongestion << " \n";
	stream << "}\n";
}

inline void runAlgo(StaticMapping::StaticMapping& algo,
					const NetworKit::Graph& cg, const NetworKit::Graph& pg,
					bool save_mapping, std::string& mapping_fn,
					bool save_benchmark, std::string& benchmark_fn,
					bool verbose, bool no_benchmark,
					std::ofstream& stream)
{
	if (!no_benchmark) {
		std::cout << "[INFO]: running benchmark of " << typeid(algo).name() << std::endl;
		StaticMapping::Benchmark bm(cg, pg, algo);
		bm.run();

		auto results = bm.getResults();

		if (verbose) {
			std::cout << "[INFO] runtime: " << results.runtime << "ms \n";
			std::cout << "[INFO] average dilation: " << results.avgDilation << "\n";
			std::cout << "[INFO] max dilation: " << results.maxDilation << "\n";
			std::cout << "[INFO] max congestion: " << results.maxCongestion << "\n";
		}

		if (save_benchmark) {
			write_benchmark(results, stream, typeid(algo).name());
		}
	} else {
		std::cout << "[INFO]: running algorithm " << typeid(algo).name() << std::endl;
		
		algo.run();
	}
	std::cout << "[DONE]" << std::endl;
	
	if (save_mapping)
		write_mapping(algo.getMapping(), mapping_fn);
}

int main(int argc, char** argv) {
	namespace po = boost::program_options;

	bool initial_flag = false;
	bool greedy_flag = false;
	bool random_flag = false;
	
	std::string cg_filename("none"), pg_filename("none");
	std::string benchmark_filename("none"), mapping_filename("none");

	bool save_mapping = false;
	bool save_benchmark = false;

	bool no_benchmark = false;
	bool verbose = false;
	
	po::options_description desc("Options");
	desc.add_options()
		("help", "show help menu")
		("cg,c", po::value<std::string>(&cg_filename)->required(),
		 "communication graph file, required")
		("pg,p", po::value<std::string>(&pg_filename)->required(),
		 "processor graph file, required")
		("benchmark_output,b", po::value<std::string>(&benchmark_filename),
		 "output filename for benchmark data")
		("mapping_output,m", po::value<std::string>(&mapping_filename),
		 "output filename for mapping data")
		("initial,i", po::value<bool>(&initial_flag)->implicit_value(true), "switch for initial mapping")
		("greedy,g", po::value<bool>(&greedy_flag)->implicit_value(true), "switch for greedy mapping")
		("random,r", po::value<bool>(&random_flag)->implicit_value(true), "switch for random mapping")
		("verbose", po::value<bool>(&verbose)->implicit_value(true), "show results on stdout")
		("no_bm", po::value<bool>(&no_benchmark)->implicit_value(true), "run algorithms without benchmarking");

	po::variables_map vm;

	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		if (vm.count("help")) {
			std::cout << desc;
			return 0;
		}

		if (vm.count("benchmark_output")) {
			save_benchmark = true;
		}

		if (vm.count("mapping_output")) {
			save_mapping = true;
		}
		
	} catch (po::error& e) {
		std::cerr << "ERROR:" <<  e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (!initial_flag && !greedy_flag && !random_flag) {
		std::cerr << "ERROR: no algorithms in use" << std::endl;
		return 1;
	}

	NetworKit::METISGraphReader reader;
	NetworKit::Graph cg = reader.read(cg_filename);
	NetworKit::Graph pg = reader.read(pg_filename);
	pg.indexEdges();

	std::ofstream stream;
	stream.open(benchmark_filename);
	
	if (initial_flag) {
		StaticMapping::InitialMapping algo(cg, pg);
		runAlgo(algo,
				cg, pg,
				save_mapping, mapping_filename,
				save_benchmark, benchmark_filename,
				verbose, no_benchmark, stream);
	}

	if (random_flag) {
		StaticMapping::RandomMapping algo(cg, pg);
		runAlgo(algo, cg, pg,
				save_mapping, mapping_filename,
				save_benchmark, benchmark_filename,
				verbose, no_benchmark, stream);
	}

	if (greedy_flag) {
		StaticMapping::GreedyMapping algo(cg, pg);
		runAlgo(algo, cg, pg,
				save_mapping, mapping_filename,
				save_benchmark, benchmark_filename,
				verbose, no_benchmark, stream);
	}

	stream.close();
	
	return 0;
}
