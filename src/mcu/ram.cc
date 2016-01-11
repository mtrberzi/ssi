#include "ram.h"
#include <cstring>

RAM::RAM(uint32_t nPages)
: nPages(nPages), memory(NULL) {
	memory = (uint8_t*)malloc(sizeof(uint8_t) * 1024 * nPages);
}

RAM::~RAM() {
	if (memory != NULL) {
		free(memory);
		memory = NULL;
	}
}

void RAM::set_contents(uint8_t *contents) {
	if (contents == NULL) {
		return;
	}
	memcpy(memory, contents, 1024*nPages*sizeof(uint8_t));
}

uint8_t RAM::read_byte(uint32_t pAddr) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr < 1024*nPages) {
		return memory[local_addr];
	} else {
		return 0;
	}
}

uint16_t RAM::read_halfword(uint32_t pAddr) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr+1 < 1024*nPages) {
		uint16_t retval = (uint16_t)memory[local_addr+0];
		retval |= ((uint16_t)memory[local_addr+1]) << 8;
		return retval;
	} else {
		return 0;
	}
}

uint32_t RAM::read_word(uint32_t pAddr) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr+3 < 1024*nPages) {
		uint32_t retval = (uint32_t)memory[local_addr+0];
		retval |= ((uint32_t)memory[local_addr+1]) << 8;
		retval |= ((uint32_t)memory[local_addr+2]) << 16;
		retval |= ((uint32_t)memory[local_addr+3]) << 24;
		return retval;
	} else {
		return 0;
	}
}

void RAM::write_byte(uint32_t pAddr, uint8_t value) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr < 1024*nPages) {
		memory[local_addr] = value;
	}
}

void RAM::write_halfword(uint32_t pAddr, uint16_t value) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr+1 < 1024*nPages) {
		memory[local_addr+0] = (uint8_t)((value & 0x00FF));
		memory[local_addr+1] = (uint8_t)((value & 0xFF00) >>  8);
	}
}

void RAM::write_word(uint32_t pAddr, uint32_t value) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr+3 < 1024*nPages) {
		memory[local_addr+0] = (uint8_t)((value & 0x000000FF));
		memory[local_addr+1] = (uint8_t)((value & 0x0000FF00) >>  8);
		memory[local_addr+2] = (uint8_t)((value & 0x00FF0000) >> 16);
		memory[local_addr+3] = (uint8_t)((value & 0xFF000000) >> 24);
	}
}
