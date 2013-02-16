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


// Autopilot demands

fractional auto_navigation_error	= 0;

struct relative2D auto_pitchDemand	= {RMAX, 0};
struct relative2D auto_rollDemand	= {0, RMAX};

fractional nav_rollPositionMax = RMAX * NAV_MAX_R_ANGLE / 90.0;

// Calculate the navigation rotation from a given actual roatation and a target rotation
fractional autopilot_calc_nav_rotation( struct relative2D actual, unsigned char target );

struct relative2D convert_earth_roll_to_rmat(fractional );

inline struct relative2D get_auto_rollDemand(void) {return auto_rollDemand;};
inline struct relative2D get_auto_pitchDemand(void) {return auto_pitchDemand;};


// Multiplexing of demand inputs from different sources
// Calculation of rotations required
void autopilotCntrl( void )
{
	signed char earthpitchDemand 	= 0;
	fractional earthrollDemand 	= 0;

	if ( mode_navigation_enabled() )
	{
		earthpitchDemand 	= (fractional) get_airspeed_pitch_adjustment();

		auto_navigation_error = autopilot_calc_nav_rotation(get_actual_heading(), desired_dir);

		earthrollDemand  = determine_navigation_deflection( 'a' );
	}
	else
	{
		if(get_flightmode() == FLIGHT_MODE_ASSISTED)
		{
			if(fbw_roll_mode == FBW_ROLL_MODE_POSITION)
			{
				earthrollDemand = fbw_desiredRollPosition();
			}
			else
			{
				earthrollDemand = 0;
			}
		}
		else
		{
			earthrollDemand = 0;
		}
	}

	if(earthpitchDemand > alt_hold_pitch_max)
		earthpitchDemand = alt_hold_pitch_max;
	else if(earthpitchDemand < alt_hold_pitch_min)
		earthpitchDemand = alt_hold_pitch_min;

        auto_pitchDemand.x = cosine(earthpitchDemand >> 8);
        auto_pitchDemand.y = sine(earthpitchDemand >> 8);

        auto_rollDemand.x = cosine(-earthrollDemand >> 8);
        auto_rollDemand.y = sine(-earthrollDemand >> 8);
}


fractional autopilot_calc_nav_rotation( struct relative2D actual, unsigned char targetDir )
{
	union longww deflectionAccum ;
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
		deflectionAccum.WW = crossprod._.W1;
	}
	else
	{
		if ( crossprod._.W1 > 0 )
		{
			deflectionAccum._.W1 = RMAX ;
		}
		else
		{
			deflectionAccum._.W1 = -RMAX ; 
		}
	}
	
	return deflectionAccum._.W1 ;
}

// Values for navType:
// 'y' = yaw/rudder, 'a' = aileron/roll, 'h' = aileron/hovering

// Outputs an RMAX scaled value RMAX*SIN(A) where A is the angle difference
// between two angles
int determine_navigation_deflection(char navType)
{
    union longww temp ;

        if (navType == 'h') return -auto_navigation_error ;

	// This is where navigation error turns into roll position

	temp._.W1 = -auto_navigation_error;		// Invert direction here to be rmat compatible
	temp.WW >>= 2;	// Divide now to make sure sqrt does not overflow.

    if(temp._.W1 < 0)
        auto_navigation_error = -sqrt_long(-temp.WW);
    else
        auto_navigation_error = sqrt_long(temp.WW);

    temp.WW = __builtin_mulss( auto_navigation_error, (RMAX * 0.5) ) << 2;
    auto_navigation_error = temp._.W1;

	if(auto_navigation_error > nav_rollPositionMax)
		return nav_rollPositionMax;
	else if(auto_navigation_error < -nav_rollPositionMax)
		return -nav_rollPositionMax;

	return auto_navigation_error;
}

