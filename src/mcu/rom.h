#ifndef _MCU_ROM_
#define _MCU_ROM_

#include <cstdint>
#include "system_bus.h"

class ROM: public SystemBusPeripheral {
public:
	ROM(uint32_t nPages);
	virtual ~ROM();

	uint32_t get_number_of_pages() const { return nPages; }
	void set_contents(uint8_t *contents);

    uint8_t read_byte(uint32_t pAddr);
    uint16_t read_halfword(uint32_t pAddr);
    uint32_t read_word(uint32_t pAddr);

    void write_byte(uint32_t pAddr, uint8_t value) {}
    void write_halfword(uint32_t pAddr, uint16_t value) {}
    void write_word(uint32_t pAddr, uint32_t value) {}

    void cycle() {}
    void timestep() {}
protected:
	uint32_t nPages;
	uint8_t * memory;
};

#endif /* MCU_ROM_H_ */
