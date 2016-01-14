#include "rv32core.h"

RV32Core::RV32Core()
: pc(0), next_pc(0), system_bus(new SystemBus()),
  mstatus_ie(false), mstatus_ie1(false),
  mscratch(0), mepc(0), mcause(0), mbadaddr(0),
  instret(0)
{
	for (int i = 0; i < 32; ++i) {
		xRegister[i] = 0;
	}
}

RV32Core::~RV32Core() {
    if (system_bus != NULL) {
        delete system_bus;
    }
}

void RV32Core::step() {
	uint32_t insn = system_bus->load_word(pc);
	next_pc = pc + 4;
	execute(insn);
	instret += 1L;
	pc = next_pc;
}

/* Register 0 is fixed to the value zero, so reads and writes
 * are handled specially.
 */

uint32_t RV32Core::get_register(int idx) const {
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

void RV32Core::processor_trap(uint32_t cause) {
    // when a trap is taken, the mstatus stack is pushed to the left
    // and IE is set to 0
    mstatus_ie1 = mstatus_ie;
    mstatus_ie = false;
    // save program counter
    mepc = pc;
    // set mcause
    mcause = cause;
    // counts as a context switch
    system_bus->clear_all_reservations();
    // jump to the correct trap handler, which is always at 0x100 + whatever offset
    if (true) {
      // trap from machine mode
      next_pc = 0x000001C0;
    }
}

void RV32Core::illegal_instruction() {
	processor_trap(2);
}

void RV32Core::external_interrupt() {
	processor_trap(15);
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
	    uint32_t imm = (insn & 0b11111111111111111111000000000000);
        int rd = (insn & 0b00000000000000000000111110000000) >> 7;
        set_register(rd, pc + imm);
	} break;
		// 6: OP-IMM-32
		// 7: 48b
		// 8: STORE
	case 8:
		execute_STORE(insn); break;
		// 9: STORE-FP
		// 10: custom-1
		// 11: AMO
	case 11:
		execute_AMO(insn); break;
		// 12: OP
	case 12:
		execute_OP(insn); break;
		// 13: LUI
	case 13:
	{
	    uint32_t imm = (insn & 0b11111111111111111111000000000000);
	    int rd = (insn & 0b00000000000000000000111110000000) >> 7;
	    set_register(rd, imm);
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
	    int32_t imm = ( ((int32_t)insn) & (int32_t)0b11111111111100000000000000000000) >> 20;
        int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;
        int rd = (insn & 0b00000000000000000000111110000000) >> 7;

        uint32_t target = (uint32_t) ((int32_t)get_register(rs1) + imm);
        target &= ~(0x00000001);
        next_pc = target;
        set_register(rd, pc + 4);
	} break;
		// 26: reserved
		// 27: JAL
	case 27:
	{
	    // immediate is broken over effectively four fields
	    // [20] [10:1] [11] [19:12]
	    uint32_t imm =    (insn & 0x80000000) >> (31-20) // [20]
	                    | (insn & 0x000ff000)            // [19:12]
	                    | (insn & 0x00100000) >> (20-11) // [11]
	                    | (insn & 0x7fe00000) >> (21-1)  // [10:1]
	                    ;
	    // sign-extend bit 20
	    if (imm & 0x00100000) {
	        imm |= 0xFFE00000;
	    }
	    int32_t offset = (int32_t)imm;
	    int rd = (insn & 0b00000000000000000000111110000000) >> 7;
	    next_pc = (uint32_t) ((int32_t)pc + offset);
	    set_register(rd, pc + 4);
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
	uint8_t funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
    int32_t imm = ( ((int32_t)insn) & (int32_t)0b11111111111100000000000000000000) >> 20;
	int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;
	int rd = (insn & 0b00000000000000000000111110000000) >> 7;

	uint32_t addr = (uint32_t)((int32_t)get_register(rs1) + imm);
	uint32_t val = 0;
	bool cancel_load = false; // will be set to true if a bus error occurs

	// TODO check legal address on system bus

	switch (funct3) {
	case 0b000: // LB
	{
		uint8_t di = system_bus->load_byte(addr);
		// sign-extend to 32 bits
		val = (uint32_t)di;
		if (di & 0x80) {
			val |= 0xFFFFFF00;
		}
	} break;
	case 0b001: // LH
	{
		uint16_t di = system_bus->load_halfword(addr);
		// sign-extend to 32 bits
		val = (uint32_t)di;
		if (di & 0x8000) {
			val |= 0xFFFF0000;
		}
	} break;
	case 0b010: // LW
	{
		val = system_bus->load_word(addr);
	} break;
	case 0b100: // LBU
	{
		// do not sign-extend
		val = (uint32_t)system_bus->load_byte(addr);
	} break;
	case 0b101: // LHU
	{
		// do not sign-extend
		val = (uint32_t)system_bus->load_halfword(addr);
	} break;
	default:
		illegal_instruction(); cancel_load = true; break;
	}

	if (!cancel_load) {
		set_register(rd, val);
	}
}

void RV32Core::execute_MISC_MEM(uint32_t insn) {
    uint8_t funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
    switch (funct3) {
    case 0b000: // FENCE
        // should be a no-op here.
        break;
    case 0b001: // FENCEI
        // clear instruction cache, if we have one
        break;
    default:
        illegal_instruction(); break;
    }
}

void RV32Core::execute_OP_IMM(uint32_t insn) {
	int32_t imm = ( ((int32_t)insn) & (int32_t)0b11111111111100000000000000000000) >> 20;
	int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;
	uint8_t funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
	int rd = (insn & 0b00000000000000000000111110000000) >> 7;
	uint8_t funct7 = (insn & 0b11111110000000000000000000000000) >> 25;
	uint8_t shamt = (insn & 0b00000001111100000000000000000000) >> 20;
	switch (funct3) {
	case 0: // ADDI
	{
		set_register(rd, get_register(rs1) + imm);
	} break;
	case 1:
	{
		if (funct7 == 0) {
			// SLLI
			set_register(rd, get_register(rs1) << shamt);
		} else {
			illegal_instruction();
		}
	} break;
	case 2: // SLTI
	{
		int32_t val = (int32_t)get_register(rs1);
		if (val < imm) {
		    set_register(rd, 1);
		} else {
		    set_register(rd, 0);
		}
	} break;
	case 3: // SLTIU
	{
		uint32_t cnst = (uint32_t)imm;
		if (get_register(rs1) < cnst) {
		    set_register(rd, 1);
		} else {
		    set_register(rd, 0);
		}
	} break;
	case 4: // XORI
	{
		set_register(rd, get_register(rs1) ^ imm);
	} break;
	case 5:
	{
		if (funct7 == 0) {
			// SRLI
			set_register(rd, get_register(rs1) >> shamt);
		} else if (funct7 == 0b0100000) {
			// SRAI
			int32_t val = (int32_t)get_register(rs1);
			val = val >> shamt;
			set_register(rd, (uint32_t)val);
		} else {
			illegal_instruction();
		}
	} break;
	case 6: // ORI
	{
		set_register(rd, get_register(rs1) | imm);
	} break;
	case 7: // ANDI
	{
		set_register(rd, get_register(rs1) & imm);
	} break;
	default:
		illegal_instruction(); break;
	}
}

void RV32Core::execute_STORE(uint32_t insn) {
	int funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
    uint32_t imm = (insn & 0b11111110000000000000000000000000) >> (25-5)
        | (insn & 0b00000000000000000000111110000000) >> 7;
    // sign-extend to 32 bits
    if (imm & 0x00000800) {
    	imm |= 0xFFFFF000;
    }
    int32_t offset = (int32_t)imm;
    int rs2 = (insn & 0b00000001111100000000000000000000) >> 20;
    int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;

    uint32_t addr = (uint32_t)((int32_t)get_register(rs1) + offset);

    // TODO system bus checks valid address

	switch (funct3) {
	case 0b000: // SB
	{
		uint8_t dout = (uint8_t) (get_register(rs2) & 0x000000FF);
		system_bus->store_byte(addr, dout);
	} break;
	case 0b001: // SH
	{
		uint16_t dout = (uint16_t) (get_register(rs2) & 0x0000FFFF);
		system_bus->store_halfword(addr, dout);
	} break;
	case 0b010: // SW
	{
		uint32_t dout = get_register(rs2);
		system_bus->store_word(addr, dout);
	} break;
	default:
		illegal_instruction(); break;
	}
}

void RV32Core::execute_AMO(uint32_t insn) {
	uint8_t funct7 = (insn & 0b11111110000000000000000000000000) >> 25;
	uint8_t funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
	int rs2 = (insn & 0b00000001111100000000000000000000) >> 20;
	int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;
	int rd = (insn & 0b00000000000000000000111110000000) >> 7;
	// check funct3 = 010
	if (funct3 == 0b010) {
		// early validate address
		uint32_t addr = get_register(rs1);
		// TODO system bus validate address
		uint32_t data_out = get_register(rs2);
		// decode the 5 highest bits of funct7
		uint8_t amofunct = funct7 >> 2;
		switch (amofunct) {
		case 0b00010: // LRW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			system_bus->set_reservation(addr);
		} break;
		case 0b00011: // SCW
		{
			if (system_bus->is_reserved(addr)) {
				system_bus->store_word(addr, data_out);
				system_bus->clear_reservation(addr);
			} else {
				set_register(rd, 1);
			}
		} break;
		case 0b00001: // AMOSWAPW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			system_bus->store_word(addr, data_out);
		} break;
		case 0b00000: // AMOADDW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			system_bus->store_word(addr, data_in + data_out);
		} break;
		case 0b00100: // AMOXORW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			system_bus->store_word(addr, data_in ^ data_out);
		} break;
		case 0b01100: // AMOANDW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			system_bus->store_word(addr, data_in & data_out);
		} break;
		case 0b01000: // AMOORW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			system_bus->store_word(addr, data_in | data_out);
		} break;
		case 0b10000: // AMOMINW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);

			int32_t data_in_s = (int32_t)data_in;
			int32_t data_out_s = (int32_t)data_out;
			if (data_in_s < data_out_s) {
				system_bus->store_word(addr, data_in);
			} else {
				system_bus->store_word(addr, data_out);
			}
		} break;
		case 0b10100: // AMOMAXW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);

			int32_t data_in_s = (int32_t)data_in;
			int32_t data_out_s = (int32_t)data_out;
			if (data_in_s > data_out_s) {
				system_bus->store_word(addr, data_in);
			} else {
				system_bus->store_word(addr, data_out);
			}
		} break;
		case 0b11000: // AMOMINUW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			if (data_in < data_out) {
				system_bus->store_word(addr, data_in);
			} else {
				system_bus->store_word(addr, data_out);
			}
		} break;
		case 0b11100: // AMOMAXUW
		{
			uint32_t data_in = system_bus->load_word(addr);
			set_register(rd, data_in);
			if (data_in > data_out) {
				system_bus->store_word(addr, data_in);
			} else {
				system_bus->store_word(addr, data_out);
			}
		} break;
		default:
			illegal_instruction(); break;
		}
	} else {
		illegal_instruction(); return;
	}
}

