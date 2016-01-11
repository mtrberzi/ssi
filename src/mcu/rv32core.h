#ifndef _MCU_RV32CORE_
#define _MCU_RV32CORE_

#include <cstdint>
#include "system_bus.h"

class RV32Core {
public:
	RV32Core();
	virtual ~RV32Core();

	void step();
	void execute(uint32_t insn);
	bool interrupts_enabled() const { return mstatus_ie; }
	void external_interrupt();

	SystemBus * get_system_bus() const { return system_bus; }
protected:
	uint32_t xRegister[32];
	uint32_t get_register(int idx) const;
	void set_register(int idx, uint32_t val);

	uint32_t pc;
	uint32_t next_pc;

	SystemBus * system_bus;

	// use a split mstatus register; we only need two fields
	bool mstatus_ie;
	bool mstatus_ie1;
	uint32_t get_mstatus();
	void set_mstatus(uint32_t mstatus);

	uint32_t mscratch;

	// machine exception program counter
	uint32_t mepc;

	// machine exception cause
	uint32_t mcause;

	// machine bad address register
	uint32_t mbadaddr;

	// instructions-retired counter
	uint64_t instret;

	uint32_t read_csr(int csr);
	void write_csr(int csr, uint32_t val);

	void execute_LOAD(uint32_t insn);
	void execute_MISC_MEM(uint32_t insn);
	void execute_OP_IMM(uint32_t insn);
	void execute_STORE(uint32_t insn);
	void execute_AMO(uint32_t insn);
	void execute_OP(uint32_t insn);
	void execute_BRANCH(uint32_t insn);
	void execute_SYSTEM(uint32_t insn);

	void illegal_instruction();
	void processor_trap(uint32_t cause);

};

#endif // _MCU_RV32CORE_
