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


#include "libUDB_internal.h"
//#include "osd.h"

uint8_t osd_spi_read(int8_t addr);

#if (USE_OSD == 1)


//  UDB4 uses SPI1 port
// AUAV3 uses SPI3 port

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)

#define OSD_CS          _LATF7 // _LATB2
#define OSD_SCK         _LATF8 // _LATF6
#define OSD_MOSI        _LATF6 // _LATF7
//#define OSD_MISO        0      // _LATF8

#define OSD_CS_TRIS     _TRISF7
#define OSD_SCK_TRIS    _TRISF8
#define OSD_MOSI_TRIS   _TRISF6
#define OSD_MISO_TRIS   _TRISD12

#define SPIxSTAT        SPI1STAT
#define SPIxCON1        SPI1CON1
#define SPIxBUF         SPI1BUF
#define SPIxSTATbits    SPI1STATbits
#define SPIxCON1bits    SPI1CON1bits

#elif (BOARD_TYPE == AUAV3_BOARD)

#define OSD_CS          _LATD2
#define OSD_SCK         _LATD1
#define OSD_MOSI        _LATD3
#define OSD_MISO        _RD12

#define OSD_CS_TRIS     _TRISD2
#define OSD_SCK_TRIS    _TRISD1
#define OSD_MOSI_TRIS   _TRISD3
#define OSD_MISO_TRIS   _TRISD12

#define SPIxSTAT        SPI3STAT
#define SPIxCON1        SPI3CON1
#define SPIxBUF         SPI3BUF
#define SPIxSTATbits    SPI3STATbits
#define SPIxCON1bits    SPI3CON1bits

#endif


extern void __delay32(unsigned long cycles);

#define SF 5

void osd_reset(void)
{
	osd_spi_write(0x00, 0x42);    // VM0: enable display of PAL OSD image, force software reset
	__delay32(400000 * SF);
//	osd_spi_write(0x00, 0x08);    // VM0: enable display of NTSC OSD image
	osd_spi_write(0x00, 0x48);    // VM0: enable display of PAL OSD image

//	osd_spi_write(0x03, 0x00);    // VOS set to +15 pixels (farthest up)
//	osd_spi_write(0x03, 0x10);    // VOS set to +-0 pixels (no shift, default)
	osd_spi_write(0x03, 0x1F);    // VOS set to -15 pixels (farthest down)
//	osd_spi_write(0x03, 0x10);    // VOS set to -8 pixels

//	osd_spi_write(0x04, 0x00);    // DMM set to 0
	osd_spi_write(0x04, 0x04);    // DMM set to clear display memory

	__delay32(20000 * SF);
}

void udb_init_osd(void)
{
	uint8_t vm0;

	printf("udb_init_osd()\r\n");

	SPIxSTAT = 0x0;               // disable the SPI module (just in case)
	SPIxCON1 = 0x0122;            // DISSCK = 0, DISSDO = 0, MODE16 = 0; SMP = 0;      CKE = 1;
	                              //   SSEN = 0;    CKP = 0;  MSTEN = 1; SPRE = 0b000, PPRE = 0b10
	SPIxSTAT = 0x8000;            // enable the SPI module

	OSD_MISO_TRIS = 1;
	OSD_CS_TRIS = 0;
	OSD_SCK_TRIS = 0;
	OSD_MOSI_TRIS = 0;
	OSD_SCK = 1;
	OSD_MOSI = 1;
	OSD_CS = 1;

	osd_reset();

	vm0 = osd_spi_read(0x80);
	if (vm0 & 0x40) {
		printf("OSD PAL mode\r\n");
	} else {
		printf("OSD NTSC mode\r\n");
	}
}


void spi_write_raw_byte(uint8_t byte)
{
	short temp;

	temp = SPIxBUF;                    // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = byte;                    // write the data out to the SPI peripheral
	while (!SPIxSTATbits.SPIRBF) {}    // wait for the data to be sent out
}


void osd_spi_write_byte(int8_t byte)
{
	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(byte);   // Send the data
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 
}


void osd_spi_write(int8_t addr, int8_t byte)
{
	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr);   // Send the Address
	spi_write_raw_byte(byte);   // Send the data
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 
}


uint8_t spi_xfer_raw_byte(uint8_t data)
{
	uint8_t SPIData;

	SPIData = SPIxBUF;              // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = data;                 // write the data out to the SPI peripheral
	while (!SPIxSTATbits.SPIRBF) {} // wait for the data to be transferred
	SPIData = SPIxBUF;
	return SPIData;
}


uint8_t osd_spi_read(int8_t addr)
{
	uint8_t SPIData;

	OSD_CS = 0;                     // Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr);       // Send the Address
	__delay32(20000 * SF);
	SPIData = spi_xfer_raw_byte(0);
	OSD_CS = 1;                     // Set active-low CS high to end the SPI cycle 
	return SPIData;
}


void osd_spi_write_location(int16_t loc)
{
	osd_spi_write(0x05, (uint8_t)(loc>>8));     // DMAH
	osd_spi_write(0x06, (uint8_t)(loc & 0xFF)); // DMAL
}


