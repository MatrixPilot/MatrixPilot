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
#include "oscillator.h"
#include "osd_config.h"
#include "osd.h"

//#define SF 2

#if (USE_OSD == OSD_NATIVE)

void osd_reset(void)
{
	osd_spi_write(MAX7456_VM0, 0x42);   // VM0: enable display of PAL OSD image, force software reset
	delay_us(200);                      // The register reset process requires 100µs

//	__delay32(400000UL * SF);
//	osd_spi_write(MAX7456_VM0, 0x08);   // VM0: enable display of NTSC OSD image
//	osd_spi_write(MAX7456_VM0, 0x48);   // VM0: enable display of PAL OSD image

//	osd_spi_write(MAX7456_VOS, 0x00);   // VOS set to +15 pixels (farthest up)
//	osd_spi_write(MAX7456_VOS, 0x10);   // VOS set to +-0 pixels (no shift, default)
//	osd_spi_write(MAX7456_VOS, 0x1F);   // VOS set to -15 pixels (farthest down)
//	osd_spi_write(MAX7456_HOS, 0x00);   // HOS set to -32 pixels (farthest left)
//	osd_spi_write(MAX7456_HOS, 0x20);   // HOS set to +-0 pixels (no offset, default)
//	osd_spi_write(MAX7456_HOS, 0x3F);   // HOS set to +31 pixels (farthest right)

//	osd_spi_write(MAX7456_DMM, 0x00);   // DMM set to 0
//	osd_spi_write(MAX7456_DMM, 0x04);   // DMM set to clear display memory

//	__delay32(20000UL * SF);
}

void udb_init_osd(void)
{
	osd_spi_init();
	osd_reset();
}

void osd_spi_write_location(int16_t loc)
{
	osd_spi_write(0x05, (uint8_t)(loc >> 8));   // DMAH
	osd_spi_write(0x06, (uint8_t)(loc & 0xFF)); // DMAL
}

void osd_spi_write_string(const uint8_t *str)
{
	osd_spi_write(MAX7456_DMM, 1);      // DMM: Enable auto-increment mode
	
	while (1)
	{
		osd_spi_write_byte(*str);       // Disables auto-increment mode when sending 0xFF at the end of a string
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
	osd_spi_write(MAX7456_DMM, 1);      // DMM: Enable auto-increment mode

	while (n)
	{
		osd_spi_write_byte(0);          // Write a blank space
		n--;
	}
	osd_spi_write_byte(0xFF);           // Disable auto-increment mode 
}

void osd_spi_write_number(int32_t val, int8_t num_digits, int8_t decimal_places, int8_t num_flags, int8_t header, int8_t footer)
{
	boolean startWriting = 0;
	int32_t d;

	osd_spi_write(MAX7456_DMM, 1);      // DMM: Enable auto-increment mode

	if (header)
	{
		osd_spi_write_byte(header);
	}
	if (num_flags & NUM_FLAG_SIGNED)
	{
		if (val < 0)
		{
			osd_spi_write_byte(0x49);   // '-'
			val = -val;
		}
		else
		{
			osd_spi_write_byte(0x00);   // ' '
		}
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
	
	osd_spi_write_byte(0xFF);    // Disables auto-increment mode
}

#else

void udb_init_osd(void) {}

#endif // USE_OSD
