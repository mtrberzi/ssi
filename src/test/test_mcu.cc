#include "gtest/gtest.h"
#include "rv32core.h"

class RV32CoreTest: public RV32Core, public ::testing::Test {
};

TEST_F (RV32CoreTest, X0_ConstantZero) {
	EXPECT_EQ((uint32_t)0, get_register(0));
}

int main (int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
