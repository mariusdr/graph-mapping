#include "CommTimeTest.h"

#include <graph/Dijkstra.h>
#include <graph/APSP.h>
#include <generators/RegularRingLatticeGenerator.h>

void ModDijsktraTest::SetUp() {
	graph = NetworKit::Graph(5, true, true);

	graph.addEdge(0, 1, 2.0);
	graph.addEdge(0, 2, 8.0);
	graph.addEdge(0, 3, 5.0);
	graph.addEdge(1, 2, 1.0);
	graph.addEdge(2, 4, 3.0);
	graph.addEdge(3, 4, 4.0);
}

TEST_F(ModDijsktraTest, sanityCheck) {
	NetworKit::Dijkstra dijk(graph, 0);
	dijk.run();

	StaticMapping::ModDijsktra mdijk(graph, 0, [](auto ew) {return ew;});
	mdijk.run();

	for (NetworKit::index i = 0; i < graph.numberOfNodes(); i++) {
		auto d0 = dijk.distance(i);
		auto d1 = mdijk.distance(i);
		ASSERT_EQ(d0, d1);
	}
}

TEST_F(ModDijsktraTest, test1) {
	StaticMapping::ModDijsktra dijk(graph, 0, [](auto ew) {return 1.0 / ew;});
	dijk.run();

	std::vector<NetworKit::edgeweight> expected = {
		0.0,
		0.5,
		0.125,
		0.2,
		0.45,
	};

	for (NetworKit::index i = 0; i < graph.numberOfNodes(); i++) {
		auto d = dijk.distance(i);
		auto e = expected[i];
		EXPECT_DOUBLE_EQ(d, e);
	}
}

//-----------------------------------------------------------------------//

void ModAPSPTest::SetUp() {
	graph = NetworKit::Graph(5, true, true);

	graph.addEdge(0, 1, 2.0);
	graph.addEdge(0, 2, 8.0);
	graph.addEdge(0, 3, 5.0);
	graph.addEdge(1, 2, 1.0);
	graph.addEdge(2, 4, 3.0);
	graph.addEdge(3, 4, 4.0);
}

TEST_F(ModAPSPTest, sanityCheck) {
	NetworKit::APSP apsp(graph);
	apsp.run();

	StaticMapping::ModAPSP mapsp(graph, [](auto ew) {return ew;});
	mapsp.run();

	for (auto& u: graph.nodes()) {
		for (auto& v: graph.nodes()) {
			auto d0 = apsp.getDistance(u, v);
			auto d1 = mapsp.getDistance(u, v);
			EXPECT_EQ(d0, d1);
		}
	}
}

//-----------------------------------------------------------------------//

void CommTimeTest1::SetUp() {
	graph = NetworKit::Graph(3, true, false);

	graph.addEdge(0, 1, 2);
	graph.addEdge(0, 2, 1);
	graph.addEdge(1, 2, 3);

	/* simple triangle graph
	 *        2 
	 *     A_____B
     *     \     /
	 *    1 \   / 3
	 *       \ /
	 *        C
	 *
	 * expected outcome:
	 *     time_on_path(A,B) = 1/2 
	 *     time_on_path(B,C) = 1/3
	 *     time_on_path(A,C) = 1/2 + 1/3
	 */
}

TEST_F(CommTimeTest1, SimpleTriangleTest) {
	StaticMapping::CommTime ct(graph);
	ct.run();

	EXPECT_DOUBLE_EQ(ct.time(0, 2), (1.0 / 2.0) + (1.0 / 3.0));
	EXPECT_DOUBLE_EQ(ct.time(0, 1), (1.0 / 2.0));
	EXPECT_DOUBLE_EQ(ct.time(1, 2), (1.0 / 3.0));
}

void CommTimeTest2::SetUp() {
	graph = NetworKit::Graph(4, true, false);

	graph.addEdge(0, 1, 2);
	graph.addEdge(0, 2, 1);
	graph.addEdge(1, 3, 3);
	graph.addEdge(2, 3, 3);

	/* simple rectangle graph
	 *      2
	 * A----------B 
	 * |          |
	 * |1         |3
	 * |          |
	 * C----------D 
	 *      3 
	 * expected outcome:
	 *     time_on_path(A, B) = 1/2
	 *     time_on_path(B, D) = 1/3
	 *     time_on_path(C, D) = 1/3
	 *     time_on_path(A, C) = 1 
	 *     time_on_path(A, D) = 1/2 + 1/3
	 *     time_on_path(B, C) = 1/3 + 1/3
	 */
}

TEST_F(CommTimeTest2, SimpleRectangleTest) {
	StaticMapping::CommTime ct(graph);
	ct.run();

	EXPECT_DOUBLE_EQ(ct.time(0, 1), 1.0/2.0);
	EXPECT_DOUBLE_EQ(ct.time(1, 3), 1.0/3.0);
	EXPECT_DOUBLE_EQ(ct.time(2, 3), 1.0/3.0);
	EXPECT_DOUBLE_EQ(ct.time(0, 3), (1.0/2.0) + (1.0/3.0));
	EXPECT_DOUBLE_EQ(ct.time(1, 2), 2.0/3.0);
}

void CommTimeTest3::SetUp() {
	graph = NetworKit::Graph(4, true, false);

	graph.addEdge(0, 1, 2);
	graph.addEdge(0, 2, 5);
	graph.addEdge(0, 3, 1);

	graph.addEdge(1, 3, 4);
	graph.addEdge(1, 2, 3);

	graph.addEdge(2, 3, 4);
	
	/* connected rectangle graph
	 *   2 
	 * A----B
	 * |\1 /|
	 *5| \/ |4
	 * | /\ |
	 * |/3 \|
	 * C----D
	 *   4
	 */
}

TEST_F(CommTimeTest3, ConnectedRectangleTest) {
	StaticMapping::CommTime ct(graph);
	ct.run();

	EXPECT_DOUBLE_EQ(ct.time(0, 1), 1.0/2.0);
	EXPECT_DOUBLE_EQ(ct.time(0, 2), 1.0/5.0);
	EXPECT_DOUBLE_EQ(ct.time(0, 3), 1.0/5.0 + 1.0/4.0);

	EXPECT_DOUBLE_EQ(ct.time(1, 3), 1.0/4.0);
	EXPECT_DOUBLE_EQ(ct.time(1, 2), 1.0/3.0);

	EXPECT_DOUBLE_EQ(ct.time(2, 3), 1.0/4.0);
}
