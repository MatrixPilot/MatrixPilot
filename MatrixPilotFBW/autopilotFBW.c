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


#include "../matrixpilot/defines.h"
#include "../libUDB/libUDB.h"
#include "nav_options.h"
#include "fbwCntrl.h"
#include "motionCntrl.h"
#include "navigateFBW.h"

// The autopilot mopdule is responsible for moving the aircrft to meet
// navigation demands. The output from the autopilot will be fed to the
// axis control loops to make flight adjustments.
// Output is rmat rotations and rates.

#define Q16PI (65536 * 3.14159265)

int16_t alt_hold_pitch_min = ALT_HOLD_PITCH_MIN*(RMAX/57.3);
int16_t alt_hold_pitch_max = ALT_HOLD_PITCH_MAX*(RMAX/57.3);

// roll rate limit in RMAX scale rad/s
int16_t nav_roll_rate = NAV_ROLL_RATE_DEFAULT*(RMAX/57.3);

// Autopilot demand
_Q16 auto_navigation_error	= 0;

fractional auto_nav_roll_gain = RMAX * AUTO_NAV_ROLL_GAIN;

struct relative2D auto_pitchDemand	= {RMAX, 0};
struct relative2D auto_rollDemand	= {0, RMAX};
struct relative2D auto_yawDemand	= {0, RMAX};

fractional nav_rollPositionMax = RMAX * NAV_MAX_R_ANGLE / 90.0;

// Calculate the navigation angle from a given actual roatation and a target rotation
// Return in Q16 radians
_Q16 autopilot_calc_nav_rotation( struct relative2D actual, _Q16 target );

// Calculate the aircraft attitude required for the given navigation error
// Returns aircraft roll angle in Q16 radians
_Q16 determine_navigation_attitude(signed char navigation_error);

// The last autopilot navigation roll demand in Q16 radians
_Q16 auto_last_nav_roll_demand = 0;

inline struct relative2D get_auto_rollDemand(void) {return auto_rollDemand;};
inline struct relative2D get_auto_pitchDemand(void) {return auto_pitchDemand;};


// Multiplexing of demand inputs from different sources
// Calculation of rotations required
void autopilotCntrl( void )
{
	union longww temp ;
	_Q16 earthpitchDemand 	= 0;
	_Q16 earthrollDemand 	= 0;
	_Q16 limit;

	if ( mode_navigation_enabled() )
	{
		earthpitchDemand 	= (fractional) get_airspeed_pitch_adjustment();

		auto_navigation_error = autopilot_calc_nav_rotation(get_actual_heading(), desired_dir_q16);

		earthrollDemand  = determine_navigation_attitude( auto_navigation_error );

		// Restrict roll rate
		// Divide roll rate by frame rate.

		// temp contains the maximum roll step per timeframe.
		temp.WW = __builtin_mulss( nav_roll_rate , (RMAX/40.0) ) << 1 ;
	
		if( (earthrollDemand - auto_last_nav_roll_demand) > temp._.W1)
			earthrollDemand = auto_last_nav_roll_demand + temp._.W1;
		else if( (earthrollDemand - auto_last_nav_roll_demand) < -temp._.W1)
			earthrollDemand = auto_last_nav_roll_demand - temp._.W1;
	
		auto_last_nav_roll_demand = earthrollDemand;
	}
	else
	{

		if(get_flightmode() == FLIGHT_MODE_ASSISTED)
		{
			switch(fbw_get_pitch_mode())
			{
			case FBW_PITCH_MODE_NONE:
				break;
			case FBW_PITCH_MODE_ASPD:
				earthpitchDemand 	= get_airspeed_pitch_adjustment();
				break;
			case FBW_PITCH_MODE_PITCH:
				earthpitchDemand 	= fbw_desiredPitchPosition();
				break;
			}

			switch(fbw_get_roll_mode())
			{
			case FBW_ROLL_MODE_POSITION:
				earthrollDemand = fbw_desiredRollPosition();
				break;
			default:
				earthrollDemand = 0;
				earthpitchDemand = 0;
				break;
			}
		}
		else
		{
			earthrollDemand = 0;
			earthpitchDemand = 0;
		}
		
		//Copy roll demand to nav roll demand to make sure transitions are smooth
		auto_last_nav_roll_demand = earthrollDemand;
	}

	limit = ((_Q16) alt_hold_pitch_max) << 2;
	if(earthpitchDemand > limit)
		earthpitchDemand = limit;

	limit = ((_Q16) alt_hold_pitch_min) << 2;
	if(earthpitchDemand < limit)
		earthpitchDemand = limit;

		// TODO - remove this //		
//		earthpitchDemand = 1;

		// translate earth polar to RMAX scaled rectangular

        auto_pitchDemand.x = _Q16cos(earthpitchDemand) >> 2;
        auto_pitchDemand.y = _Q16sin(earthpitchDemand) >> 2;

        auto_rollDemand.x = _Q16cos(-earthrollDemand) >> 2;
        auto_rollDemand.y = _Q16sin(-earthrollDemand) >> 2;
}

