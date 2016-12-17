#include "GreedyMappingTest.h"
#include "../GreedyMapping.h"

#include <graph/Graph.h>

#include <algorithm>


void GreedyMappingTest1::SetUp() {
	communicationGraph = NetworKit::Graph(5, true, false);
	processorGraph = NetworKit::Graph(5, true, false);

	communicationGraph.addEdge(0, 1, 1);
	communicationGraph.addEdge(0, 2, 3);
	communicationGraph.addEdge(0, 3, 2);
	communicationGraph.addEdge(2, 3, 4);
	communicationGraph.addEdge(2, 4, 2);

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
TEST_F(GreedyMappingTest1, noDoubleAssignments) {
	StaticMapping::GreedyMapping mapAlgo(communicationGraph, processorGraph);
	mapAlgo.run();

	auto mapping = mapAlgo.getMapping();
	std::sort(mapping.begin(), mapping.end());

	for (NetworKit::index i = 0; i < mapping.size() - 1; i++)
		ASSERT_NE(mapping[i], mapping[i+1]);
}


void GreedyMappingTest2::SetUp() {
	cg = NetworKit::Graph(4, true, false);
	pg = NetworKit::Graph(4, true, false);

	cg.addEdge(0, 1, 2);
	cg.addEdge(0, 2, 1);
	cg.addEdge(1, 2, 2);
	cg.addEdge(2, 3, 3);

	pg.addEdge(0, 1, 4);
	pg.addEdge(0, 2, 1);
	pg.addEdge(1, 3, 2);
	pg.addEdge(2, 3, 1);

}

TEST_F(GreedyMappingTest2, noDoubleAssignments) {
	StaticMapping::GreedyMapping algo(cg, pg);
	algo.run();

	auto mapping = algo.getMapping();
	std::sort(mapping.begin(), mapping.end());

	for (NetworKit::index i = 0; i < mapping.size() - 1; i++)
		ASSERT_NE(mapping[i], mapping[i+1]);
}

TEST_F(GreedyMappingTest2, CorrectCommTimes) {
	StaticMapping::GreedyMapping algo(cg, pg);
	algo.run();

	StaticMapping::CommTime ct = algo.getCommTime();

	EXPECT_DOUBLE_EQ(ct.time(0, 1), 1.0/4.0);
	EXPECT_DOUBLE_EQ(ct.time(0, 2), 1.0);
	EXPECT_DOUBLE_EQ(ct.time(0, 3), 3.0/4.0);

	EXPECT_DOUBLE_EQ(ct.time(1, 2), 5.0/4.0);
	EXPECT_DOUBLE_EQ(ct.time(1, 3), 1.0/2.0);

	EXPECT_DOUBLE_EQ(ct.time(2, 3), 1.0);
}

TEST_F(GreedyMappingTest2, CorrectMapping) {
	StaticMapping::GreedyMapping algo(cg, pg);
	algo.run();

	auto mapping = algo.getMapping();

	EXPECT_EQ(mapping[0], 2);
	EXPECT_EQ(mapping[1], 3);
	EXPECT_EQ(mapping[2], 0);
	EXPECT_EQ(mapping[3], 1);
}
