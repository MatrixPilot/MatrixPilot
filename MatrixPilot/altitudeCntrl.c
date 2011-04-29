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

union longww throttleFiltered = { 0 } ;

#define THROTTLEFILTSHIFT 12

#define DEADBAND 150

#define MAXTHROTTLE			(2.0*SERVORANGE*ALT_HOLD_THROTTLE_MAX)
#define FIXED_WP_THROTTLE	(2.0*SERVORANGE*RACING_MODE_WP_THROTTLE)

#define THROTTLEHEIGHTGAIN (((ALT_HOLD_THROTTLE_MAX - ALT_HOLD_THROTTLE_MIN )*2.0*SERVORANGE )/(HEIGHT_MARGIN*2.0))

#define PITCHATMAX (ALT_HOLD_PITCH_MAX*(RMAX/57.3))
#define PITCHATMIN (ALT_HOLD_PITCH_MIN*(RMAX/57.3))
#define PITCHATZERO (ALT_HOLD_PITCH_HIGH*(RMAX/57.3))

#define PITCHHEIGHTGAIN ((PITCHATMAX - PITCHATMIN) / (HEIGHT_MARGIN*2.0))

#define HEIGHTTHROTTLEGAIN (( 1.5*HEIGHT_TARGET_MAX* 1024.0 ) / ( SERVORANGE*SERVOSAT ))

int pitchAltitudeAdjust = 0 ;
boolean filterManual = false;

int desiredHeight ;

void normalAltitudeCntrl(void) ;
void manualThrottle(int throttleIn) ;
void hoverAltitudeCntrl(void) ;

#if ( SPEED_CONTROL == 1)  // speed control loop

#define DESIRED_ENERGY ( unsigned long ) ( ( 58.0 * DESIRED_SPEED ) * ( 58.0 * DESIRED_SPEED ) )

long excess_energy_height(void) // computes (1/2gravity)*( actual_speed^2 - desired_speed^2 )
{
	long equivalent_energy_air_speed = -DESIRED_ENERGY ;
	long equivalent_energy_ground_speed = -DESIRED_ENERGY ;
	int speed_component ;
	union longww accum ;

	speed_component = IMUvelocityx._.W1 - estimatedWind[0] ;
	accum.WW = __builtin_mulsu ( speed_component , 37877 ) ;
	equivalent_energy_air_speed += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	speed_component = IMUvelocityy._.W1 - estimatedWind[1] ;
	accum.WW = __builtin_mulsu ( speed_component , 37877 ) ;
	equivalent_energy_air_speed += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	speed_component = IMUvelocityz._.W1 - estimatedWind[2] ;
	accum.WW = __builtin_mulsu ( speed_component , 37877 ) ;
	equivalent_energy_air_speed += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( IMUvelocityx._.W1 , 37877 ) ;
	equivalent_energy_ground_speed += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( IMUvelocityy._.W1 , 37877 ) ;
	equivalent_energy_ground_speed += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

	accum.WW = __builtin_mulsu ( IMUvelocityz._.W1 , 37877 ) ;
	equivalent_energy_ground_speed += __builtin_mulss ( accum._.W1 , accum._.W1 ) ;

//	return the smaller of the energies of ground and air speed
//	to keep both of them from getting too small

	if ( equivalent_energy_ground_speed < equivalent_energy_air_speed )
	{
		return equivalent_energy_ground_speed ;
	}
	else
	{
		return equivalent_energy_air_speed ;
	}

}
#else
long excess_energy_height(void) 
{
	return 0 ;
}
#endif

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