// return error in navigation target in Q16 radians
_Q16 autopilot_calc_nav_rotation( struct relative2D actual, _Q16 targetDir )
{
	union longww dotprod ;
	union longww crossprod ;
	int16_t desiredX ;
	int16_t desiredY ;
	int16_t actualX ;
	int16_t actualY ;
	
	actualX = actual.x;
	actualY = actual.y;

	// Q16 operation on target directions and scale to RMAX	
	desiredX = -_Q16cos( targetDir ) >> 2;
	desiredY = _Q16sin( targetDir ) >> 2;

	dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
	crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
	crossprod.WW = crossprod.WW<<3 ; // at this point, we have 1/2 of the cross product
									// cannot go any higher than that, could get overflow
	if ( dotprod._.W1 > 0 )
	{
		return -_Q16asin(crossprod._.W1);
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			return -(Q16PI / 2.0) ;
		}
		else
		{
			return (Q16PI / 2.0) ; 
		}
	}
	
	return 0 ;
}


// 
_Q16 determine_navigation_attitude(signed char navigation_error)
{
    union longww temp ;
	fractional rategain;
	_Q16 roll_demand = 0;

	// This is where navigation error turns into roll position

	// Roll rate correction of gain
	if(nav_roll_rate < (NAV_ROLL_RATE_DEFAULT*(RMAX/57.3)) )
	{
//		temp._.W1 = nav_roll_rate ;
		temp.WW = __builtin_mulss( nav_roll_rate, 244 );	//(NAV_ROLL_RATE_DEFAULT*(RMAX/57.3)/128.0)
		rategain = temp._.W0; 
		temp.WW >>= 7;
		rategain = sqrt_int(temp._.W0) << 7;
			//__builtin_divsd ( temp.WW , (NAV_ROLL_RATE_DEFAULT*(RMAX/57.3)/128.0) ) << 7;
	}
	else
		rategain = RMAX;


	// TODO - Add airspeed correction

	temp._.W0 = -(int16_t)navigation_error;		// Invert direction here to be rmat compatible
	temp._.W0 <<= 8;	// Put in upper byte range for sqrt

    if(temp._.W0 < 0)
        temp._.W0 = -sqrt_int(-temp._.W0);
    else
        temp._.W0 = sqrt_int(temp._.W0);

	temp._.W1 = 0;	// This line for debug only TODO - remove
	// TODO - Fix the gain here, original = 0.87
    temp.WW = __builtin_mulss( temp._.W0, auto_nav_roll_gain );

	temp.WW =__builtin_mulss( temp._.W1, rategain ) << 2;
	roll_demand = temp.WW;

	if(roll_demand > (nav_rollPositionMax) )
		return nav_rollPositionMax;
	else if(roll_demand < -nav_rollPositionMax)
		return -nav_rollPositionMax;
	else
		return roll_demand;
}

