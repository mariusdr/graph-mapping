#ifndef COMM_TIME_TEST_H
#define COMM_TIME_TEST_H

#include <gtest/gtest.h>

#include <graph/Graph.h>

#include "../CommTime.h"

class ModDijsktraTest : public ::testing::Test {
protected:
	virtual void SetUp();
public:
	NetworKit::Graph graph;
};

class ModAPSPTest : public ::testing::Test {
protected:
	virtual void SetUp();
public:
	NetworKit::Graph graph;
};


#endif


