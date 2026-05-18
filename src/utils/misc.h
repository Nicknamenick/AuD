#ifndef SIM_MISC_H
#define SIM_MISC_H
#include <stdint.h>
#include <stdbool.h>

/* Return next pseudorandom 32-bit value (xorshift32). */
uint32_t rand_xor(void);
/* Return the absolute value of an integer. */
int abs_int(int value);

bool array_contains_int(const int *arr,int size,int value);

#endif
