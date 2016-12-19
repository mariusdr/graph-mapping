#include "WeightedGreedyMappingTest.h"

void WeightedGreedyMappingTest1::SetUp() {
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


TEST_F(WeightedGreedyMappingTest1, sanityCheckCommTimes) {
	std::vector<unsigned int> cgMemMap {
		1, 1, 1, 1
	};

	std::vector<unsigned int> pgMemMap {
		1, 1, 1, 1
	};

	StaticMapping::WeightedGreedyMapping algo(cg, pg, cgMemMap, pgMemMap);
	algo.run();

	auto ct = algo.getCommTime();

	EXPECT_DOUBLE_EQ(ct.time(0, 1), 1.0/4.0);
	EXPECT_DOUBLE_EQ(ct.time(0, 2), 1.0);
	EXPECT_DOUBLE_EQ(ct.time(0, 3), 3.0/4.0);

	EXPECT_DOUBLE_EQ(ct.time(1, 2), 5.0/4.0);
	EXPECT_DOUBLE_EQ(ct.time(1, 3), 1.0/2.0);

	EXPECT_DOUBLE_EQ(ct.time(2, 3), 1.0);
}

TEST_F(WeightedGreedyMappingTest1, sanityCheckMapping) {
	std::vector<unsigned int> cgMemMap {
		1, 1, 1, 1
	};

	std::vector<unsigned int> pgMemMap {
		1, 1, 1, 1
	};

	StaticMapping::WeightedGreedyMapping algo(cg, pg, cgMemMap, pgMemMap);
	algo.run();

	auto mapping = algo.getMapping();

	EXPECT_EQ(mapping[0], 2);
	EXPECT_EQ(mapping[1], 3);
	EXPECT_EQ(mapping[2], 0);
	EXPECT_EQ(mapping[3], 1);
}

// TEST_F(WeightedGreedyMappingTest1, unsolvableSituation) {
// 	std::vector<unsigned int> cgMemMap {
// 		2, 2, 2, 2
// 	};

// 	std::vector<unsigned int> pgMemMap {
// 		1, 1, 1, 1
// 	};

// 	StaticMapping::WeightedGreedyMapping algo(cg, pg, cgMemMap, pgMemMap);
// 	algo.run();

// 	try {
// 		auto mapping = algo.getMapping();
// 	} catch (const std::runtime_error& e) {
// 		ASSERT_EQ(1, 1);
// 	} 
// }

void WeightedGreedyMappingTest2::SetUp() {
	cg = NetworKit::Graph(4, true, false);
	pg = NetworKit::Graph(4, true, false);

	/*    0 ---- 1
	 *    |      |
	 *    |      |
	 *    2 ---- 3
	 */
	
	cg.addEdge(0, 1, 1.0);
	cg.addEdge(0, 2, 1.0);
	cg.addEdge(1, 3, 1.0);
	cg.addEdge(2, 3, 1.0);

	pg.addEdge(0, 1, 1.0);
	pg.addEdge(0, 2, 1.0);
	pg.addEdge(1, 3, 1.0);
	pg.addEdge(2, 3, 1.0);
}

TEST_F(WeightedGreedyMappingTest2, trivialTest) {
	std::vector<unsigned int> cgMemMap {
		1, 2, 3, 4
	};

	std::vector<unsigned int> pgMemMap {
		4, 3, 2, 1
	};

	StaticMapping::WeightedGreedyMapping algo(cg, pg, cgMemMap, pgMemMap);
	algo.run();

	auto mapping = algo.getMapping();
	
	EXPECT_EQ(mapping[0], 3);
	EXPECT_EQ(mapping[1], 2);
	EXPECT_EQ(mapping[2], 1);
	EXPECT_EQ(mapping[3], 0);
}
