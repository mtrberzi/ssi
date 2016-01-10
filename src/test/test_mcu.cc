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
    // load x1 with 0x000000FF
    // shift left by 8
    // should be 0x0000FF00

    set_register(1, 0x000000FF);
    // SLLI x1, x1, 8
    execute(0b00000000100000001001000010010011);
    ASSERT_EQ(0x0000FF00, get_register(1));
}

/*
@Test
  public void testExecuteSLTI_WritesOne() {
    // load -5 into x1 and compare it to -1
    // should place 1 into x2
    // so SLTI x2, x1, -1
    RV32_SLTI insn = new RV32_SLTI(0b11111111111100001010000100010011);
    RV32Core cpu = new RV32Core();
    cpu.setXRegister(1, -5);
    cpu.execute(insn);
    assertEquals(1, cpu.getXRegister(2));
  }

  @Test
  public void testExecuteSLTI_WritesZero() {
    // load 3 into x1 and compare it to -1
    // should place 0 into x2
    // so SLTI x2, x1, -1
    RV32_SLTI insn = new RV32_SLTI(0b11111111111100001010000100010011);
    RV32Core cpu = new RV32Core();
    cpu.setXRegister(1, 3);
    cpu.setXRegister(2, -50); // allows us to check that x2 actually gets written
    cpu.execute(insn);
    assertEquals(0, cpu.getXRegister(2));
  }

*/

TEST_F (RV32CoreTest, SLTI) {
	// test path: write 1 to rd
    // load -5 into x1 and compare to -1
    // expect 1 in x2

    set_register(1, -5);
    set_register(2, 9001);
    // SLTI x2, x1, -1
    execute(0b11111111111100001010000100010011);
    ASSERT_EQ(1, get_register(2));

    // test path: write 0 to rd
    // load 3 into x1 and compare to -1
    // expect 0 in x2

    set_register(1, 3);
    set_register(2, 9001);
    // SLTI x2, x1, -1
    execute(0b11111111111100001010000100010011);
    ASSERT_EQ(0, get_register(2));
}

TEST_F (RV32CoreTest, SLTIU) {
	// load -1 into x1 and compare it to 0
    // should place 0 into x2

    set_register(1, -1);
    // SLTIU x2, x1, 0
    execute(0b00000000000000001011000100010011);
    ASSERT_EQ(0, get_register(2));
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
	// load x1 with 0xFF000000
    // shift right by 8
    // should be 0x00FF0000

    set_register(1, 0xFF000000);
    // SRLI x1, x1, 8
    execute(0b00000000100000001101000010010011);
    ASSERT_EQ(0x00FF0000, get_register(1));
}

TEST_F (RV32CoreTest, SRAI) {
	// load x1 with 0xFF000000
    // shift right by 8
    // should be 0xFFFF0000

    set_register(1, 0xFF000000);
    // SRAI x1, x1, 8
    execute(0b01000000100000001101000010010011);
    ASSERT_EQ(0xFFFF0000, get_register(1));
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
