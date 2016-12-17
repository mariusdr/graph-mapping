#include "GraphGenerators.h"

#include <cmath>

namespace StaticMapping {
namespace Util {

HyperCubeGenerator::HyperCubeGenerator(unsigned int dim):
	dim(dim), numberOfNodes(std::pow(2, dim))
{}

int hammingDist(unsigned long a, unsigned long b) {
	int dist = 0;

	unsigned long val = a^b;
	while (val) {
		++dist;
		val &= val - 1;
	}
	return dist;
}

NetworKit::Graph HyperCubeGenerator::generate() {
	NetworKit::Graph graph(numberOfNodes, false, false);

	for (auto& x: graph.nodes()) {
		for (auto& y: graph.nodes()) {
			if (hammingDist(x, y) == 1)
				graph.addEdge(x, y);
		}
	}
	return graph;
}


}} // namespace
