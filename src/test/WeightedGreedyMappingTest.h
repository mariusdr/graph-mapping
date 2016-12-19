#ifndef WEIGHTED_GREEDY_MAPPING_TEST_H
#define WEIGHTED_GREEDY_MAPPING_TEST_H

#include <gtest/gtest.h>

#include <graph/Graph.h>

#include "../GreedyMapping.h"
#include "../WeightedGreedyMapping.h"

class WeightedGreedyMappingTest1 : public ::testing::Test {
public:
	void SetUp();
	
protected:
	NetworKit::Graph cg;
	NetworKit::Graph pg;
};

class WeightedGreedyMappingTest2 : public ::testing::Test {
public:
	void SetUp();
	
protected:
	NetworKit::Graph cg;
	NetworKit::Graph pg;
};

#endif
