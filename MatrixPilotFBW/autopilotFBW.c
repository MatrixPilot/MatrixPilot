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

// The autopilot mopdule is responsible for moving the aircrft to meet
// navigation demands. The output from the autopilot will be fed to the
// axis control loops to make flight adjustments.
// Output is rmat rotations and rates.

int alt_hold_pitch_min = ALT_HOLD_PITCH_MIN*(RMAX/57.3);
int alt_hold_pitch_max = ALT_HOLD_PITCH_MAX*(RMAX/57.3);

int nav_roll_rate = NAV_ROLL_RATE_DEFAULT*(RMAX/57.3);

// Autopilot demands
signed char auto_navigation_error	= 0;

fractional auto_nav_roll_gain = RMAX * AUTO_NAV_ROLL_GAIN;

struct relative2D auto_pitchDemand	= {RMAX, 0};
struct relative2D auto_rollDemand	= {0, RMAX};
struct relative2D auto_yawDemand	= {0, RMAX};

fractional nav_rollPositionMax = RMAX * NAV_MAX_R_ANGLE / 90.0;

// Calculate the navigation angle from a given actual roatation and a target rotation
signed char autopilot_calc_nav_rotation( struct relative2D actual, unsigned char target );

// Calculate the aircraft attitude required for the given navigation error
// Returns aircraft roll angle
signed char determine_navigation_attitude(signed char navigation_error);

// The last autopilot navigation roll demand
// In format of int circular angle
// Needs to be 16bit to have enough underflow for 40Hz update rate.
int auto_last_nav_roll_demand = 0;

inline struct relative2D get_auto_rollDemand(void) {return auto_rollDemand;};
inline struct relative2D get_auto_pitchDemand(void) {return auto_pitchDemand;};


// Multiplexing of demand inputs from different sources
// Calculation of rotations required
void autopilotCntrl( void )
{
	union longww temp ;
	signed char earthpitchDemand 	= 0;
	signed char earthrollDemand 	= 0;
	int			earthrollDemand16 	= 0;

	if ( mode_navigation_enabled() )
	{
		earthpitchDemand 	= (fractional) get_airspeed_pitch_adjustment();

		auto_navigation_error = autopilot_calc_nav_rotation(get_actual_heading(), desired_dir);

		earthrollDemand  = determine_navigation_attitude( auto_navigation_error );

		// Restrict roll rate
		// Divide roll rate by frame rate.
		earthrollDemand16 = earthrollDemand << 8;
		// temp contains the maximum roll step per timeframe.
		temp.WW = __builtin_mulss( nav_roll_rate , (RMAX/40.0) ) << 1 ;
	
		if( (earthrollDemand16 - auto_last_nav_roll_demand) > temp._.W1)
			earthrollDemand16 = auto_last_nav_roll_demand + temp._.W1;
		else if( (earthrollDemand16 - auto_last_nav_roll_demand) < -temp._.W1)
			earthrollDemand16 = auto_last_nav_roll_demand - temp._.W1;
	
		auto_last_nav_roll_demand = earthrollDemand16;
		earthrollDemand = earthrollDemand16 >> 8;

	}
	else
	{

		if(get_flightmode() == FLIGHT_MODE_ASSISTED)
		{
			earthpitchDemand 	= (fractional) get_airspeed_pitch_adjustment();

			if(fbw_roll_mode == FBW_ROLL_MODE_POSITION)
			{
				earthrollDemand = fbw_desiredRollPosition() >> 8;
			}
			else
			{
				earthrollDemand = 0;
				earthpitchDemand = 0;
			}
		}
		else
		{
			earthrollDemand = 0;
			earthpitchDemand = 0;
		}
		
		//Copy roll demand to nav roll demand to make sure transitions are smooth
		auto_last_nav_roll_demand = earthrollDemand << 8;
	}

	if(earthpitchDemand > (alt_hold_pitch_max >> 8))
		earthpitchDemand = (alt_hold_pitch_max >> 8);
	else if(earthpitchDemand < (alt_hold_pitch_min >> 8))
		earthpitchDemand = (alt_hold_pitch_min >> 8);

		// TODO - remove this //		
//		earthpitchDemand = 1;

        auto_pitchDemand.x = cosine(earthpitchDemand);
        auto_pitchDemand.y = sine(earthpitchDemand);

        auto_rollDemand.x = cosine(-earthrollDemand);
        auto_rollDemand.y = sine(-earthrollDemand);
}


signed char autopilot_calc_nav_rotation( struct relative2D actual, unsigned char targetDir )
{
	union longww dotprod ;
	union longww crossprod ;
	int desiredX ;
	int desiredY ;
	int actualX ;
	int actualY ;
	unsigned int yawkp ;
	
	actualX = actual.x;
	actualY = actual.y;
	
	desiredX = -cosine( targetDir ) ;
	desiredY = sine( targetDir ) ;

	dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
	crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
	crossprod.WW = crossprod.WW<<3 ; // at this point, we have 1/2 of the cross product
									// cannot go any higher than that, could get overflow
	if ( dotprod._.W1 > 0 )
	{
		return arcsine(crossprod._.W1);
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			return 64 ;
		}
		else
		{
			return -64 ; // deflectionAccum._.W1 = -64 ; 
		}
	}
	
	return 0 ;
}

signed char determine_navigation_attitude(signed char navigation_error)
{
    union longww temp ;
	fractional rategain;
	signed char roll_demand = 0;

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

	temp._.W0 = -(int)navigation_error;		// Invert direction here to be rmat compatible
	temp._.W0 <<= 8;	// Put in upper byte range for sqrt

    if(temp._.W0 < 0)
        temp._.W0 = -sqrt_int(-temp._.W0);
    else
        temp._.W0 = sqrt_int(temp._.W0);

	temp._.W1 = 0;	// This line for debug only TODO - remove
	// TODO - Fix the gain here, original = 0.87
    temp.WW = __builtin_mulss( temp._.W0, auto_nav_roll_gain );
    roll_demand = temp._.W1;

	temp.WW =__builtin_mulss( roll_demand, rategain ) << 2;
	roll_demand = temp._.W1;

	if(roll_demand > (nav_rollPositionMax >> 8) )
		return (nav_rollPositionMax >> 8);
	else if(roll_demand < -(nav_rollPositionMax >> 8))
		return -(nav_rollPositionMax >> 8);
	else
		return roll_demand;
}

