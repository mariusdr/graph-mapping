#ifndef INITIAL_MAPPING_TEST_H
#define INITIAL_MAPPING_TEST_H

#include <gtest/gtest.h>

#include <graph/Graph.h>

#include "../InitialMapping.h"

class InitialMappingTest : public ::testing::Test {
public:
	void SetUp();

	
protected:
	NetworKit::Graph communicationGraph;
	NetworKit::Graph processorGraph;
	
};


#endif
