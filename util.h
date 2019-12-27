#ifndef UTIL_H_
#define UTIL_H_

#include <inttypes.h>

typedef enum {
	SUCCESS,
	ERROR,
	E_TIMEOUT,
	E_NOCARD,
	E_UNKNOWN_CARD,
} error_t;

uint8_t nibble2Ascii(uint8_t val);
char *int2string(int16_t val);
char *hex2string(int16_t val);
#endif
