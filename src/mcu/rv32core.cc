#include "rv32core.h"

RV32Core::RV32Core()
: pc(0), next_pc(0), mstatus_ie(false), mstatus_ie1(false),
  mscratch(0), mepc(0), mcause(0), mbadaddr(0), instret(0)
{
	for (int i = 0; i < 32; ++i) {
		xRegister[i] = 0;
	}
}

RV32Core::~RV32Core() {

}

void RV32Core::step() {
	// TODO step
}

/* Register 0 is fixed to the value zero, so reads and writes
 * are handled specially.
 */

uint32_t RV32Core::get_register(int idx) {
	if (idx == 0) {
		return 0;
	} else {
		return xRegister[idx];
	}
}

void RV32Core::set_register(int idx, uint32_t val) {
	if (idx != 0) {
		xRegister[idx] = val;
	}
}

uint32_t RV32Core::get_mstatus() {
	// [31:6] are all zeroes
	// [5:4] = "11"
	// [3] is mstatus_ie1
	// [2:1] = "11"
	// [0] is mstatus_ie
	uint32_t mstatus = 0x00000036;
	if (mstatus_ie) mstatus |= 0x00000001;
	if (mstatus_ie1) mstatus |= 0x00000008;
	return mstatus;
}

void RV32Core::set_mstatus(uint32_t mstatus) {
	mstatus_ie = ((mstatus & 0x00000001) != 0);
	mstatus_ie1 = ((mstatus & 0x00000008) != 0);
}

uint32_t RV32Core::read_csr(int csr) {
	switch (csr) {
	case 0x300:
		return get_mstatus();
	case 0x340:
		return mscratch;
	case 0x341:
		return mepc;
	case 0x342:
		return mcause;
	case 0x343:
		return mbadaddr;
	case 0xC00:
		// TODO RDCYCLE
		return 0;
	case 0xC80:
		// TODO RDCYCLEH
		return 0;
	case 0xC01:
		// TODO RDTIME
		return 0;
	case 0xC81:
		// TODO RDTIMEH
		return 0;
	case 0xC02:
		// RDINSTRET
		return (uint32_t)(instret & 0x00000000FFFFFFFFL);
	case 0xC82:
		// RDINSTRETH
		return (uint32_t)((instret & 0xFFFFFFFF00000000L) >> 32);
	case 0xF00:
		// mcpuid
		// base 00 (RV32I),
		// extensions I, M, A
		return 0b00000000000000000001000100000001;
	case 0xF01:
		// mimpid
		// 0x8000 = anonymous source
		// this is microcontroller version 0.0.1,
		// patch level 0
		return 0x00108000;
	case 0xF10:
		// mhartid
		return 0;
	default:
		// attempts to access a non-existent CSR raise an illegal instruction exception
		illegal_instruction(); return 0;
	}
}

void RV32Core::write_csr(int csr, uint32_t value) {
	switch (csr) {
	case 0x300:
		set_mstatus(value);
		break;
	case 0x340:
		mscratch = value;
		break;
	case 0x341:
		mepc = value;
		break;
	case 0x342:
		mcause = value;
		break;
	case 0x343:
		mbadaddr = value;
		break;
	default:
		// attempts to access a non-existent CSR
		// or write to a read-only CSR
		// raise an illegal instruction exception
		illegal_instruction(); break;
	}
}

void RV32Core::illegal_instruction() {
	// TODO illegal instruction handler
}

bool RV32Core::interrupts_enabled() const {
	return mstatus_ie;
}

void RV32Core::external_interrupt() {
	// TODO external interrupt
}

