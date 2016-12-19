#include <gtest/gtest.h>

#include <limits>

#define MAX_DOUBLE std::numeric_limits<double>::max()

TEST(SimpleSanityCheck, DoubleMaxTest) {
	ASSERT_LT(MAX_DOUBLE * 0.99, MAX_DOUBLE);
}

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