void set_throttle_control(int throttle)
{
	int throttleIn ;
	
	if ( flags._.altitude_hold_throttle || flags._.altitude_hold_pitch || filterManual )
	{
		if ( udb_flags._.radio_on == 1 )
		{
			throttleIn = udb_pwIn[THROTTLE_INPUT_CHANNEL] ;
		}
		else
		{
			throttleIn = udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;
		}
		
		int temp = throttleIn + REVERSE_IF_NEEDED(THROTTLE_CHANNEL_REVERSED, throttle) ;
		
		if ( THROTTLE_CHANNEL_REVERSED )
		{
			if (temp > udb_pwTrim[THROTTLE_INPUT_CHANNEL]) throttle = throttleIn - udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;
		}
		else
		{
			if (temp < udb_pwTrim[THROTTLE_INPUT_CHANNEL]) throttle = udb_pwTrim[THROTTLE_INPUT_CHANNEL] - throttleIn ;
		}
		
		throttle_control = throttle ;
	}
	else
	{
		throttle_control = 0 ;
	}
	
	return ;
}


void setTargetAltitude(int targetAlt)
{
	desiredHeight = targetAlt ;
	return ;
}

long speed_height = 0 ;

void normalAltitudeCntrl(void)
{
	union longww throttleAccum ;
	union longww pitchAccum ;
	int throttleIn ;
	int throttleInOffset ;
	union longww heightError = { 0 } ;

	speed_height = excess_energy_height() ; // equivalent height of the airspeed
	
	if ( udb_flags._.radio_on == 1 )
	{
		throttleIn = udb_pwIn[THROTTLE_INPUT_CHANNEL] ;
		// keep the In and Trim throttle values within 2000-4000 to account for
		// Spektrum receivers using failsafe values below 2000.
		throttleInOffset = udb_servo_pulsesat( udb_pwIn[THROTTLE_INPUT_CHANNEL] ) - udb_servo_pulsesat( udb_pwTrim[THROTTLE_INPUT_CHANNEL] ) ;
	}
	else
	{
		throttleIn = udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;
		throttleInOffset = 0 ;
	}
	
	if ( flags._.altitude_hold_throttle || flags._.altitude_hold_pitch )
	{
		if ( THROTTLE_CHANNEL_REVERSED ) throttleInOffset = - throttleInOffset ;
		
		if ( flags._.GPS_steering )
		{
			if ( desired_behavior._.takeoff || desired_behavior._.altitude )
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
#if (ALTITUDEHOLD_STABILIZED == AH_PITCH_ONLY)
			// In stabilized mode using pitch-only altitude hold, use desiredHeight as
			// set from the state machine upon entering stabilized mode in ent_stabilizedS().
			if (desiredHeight < (int)( HEIGHT_TARGET_MIN )) desiredHeight = (int)( HEIGHT_TARGET_MIN ) ;
			if (desiredHeight > (int)( HEIGHT_TARGET_MAX )) desiredHeight = (int)( HEIGHT_TARGET_MAX ) ;
#elif (ALTITUDEHOLD_STABILIZED == AH_FULL)
			// In stabilized mode using full altitude hold, use the throttle stick value to determine desiredHeight,
			desiredHeight =(( __builtin_mulss( (int)( HEIGHTTHROTTLEGAIN ), throttleInOffset )) >> 11) ;
#endif
		}
		
		if ( throttleInOffset < (int)( DEADBAND ) && udb_flags._.radio_on )
		{
			pitchAltitudeAdjust = 0 ;
			throttleAccum.WW  = 0 ;
		}
		else
		{

			heightError._.W1 = - desiredHeight ;
			heightError.WW = ( heightError.WW + IMUlocationz.WW + speed_height ) >> 13 ;
			if ( heightError._.W0 < ( - (int)(HEIGHT_MARGIN*8.0)) )
			{
				throttleAccum.WW = (int)(MAXTHROTTLE) ;
			}
			else if (  heightError._.W0 > (int)(HEIGHT_MARGIN*8.0) )
			{
				throttleAccum.WW = 0 ;
			}
			else
			{
				throttleAccum.WW = (int)(MAXTHROTTLE) + (__builtin_mulss( (int)(THROTTLEHEIGHTGAIN), ( -heightError._.W0 - (int)(HEIGHT_MARGIN*8.0) ) )>>3) ;
				if ( throttleAccum.WW > (int)(MAXTHROTTLE) ) throttleAccum.WW = (int)(MAXTHROTTLE) ;
			}	

			heightError._.W1 = - desiredHeight ;
			heightError.WW = ( heightError.WW + IMUlocationz.WW - speed_height ) >> 13 ;
			if ( heightError._.W0 < ( - (int)(HEIGHT_MARGIN*8.0)) )
			{
				pitchAltitudeAdjust = (int)(PITCHATMAX) ;
			}
			else if (  heightError._.W0 > (int)(HEIGHT_MARGIN*8.0) )
			{
				pitchAltitudeAdjust = (int)( PITCHATZERO ) ;
			}
			else
			{
				pitchAccum.WW = __builtin_mulss( (int)(PITCHHEIGHTGAIN) , - heightError._.W0 - (int)(HEIGHT_MARGIN*8.0 ))>>3 ;
				pitchAltitudeAdjust = (int)(PITCHATMAX) + pitchAccum._.W0 ;
			}
	
		
#if (RACING_MODE == 1)
			if ( flags._.GPS_steering )
			{
				throttleAccum.WW = (long)(FIXED_WP_THROTTLE) ;
			}
#endif
		}
		
		if ( !flags._.altitude_hold_throttle )
		{
			manualThrottle(throttleIn) ;
		}
		else if ( flags._.GPS_steering && desired_behavior._.land )
		{
			// place a ceiling, in other words, go down, but not up.
			if ( pitchAltitudeAdjust > 0 )
			{
				pitchAltitudeAdjust = 0 ;
			}
			
			throttleFiltered.WW += (((long)(udb_pwTrim[THROTTLE_INPUT_CHANNEL] - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT ) ;
			set_throttle_control(throttleFiltered._.W1 - throttleIn) ;
		}
		else
		{
			// Servo reversing is handled in servoMix.c
			int throttleOut = udb_servo_pulsesat( udb_pwTrim[THROTTLE_INPUT_CHANNEL] + throttleAccum.WW ) ;
			throttleFiltered.WW += (((long)( throttleOut - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
			set_throttle_control(throttleFiltered._.W1 - throttleIn) ;
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
	int throttle_control_pre ;
	
	throttleFiltered.WW += (((long)( throttleIn - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
	
	if (filterManual) {
		// Continue to filter the throttle control value in manual mode to avoid large, instant
		// changes to throttle value, which can burn out a brushed motor.  But after fading over
		// to the new throttle value, stop applying the filter to the throttle out to allow
		// faster control.
		throttle_control_pre = throttleFiltered._.W1 - throttleIn ;
		if (throttle_control_pre < 10) filterManual = false ;
	}
	else {
		throttle_control_pre = 0 ;
	}
	
	set_throttle_control(throttle_control_pre) ;
	
	return ;
}


// For now, hovering does not attempt to control the throttle, and instead
// gives manual throttle control back to the pilot.
void hoverAltitudeCntrl(void)
{
	int throttle_control_pre ;
	int throttleIn = ( udb_flags._.radio_on == 1 ) ? udb_pwIn[THROTTLE_INPUT_CHANNEL] : udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;
	
	throttleFiltered.WW += (((long)( throttleIn - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
	
	if (filterManual) {
		// Continue to filter the throttle control value in manual mode to avoid large, instant
		// changes to throttle value, which can burn out a brushed motor.  But after fading over
		// to the new throttle value, stop applying the filter to the throttle out to allow
		// faster control.
		throttle_control_pre = throttleFiltered._.W1 - throttleIn ;
		if (throttle_control_pre < 10) filterManual = false ;
	}
	else {
		throttle_control_pre = 0 ;
	}
	
	set_throttle_control(throttle_control_pre) ;
	
	return ;
}
