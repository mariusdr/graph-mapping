#ifndef GREEDY_MAPPING_TEST_H
#define GREEDY_MAPPING_TEST_H

#include <gtest/gtest.h>

#include <graph/Graph.h>

#include "../GreedyMapping.h"


class GreedyMappingTest1 : public ::testing::Test {
public:
	void SetUp();
	
protected:
	NetworKit::Graph communicationGraph;
	NetworKit::Graph processorGraph;
};

class GreedyMappingTest2 : public ::testing::Test {
public:
	void SetUp();
	
protected:
	NetworKit::Graph cg;
	NetworKit::Graph pg;
};

#endif
