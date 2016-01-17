#ifndef _MODEL_UUID_
#define _MODEL_UUID_

// Simplistic UUID implementation.

#include <random>
#include <cstdint>

class UUID {
public:
    UUID(uint64_t upper, uint64_t lower) : upper_bits(upper), lower_bits(lower) {} // fixed UUID
    UUID(); // random UUID
    ~UUID() {}
protected:
    static std::default_random_engine rng;
    uint64_t upper_bits;
    uint64_t lower_bits;
};

#endif // _MODEL_UUID_
