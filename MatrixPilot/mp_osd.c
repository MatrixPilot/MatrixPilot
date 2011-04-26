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


#include "defines.h"
#include "../libDCM/libDCM_internal.h"


#if (USE_OSD == 1)


//#define VARIOMETER_LOW		5
//#define VARIOMETER_HIGH		24


#if (OSD_VIDEO_FORMAT == OSD_NTSC)
#define OSD_SPACING			4
#else
#define OSD_SPACING			5
#endif


// callsign
const unsigned char callsign[] = OSD_CALL_SIGN ;

unsigned char osd_phase = 0 ;
boolean osd_was_on = 0 ;


#if (OSD_SHOW_HORIZON == 1)

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

#endif


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
	osd_spi_write_location(1, 4) ;
	osd_spi_write(0x7, 0xA6) ;			// Altitude symbol
	
	//osd_spi_write_location(1, 7) ;
	//osd_spi_write(0x7, 0xE8) ;		// CPU symbol
	//osd_spi_write_location(1, 11) ;
	//osd_spi_write(0x7, 0xA5) ;		// % symbol
	
	osd_spi_write_location(1, 13) ;
	osd_spi_write(0x7, 0xA7) ;			// Distance symbol
	
	osd_spi_write_location(1, 21) ;
	osd_spi_write(0x7, 0xAB) ;			// Direction symbol
	osd_spi_write_location(1, 25) ;
	osd_spi_write(0x7, 0x4D) ;			// Degrees symbol
	
	osd_spi_write_location(2, 25) ;
	//osd_spi_write(0x7, 0xDD) ;		// m/s symbol
	osd_spi_write(0x7, 0xDF) ;			// mi/hr symbol
	//osd_spi_write(0x7, 0xDE) ;		// km/hr symbol
	
#if (OSD_SHOW_HORIZON == 1)
	osd_spi_write_location(OSD_SPACING + 3, 14) ;
	osd_spi_write(0x7, 0x4E) ;			// center dot
	osd_spi_write_location(OSD_SPACING + 3, 15) ;
	osd_spi_write(0x7, 0x4F) ;			// center dot
	
	osd_spi_write_location(OSD_SPACING + 3, 4) ;
	osd_spi_write(0x7, 0xF1) ;			// horizon center
	osd_spi_write_location(OSD_SPACING + 3, 25) ;
	osd_spi_write(0x7, 0xF0) ;			// horizon center
#endif
	
	osd_spi_write_location(12, 3) ;
	osd_spi_write(0x7, 0xEB) ;			// Sat dish symbol
	
	//osd_spi_write_location(0, 12) ;
	//osd_spi_write_location(2 * OSD_SPACING + 5, 12) ;
	osd_spi_write_vertical_string_at_location(0, 28, callsign) ;	// callsign
	
	return ;
}


