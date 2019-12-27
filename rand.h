
#ifndef RAND_H
#define RAND_H

#include <stdint.h>

/**
 * Shift the LFSR to the right, shifting in the LSB of the parameter. Usually
 * not called directly; use the high−level functions below.
 *
 * Returns: The bit shifted out of the LFSR.
 */
uint8_t rand_shift(uint8_t in);

/**
 * Feed one bit of random data to the LFSR (reseeding).
 */
void rand_feed(uint8_t in);

/**
 * Get one bit of random data from the LFSR.
 */
uint8_t rand1(void);

/**
 * Generate a random 16−bit number.
 */
uint16_t rand16(void);

#endif

