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
#include "airspeedCntrlFBW.h"

// The autopilot mopdule is responsible for moving the aircrft to meet
// navigation demands. The output from the autopilot will be fed to the
// axis control loops to make flight adjustments.
// Output is rmat rotations and rates.

#define Q16PI (65536 * 3.14159265)

int16_t alt_hold_pitch_min = ALT_HOLD_PITCH_MIN*(RMAX/57.3);
int16_t alt_hold_pitch_max = ALT_HOLD_PITCH_MAX*(RMAX/57.3);

// roll rate limit in Q16 scale rad/s from deg/s
int32_t nav_roll_rate = NAV_ROLL_RATE_DEFAULT * (65536.0 / 57.3);

// Autopilot demand
_Q16 auto_navigation_error	= 0;

fractional auto_nav_roll_gain = RMAX * AUTO_NAV_ROLL_GAIN;

struct relative2D auto_pitchDemand	= {RMAX, 0};
struct relative2D auto_rollDemand	= {0, RMAX};
struct relative2D auto_yawDemand	= {0, RMAX};

int32_t nav_rollPositionMax =  NAV_MAX_R_ANGLE * 65536.0 / 57.3;

// Calculate the navigation angle from a given actual roatation and a target rotation
// Return in Q16 radians
_Q16 autopilot_calc_nav_rotation( struct relative2D actual, struct relative2D target);

// Calculate the aircraft attitude required for the given navigation error
// Returns aircraft roll angle in Q16 radians
_Q16 determine_navigation_attitude(_Q16 navigation_error, int16_t airspeed );

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

		auto_navigation_error = autopilot_calc_nav_rotation(get_actual_heading(), get_desired_heading() );

		earthrollDemand  = determine_navigation_attitude( auto_navigation_error, get_filtered_airspeed() );

		// Restrict roll rate
		// Divide roll rate by frame rate.

		// temp contains the maximum roll step per timeframe.
		// Convert to Q16 from RMAX scale
		temp.WW = __builtin_mulss( nav_roll_rate >> 2 , (65536.0 / 40.0) ) << 1 ;
		temp.WW = temp._.W1;
	
		if( (earthrollDemand - auto_last_nav_roll_demand) > temp.WW)
			earthrollDemand = auto_last_nav_roll_demand + temp.WW;
		else if( (earthrollDemand - auto_last_nav_roll_demand) < -temp.WW)
			earthrollDemand = auto_last_nav_roll_demand - temp.WW;
	
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
_Q16 autopilot_calc_nav_rotation( struct relative2D actual, struct relative2D target )
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
	desiredX = -target.x;
	desiredY = target.y;

	dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
	crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
	crossprod.WW >>= 12 ; 			// scale result to 2^16
	limitRMAX(crossprod.WW);

	if ( dotprod._.W1 > 0 )
	{
		return -_Q16asin(crossprod.WW);
	}
	else
	{
		if ( crossprod.WW > 0 )
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


// Determine roll attitude depending on navigation error
// Input in Q16 radians
_Q16 determine_navigation_attitude(_Q16 navigation_error, int16_t airspeed)
{
//    union longww temp ;
//	fractional rategain;
	_Q16 roll_demand = 0;

	minifloat nav_roll_gain_mf = RMAXtomf(auto_nav_roll_gain);

	minifloat nav_error_mf;;
	minifloat aspd_gain = ltomf( (long) airspeed);

	minifloat roll_rate_mf = Q16tomf(nav_roll_rate);

	minifloat rategain_mf = Q16tomf( NAV_ROLL_RATE_STANDARD*(65536.0/57.3) );

	//Divide rate by the standard rate to get a rate ratio
	rategain_mf = mf_div(roll_rate_mf, rategain_mf);

	// Take the sqrt of the change
	rategain_mf = mf_sqrt(rategain_mf);

	// TODO - Add airspeed correction
	aspd_gain = mf_div( aspd_gain , ltomf(NAV_ASPD_STANDARD * 100) );


	if(navigation_error >= 0)
		// Take sqrt of navigation error
		nav_error_mf = mf_sqrt(Q16tomf(navigation_error));
	else
		nav_error_mf = mf_inv(Q16tomf(-navigation_error));

	// Correct for the rotation rate gain
	nav_error_mf = mf_mult(nav_error_mf, rategain_mf);	

	// Correct for the user gain-
	nav_error_mf = mf_mult(nav_error_mf, nav_roll_gain_mf);	

	roll_demand = _Q16neg(mftoQ16(nav_error_mf));

	if(roll_demand > (nav_rollPositionMax) )
		return nav_rollPositionMax;
	else if(roll_demand < -nav_rollPositionMax)
		return -nav_rollPositionMax;
	else
		return roll_demand;
}