void osd_update_values( void )
{
	switch (osd_phase)
	{
		case 0:
		{
			osd_spi_write_location(1, 5) ;
			osd_spi_write_number(IMUlocationz._.W1, 0, NUM_FLAG_SIGNED, 0, 0) ;		// Altitude
			
			osd_spi_write_location(2, 4) ;
			osd_spi_write_number(IMUvelocityz._.W1, 0, NUM_FLAG_SIGNED, 0, 0) ;		// Variometer
			/*
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
			*/
			
			//osd_spi_write_location(5, 8) ;
			//osd_spi_write_number(udb_cpu_load(), 3, 0, 0, 0) ;					// CPU
			
			osd_spi_write_location(1, 11) ;
			if (!flags._.pitch_feedback)
				osd_spi_write(0x7, 0x97) ;						// M : Manual Mode
			else if (!flags._.GPS_steering)
				osd_spi_write(0x7, 0x9D) ;						// S : Stabilized Mode
			else if (udb_flags._.radio_on && !flags._.rtl_hold)
				osd_spi_write(0x7, 0xA1) ;						// W : Waypoint Mode
			else if (flags._.rtl_hold && udb_flags._.radio_on)
				osd_spi_write(0x7, 0x92) ;						// H : RTL Hold, has signal
			else
				osd_spi_write(0x7, 0x9C) ;						// R : RTL Mode, lost signal
			break ;
		}
		case 1:
		{
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
			osd_spi_write_number(dist_to_goal, 0, 0, 0, 0) ;	// Distance to wp/home
	
			osd_spi_write_location(2, 14) ;
			osd_write_arrow(dir_to_goal) ;
	
			osd_spi_write_location(1, 22) ;
			// calculated_heading								// 0-255 (ccw, 0=East)
			int angle = (calculated_heading * 180 + 64) >> 7 ;	// 0-359 (ccw, 0=East)
			angle = -angle + 90;								// 0-359 (clockwise, 0=North)
			if (angle < 0) angle += 360 ;						// 0-359 (clockwise, 0=North)
			osd_spi_write_number(angle, 3, NUM_FLAG_ZERO_PADDED, 0, 0) ;	// heading
			
			// Vertical angle from origin to plane
			int verticalAngle = 0 ;
			if (dist_to_goal != 0)
			{
				struct relative2D componentsToPlane ;
				componentsToPlane.x = dist_to_goal ;
				componentsToPlane.y = IMUlocationz._.W1 ;
				verticalAngle = rect_to_polar(&componentsToPlane) ;		// binary angle (0 - 256 = 360 degrees)
				verticalAngle = (verticalAngle * BYTECIR_TO_DEGREE) >> 16 ;	// switch polarity, convert to -180 - 180 degrees
			}
			osd_spi_write_location(3, 3) ;
			osd_spi_write_number(verticalAngle, 0, NUM_FLAG_SIGNED, 0, 0x4D); // Footer: Degree symbol
			break ;
		}
		case 2:
		{
#if (OSD_SHOW_HORIZON == 1)
			osd_update_horizon() ;
#endif
			break ;
		}
		case 3:
		{
			osd_spi_write_location(2, 20) ;
			//osd_spi_write_number(air_speed_3DGPS/100, 5, 0, 0, 0) ;	// speed in m/s
			osd_spi_write_number(air_speed_3DGPS/45, 5, 0, 0, 0) ;		// speed in mi/hr
			//osd_spi_write_number(air_speed_3DGPS/28, 5, 0, 0, 0) ;	// speed in km/hr
			
			osd_spi_write_location(12, 4) ;
			osd_spi_write_number(svs, 0, 0, 0, 0) ;						// Num satelites locked
			
			osd_spi_write_location(2 * OSD_SPACING + 4, 7) ;
			osd_spi_write_number(labs(lat_gps.WW/10), 8, 0, 0, (lat_gps.WW >= 0) ? 0x98 : 0x9D) ; // Footer: N/S
	
			osd_spi_write_location(2 * OSD_SPACING + 4, 17) ;
			osd_spi_write_number(labs(long_gps.WW/10), 9, 0, 0, (long_gps.WW >= 0) ? 0x8F : 0xA1) ; // Footer: E/W
			break ;
		}
	}
	return ;
}


void osd_run_step( void )
{
	boolean osd_on = (OSD_MODE_SWITCH_INPUT_CHANNEL == CHANNEL_UNUSED || udb_pwIn[OSD_MODE_SWITCH_INPUT_CHANNEL] >= 3000 || !udb_flags._.radio_on) ;
	
	int countdown = 0 ;
	if (!dcm_flags._.init_finished && udb_heartbeat_counter < 100)
	{
		countdown = 100 - udb_heartbeat_counter ;
	}
	
	if (countdown == 61)
	{
		osd_spi_write_byte(0xFF) ;	// Terminate sending a string, in case that was happening (Prep for reset)
	}
	else if (countdown == 60)
	{
		osd_spi_write(0x0, 0x02) ;	// VM0: Reset the OSD
	}
	else if (countdown == 48)
	{
		osd_spi_write(0x04, 0) ;	// DMM set to 0
	}
	else if (countdown < 48)
	{
		if (!osd_was_on && osd_on)
		{
#if (OSD_VIDEO_FORMAT == OSD_NTSC)
			osd_spi_write(0x0, 0x08) ;	// VM0: enable display of OSD image, NTSC
#else
			osd_spi_write(0x0, 0x48) ;	// VM0: enable display of OSD image, PAL
#endif
			osd_phase = 0 ;
			osd_setup_screen() ;
			osd_was_on = 1 ;
		}
		else if (osd_was_on && !osd_on)	// just turned off
		{
			osd_spi_write(0x04, 4) ;	// DMM set to 6 (Blank screen)
			
#if (OSD_VIDEO_FORMAT == OSD_NTSC)
			osd_spi_write(0x0, 0x00) ;	// VM0: disable display of OSD image, NTSC
#else
			osd_spi_write(0x0, 0x40) ;	// VM0: disable display of OSD image, PAL
#endif
			
			osd_was_on = 0 ;
		}
		
		if (osd_on)
		{
			osd_update_values() ;
			osd_phase = (osd_phase+1) % 4 ;
		}
	}
	
	return ;
}

#endif
