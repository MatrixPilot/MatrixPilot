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


#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"
#include "options.h"

union longww throttleFiltered = { 0 } ;

#define THROTTLEFILTSHIFT 12

#define DEADBAND 150

#define MAXTHROTTLE			( 2.0*SERVORANGE*ALT_HOLD_THROTTLE_MAX  )
#define FIXED_WP_THROTTLE	( 2.0*SERVORANGE*RACING_MODE_WP_THROTTLE  )

#define THROTTLEHEIGHTGAIN (((ALT_HOLD_THROTTLE_MAX - ALT_HOLD_THROTTLE_MIN )*2.0*SERVORANGE )/(HEIGHT_MARGIN*2.0))

#define PITCHATMAX (ALT_HOLD_PITCH_MAX*(RMAX/57.3))
#define PITCHATMIN (ALT_HOLD_PITCH_MIN*(RMAX/57.3))
#define PITCHATZERO (ALT_HOLD_PITCH_HIGH*(RMAX/57.3))
#define PITCHATLAND (ALT_HOLD_PITCH_LAND*(RMAX/57.3))

#define PITCHHEIGHTGAIN ((PITCHATMAX - PITCHATMIN) / (HEIGHT_MARGIN*2.0)) 

#define HEIGHTTHROTTLEGAIN (( 1.5*HEIGHT_TARGET_MAX* 1024.0 ) / ( SERVORANGE*SERVOSAT ))

int pitchAltitudeAdjust = 0 ;
boolean filterManual = false;

int desiredHeight ;

extern struct waypointparameters goal ;

void normalAltitudeCntrl(void) ;
void manualThrottle(int throttleIn) ;
void hoverAltitudeCntrl(void) ;


void altitudeCntrl(void)
{
	if ( canStabilizeHover() && current_orientation == F_HOVER )
	{
		hoverAltitudeCntrl() ;
	}
	else
	{
		normalAltitudeCntrl() ;
	}
	
	return ;
}


void set_altitude_control(int alt)
{
	int throttleIn ;
	
	if ( flags._.altitude_hold_throttle || flags._.altitude_hold_pitch || filterManual )
	{
		if ( flags._.radio_on == 1 )
		{
			throttleIn = pwIn[THROTTLE_INPUT_CHANNEL] ;
		}
		else
		{
			throttleIn = pwTrim[THROTTLE_INPUT_CHANNEL] ;
		}
		
		int temp = throttleIn + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, alt) ;
		
		if ( THROTTLE_CHANNEL_REVERSED )
		{
			if (temp > pwTrim[THROTTLE_INPUT_CHANNEL]) alt = throttleIn - pwTrim[THROTTLE_INPUT_CHANNEL] ;
		}
		else
		{
			if (temp < pwTrim[THROTTLE_INPUT_CHANNEL]) alt = pwTrim[THROTTLE_INPUT_CHANNEL] - throttleIn ;
		}
		
		altitude_control = alt ;
	}
	else
	{
		altitude_control = 0 ;
	}
	
	return ;
}


