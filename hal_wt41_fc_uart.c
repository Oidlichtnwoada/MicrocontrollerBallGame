#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "common.h"
#include "util.h"
#include "hal_wt41_fc_uart.h"

void (*sendCallback)(void);
void (*receiveCallback)(uint8_t);
static bool callback_is_executed = false;
static bool sendRequest = false;
static bool resetted = false;
static uint8_t buffer[BUFFERSIZE];
static uint8_t sendBuffer;
static uint8_t writeOffset = 0;
static uint8_t readOffset = 0;
static uint8_t elemCount = 0;
static uint32_t counter = 0;

void pushReceive(uint8_t elem) {
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		buffer[writeOffset] = elem;
		writeOffset = writeOffset + 1;
		if (writeOffset == BUFFERSIZE) writeOffset = 0;
		elemCount++;
		if (BUFFERSIZE - elemCount < 5) PORTJ |= 1 << PJ3;
	}
}

uint8_t popReceive(void) {
	uint8_t ret;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		ret = buffer[readOffset];
		readOffset = readOffset + 1;
		if (readOffset == BUFFERSIZE) readOffset = 0;
		elemCount--;
		if (elemCount < 16) PORTJ &= ~(1 << PJ3);
		return ret;
	}
	return ret;
}

error_t halWT41FcUartInit(void (*sndCallback)(void), void (*rcvCallback)(uint8_t)) {

	//initializing PORTJ
	DDRJ |= ~(1 << PJ2 | 1 << PJ0);
	DDRJ &= ~(1 << PJ2 | 1 << PJ0);
	
	//pull PJ5 to low
	PORTJ &= ~(1<<PJ5);
	
	//setting callbacks
	sendCallback = sndCallback;
	receiveCallback = rcvCallback;

	//setting up timer1 for resetting the bluetooth module
    TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0) | _BV(COM1C1) | _BV(COM1C0) | _BV(WGM11) | _BV(WGM10));
    TCCR1B &= ~(_BV(ICNC1) | _BV(WGM13) | _BV(ICES1) | _BV(CS12) | _BV(CS11) | _BV(CS10));
    OCR1A = 100;
    TIMSK1 |= _BV(OCIE1A);
    TCCR1B |= _BV(CS12) | _BV(CS10) | _BV(WGM12);

	//PJ2 change interrupt
	PCICR |= (1<<PCIE1);
	PCMSK1 |= (1<<PCINT11);

	//setting baudrate
	UBRR3H = 0x00;
	UBRR3L = 0x00;

	//configuring USART3
	UCSR3A = (1<<RXC3);
	UCSR3B = (1<<RXEN3)|(1<<TXEN3)|(1<<RXCIE3);
	UCSR3C = (0<<USBS3)|(3<<UCSZ30);

	return SUCCESS;
}

void sendData(void) {
	//send data only if the device is resetted
	if (resetted) {
		UDR3 = sendBuffer;
		UCSR3B &= ~(1<<UDRIE3);
		sendCallback();
		sendRequest = false;
	}
}

error_t halWT41FcUartSend(uint8_t byte) {
    sendBuffer = byte;
    sendRequest = true;
    if (!BIT(PINJ, PJ2)) {
		UCSR3B |= (1<<UDRIE3);
    }
	return SUCCESS;
}

ISR(USART3_RX_vect) {
	pushReceive(UDR3);
	if (!callback_is_executed) {
		callback_is_executed = true;
		sei();
		while (elemCount > 0) {
			receiveCallback(popReceive());
		}
		callback_is_executed = false;
	}
}

ISR (USART3_UDRE_vect) {
	//buffer is free, send data
	sendData();
}

ISR (PCINT1_vect) {
	//if there was a send request and PJ2 goes to low
	if (sendRequest && !BIT(PINJ, PJ2)) {
		UCSR3B |= (1<<UDRIE3);
	}
}


ISR(TIMER1_COMPA_vect)
{		
	counter++;
	
	if (counter == 2) {
		
		//set PJ5 to high
		PORTJ |= 1 << PJ5;
		
		//setting the resetted flag
		resetted = true;
		
		//disabling timer interrupt
		TIMSK1 &= ~( 1 << OCIE1A);
		
		//send the frame a second time if it was sent during reset
		if (sendRequest) {
			halWT41FcUartSend(sendBuffer);
		}
	}
}
