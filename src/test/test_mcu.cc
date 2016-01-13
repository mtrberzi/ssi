#include "gtest/gtest.h"
#include "rv32core.h"
#include "rom.h"
#include "ram.h"
#include <cstdint>
#include <vector>
#include <iomanip>

static const uint32_t textMemoryBase = 0x00000000;
static const uint32_t textMemoryPages = 4;
static const uint32_t dataMemoryBase = 0x10000000;
static const uint32_t dataMemoryPages = 4;

class RV32CoreTest: public RV32Core, public ::testing::Test {
};

class RV32CodeExecution : public RV32Core, public ::testing::Test {
public:
	ROM * text_memory;
	RAM * data_memory;

	void SetUp() {
		text_memory = new ROM(textMemoryPages);
		system_bus->attach_peripheral(text_memory, textMemoryBase);
		data_memory = new RAM(dataMemoryPages);
		system_bus->attach_peripheral(data_memory, dataMemoryBase);
		// stack pointer initially goes to top of RAM, 16-byte aligned
		uint32_t dataMemoryTop = dataMemoryBase + (dataMemoryPages * 1024) - 1;
		set_register(2, dataMemoryTop & 0xFFFFFFF0);
	}

	void TearDown() {
		delete text_memory;
		delete data_memory;
	}

	void load_program(uint32_t *text, uint32_t nTextWords) {
		if (nTextWords > textMemoryPages * (1024/4)) {
			FAIL() << "insufficient memory to load program";
		}
		uint8_t *byteText = (uint8_t*)malloc(sizeof(uint8_t) * nTextWords * 4);
		for (uint32_t tPtr = 0; tPtr < nTextWords; ++tPtr) {
			uint32_t insn = text[tPtr];
			uint32_t bPtr = 4 * tPtr;
			byteText[bPtr+0] = (uint8_t)((insn & 0x000000FF));
			byteText[bPtr+1] = (uint8_t)((insn & 0x0000FF00) >>  8);
			byteText[bPtr+2] = (uint8_t)((insn & 0x00FF0000) >> 16);
			byteText[bPtr+3] = (uint8_t)((insn & 0xFF000000) >> 24);
		}
		text_memory->set_contents(byteText);
		free(byteText);
	}

	void load_program(std::vector<uint32_t> program) {
		load_program(program.data(), program.size());
	}

