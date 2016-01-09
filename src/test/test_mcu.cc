#include "gtest/gtest.h"
#include "rv32core.h"

class RV32CoreTest: public RV32Core, public ::testing::Test {
};

TEST_F (RV32CoreTest, X0_ConstantZero) {
	EXPECT_EQ((uint32_t)0, get_register(0));
	// should still be 0 after write
	set_register(0, 9001);
	EXPECT_EQ((uint32_t)0, get_register(0));
}

TEST_F (RV32CoreTest, ADDI) {
	// ADDI x1, x0, 1
	execute(0b00000000000100000000000010010011);
	ASSERT_EQ(1, get_register(1));
}

TEST_F (RV32CoreTest, SLLI) {
	FAIL();
}

TEST_F (RV32CoreTest, SLTI) {
	FAIL();
}

TEST_F (RV32CoreTest, SLTIU) {
	FAIL();
}

TEST_F (RV32CoreTest, XORI) {
	// load x1 with 0xFF00F00F
	// XOR it with 0x0FF
	// should be 0xFF00F0F0

	set_register(1, 0xFF00F00F);
	// XORI x1, x1, 0x0FF
	execute(0b00001111111100001100000010010011);
	ASSERT_EQ(0xFF00F0F0, get_register(1));
}

TEST_F (RV32CoreTest, SRLI) {
	FAIL();
}

TEST_F (RV32CoreTest, SRAI) {
	FAIL();
}

TEST_F (RV32CoreTest, ORI) {
	// load x1 with 0xFF000F0F
	// OR it with 0x0FF
	// result should be 0xFF000FFF

	set_register(1, 0xFF000F0F);
	// ORI x1, x1, 0x0FF
	execute(0b00001111111100001110000010010011);
	ASSERT_EQ(0xFF000FFF, get_register(1));
}

TEST_F (RV32CoreTest, ANDI) {
	// initialize x1 = 0xFF000F0F
	// AND it with 0x000000FF
	// result should be 0x0000000F

	set_register(1, 0xFF000F0F);
	// ANDI x1, x1, 0x0FF
	execute(0b00001111111100001111000010010011);
	ASSERT_EQ(0x0000000F, get_register(1));
}

int main (int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