void RV32Core::execute_OP(uint32_t insn) {
    // first decode funct7 as MUL/DIV have a different prefix
    uint8_t funct7 = (insn & 0b11111110000000000000000000000000) >> 25;
    uint8_t funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
    int rs2 = (insn & 0b00000001111100000000000000000000) >> 20;
    int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;
    int rd = (insn & 0b00000000000000000000111110000000) >> 7;

    switch (funct7) {
    case 0b0000000:
        // standard integer ops
    {
        switch (funct3) {
        case 0b000: // ADD
            set_register(rd, get_register(rs1) + get_register(rs2));
            break;
        case 0b001: // SLL
            set_register(rd, get_register(rs1) << (get_register(rs2) & 0x0000001F));
            break;
        case 0b010: // SLT
        {
            int32_t x1 = (int32_t)get_register(rs1);
            int32_t x2 = (int32_t)get_register(rs2);
            if (x1 < x2) {
                set_register(rd, 1);
            } else {
                set_register(rd, 0);
            }
        } break;
        case 0b011: // SLTU
        {
            uint32_t x1 = get_register(rs1);
            uint32_t x2 = get_register(rs2);
            if (x1 < x2) {
                set_register(rd, 1);
            } else {
                set_register(rd, 0);
            }
        } break;
        case 0b100: // XOR
            set_register(rd, get_register(rs1) ^ get_register(rs2));
            break;
        case 0b101: // SRL
            set_register(rd, get_register(rs1) >> (get_register(rs2) & 0x0000001F));
            break;
        case 0b110: // OR
            set_register(rd, get_register(rs1) | get_register(rs2));
            break;
        case 0b111: // AND
            set_register(rd, get_register(rs1) & get_register(rs2));
            break;
        default:
            illegal_instruction(); break;
        }
    } break;
    case 0b0000001:
        // MUL/DIV
    {
        switch (funct3) {
        case 0b000: // MUL
        {
            int32_t x1 = (int32_t)get_register(rs1);
            int32_t x2 = (int32_t)get_register(rs2);
            set_register(rd, x1 * x2);
        } break;
        case 0b001: // MULH
        {
            int32_t x1 = (int32_t)get_register(rs1);
            int32_t x2 = (int32_t)get_register(rs2);
            int64_t product = (int64_t)x1 * (int64_t)x2;
            set_register(rd, (uint32_t)(product >> 32));
        } break;
        case 0b010: // MULHSU
        {
            int32_t x1 = (int32_t)get_register(rs1);
            int32_t x2 = (int32_t)get_register(rs2);
            int64_t product = (int64_t)x1 * ((int64_t)x2) & 0x00000000FFFFFFFFl;
            set_register(rd, (uint32_t)(product >> 32));
        } break;
        case 0b011: // MULHU
        {
            int32_t x1 = (int32_t)get_register(rs1);
            int32_t x2 = (int32_t)get_register(rs2);
            int64_t product = ((int64_t)x1)&0x00000000FFFFFFFFL * ((int64_t)x2)&0x00000000FFFFFFFFL;
            set_register(rd, (uint32_t)(product >> 32));
        } break;
        case 0b100: // DIV
        {
            int32_t x1 = (int32_t)get_register(rs1);
            int32_t x2 = (int32_t)get_register(rs2);
            if (x2 == 0) {
                // DIV/0
                set_register(rd, -1);
            } else if (x1 == -2147483648 && x2 == -1) {
                // signed overflow
                set_register(rd, -2147483648);
            } else {
                set_register(rd, x1 / x2);
            }
        } break;
        case 0b101: // DIVU
        {
            uint32_t x1 = get_register(rs1);
            uint32_t x2 = get_register(rs2);
            if (x2 == 0) {
                // DIV/0
                set_register(rd, 0xFFFFFFFF);
            } else {
                set_register(rd, x1 / x2);
            }
        } break;
        case 0b110: // REM
        {
            int32_t x1 = (int32_t)get_register(rs1);
            int32_t x2 = (int32_t)get_register(rs2);
            if (x2 == 0) {
                // DIV/0
                set_register(rd, x1);
            } else if (x1 == -2147483648 && x2 == -1) {
                // signed overflow
                set_register(rd, 0);
            } else {
                set_register(rd, x1 % x2);
            }
        } break;
        case 0b111: // REMU
        {
            uint32_t x1 = get_register(rs1);
            uint32_t x2 = get_register(rs2);
            if (x2 == 0) {
                // DIV/0
                set_register(rd, x1);
            } else {
                set_register(rd, x1 % x2);
            }
        } break;
        default:
            illegal_instruction(); break;
        }
    } break;
    case 0b0100000:
        // other standard integer ops
    {
        switch (funct3) {
        case 0b000: // SUB
            set_register(rd, get_register(rs1) - get_register(rs2));
            break;
        case 0b101: // SRA
        {
            int32_t val = (int32_t)get_register(rs1);
            val = val >> (int32_t)(get_register(rs2) & 0x0000001F);
            set_register(rd, (uint32_t)val);
        } break;
        default:
            illegal_instruction(); break;
        }
    } break;
    default:
        illegal_instruction(); break;
    }
}

