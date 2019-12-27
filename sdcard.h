/*
simple sdcardlib
Copyright (C) 2011 Andreas Hagmann, Embedded Computing Systems group - TU Wien
thanks for infos and hints go to
 - http://elm-chan.org/docs/mmc/sdinit.png
 - Chris Hinger
 - CC Dharmani (http://www.dharmanitech.com)

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

#ifndef SDCARD_H
#define SDCARD_H

#include <avr/io.h>
#include "util.h"
#include <stdbool.h>
#include "spi.h"

#define SDCARD_PORT	PORTG
#define SDCARD_DDR	DDRG
#define SDCARD_PIN	PING

#define SDCARD_CS	PG1
#define SDCARD_CD	PG2

#define BLOCK_SIZE	32

typedef uint8_t sdcard_block_t[BLOCK_SIZE];

extern bool sdcardAvailable(void); // depreacated, returns always true
extern error_t sdcardInit(void);
extern error_t sdcardReadBlock(uint32_t byteAddress, sdcard_block_t buffer);

#endif