void RV32Core::execute(uint32_t insn) {
	// Table 8.1 gives us a quick way to decode most of an instruction:
	// check that bits [1:0] are "11", then
	// check bits [6:2] of the instruction word
	// and jump to an appropriate handler

	if ((insn & 0x00000003) != 3) {
		// bits [1:0] not "11", this is not an RV32 base opcode
		illegal_instruction(); return;
	}

	uint8_t opcodeTag = (insn & 0x0000007C) >> 2;

	switch (opcodeTag) {
	// 0: LOAD
	case 0:
		execute_LOAD(insn); break;
		// 1: LOAD-FP
		// 2: custom-0
		// 3: MISC-MEM
	case 3:
		execute_MISC_MEM(insn); break;
		// 4: OP-IMM
	case 4:
		execute_OP_IMM(insn); break;
		// 5: AUIPC
	case 5:
	{
		// TODO AUIPC
		illegal_instruction();
	} break;
		// 6: OP-IMM-32
		// 7: 48b
		// 8: STORE
	case 6:
		execute_STORE(insn); break;
		// 9: STORE-FP
		// 10: custom-1
		// 11: AMO
	case 11:
		execute_AMO(insn); break;
		// 12: OP
	case 0b01100:
		execute_OP(insn); break;
		// 13: LUI
	case 0b01101:
	{
		// TODO LUI
		illegal_instruction();
	} break;
		// 14: OP-32
		// 15: 64b
		// 16: MADD
		// 17: MSUB
		// 18: NMSUB
		// 19: NMADD
		// 20: OP-FP
		// 21: reserved
		// 22: custom-2
		// 23: 48b
		// 24: BRANCH
	case 24:
		execute_BRANCH(insn); break;
		// 25: JALR
	case 25:
	{
		// TODO JALR
		illegal_instruction();
	} break;
		// 26: reserved
		// 27: JAL
	case 27:
	{
		// TODO JAL
		illegal_instruction();
	} break;
		// 28: SYSTEM
	case 28:
		execute_SYSTEM(insn); break;
		// 29: reserved
		// 30: custom-3
		// 31: >= 80b
	default:
		illegal_instruction(); break;
	}

}

void RV32Core::execute_LOAD(uint32_t insn) {
	// TODO
	illegal_instruction();
}

void RV32Core::execute_MISC_MEM(uint32_t insn) {
	// TODO
	illegal_instruction();
}

void RV32Core::execute_OP_IMM(uint32_t insn) {
	// TODO
	illegal_instruction();
}

void RV32Core::execute_STORE(uint32_t insn) {
	// TODO
	illegal_instruction();
}

void RV32Core::execute_AMO(uint32_t insn) {
	// TODO
	illegal_instruction();
}

void RV32Core::execute_OP(uint32_t insn) {
	// TODO
	illegal_instruction();
}

void RV32Core::execute_BRANCH(uint32_t insn) {
	// TODO
	illegal_instruction();
}

void RV32Core::execute_SYSTEM(uint32_t insn) {
	// TODO
	illegal_instruction();
}

