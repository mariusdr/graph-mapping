#include "GreedyMappingTest.h"
#include "../GreedyMapping.h"

#include <graph/Graph.h>

#include <algorithm>


void GreedyMappingTest::SetUp() {
	communicationGraph = NetworKit::Graph(5, true, false);

	communicationGraph.addEdge(0, 1, 1);
	communicationGraph.addEdge(0, 2, 3);
	communicationGraph.addEdge(0, 3, 2);
	communicationGraph.addEdge(2, 3, 4);
	communicationGraph.addEdge(2, 4, 2);

	processorGraph = NetworKit::Graph(5, true, false);

	processorGraph.addEdge(0, 1, 2);
	processorGraph.addEdge(0, 3, 4);
	processorGraph.addEdge(1, 2, 2);
	processorGraph.addEdge(1, 4, 5);
	processorGraph.addEdge(3, 4, 4);
	processorGraph.addEdge(4, 2, 5);
	
}

/* 
 * Assert that no two different CommNodes are mapped to the same 
 * Processor
 */
TEST_F(GreedyMappingTest, noDoubleAssignments) {
	StaticMapping::GreedyMapping mapAlgo(communicationGraph, processorGraph);
	mapAlgo.run();

	auto mapping = mapAlgo.getMapping();
	std::sort(mapping.begin(), mapping.end());

	for (NetworKit::index i = 0; i < mapping.size() - 1; i++)
		ASSERT_NE(mapping[i], mapping[i+1]);
}

