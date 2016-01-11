#include "rom.h"
#include <cstring>

ROM::ROM(uint32_t nPages)
: nPages(nPages), memory(NULL) {
	memory = (uint8_t*)malloc(sizeof(uint8_t) * 1024 * nPages);
}

ROM::~ROM() {
	if (memory != NULL) {
		free(memory);
		memory = NULL;
	}
}

void ROM::set_contents(uint8_t *contents) {
	if (contents == NULL) {
		return;
	}
	memcpy(memory, contents, 1024*nPages*sizeof(uint8_t));
}

uint8_t ROM::read_byte(uint32_t pAddr) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr < 1024*nPages) {
		return memory[local_addr];
	} else {
		return 0;
	}
}

uint16_t ROM::read_halfword(uint32_t pAddr) {
	uint32_t local_addr = translate_address(pAddr);
	if (local_addr+1 < 1024*nPages) {
		uint16_t retval = (uint16_t)memory[local_addr+0];
		retval |= ((uint16_t)memory[local_addr+1]) << 8;
		return retval;
	} else {
		return 0;
	}
}

uint32_t ROM::read_word(uint32_t pAddr) {
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
