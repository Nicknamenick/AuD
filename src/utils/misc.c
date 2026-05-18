#include "misc.h"
#include <stdint.h>
#include <time.h>

/*
 * Provide a small, fast PRNG (xorshift32) instead of using rand() to avoid
 * clang-tidy warnings about limited randomness and to have a reproducible,
 * lightweight generator suitable for simulation purposes.
 */
static uint32_t rng_state;
static int rng_initialized = 0;

/* Return next pseudorandom 32-bit value (xorshift32). */
uint32_t rand_xor(void) {
    if (!rng_initialized) {
        uint32_t seed = (uint32_t)time(NULL);
        /* Mix in the address to vary seed across runs in the same second */
        seed ^= (uint32_t)(uintptr_t)&rng_state;
        if (seed == 0) seed = 0xDEADBEEF;
        rng_state = seed;
        rng_initialized = 1;
    }
    uint32_t x = rng_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng_state = x;
    return x;
}

int abs_int(const int value) {
    return value < 0 ? -value : value;
}

bool array_contains_int(const int *arr, int size, int value) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == value) {
            return true;
        }
    }
    return false;
}