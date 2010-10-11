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
#include "../libDCM/libDCM_internal.h"


#if (USE_OSD == 1)

// callsign
const unsigned char callsign[] = {0x8B, 0x8C, 0x87, 0x8D, 0x8E, 0xFF} ;


int lastRoll = 0 ;
int lastPitch = 0 ;


void osd_update_horizon( void )
{
	// TODO: Change away from using roll degrees.  Use tangent as the slope.
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
		int h = earth_roll * i - earth_pitch * 16 + 60 ;
		char height = h / 120 ;
		char subHeight = ((h % 120) * 16 / 120) ;
		if (h < 0) { height-- ; subHeight-- ; }
		subHeight &= 0x0F ;
		
		h = lastRoll * i - lastPitch * 16 + 60 ;
		char lastHeight = h / 120 ;
		if (h < 0) lastHeight-- ;
		
		if (height >= -4 && height <= 4)
		{
			osd_spi_write_location(7-height, 15+i) ;
			osd_spi_write(0x7, 0xC0 + subHeight) ;	// DMDI: Write a '-'
		}
		
		if (height != lastHeight && lastHeight >= -4 && lastHeight <= 4)
		{
			osd_spi_write_location(7-lastHeight, 15+i) ;
			osd_spi_write(0x7, 0x00) ;	// DMDI: Write a ' '
		}
	}
	lastRoll = earth_roll ;
	lastPitch = earth_pitch ;
	
	return ;
}


void osd_write_arrow( void )
{
	osd_spi_write(0x04,1) ;		// DMM: Enable auto-increment mode
	int d = desired_dir + 8;
	if (d < 0) d += 256 ;
	osd_spi_write_byte(0x50 + ((d/16) * 2)) ;
	osd_spi_write_byte(0x51 + ((d/16) * 2)) ;
	osd_spi_write_byte(0xFF) ;
	
	return ;
}


void osd_setup_screen( void )
{
	// sat symbol     dist symbol      dir symbol
	// alt symbol                            M/s
	// (cpu symbol %)
	// 
	//                 callsign
	
	osd_spi_write_location(1, 2) ;
	osd_spi_write(0x7, 0xEB) ;			// Sat dish symbol
	
	osd_spi_write_location(2, 2) ;
	osd_spi_write(0x7, 0xA6) ;			// Altitude symbol
	
	// osd_spi_write_location(1, 7) ;
	// osd_spi_write(0x7, 0xE8) ;			// CPU symbol
	// osd_spi_write_location(1, 11) ;
	// osd_spi_write(0x7, 0xA5) ;			// % symbol
	
	osd_spi_write_location(1, 12) ;
	osd_spi_write(0x7, 0xA7) ;			// Distance symbol
	
	osd_spi_write_location(1, 23) ;
	osd_spi_write(0x7, 0xAB) ;			// Direction symbol
	
	osd_spi_write_location(2, 27) ;
	osd_spi_write(0x7, 0xDD) ;			// m/s symbol
	
	osd_spi_write_location(7, 4) ;
	osd_spi_write(0x7, 0xC8) ;			// horizon center
	osd_spi_write_location(7, 25) ;
	osd_spi_write(0x7, 0xC8) ;			// horizon center
	
	osd_spi_write_location(12, 12) ;
	osd_spi_write_string(callsign) ;	// callsign
	
	return ;
}


void osd_update_values( void )
{
	// #sats               dist              dir
	//   alt              arrow        speed
	// (CPU)
	// 
	// long                                  lat
	
	osd_spi_write_location(1, 3) ;
	osd_spi_write_uchar(svs) ;					// Num satelites locked
	
	osd_spi_write_location(2, 3) ;
	osd_spi_write_int(IMUlocationz._.W1) ;		// Altitude
	
	// osd_spi_write_location(1, 8) ;
	// osd_spi_write_uchar((unsigned char)udb_cpu_load()) ;	// CPU
	
	osd_spi_write_location(1, 13) ;
	osd_spi_write_int(tofinish_line) ;			// Distance to wp/home
	
	osd_spi_write_location(2, 14) ;
	osd_write_arrow() ;
	
	osd_spi_write_location(1, 24) ;
	osd_spi_write_uchar(calculated_heading) ;	// heading
	osd_spi_write_location(1, 27) ;
	osd_spi_write(0x7, 0x4D) ;					// Degrees symbol
		
	osd_spi_write_location(2, 22) ;
	osd_spi_write_int(air_speed_magnitude) ;	// speed in m/s
	
	osd_spi_write_location(12, 2) ;
	osd_spi_write_int(IMUlocationx._.W1) ;		// longitude
	
	osd_spi_write_location(12, 23) ;
	osd_spi_write_int(IMUlocationy._.W1) ;		// latitude
	
	return ;
}


char skip = 0 ;

void osd_countdown(int countdown)
{
	// unsigned char x ;
	
	if (countdown == 961)
	{
		osd_spi_write_byte(0xFF) ;	// Terminate sending a string, in case that was happening (Prep for reset)
	}
	else if (countdown == 960)
	{
		osd_spi_write(0x0, 0x02) ;	// VM0: Reset the OSD
	}
	else if (countdown == 950)
	{
		// automatic black level control, have to read, augment and rewrite
		// The data sheet is rather specific about this
		// x = osd_spi_read(0xEC) ;	// OSDBD
		// x &= 0xEF ;
	}
	else if (countdown == 949)
	{
		// osd_spi_write(0x6C, x) ;	// OSDBL
	}
	else if (countdown == 948)
	{
		osd_spi_write(0x04, 0) ;	// DMM set to 0
	}
	else if (countdown == 947)
	{
		osd_spi_write(0x04, 0) ;	// DMM set to 0
		osd_spi_write(0x0, 0x08) ;	// VM0: enable display of OSD image
		
		osd_setup_screen() ;
	}
	else if (countdown < 947)
	{
		if (!skip)
		{
			osd_update_horizon() ;
			osd_update_values() ;
		}
		skip = !skip ;
	}
	
	return ;
}

#endif
