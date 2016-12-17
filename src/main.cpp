#include <iostream>
#include <string>

#include <graph/Graph.h>
#include <io/METISGraphReader.h>

#include "boost/program_options.hpp"

#include "Demo.h"

int main(int argc, char** argv) {
	namespace po = boost::program_options;

	bool initial_flag = false;
	bool greedy_flag = false;
	bool random_flag = false;
	
	std::string cg_filename("none"), pg_filename("none");
	std::string benchmark_filename("benchmark.txt"), mapping_filename("mapping.txt");

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
		std::cerr << "[ERROR]:" <<  e.what() << std::endl;
		std::cerr << desc << std::endl;
		return 1;
	}

	if (!initial_flag && !greedy_flag && !random_flag) {
		std::cerr << "[ERROR]: no algorithms in use" << std::endl;
		return 1;
	}

	NetworKit::METISGraphReader reader;
	NetworKit::Graph cg = reader.read(cg_filename);
	NetworKit::Graph pg = reader.read(pg_filename);
	pg.indexEdges();

	StaticMapping::Util::MappingRunner
		runner(cg, pg,
			   benchmark_filename, mapping_filename,
			   save_mapping, save_benchmark,
			   no_benchmark, verbose,
			   initial_flag, random_flag, greedy_flag);

	runner.run();
	
	return 0;
}