void RV32Core::execute_BRANCH(uint32_t insn) {
    uint8_t funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
    int32_t imm = (insn & 0b10000000000000000000000000000000) >> (31-12)
        | (insn & 0b01111110000000000000000000000000) >> (25-5)
        | (insn & 0b00000000000000000000111100000000) >> (8-1)
        | (insn & 0b00000000000000000000000010000000) << (11-7);
    int rs2 = (insn & 0b00000001111100000000000000000000) >> 20;
    int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;

    uint32_t x1_u = get_register(rs1);
    uint32_t x2_u = get_register(rs2);
    int32_t x1_s = (int32_t)get_register(rs1);
    int32_t x2_s = (int32_t)get_register(rs2);

    bool branch_taken;

    switch (funct3) {
    case 0b000: // BEQ
        branch_taken = (x1_u == x2_u); break;
    case 0b001: // BNE
        branch_taken = (x1_u != x2_u); break;
    case 0b100: // BLT
        branch_taken = (x1_s < x2_s); break;
    case 0b101: // BGE
        branch_taken = (x1_s >= x2_s); break;
    case 0b110: // BLTU
        branch_taken = (x1_u < x2_u); break;
    case 0b111: // BGEU
        branch_taken = (x1_u >= x2_u); break;
    default:
        illegal_instruction(); return;
    }

    if (branch_taken) {
        next_pc = (uint32_t) ( (int32_t)pc + imm );
    }
}

