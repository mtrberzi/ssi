#include "uuid.h"
#include <random>

std::default_random_engine UUID::rng;

UUID::UUID() {
    std::uniform_int_distribution<uint64_t> dist(0, UINT64_MAX);
    upper_bits = dist(rng);
    // set bits 15-12 to "0100" for a type 4 random UUID
    upper_bits &= 0xFFFFFFFFFFFF0FFFUL;
    upper_bits |= 0x0000000000004000UL;

    lower_bits = dist(rng);
    // set bits 63-62 to "10" for RFC 4122 UUID
    lower_bits &= 0x3FFFFFFFFFFFFFFFUL;
    lower_bits |= 0x8000000000000000UL;
}
