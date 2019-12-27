/**
 * author:	Andreas Hagmann <ahagmann@ecs.tuwien.ac.at>
 * date:	21.01.2012
 */

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

typedef enum {
	SPI_PRESCALER_128	= 3,
	SPI_PRESCALER_4		= 0,
	SPI_PRESCALER_16	= 1,
} spi_prescaler_t;

void spiInit(void);
void spiSend(uint8_t data);
uint8_t spiReceive(void);
void spiSetPrescaler(spi_prescaler_t prescaler);

#endif