void normalAltitudeCntrl(void)
{
	union longww throttleAccum ;
	union longww pitchAccum ;
	int throttleIn ;
	int throttleInOffset ;
	union longww heightError ;
	
	if ( flags._.radio_on == 1 )
	{
		throttleIn = pwIn[THROTTLE_INPUT_CHANNEL] ;
		// keep the In and Trim throttle values within 2000-4000 to account for
		// Spektrum receivers using failsafe values below 2000.
		throttleInOffset = pulsesat( pwIn[THROTTLE_INPUT_CHANNEL] ) - pulsesat( pwTrim[THROTTLE_INPUT_CHANNEL] ) ;
	}
	else
	{
		throttleIn = pwTrim[THROTTLE_INPUT_CHANNEL] ;
		throttleInOffset = 0 ;
	}
	
	if ( flags._.altitude_hold_throttle || flags._.altitude_hold_pitch )
	{
		if ( THROTTLE_CHANNEL_REVERSED ) throttleInOffset = - throttleInOffset ;
		
		if ( flags._.GPS_steering )
		{
			if ( desired_behavior._.climbout )
			{
				desiredHeight = goal.height ;
			}
			else
			{
				desiredHeight = goal.fromHeight + (((goal.height - goal.fromHeight) * (long)progress_to_goal)>>12)  ;
			}
		}
		else
		{
			desiredHeight =(( __builtin_mulss((int)( HEIGHTTHROTTLEGAIN ), throttleInOffset )) >> 11) ;
			if (desiredHeight < (int)(HEIGHT_TARGET_MIN)) desiredHeight = (int)(HEIGHT_TARGET_MIN) ;
		}
		
		if ( throttleInOffset < (int)(DEADBAND) && flags._.radio_on )
		{
			pitchAltitudeAdjust = 0 ;
			throttleAccum.WW  = 0 ;
		}
		else
		{
			heightError._.W1 = - desiredHeight ;
			heightError.WW += IMUlocationz.WW ;
			heightError.WW = heightError.WW >> 13 ;
			if ( heightError._.W0 < (- (int)(HEIGHT_MARGIN*8.0)) )
			{
				throttleAccum.WW = (int)(MAXTHROTTLE) ;
				pitchAltitudeAdjust = (int)(PITCHATMAX) ;
			}
			else if ( heightError._.W0 > (int)(HEIGHT_MARGIN*8.0) )
			{
				throttleAccum.WW = 0 ;
				pitchAltitudeAdjust = (int)(PITCHATZERO) ;
			}
			else
			{
				throttleAccum.WW = (int)(MAXTHROTTLE) + (__builtin_mulss( (int)(THROTTLEHEIGHTGAIN), ( - heightError._.W0 - (int)(HEIGHT_MARGIN*8.0) ) )>>3 );
				if ( throttleAccum.WW > (int)(MAXTHROTTLE) ) throttleAccum.WW = (int)(MAXTHROTTLE) ;
				pitchAccum.WW = __builtin_mulss( (int)(PITCHHEIGHTGAIN) , - heightError._.W0 - (int)(HEIGHT_MARGIN*8.0 ))>>3 ;
				pitchAltitudeAdjust = (int)(PITCHATMAX) + pitchAccum._.W0 ; 
			}
			
#if (RACING_MODE == 1)
			if ( flags._.GPS_steering )
			{
				throttleAccum.WW = (long) (FIXED_WP_THROTTLE) ;
			}
#endif
		}
		
		if ( !flags._.altitude_hold_throttle )
		{
			manualThrottle(throttleIn) ;
		}
		else if ( flags._.GPS_steering && desired_behavior._.land )
		{
			pitchAltitudeAdjust = (int) (PITCHATLAND) ;
			
			throttleFiltered.WW += (((long)(pwTrim[THROTTLE_INPUT_CHANNEL] - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT ) ;
			set_altitude_control(throttleFiltered._.W1 - throttleIn) ;
		}
		else
		{
			// Servo reversing is handled in servoMix.c
			int throttleOut = pulsesat( pwTrim[THROTTLE_INPUT_CHANNEL] + throttleAccum.WW ) ;
			throttleFiltered.WW += (((long)( throttleOut - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
			set_altitude_control(throttleFiltered._.W1 - throttleIn) ;
		}
		
		if ( !flags._.altitude_hold_pitch )
		{
			pitchAltitudeAdjust = 0 ;
		}
		
		filterManual = true;
	}
	else
	{
		pitchAltitudeAdjust = 0 ;
		manualThrottle(throttleIn) ;
	}
	
	return ;
}


void manualThrottle( int throttleIn )
{
	int altitude_control_pre ;
	
	throttleFiltered.WW += (((long)( throttleIn - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
	
	if (filterManual) {
		// Continue to filter the throttle control value in manual mode to avoid large, instant
		// changes to throttle value, which can burn out a brushed motor.  But after fading over
		// to the new throttle value, stop applying the filter to the throttle out to allow
		// faster control.
		altitude_control_pre = throttleFiltered._.W1 - throttleIn ;
		if (altitude_control_pre < 10) filterManual = false ;
	}
	else {
		altitude_control_pre = 0 ;
	}
	
	set_altitude_control(altitude_control_pre) ;
}


// For now, hovering does not attempt to control the throttle, and instead
// gives manual throttle control back to the pilot.
void hoverAltitudeCntrl(void)
{
	int altitude_control_pre ;
	int throttleIn = ( flags._.radio_on == 1 ) ? pwIn[THROTTLE_INPUT_CHANNEL] : pwTrim[THROTTLE_INPUT_CHANNEL] ;
	
	throttleFiltered.WW += (((long)( throttleIn - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
	
	if (filterManual) {
		// Continue to filter the throttle control value in manual mode to avoid large, instant
		// changes to throttle value, which can burn out a brushed motor.  But after fading over
		// to the new throttle value, stop applying the filter to the throttle out to allow
		// faster control.
		altitude_control = throttleFiltered._.W1 - throttleIn ;
		if (altitude_control_pre < 10) filterManual = false ;
	}
	else {
		altitude_control_pre = 0 ;
	}
	
	set_altitude_control(altitude_control_pre) ;
}
