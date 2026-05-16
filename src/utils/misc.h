#ifndef SIM_MISC_H
#define SIM_MISC_H
#include <stdint.h>

/* Return next pseudorandom 32-bit value (xorshift32). */
uint32_t rand_xor(void);

#endif
