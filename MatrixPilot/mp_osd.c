// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


#include "defines.h"

#define OSD_UPDATE_GLYPHS 0


// "MATRIX PILOT" with a 0xFF terminal byte
const unsigned char sample[] = {0x17, 0x0B, 0x1E, 0x1C, 0x13, 0x22, 0x00, 0x1A, 0x13, 0x16, 0x19, 0x1E, 0xFF} ;

int lastRoll = 0 ;
int lastPitch = 0 ;


void osd_create_horizon_glyph(char position, char glyph)
{
#if (OSD_UPDATE_GLYPHS == 1)
	if (position < 0x50 || position > 0x5F) return ;
	
	osd_spi_write(0x9, position) ;	// CMAH: set glyph to overwrite	
	
	char a ;
	for (a = 0; a < 54; a++)
	{
		osd_spi_write(0xA, a) ;		// CMAL: set the 4-px chunk of the glyph to overwrite
		
		char line = (53-a)/3 ;
		char d ;
		if (line == glyph || line == glyph + 2) d = 0x00 ;
		else if (line == glyph + 1) d = 0xAA ;
		else d = 0x55 ;
		osd_spi_write(0xB, d) ;		// CMDI: update the data representing the 4-px chunk of the glyph
	}
	
	osd_spi_write(0x8, 0xA0) ;	// CMM: write glyph to NVRAM
#endif
}


void osd_draw_horizon( void )
{
	// Change away from using roll degrees.  Use tangent as the slope.
	struct relative2D matrix_accum ;
	matrix_accum.x = rmat[8] ;
	matrix_accum.y = rmat[6] ;
	long earth_roll = rect_to_polar(&matrix_accum) ;			// binary angle (0 - 256 = 360 degrees)
	earth_roll = (-earth_roll * BYTECIR_TO_DEGREE) >> 16 ;		// switch polarity, convert to -180 - 180 degrees
	
	matrix_accum.y = rmat[7] ;
	long earth_pitch = rect_to_polar(&matrix_accum) ;			// binary angle (0 - 256 = 360 degrees)
	earth_pitch = (-earth_pitch * BYTECIR_TO_DEGREE) >> 16 ;	// switch polarity, convert to -180 - 180 degrees
	
	char i ;
	for (i = -10; i<10; i++)
	{
		char height = (earth_roll * i - earth_pitch * 8 + 60) / 120 ;
		char lastHeight = (lastRoll * i - lastPitch * 8 + 60) / 120 ;
		char subHeight = (((earth_roll * i - earth_pitch * 8 + 60) % 120) * 16 / 120) ;
		if (earth_roll * i - earth_pitch * 8 + 60 < 0) { height-- ; subHeight-- ; }
		if (lastRoll * i - lastPitch * 8 + 60 < 0) lastHeight-- ;
		subHeight &= 0x0F ;
		
		if (height > -6 && height < 6)
		{
			osd_spi_write_location(6-height, 15+i) ;
			osd_spi_write(0x7, 0x50 + subHeight) ;	// DMDI: Write a '-'
		}
		
		if (height != lastHeight && lastHeight > -6 && lastHeight < 6)
		{
			osd_spi_write_location(6-lastHeight, 15+i) ;
			osd_spi_write(0x7, 0x00) ;	// DMDI: Write a ' '
		}
	}
	lastRoll = earth_roll ;
	lastPitch = earth_pitch ;
}


void osd_countdown(int countdown)
{
	// unsigned char x ;
	
	if (countdown == 960)
	{
		osd_spi_write_byte(0xFF) ;	// Terminate sending a string, in case that was happening (Prep for reset)
	}
	else if (countdown == 961)
	{
		osd_spi_write(0x0, 0x02) ;	// VM0: Reset the OSD
	}
	else if (countdown <= 950 && countdown > 918)
	{
		if (countdown%2 == 0)
		{
			char g = (950-countdown) / 2 ;
			osd_create_horizon_glyph(0x50 + g, g) ;
		}
	}
	else if (countdown == 910)
	{
		// automatic black level control, have to read, augment and rewrite
		// The data sheet is rather specific about this
		// x = osd_spi_read(0xEC) ;	// OSDBD
		// x &= 0xEF ;
	}
	else if (countdown == 909)
	{
		// osd_spi_write(0x6C, x) ;	// OSDBL
	}
	else if (countdown == 905)
	{
		osd_spi_write(0x04, 0) ;	// DMM set to 0
	}
	else if (countdown == 900)
	{
		osd_spi_write(0x0, 0x08) ;	// VM0: enable display of OSD image
		
		osd_spi_write_location(12, 9) ;
		osd_spi_write_string(sample) ;
	}
	else if (countdown < 900) {
		osd_spi_write(0x0, 0x08) ;	// VM0: enable display of OSD image
		
		osd_spi_write_location(12, 23) ;
		osd_spi_write_int(calculated_heading) ;
		
		osd_spi_write_location(12, 2) ;
		osd_spi_write_int(countdown) ;
		
		osd_draw_horizon() ;
	}
}