void RV32Core::execute_SYSTEM(uint32_t insn) {
    uint8_t funct3 = (insn & 0b00000000000000000111000000000000) >> 12;
    int32_t imm = ( ((int32_t)insn) & (int32_t)0b11111111111100000000000000000000) >> 20;
    int rs1 = (insn & 0b00000000000011111000000000000000) >> 15;
    int rd = (insn & 0b00000000000000000000111110000000) >> 7;

    switch (funct3) {
    case 0b000:
    {
        // decode imm (funct12)
        int imm = (insn & 0b11111111111100000000000000000000) >> 20;
        switch (imm) {
        case 0b000000000000: // SCALL
            processor_trap(11); break;
        case 0b000000000001: // SBREAK
            processor_trap(3); break;
        case 0b000100000000: // ERET
            // pop the interrupt stack to the right and set
            // the leftmost entry to interrupts enabled
            mstatus_ie = mstatus_ie1;
            mstatus_ie1 = true;
            next_pc = mepc;
            // counts as a context switch
            system_bus->clear_all_reservations();
            break;
        default:
            illegal_instruction(); break;
        }
    } break;
    case 0b001: // CSRRW
    {
        int csr = imm;
        uint32_t old_value = read_csr(csr);
        write_csr(csr, get_register(rs1));
        set_register(rd, old_value);
    } break;
    case 0b010: // CSRRS
    {
        int csr = imm;
        uint32_t old_value = read_csr(csr);
        if (rs1 != 0) {
            write_csr(csr, old_value | get_register(rs1));
        }
        set_register(rd, old_value);
    } break;
    case 0b011: // CSRRC
    {
        int csr = imm;
        uint32_t old_value = read_csr(csr);
        if (rs1 != 0) {
            write_csr(csr, old_value & ~get_register(rs1));
        }
        set_register(rd, old_value);
    } break;
    case 0b101: // CSRRWI
    {
        int csr = imm;
        uint32_t old_value = read_csr(csr);
        write_csr(csr, rs1);
        set_register(rd, old_value);
    } break;
    case 0b110: // CSRRSI
    {
        int csr = imm;
        uint32_t old_value = read_csr(csr);
        if (rs1 != 0) {
            write_csr(csr, old_value | rs1);
        }
        set_register(rd, old_value);
    } break;
    case 0b111: // CSRRCI
    {
        int csr = imm;
        uint32_t old_value = read_csr(csr);
        if (rs1 != 0) {
            write_csr(csr, old_value & ~rs1);
        }
        set_register(rd, old_value);
    } break;
    default:
        illegal_instruction(); break;
    }
}