void osd_spi_write_string(const uint8_t *str)
{
	osd_spi_write(0x04,1);          // DMM: Enable auto-increment mode
	
	while (1)
	{
		osd_spi_write_byte(*str);   // Disables auto-increment mode when sending 0xFF at the end of a string
		if (*str == 0xFF) break;
		str++;
	}
}


void osd_spi_write_vertical_string_at_location(int16_t loc, const uint8_t *str)
{
	while (1)
	{
		if (*str == 0xFF) break;
		if (loc >= 480) break;      // 30*16
		osd_spi_write_location(loc);
		osd_spi_write(0x07, *str);
		str++;
		loc += 30;
	}
}


void osd_spi_erase_chars(uint8_t n)
{
	osd_spi_write(0x04,1);      // DMM: Enable auto-increment mode

	while (n)
	{
		osd_spi_write_byte(0);  // Write a blank space
		n--;
	}
	osd_spi_write_byte(0xFF);   // Disable auto-increment mode 
}


void osd_spi_write_number(int32_t val, int8_t num_digits, int8_t decimal_places, int8_t num_flags, int8_t header, int8_t footer)
{
	int startWriting = 0;
	int32_t d;

	osd_spi_write(0x04,1);      // DMM: Enable auto-increment mode

	if (header)
		osd_spi_write_byte(header);

	if (num_flags & NUM_FLAG_SIGNED)
	{
		if (val < 0)
		{
			osd_spi_write_byte(0x49);   // '-'
			val = -val;
		}
		else
			osd_spi_write_byte(0x00);   // ' '
	}

	switch (num_digits)
	{
		case 0:
		case 10:
			d = (val / 1000000000);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(0x80 + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x8A);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*1000000000;

		case 9:
			d = (val / 100000000);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 9) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*100000000;

		case 8:
			d = (val / 10000000);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 8) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*10000000;

		case 7:
			d = (val / 1000000);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 7) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*1000000;

		case 6:
			d = (val / 100000);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 6) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*100000;

		case 5:
			d = (val / 10000);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 5) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*10000;

		case 4:
			d = (val / 1000);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 4) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*1000;

		case 3:
			d = (val / 100);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 3) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*100;

		case 2:
			d = (val / 10);
			if (d) startWriting = 1;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 2) ? 0xE0 : 0x80) + d);
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80);
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00);
			val -= d*10;

		case 1:
			osd_spi_write_byte(((decimal_places == 1) ? 0xE0 : 0x80) + val);
	}

	if (footer)
		osd_spi_write_byte(footer);

	if (num_digits == 0)
		osd_spi_write_byte(0x00);
	
	osd_spi_write_byte(0xFF);       // Disables auto-increment mode
}

void osd_set_xy(int x, int y)
{
	int loc = OSD_LOC(x, y);
	osd_spi_write(0x05, (uint8_t)(loc>>8));     // DMAH
	osd_spi_write(0x06, (uint8_t)(loc & 0xFF)); // DMAL
}

#if 0
void osd_print(int8_t *str)
{
	osd_spi_write(0x04,1);             // DMM: Enable auto-increment mode

	while (*str)
	{
		osd_spi_write_byte((*str) - 60);
		str++;
	}
	osd_spi_write_byte(0xFF);          // Disables auto-increment mode when sending 0xFF at the end of a string
}
#else

void spi_write_raw_short(unsigned short data)
{
	unsigned short temp;

	temp = SPIxBUF;                    // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = data;                    // write the data out to the SPI peripheral
	while (!SPIxSTATbits.SPIRBF) {}    // wait for the data to be sent out
}

void spi_write_raw_fast(unsigned short byte)
{
	unsigned short temp;

	temp = SPIxBUF;                    // dummy read of the SPIxBUF register to clear the SPIRBF flag
	SPIxBUF = byte;                    // write the data out to the SPI peripheral 
	while (!SPIxSTATbits.SPIRBF) {}    // wait for the data to be sent out
}


void osd_print(int8_t *str)
{
//	uint8_t ch;
	unsigned short ch;

	osd_spi_write(0x04,1);      // DMM: Enable auto-increment mode

	SPIxSTATbits.SPIEN = 0;     // disable the SPI module
	SPIxCON1bits.MODE16 = 1;
	SPIxSTATbits.SPIEN = 1;     // enable the SPI module

	OSD_CS = 0;                 // Set active-low CS low to start the SPI cycle 

	while (*str)
	{
		if (*str == ' ') {
			ch = 138;
		} else {
//			ch = ((*str) - 60); // write the data out to the SPI peripheral
			ch = ((*str) + 42); // write the data out to the SPI peripheral
		}
		ch = ch * 256;

//		OSD_CS = 0;             // Set active-low CS low to start the SPI cycle 
//		spi_write_raw_fast(ch);
		spi_write_raw_short(ch);
//		spi_write_raw_fast(0);
//		OSD_CS = 1;             // Set active-low CS low to start the SPI cycle 

		str++;
	}
	OSD_CS = 1;                 // Set active-low CS high to end the SPI cycle 

	SPIxSTATbits.SPIEN = 0;     // disable the SPI module
	SPIxCON1bits.MODE16 = 0;
	SPIxSTATbits.SPIEN = 1;     // enable the SPI module

	osd_spi_write_byte(0xFF);   // Disables auto-increment mode
}

#endif // 0

#endif // USE_OSD