/*
private RV32Instruction decode_LOAD(int insn) {
	// opcode = 0000011
	// now decode funct3
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	switch (funct3) {
	case 0b000:
		return new RV32_LB(insn);
	case 0b001:
		return new RV32_LH(insn);
	case 0b010:
		return new RV32_LW(insn);
	case 0b100:
		return new RV32_LBU(insn);
	case 0b101:
		return new RV32_LHU(insn);
	default:
		return new RV32IllegalInstruction(insn);
	}
}

private RV32Instruction decode_MISC_MEM(int insn) {
	// opcode = 0001111
	// now decode funct3
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	switch (funct3) {
	case 0b000:
		return new RV32_FENCE(insn);
	case 0b001:
		return new RV32_FENCEI(insn);
	default:
		return new RV32IllegalInstruction(insn);
	}
}

private RV32Instruction decode_OP_IMM(int insn) {
	// opcode = 0010011
	// now decode funct3
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	switch (funct3) {
	case 0b000:
		return new RV32_ADDI(insn);
	case 0b001:
	{
		// decode funct7
		int funct7 = (insn & 0b11111110000000000000000000000000) >>> 25;
		if (funct7 == 0b0000000) {
			return new RV32_SLLI(insn);
		} else {
			return new RV32IllegalInstruction(insn);
		}
	}
	case 0b010:
		return new RV32_SLTI(insn);
	case 0b011:
		return new RV32_SLTIU(insn);
	case 0b100:
		return new RV32_XORI(insn);
	case 0b101:
	{
		// decode funct7
		int funct7 = (insn & 0b11111110000000000000000000000000) >>> 25;
		if (funct7 == 0b0000000) {
			return new RV32_SRLI(insn);
		} else if (funct7 == 0b0100000) {
			return new RV32_SRAI(insn);
		} else {
			return new RV32IllegalInstruction(insn);
		}
	}
	case 0b110:
		return new RV32_ORI(insn);
	case 0b111:
		return new RV32_ANDI(insn);
	default:
		return new RV32IllegalInstruction(insn);
	}
}

private RV32Instruction decode_STORE(int insn) {
	// opcode = 0100011
	// now decode funct3
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	switch (funct3) {
	case 0b000:
		return new RV32_SB(insn);
	case 0b001:
		return new RV32_SH(insn);
	case 0b010:
		return new RV32_SW(insn);
	default:
		return new RV32IllegalInstruction(insn);
	}
}

private RV32Instruction decode_AMO(int insn) {
	// opcode = 0101111
	int funct7 = (insn & 0b11111110000000000000000000000000) >>> 25;
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	// check funct3 = 010
	if (funct3 == 0b010) {
		// decode the 5 highest bits of funct7
		int amofunct = funct7 >>> 2;
		switch (amofunct) {
		case 0b00010: return new RV32_LRW(insn);
		case 0b00011: return new RV32_SCW(insn);
		case 0b00001: return new RV32_AMOSWAPW(insn);
		case 0b00000: return new RV32_AMOADDW(insn);
		case 0b00100: return new RV32_AMOXORW(insn);
		case 0b01100: return new RV32_AMOANDW(insn);
		case 0b01000: return new RV32_AMOORW(insn);
		case 0b10000: return new RV32_AMOMINW(insn);
		case 0b10100: return new RV32_AMOMAXW(insn);
		case 0b11000: return new RV32_AMOMINUW(insn);
		case 0b11100: return new RV32_AMOMAXUW(insn);
		default:
			return new RV32IllegalInstruction(insn);
		}
	} else {
		return new RV32IllegalInstruction(insn);
	}
}

private RV32Instruction decode_OP(int insn) {
	// opcode = 0110011
	// first decode funct7 as MUL/DIV have a different prefix
	int funct7 = (insn & 0b11111110000000000000000000000000) >>> 25;
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	switch (funct7) {
	case 0b0000000:
		// standard integer ops
	{
		switch (funct3) {
		case 0b000:
			return new RV32_ADD(insn);
		case 0b001:
			return new RV32_SLL(insn);
		case 0b010:
			return new RV32_SLT(insn);
		case 0b011:
			return new RV32_SLTU(insn);
		case 0b100:
			return new RV32_XOR(insn);
		case 0b101:
			return new RV32_SRL(insn);
		case 0b110:
			return new RV32_OR(insn);
		case 0b111:
			return new RV32_AND(insn);
		default:
			return new RV32IllegalInstruction(insn);
		}
	}
	case 0b0000001:
		// MUL/DIV
	{
		switch (funct3) {
		case 0b000:
			return new RV32_MUL(insn);
		case 0b001:
			return new RV32_MULH(insn);
		case 0b010:
			return new RV32_MULHSU(insn);
		case 0b011:
			return new RV32_MULHU(insn);
		case 0b100:
			return new RV32_DIV(insn);
		case 0b101:
			return new RV32_DIVU(insn);
		case 0b110:
			return new RV32_REM(insn);
		case 0b111:
			return new RV32_REMU(insn);
		default:
			return new RV32IllegalInstruction(insn);
		}
	}
	case 0b0100000:
		// other standard integer ops
	{
		switch (funct3) {
		case 0b000:
			return new RV32_SUB(insn);
		case 0b101:
			return new RV32_SRA(insn);
		default:
			return new RV32IllegalInstruction(insn);
		}
	}
	default:
		return new RV32IllegalInstruction(insn);
	}
}

private RV32Instruction decode_BRANCH(int insn) {
	// opcode = 1100011
	// now decode funct3
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	switch (funct3) {
	case 0b000:
		return new RV32_BEQ(insn);
	case 0b001:
		return new RV32_BNE(insn);
	case 0b100:
		return new RV32_BLT(insn);
	case 0b101:
		return new RV32_BGE(insn);
	case 0b110:
		return new RV32_BLTU(insn);
	case 0b111:
		return new RV32_BGEU(insn);
	default:
		return new RV32IllegalInstruction(insn);
	}
}

private RV32Instruction decode_SYSTEM(int insn) {
	// opcode = 1110011
	// now decode funct3
	int funct3 = (insn & 0b00000000000000000111000000000000) >>> 12;
	switch (funct3) {
	case 0b000:
	{
		// decode imm (funct12)
		int imm = (insn & 0b11111111111100000000000000000000) >>> 20;
		switch (imm) {
		case 0b000000000000:
			return new RV32_SCALL(insn);
		case 0b000000000001:
			return new RV32_SBREAK(insn);
		case 0b000100000000:
			return new RV32_ERET(insn);
		default:
			return new RV32IllegalInstruction(insn);
		}
	}
	case 0b001:
		return new RV32_CSRRW(insn);
	case 0b010:
		return new RV32_CSRRS(insn);
	case 0b011:
		return new RV32_CSRRC(insn);
	case 0b101:
		return new RV32_CSRRWI(insn);
	case 0b110:
		return new RV32_CSRRSI(insn);
	case 0b111:
		return new RV32_CSRRCI(insn);
	default:
		return new RV32IllegalInstruction(insn);
	}
}
*/
