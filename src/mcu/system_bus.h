#ifndef _MCU_SYSTEM_BUS_
#define _MCU_SYSTEM_BUS_

#include <cstdint>
#include <map>
#include <unordered_set>

static uint32_t LAST_VALID_PAGE = 0xFFFFFFFF >> 10;

class SystemBusPeripheral {
public:
    SystemBusPeripheral() {}
    virtual ~SystemBusPeripheral() {}
    virtual uint32_t get_number_of_pages() const = 0;

    uint32_t translate_address(uint32_t pAddr) {
        uint32_t v = get_number_of_pages() * 1024;
        // TODO use a better algorithm -- this is very slow
        uint32_t mask = 1;
        while (mask < v) {
            mask = mask << 1;
        }
        mask -= 1;
        return pAddr & mask;
    }

    virtual uint8_t read_byte(uint32_t pAddr) = 0;
    virtual uint16_t read_halfword(uint32_t pAddr) = 0;
    virtual uint32_t read_word(uint32_t pAddr) = 0;

    virtual void write_byte(uint32_t pAddr, uint8_t value) = 0;
    virtual void write_halfword(uint32_t pAddr, uint16_t value) = 0;
    virtual void write_word(uint32_t pAddr, uint32_t value) = 0;

    virtual void cycle() = 0;
    // called once per timestep after all global cycles have completed
    virtual void timestep() = 0;
};

class SystemBus {
public:
    SystemBus();
    ~SystemBus();

    void attach_peripheral(SystemBusPeripheral *p, uint32_t baseAddress);

    uint8_t load_byte(uint32_t pAddr);
    uint16_t load_halfword(uint32_t pAddr);
    uint32_t load_word(uint32_t pAddr);
    void store_byte(uint32_t pAddr, uint8_t value);
    void store_halfword(uint32_t pAddr, uint16_t value);
    void store_word(uint32_t pAddr, uint32_t value);

    void set_reservation(uint32_t addr) { reserved_addresses.insert(addr >> 2); }
    bool is_reserved(uint32_t addr) const { return reserved_addresses.find(addr >> 2) != reserved_addresses.end(); }
    void clear_reservation(uint32_t addr) { reserved_addresses.erase(addr >> 2); }
    void clear_all_reservations() { reserved_addresses.clear(); }

protected:

    std::map<uint32_t, SystemBusPeripheral*> mapped_pages;
    std::unordered_set<uint32_t> reserved_addresses;


};

#endif // _MCU_SYSTEM_BUS_