	void run(uint32_t maxCycles) {
		// prime ra (x1) with a target return address
		uint32_t retTarget = 0xDDCCDDCC;
		set_register(1, retTarget);
		for (uint32_t i = 0; i < maxCycles; ++i) {
			step();
			if (pc == retTarget) {
				return;
			}
			// check for weird stuff
			if (mcause != 0) {
			    switch (mcause) {
			    case 2: FAIL() << "illegal instruction at mepc=0x" << std::hex << mepc; break;
			    default: FAIL() << "unexpected processor trap " << mcause; break;
			    }
			}
		}
		FAIL() << "CPU did not terminate after " << maxCycles << " steps";
	}
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

TEST_F(RV32CoreTest, ADD) { FAIL(); }
TEST_F(RV32CoreTest, SLL) { FAIL(); }
TEST_F(RV32CoreTest, SLT) { FAIL(); }
TEST_F(RV32CoreTest, SLTU) { FAIL(); }
TEST_F(RV32CoreTest, XOR) { FAIL(); }
TEST_F(RV32CoreTest, SRL) { FAIL(); }
TEST_F(RV32CoreTest, OR) { FAIL(); }
TEST_F(RV32CoreTest, AND) { FAIL(); }
TEST_F(RV32CoreTest, MUL) { FAIL(); }
TEST_F(RV32CoreTest, MULH) { FAIL(); }
TEST_F(RV32CoreTest, MULHSU) { FAIL(); }
TEST_F(RV32CoreTest, MULHU) { FAIL(); }
TEST_F(RV32CoreTest, DIV) { FAIL(); }
TEST_F(RV32CoreTest, DIVU) { FAIL(); }
TEST_F(RV32CoreTest, REM) { FAIL(); }
TEST_F(RV32CoreTest, REMU) { FAIL(); }
TEST_F(RV32CoreTest, SUB) { FAIL(); }
TEST_F(RV32CoreTest, SRA) { FAIL(); }
TEST_F(RV32CoreTest, LUI) { FAIL(); }
TEST_F(RV32CoreTest, AUIPC) { FAIL(); }
TEST_F(RV32CoreTest, JAL) { FAIL(); }
TEST_F(RV32CoreTest, JALR) { FAIL(); }
TEST_F(RV32CoreTest, BEQ) { FAIL(); }
TEST_F(RV32CoreTest, BNE) { FAIL(); }
TEST_F(RV32CoreTest, BLT) { FAIL(); }
TEST_F(RV32CoreTest, BGE) { FAIL(); }
TEST_F(RV32CoreTest, BLTU) { FAIL(); }
TEST_F(RV32CoreTest, BGEU) { FAIL(); }
TEST_F(RV32CoreTest, SCALL) { FAIL(); }
TEST_F(RV32CoreTest, SBREAK) { FAIL(); }
TEST_F(RV32CoreTest, ERET) { FAIL(); }
TEST_F(RV32CoreTest, CSRRW) { FAIL(); }
TEST_F(RV32CoreTest, CSRRS) { FAIL(); }
TEST_F(RV32CoreTest, CSRRC) { FAIL(); }
TEST_F(RV32CoreTest, CSRRWI) { FAIL(); }
TEST_F(RV32CoreTest, CSRRSI) { FAIL(); }
TEST_F(RV32CoreTest, CSRRCI) { FAIL(); }
TEST_F(RV32CoreTest, LB) { FAIL(); }
TEST_F(RV32CoreTest, LH) { FAIL(); }
TEST_F(RV32CoreTest, LW) { FAIL(); }
TEST_F(RV32CoreTest, LBU) { FAIL(); }
TEST_F(RV32CoreTest, LHU) { FAIL(); }
TEST_F(RV32CoreTest, SB) { FAIL(); }
TEST_F(RV32CoreTest, SH) { FAIL(); }
TEST_F(RV32CoreTest, SW) { FAIL(); }
TEST_F(RV32CoreTest, LRW) { FAIL(); }
TEST_F(RV32CoreTest, SCW) { FAIL(); }
TEST_F(RV32CoreTest, AMOSWAPW) { FAIL(); }
TEST_F(RV32CoreTest, AMOADDW) { FAIL(); }
TEST_F(RV32CoreTest, AMOXORW) { FAIL(); }
TEST_F(RV32CoreTest, AMOANDW) { FAIL(); }
TEST_F(RV32CoreTest, AMOORW) { FAIL(); }
TEST_F(RV32CoreTest, AMOMINW) { FAIL(); }
TEST_F(RV32CoreTest, AMOMAXW) { FAIL(); }
TEST_F(RV32CoreTest, AMOMINUW) { FAIL(); }
TEST_F(RV32CoreTest, AMOMAXUW) { FAIL(); }

TEST_F(RV32CodeExecution, ReturnOnly) {
	std::vector<uint32_t> program {0x00008067};
	load_program(program);
	run(1);
}

TEST_F(RV32CodeExecution, DoubleA0) {
    // copies the value in a0 to t0,
    // then adds a0 and t0 and places the result in a0
    std::vector<uint32_t> program {
      0x00050293,
      0x00550533,
      0x00008067,
    };
    load_program(program);
    set_register(10, 21);
    run(3);
    ASSERT_EQ(42, get_register(10));
}

TEST_F(RV32CodeExecution, Max_Signed) {
    // puts the larger of a0 and a1 into a0 (signed compare)
    std::vector<uint32_t> program {
        0x00a5c463,
        0x00058513,
        0x00008067,
    };
    load_program(program);

    int32_t testData_a0[] = {1, 12, 4,  -3};
    int32_t testData_a1[] = {0, 6,  23, -2};
    int32_t expected_a0[] = {1, 12, 23, -2};

    for (int i = 0; i < 4; ++i) {
      pc = 0;
      set_register(10, (uint32_t)testData_a0[i]);
      set_register(11, (uint32_t)testData_a1[i]);
      run(3);
      EXPECT_EQ(expected_a0[i], (int32_t)get_register(10)) << "failed #" << i;
    }
}

TEST_F(RV32CodeExecution, Max_Unsigned) {
    // puts the larger of a0 and a1 into a0 (unsigned compare)
    std::vector<uint32_t> program {
        0x00a5e463,
        0x00058513,
        0x00008067,
    };
    load_program(program);

    uint32_t testData_a0[] = {1, 12, 4,  0xFFFF0000};
    uint32_t testData_a1[] = {0, 6,  23, 0x0000000F};
    uint32_t expected_a0[] = {1, 12, 23, 0xFFFF0000};

    for (int i = 0; i < 4; ++i) {
      pc = 0;
      set_register(10, testData_a0[i]);
      set_register(11, testData_a1[i]);
      run(3);
      EXPECT_EQ(expected_a0[i], get_register(10)) << " failed #" << i;
    }
}

TEST_F(RV32CodeExecution, Fibonacci15) {
	// call with n in a0, returns fib(n) in a0
	std::vector<uint32_t> program {
		0xfe010113,
		0x00112e23,
		0x00812c23,
		0x00912a23,
		0x02010413,
		0xfea42623,
		0xfec42703,
		0x00100793,
		0x00e7c663,
		0xfec42783,
		0x0300006f,
		0xfec42783,
		0xfff78793,
		0x00078513,
		0xfc9ff0ef,
		0x00050493,
		0xfec42783,
		0xffe78793,
		0x00078513,
		0xfb5ff0ef,
		0x00050793,
		0x00f487b3,
		0x00078513,
		0x01c12083,
		0x01812403,
		0x01412483,
		0x02010113,
		0x00008067,
	};
	load_program(program);

	uint32_t fib[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610};

	uint32_t n = 15;
	uint32_t expected_result = fib[n];
	set_register(10, n);
	run(42415); // exact for fib(15)
	uint32_t actual_result = get_register(10);
	ASSERT_EQ(expected_result, actual_result);
}

int main (int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
