#include "system_bus.h"

SystemBus::SystemBus() {

}

SystemBus::~SystemBus() {

}

void SystemBus::attach_peripheral(SystemBusPeripheral *p, uint32_t baseAddress) {
    uint32_t basePage = baseAddress >> 10;
    // validate address
    for (uint32_t i = basePage; i < basePage + p->get_number_of_pages(); ++i) {
        if (i > LAST_VALID_PAGE) {
            return;
        }
        if (mapped_pages.find(i) != mapped_pages.end()) {
            // tried to map two peripherals into the same page
            return;
        }
    }
    for (uint32_t i = basePage; i < basePage + p->get_number_of_pages(); ++i) {
        mapped_pages[i] = p;
    }
}

uint8_t SystemBus::load_byte(uint32_t pAddr) {
    uint32_t pageNumber = (pAddr & 0xFFFFF800) >> 10;
    std::map<uint32_t, SystemBusPeripheral*>::iterator it = mapped_pages.find(pageNumber);
    if (it != mapped_pages.end()) {
        return it->second->read_byte(pAddr);
    } else {
        // bus error
        return 0;
    }
}
uint16_t SystemBus::load_halfword(uint32_t pAddr) {
    uint32_t pageNumber = (pAddr & 0xFFFFF800) >> 10;
    std::map<uint32_t, SystemBusPeripheral*>::iterator it = mapped_pages.find(pageNumber);
    if (it != mapped_pages.end()) {
        return it->second->read_halfword(pAddr);
    } else {
        // bus error
        return 0;
    }
}
uint32_t SystemBus::load_word(uint32_t pAddr) {
    uint32_t pageNumber = (pAddr & 0xFFFFF800) >> 10;
    std::map<uint32_t, SystemBusPeripheral*>::iterator it = mapped_pages.find(pageNumber);
    if (it != mapped_pages.end()) {
        return it->second->read_word(pAddr);
    } else {
        // bus error
        return 0;
    }
}
void SystemBus::store_byte(uint32_t pAddr, uint8_t value) {
    uint32_t pageNumber = (pAddr & 0xFFFFF800) >> 10;
    std::map<uint32_t, SystemBusPeripheral*>::iterator it = mapped_pages.find(pageNumber);
    if (it != mapped_pages.end()) {
        it->second->write_byte(pAddr, value);
        clear_reservation(pAddr);
    } else {
        // bus error
    }
}
void SystemBus::store_halfword(uint32_t pAddr, uint16_t value) {
    uint32_t pageNumber = (pAddr & 0xFFFFF800) >> 10;
    std::map<uint32_t, SystemBusPeripheral*>::iterator it = mapped_pages.find(pageNumber);
    if (it != mapped_pages.end()) {
        it->second->write_halfword(pAddr, value);
        clear_reservation(pAddr);
    } else {
        // bus error
    }
}
void SystemBus::store_word(uint32_t pAddr, uint32_t value) {
    uint32_t pageNumber = (pAddr & 0xFFFFF800) >> 10;
    std::map<uint32_t, SystemBusPeripheral*>::iterator it = mapped_pages.find(pageNumber);
    if (it != mapped_pages.end()) {
        it->second->write_word(pAddr, value);
        clear_reservation(pAddr);
    } else {
        // bus error
    }
}
