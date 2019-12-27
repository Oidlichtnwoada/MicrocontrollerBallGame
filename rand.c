#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/atomic.h>
#include "common.h"

//my linear feedback shift register
static uint16_t lfsr = 1;

//shift a bit into the register
uint8_t rand_shift(uint8_t in) {
	uint8_t out = 0;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
	asm volatile ("\n"
		"ror %[in]" "\n\t"
		"ror %B[lfsr]" "\n\t"
		"ror %A[lfsr]" "\n\t"
		"brcc ende%=" "\n\t"
		"ldi %[out], 0x01" "\n\t"
		"ldi r28, 0xE3" "\n\t"
		"ldi r29, 0x80" "\n\t"
		"eor %A[lfsr], r28" "\n\t"
		"eor %B[lfsr], r29" "\n\t"
		"ende%=:" "\n\t"
		: [out] "+r" (out), [lfsr] "+e" (lfsr), [in] "+r" (in)
		: 
		: "r28", "r29"
	);
	}
	return out;
}

//seed the lfsr with random noise
void rand_feed(uint8_t in) {
	rand_shift(in);
}

//get the bit that is shifted out the register
uint8_t rand1(void) {
	return rand_shift(0);
}

//constructing a uint16_t with rand1()
uint16_t rand16(void) {
	uint16_t ret = 0;
	uint8_t i = 0;
	for (; i < 16; i++) {
		ret |= rand1() << i;
	}
	return ret;
}




