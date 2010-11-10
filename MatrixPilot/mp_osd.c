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


#define VARIOMETER_LOW		5
#define VARIOMETER_HIGH		24


#if (OSD_VIDEO_FORMAT == OSD_NTSC)
#define OSD_SPACING			4
#else
#define OSD_SPACING			5
#endif


// callsign
#ifdef OSD_CALL_SIGN
const unsigned char callsign[] = OSD_CALL_SIGN ;
#else
const unsigned char callsign[] = {0x97, 0x9A, 0x99, 0x9D, 0x8E, 0xFF}
#endif

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
		
		if (height != 0 || (i != -1 && i != 0))
		{
			if (height >= -OSD_SPACING && height <= OSD_SPACING)
			{
				osd_spi_write_location(OSD_SPACING + 3 - height, 15+i) ;
				osd_spi_write(0x7, 0xC0 + subHeight) ;	// DMDI: Write a '-'
			}
		}
		
		if (lastHeight != 0 || (i != -1 && i != 0))
		{
			if (height != lastHeight && lastHeight >= -OSD_SPACING && lastHeight <= OSD_SPACING)
			{
				osd_spi_write_location(OSD_SPACING + 3 - lastHeight, 15+i) ;
				osd_spi_write(0x7, 0x00) ;	// DMDI: Write a ' '
			}
		}
	}
	lastRoll = earth_roll ;
	lastPitch = earth_pitch ;
	
	return ;
}


