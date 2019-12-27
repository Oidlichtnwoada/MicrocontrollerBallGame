#include <avr/io.h>
#include "common.h"

void initADC(void) {

	//enable ADC, complete interrupt, start on interrupt
	ADMUX = 1 << REFS0;
	ADCSRA = 1 << ADEN | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0 | 1 << ADIE;

	//configuring timer3
    TCCR3A &= ~(_BV(COM3A1) | _BV(COM3A0) | _BV(COM3B1) | _BV(COM3B0) | _BV(COM3C1) | _BV(COM3C0) | _BV(WGM31) | _BV(WGM30));
    TCCR3B &= ~(_BV(ICNC3) | _BV(WGM33) | _BV(ICES3) | _BV(CS32) | _BV(CS31) | _BV(CS30));
    OCR3A = 75;
    TIMSK3 |= _BV(OCIE3A);
    TCCR3B |= _BV(CS32) | _BV(CS30) | _BV(WGM32);
}

//implementing the linearisation function
uint8_t linear(uint8_t volume) {
	uint8_t temp0 = volume;
    uint16_t temp1 = 0;
    temp0 = 255 - temp0;
    temp1 = temp0 * temp0;
    temp1 >>= 8;
    temp1 *= temp0;
    temp1 >>= 8;
    temp1 *= temp0;
    temp1 >>= 8;
    temp0 = temp1;
    temp0 = 255 - temp0;
    return temp0;
}
