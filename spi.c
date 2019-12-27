#include <avr/io.h>
#include "spi.h"

void spiInit(void) {
	//activating spi mode 0 and slave mode
	SPCR |= 1 << SPE | 1 << MSTR | 1 << SPR1 | 1 << SPR0;
	SPCR &= ~(1 << CPOL | 1 << CPHA | 1 << DORD | 1 << SPIE);
	//SCK, MOSI, MISO
	DDRB = 0;
	DDRB |= 1 << PB1 | 1 << PB2 | 1 << PB3;
}

void spiSend(uint8_t data) {
	SPDR = data;
	while (!(SPSR >> SPIF & 1)) ;
}

uint8_t spiReceive(void) {
	SPDR = 0xFF;
	while (!(SPSR >> SPIF & 1)) ;
	return SPDR;
}

void spiSetPrescaler(spi_prescaler_t prescaler) {
	SPSR &= ~(1 << SPI2X);
	SPCR &= ~(1 << SPR1 | 1 << SPR0);
	SPCR |= prescaler;
}