void osd_write_arrow( signed char dir_to_goal )
{
	int d = dir_to_goal - 8;
	if (d < 0) d += 256 ;
	d = (15 - (d/16)) * 2 ;
	
	osd_spi_write(0x04,1) ;		// DMM: Enable auto-increment mode
	osd_spi_write_byte(0x50 + d) ;
	osd_spi_write_byte(0x51 + d) ;
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
	
	//osd_spi_write_location(1, 7) ;
	//osd_spi_write(0x7, 0xE8) ;		// CPU symbol
	//osd_spi_write_location(1, 11) ;
	//osd_spi_write(0x7, 0xA5) ;		// % symbol
	
	osd_spi_write_location(1, 12) ;
	osd_spi_write(0x7, 0xA7) ;			// Distance symbol
	
	osd_spi_write_location(1, 22) ;
	osd_spi_write(0x7, 0xAB) ;			// Direction symbol
	osd_spi_write_location(1, 27) ;
	osd_spi_write(0x7, 0x4D) ;			// Degrees symbol
	
	osd_spi_write_location(2, 27) ;
	//osd_spi_write(0x7, 0xDD) ;		// m/s symbol
	osd_spi_write(0x7, 0xDF) ;			// mi/hr symbol
	//osd_spi_write(0x7, 0xDE) ;		// km/hr symbol
	
	osd_spi_write_location(OSD_SPACING + 3, 14) ;
	osd_spi_write(0x7, 0x4E) ;			// center dot
	osd_spi_write_location(OSD_SPACING + 3, 15) ;
	osd_spi_write(0x7, 0x4F) ;			// center dot
	
	osd_spi_write_location(OSD_SPACING + 3, 4) ;
	osd_spi_write(0x7, 0xF1) ;			// horizon center
	osd_spi_write_location(OSD_SPACING + 3, 25) ;
	osd_spi_write(0x7, 0xF0) ;			// horizon center
	
	osd_spi_write_location(2 * OSD_SPACING + 4, 12) ;
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
	
	osd_spi_write_location(1, 4) ;
	osd_spi_write_uchar(svs, 1) ;						// Num satelites locked
	
	osd_spi_write_location(2, 3) ;
	osd_spi_write_int(IMUlocationz._.W1, 1) ;			// Altitude
	
	osd_spi_write_location(OSD_SPACING + 3, 2) ;
	if (IMUvelocityz._.W1 <= -VARIOMETER_HIGH)
		osd_spi_write(0x7, 0xD4) ;						// Variometer down fast
	else if (IMUvelocityz._.W1 > -VARIOMETER_HIGH && IMUvelocityz._.W1 <= -VARIOMETER_LOW)
		osd_spi_write(0x7, 0xD2) ;						// Variometer down slowly
	else if (IMUvelocityz._.W1 > -VARIOMETER_LOW && IMUvelocityz._.W1 < VARIOMETER_LOW)
		osd_spi_write(0x7, 0xD0) ;						// Variometer flat
	else if (IMUvelocityz._.W1 >= VARIOMETER_LOW && IMUvelocityz._.W1 < VARIOMETER_HIGH)
		osd_spi_write(0x7, 0xD1) ;						// Variometer flat
	else if (IMUvelocityz._.W1 >= VARIOMETER_HIGH)
		osd_spi_write(0x7, 0xD3) ;						// Variometer flat
	
	//osd_spi_write_location(1, 8) ;
	//osd_spi_write_uchar(udb_cpu_load(), 0) ;			// CPU
	
	osd_spi_write_location(1, 10) ;
	if (!flags._.pitch_feedback)
		osd_spi_write(0x7, 0x97) ;						// M : Manual Mode
	else if (!flags._.GPS_steering)
		osd_spi_write(0x7, 0x9D) ;						// S : Stabilized Mode
	else if (udb_flags._.radio_on)
		osd_spi_write(0x7, 0xA1) ;						// W : Waypoint Mode
	else
		osd_spi_write(0x7, 0x9C) ;						// R : RTL Mode
	
	signed char dir_to_goal ;
	int dist_to_goal ;
	if (flags._.GPS_steering)
	{
		dir_to_goal = desired_dir - calculated_heading ;
		dist_to_goal = abs(tofinish_line) ;
	}
	else 
	{
		struct relative2D toGoal ;
		toGoal.x = 0 - IMUlocationx._.W1 ;
		toGoal.y = 0 - IMUlocationy._.W1 ;
		dir_to_goal = rect_to_polar ( &toGoal ) - calculated_heading ;
		dist_to_goal = toGoal.x ;
	}
	
	osd_spi_write_location(1, 14) ;
	osd_spi_write_uint(dist_to_goal, 1) ;			// Distance to wp/home
	
	osd_spi_write_location(2, 14) ;
	osd_write_arrow(dir_to_goal) ;
	
	osd_spi_write_location(1, 23) ;
	// calculated_heading								// 0-255 (ccw, 0=East)
	int angle = (calculated_heading * 180 + 64) >> 7 ;	// 0-359 (ccw, 0=East)
	angle = -angle + 90;								// 0-359 (clockwise, 0=North)
	if (angle > 180) angle -= 360 ;						// -179-180 (clockwise, 0=North)
	osd_spi_write_char(angle, 0) ;						// heading
		
	osd_spi_write_location(2, 22) ;
	//osd_spi_write_uint(air_speed_magnitude/100, 0) ;	// speed in m/s
	osd_spi_write_uint(air_speed_magnitude/45, 0) ;		// speed in mi/hr
	//osd_spi_write_uint(air_speed_magnitude/28, 0) ;	// speed in km/hr
	
	osd_spi_write_location(2 * OSD_SPACING + 4, 1) ;
	osd_spi_write_ulong(labs(lat_gps.WW/10), 0) ;
	osd_spi_write_location(2 * OSD_SPACING + 4, 10) ;
	osd_spi_write(0x07, (lat_gps.WW >= 0) ? 0x98 : 0x9D) ;	// N/S
	
	osd_spi_write_location(2 * OSD_SPACING + 4, 18) ;
	osd_spi_write_ulong(labs(long_gps.WW/10), 0) ;
	osd_spi_write_location(2 * OSD_SPACING + 4, 27) ;
	osd_spi_write(0x07, (long_gps.WW >= 0) ? 0x8F : 0xA1) ;	// E/W
	
	return ;
}


char osd_skip = 0 ;

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
#if (OSD_VIDEO_FORMAT == OSD_NTSC)
		osd_spi_write(0x0, 0x08) ;	// VM0: enable display of OSD image, NTSC
#else
		osd_spi_write(0x0, 0x48) ;	// VM0: enable display of OSD image, PAL
#endif
		osd_setup_screen() ;
	}
	else if (countdown < 947)
	{
		if (!osd_skip)
		{
			osd_update_horizon() ;
			osd_update_values() ;
		}
		osd_skip = !osd_skip ;
	}
	
	return ;
}

#endif
