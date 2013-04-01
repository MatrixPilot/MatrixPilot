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


#include "../MatrixPilot/defines.h"
#include "fbw_options.h"
#include "fbwCntrl.h"
#include "airspeedCntrlFBW.h"
#include "../libDCM/libDCM.h"
#include "airframe.h"


// Initialize to the value from options.h.  Allow updating this value from LOGO/MavLink/etc.
// Stored in 10ths of meters per second
int16_t desiredSpeed = (DESIRED_SPEED*10) ;


// Calculate the target airspeed in cm/s from desiredSpd in dm/s
extern int16_t calc_target_airspeed(int16_t desiredSpd, uint16_t airspeed, uint16_t groundspeed);

// Calculate the airspeed error vs target airspeed including filtering
extern int16_t calc_airspeed_error(void);

// Calculate the airspeed error integral term with filtering and limits
extern int32_t calc_airspeed_int_error(int16_t aspdError, int32_t aspd_integral);

int16_t 	airspeed		= 0;
int16_t 	groundspeed		= 0;
int16_t 	airspeedError	= 0;
int16_t 	target_airspeed	= 0;

int16_t minimum_groundspeed		= MINIMUM_GROUNDSPEED * 100;
int16_t minimum_airspeed		= MINIMUM_AIRSPEED * 100;
int16_t maximum_airspeed		= MAXIMUM_AIRSPEED * 100;
int16_t cruise_airspeed			= CRUISE_AIRSPEED * 100;

//int airspeed_pitch_adjust_rate	= (AIRSPEED_PITCH_ADJ_RATE*(RMAX/(57.3 * 40.0)));

/// Filtered airspeed for less lumpiness
int16_t aspd_3DIMU_filtered = 0;	

inline int16_t get_filtered_airspeed( void ) {return  aspd_3DIMU_filtered;};

void airspeedCntrl(void)
{
	aspd_3DIMU_filtered >>= 1;
	aspd_3DIMU_filtered += air_speed_3DIMU >> 1;

	// If the radio is not on, force the desired airspeed to RTL airspeed
	if (udb_flags._.radio_on == 0)
		desiredSpeed = RTL_AIRSPEED * 10.0;

	target_airspeed = calc_target_airspeed(desiredSpeed, air_speed_3DIMU, ground_speed_3DIMU);

	airspeedError 	= calc_airspeed_error();
// 	airspeed_error_integral.WW = calc_airspeed_int_error(airspeedError, airspeed_error_integral.WW);
	return;
}


// Calculate the required airspeed in cm/s.  desiredSpeed is in dm/s
// airspeed and groundspeed in cm/s
int16_t calc_target_airspeed(int16_t desiredSpd, uint16_t airspeed, uint16_t groundspeed)
{
	union longww accum ;
	int16_t target;

	accum.WW = __builtin_mulsu ( desiredSpd , 10 ) ;
	target = accum._.W0 ;

	if(groundspeed < minimum_groundspeed)
		target += (minimum_groundspeed - groundspeed);

	if(target > maximum_airspeed)
		target = maximum_airspeed;

	if(target < minimum_airspeed)
		target = minimum_airspeed;

	return target;
}


// Calculate the airspeed error vs target airspeed including filtering
int16_t calc_airspeed_error(void)
{
	//Some airspeed error filtering
	airspeedError = airspeedError >> 1;
	airspeedError += ( (target_airspeed - airspeed) >> 1);

	return airspeedError;
}



