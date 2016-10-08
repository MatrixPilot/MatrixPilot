// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "libUDB.h"
#include "options_osd.h"
#include "osd.h"

#if (USE_OSD == OSD_NATIVE && USE_OSD_SPI == 0)

#warning "OSD_NATIVE in bit-bash mode enabled"

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)

#if ( OSD_LEGACY_SPI_PINOUT == 1)
#warning "Using legacy OSD pinout:- UDB4/5 board labels do not match signals"

#define OSD_CS          _LATF7 
#define OSD_SCK         _LATF8 
#define OSD_MOSI        _LATF6 
// OSD_MISO was not defined in legacy pinout

#define OSD_CS_TRIS     _TRISF7
#define OSD_SCK_TRIS    _TRISF8
#define OSD_MOSI_TRIS   _TRISF6
#define OSD_MISO_TRIS   _TRISD12

#else // OSD_LEGACY_SPI_PINOUT not being used
#warning "Using OSD SPI pinout that matches labelling on UDB4 and UDB5 boards"
#define OSD_CS          _LATB2
#define OSD_SCK         _LATF6 
#define OSD_MOSI        _LATF7 
#define OSD_MISO        _LATF8

#define OSD_CS_TRIS     _TRISB2
#define OSD_SCK_TRIS    _TRISF6
#define OSD_MOSI_TRIS   _TRISF7
#define OSD_MISO_TRIS   _TRISF8

#endif //( OSD_LEGACY_SPI_PINOUT == 1)

#elif (BOARD_TYPE == AUAV3_BOARD)

#define OSD_CS          _LATD2
#define OSD_SCK         _LATD1
#define OSD_MOSI        _LATD3
#define OSD_MISO        _RD12

#define OSD_CS_TRIS     _TRISD2
#define OSD_SCK_TRIS    _TRISD1
#define OSD_MOSI_TRIS   _TRISD3
#define OSD_MISO_TRIS   _TRISD12

#endif // BOARD_TYPE


void osd_spi_init(void)
{
	OSD_MOSI_TRIS = 0;
	OSD_CS_TRIS   = 0;
	OSD_SCK_TRIS  = 0;

	OSD_MOSI = 1;
	OSD_CS   = 1;
	OSD_SCK  = 1;
}

void spi_write_raw_byte(uint8_t byte)
{
	uint8_t SPICount;            // Counter used to clock out the data

	for (SPICount = 0; SPICount < 8; SPICount++) // Prepare to clock out the Address byte
	{
		uint8_t outBit = ((byte & 0x80) != 0); // Check for a 1 and set the MOSI line appropriately
		if (outBit) OSD_MOSI = 1; // Write this bit using the bit-set / bit-clear instrictions
		else 		OSD_MOSI = 0;
		OSD_SCK = 1;            // Toggle the clock line up
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop();    // Kill some time with SCK high to make a more solid pulse
		byte <<= 1;             // Shift to get the next bit
		OSD_SCK = 0;            // Toggle the clock line back down
	}
}

void osd_spi_write_byte(int8_t byte)
{
	OSD_CS = 1;                 // Make sure we start with active-low CS high
	OSD_SCK = 0;                // and CK low
	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(byte);   // Send the data
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 
	Nop(); Nop(); Nop(); Nop(); // Kill some time with CS high to make a more solid pulse
	OSD_MOSI = 0;
}

void osd_spi_write(int8_t addr, int8_t byte)
{
	OSD_CS = 1;                 // Make sure we start with active-low CS high
	OSD_SCK = 0;                // and CK low
	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr);   // Send the Address
	spi_write_raw_byte(byte);   // Send the data
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 
	Nop(); Nop(); Nop(); Nop(); // Kill some time with CS high to make a more solid pulse
	OSD_MOSI = 0;
}

/*
uint8_t spi_read_raw_byte( void )
{
	uint8_t SPICount;           // Counter used to clock out the data
	uint8_t SPIData = 0;        // Counter used to clock out the data

	for (SPICount = 0; SPICount < 8; SPICount++) // Prepare to clock out the Address byte
	{
		SPIData <<= 1;          // Rotate the data
		OSD_SCK = 1;            // Raise the clock to clock the data out of the MAX7456
		if (OSD_MISO) SPIData |= 1; // Read the data bit
		OSD_SCK = 0;            // Drop the clock ready for the next bit
	}
	return SPIData;
}

uint8_t osd_spi_read(int8_t addr)
{
	uint8_t SPIData = 0;

	OSD_CS = 1;                 // Make sure we start with active-low CS high
	OSD_SCK = 0;                // and CK low
	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr);   // Send the Address
	OSD_MOSI = 0;
	SPIData = spi_read_raw_byte(); // Send the data
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 
	Nop(); Nop(); Nop(); Nop(); // Kill some time with CS high to make a more solid pulse
	return SPIData;
}
*/

#endif // (USE_OSD == OSD_NATIVE && USE_OSD_SPI == 0)

