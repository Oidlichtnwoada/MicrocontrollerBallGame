/*
mp3lib for VS1011E and VS1053B
Copyright (C) 2011 Andreas Hagmann, Embedded Computing Systems group - TU Wien
Copyright (C) 2018 Thomas Lamprecht, Embedded Computing Systems group - TU Wien

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef MP3_H
#define MP3_H

#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>

// pins
#define MP3_PORT	PORTB
#define MP3_DDR		DDRB

#define MP3_CS		PB0
#define MP3_RST		PB4
#define BSYNC		PB5

#define INT_PORT	PORTD
#define	INT_DDR		DDRD
#define	INT_PIN		PIND

#define DREQ		PD0


extern void mp3Init(void (*dataRequestCallback)(void));
extern void mp3SetVolume(uint8_t volume);
extern void mp3SendMusic(uint8_t *buffer);
extern void mp3StartSineTest(void);
extern bool mp3Busy(void);
extern uint8_t mp3GetVS10xxVersion(void); // 2 => VS1011e, 4 => VS1053B

#endif
