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


////////////////////////////////////////////////////////////////////////////////
// On Screen Display
//
// Customize the OSD Layout in the osd_layout.h file.
//

#define USE_OSD         0              // USE_OSD enables the OSD system
//#define USE_OSD         OSD_NATIVE   // USE_OSD enables the OSD system
//#define USE_OSD         OSD_REMZIBI

#define USE_OSD_SPI     1   // set this to 1 to use the SPI peripheral, 0 to bit-bash
#define OSD_SF          5   // scale factor for SPI delays - TODO: get rid of

#define OSD_NTSC        0
#define OSD_PAL         1

#define MAX7456_VM0     0x00   // Video Mode 0
#define MAX7456_VM1     0x01   // Video Mode 1
#define MAX7456_HOS     0x02   // Horizontal Offset
#define MAX7456_VOS     0x03   // Vertical Offset
#define MAX7456_DMM     0x04   // Display Memory Mode
#define MAX7456_DMAH    0x05   // Display Memory Address High
#define MAX7456_DMAL    0x06   // Display Memory Address Low
#define MAX7456_DMDI    0x07   // Display Memory Data In
#define MAX7456_CMM     0x08   // Character Memory Mode
#define MAX7456_CMAH    0x09   // Character Memory Address High
#define MAX7456_CMAL    0x0A   // Character Memory Address Low
#define MAX7456_CMDI    0x0B   // Character Memory Data In
#define MAX7456_OSDM    0x0C   // OSD Insertion Mux

void udb_init_osd(void);
void osd_reset(void);
void osd_run_step(void);
void osd_spi_init(void);

void osd_spi_write(int8_t address, int8_t byte);
void osd_spi_write_byte(int8_t byte); // Used for writing chars while in auto-increment mode
void osd_spi_write_location(int16_t loc); // Set where on screen to write the next char
void osd_spi_write_string(const uint8_t *str); // OSD chars, not ASCII
void osd_spi_write_vertical_string_at_location(int16_t loc, const uint8_t *str);
void osd_spi_erase_chars(uint8_t n);

// Convert Row and Col to a location value for use in osd_spi_write_location()
#define OSD_LOC(ROW, COL) ((ROW)*30+(COL))

#define NUM_FLAG_ZERO_PADDED        1   // When num_digits > 0, left-pad with zeros instead of spaces
#define NUM_FLAG_SIGNED             2   // Reserve space for a - sign to the left of the number
void osd_spi_write_number(int32_t val, int8_t num_digits, int8_t decimal_places, int8_t num_flags, int8_t header, int8_t footer);
// num_digits == 0 means left aligned
// header or footer == 0 means skip the header or footer char
