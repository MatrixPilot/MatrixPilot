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
#include "inputCntrl.h"
#include "airspeedCntrlFBW.h"
#include "motionCntrl.h"

union longww throttleFiltered = { 0 } ;

#define THROTTLEFILTSHIFT 12

#define MAXTHROTTLE			(RMAX*ALT_HOLD_THROTTLE_MAX)
#define FIXED_WP_THROTTLE	(RMAX*RACING_MODE_WP_THROTTLE)

#define THROTTLEHEIGHTGAIN (((ALT_HOLD_THROTTLE_MAX - ALT_HOLD_THROTTLE_MIN )* RMAX )/(HEIGHT_MARGIN*2.0))


int16_t pitchAltitudeAdjust = 0 ;
boolean filterManual = false;

void normalAltitudeCntrl(void) ;
void manualThrottle(int16_t throttleIn) ;
void hoverAltitudeCntrl(void) ;

// External variables
int16_t height_margin					= HEIGHT_MARGIN;
fractional alt_hold_throttle_min	= ALT_HOLD_THROTTLE_MIN * RMAX;
fractional alt_hold_throttle_max	= ALT_HOLD_THROTTLE_MAX * RMAX;

// Internal computed variables.  Values defined above.
int16_t max_throttle			= MAXTHROTTLE;
int16_t throttle_height_gain 	= THROTTLEHEIGHTGAIN;


void throttleCntrl(void)
{
//	if ( canStabilizeHover() && current_orientation == F_HOVER )
//	{
//		hoverAltitudeCntrl() ;
//	}
//	else
//	{
		normalAltitudeCntrl() ;
//	}
//	
	return ;
}


void set_throttle_control(fractional throttle)
{	
//	if ( flags._.altitude_hold_throttle || flags._.altitude_hold_pitch || filterManual )
//	{
//		ap_cntrls[AP_CNTRL_THROTTLE] = throttle;
//		throttle_control = throttle;		// TODO: Move this
//	}
//	else
//	{
		ap_cntrls[AP_CNTRL_THROTTLE] = 0;
		throttle_control = in_cntrls[IN_CNTRL_THROTTLE]; // TODO: Move this
//	}
//	
	return ;
}

inline boolean get_throttle_manual_lockout()
{
//	return ( flags._.altitude_hold_throttle || flags._.altitude_hold_pitch || filterManual );
	return false;
}


//void setTargetAltitude(int16_t targetAlt)
//{
//	desiredHeight = targetAlt ;
//	return ;
//}


fractional throttleAltitudeControl(int32_t desiredAltitude, int32_t actualAltitude, int32_t kineticHeightError)
{

	union longww heightError = { actualAltitude - desiredAltitude + kineticHeightError } ;
	heightError.WW >>= 13;

//	temp.WW = __builtin_mulss(alt_hold_throttle_max , 2.0 * SERVORANGE );
//	temp.WW <<= 2;
//	if(temp._.W0 & 0x8000) temp._.W1 ++;
//	max_throttle =	temp._.W1;

	throttle_height_gain =	__builtin_divsd( (alt_hold_throttle_max - alt_hold_throttle_min ) , (height_margin << 1) );
	throttle_height_gain <<= 1;

	int16_t height_marginx8 = height_margin << 3;

	if ( heightError._.W0 < -height_marginx8 )
	{
		return (int16_t)(max_throttle) ;
	}
	else if (  heightError._.W0 > height_marginx8 )
	{
		return 0 ;
	}
	else
	{
		union longww temp ;
		temp.WW = (int16_t)(alt_hold_throttle_max) + (__builtin_mulss( throttle_height_gain, ( -heightError._.W0 - height_marginx8 ) )>>3) ;

		if ( temp._.W0 > (int16_t)(alt_hold_throttle_max) ) 
					temp._.W0 = (int16_t)alt_hold_throttle_max ;

		return temp._.W0;
	}

}


void normalAltitudeCntrl(void)
{



//	switch(ap_state())
//	{
//	case AP_STATE_MANUAL:
//		set_throttle_control(in_cntrls[IN_CNTRL_THROTTLE]);
//		break;
//	case AP_STATE_STABILIZED:
//		set_throttle_control( throttleAltitudeControl( get_fbw_demand_altitude(), IMUlocationz.WW, speed_height ) );
//		break;
//	case AP_STATE_GUIDED:
//		set_throttle_control( throttleAltitudeControl( get_guided_desired_altitude(), IMUlocationz.WW, speed_height ) );
//		break;
//	default:
//		set_throttle_control(in_cntrls[IN_CNTRL_THROTTLE]);
//	}

	set_throttle_control(in_cntrls[IN_CNTRL_THROTTLE]);



//	if ( flags._.altitude_hold_throttle || flags._.altitude_hold_pitch )
//	{
//
//		if ( flags._.GPS_steering )
//		{
//		}
//		else
//		{

//		if ( throttleInOffset < (int16_t)( DEADBAND ) && udb_flags._.radio_on )
//		{
//			pitchAltitudeAdjust = 0 ;
//			throttleAccum.WW  = 0 ;
//		}
//		else
//		{
//
//			heightError._.W1 = - desiredHeight ;
//			heightError.WW = ( heightError.WW + IMUlocationz.WW + speed_height ) >> 13 ;
	
//

//		
//#if (RACING_MODE == 1)
//			if ( flags._.GPS_steering )
//			{
//				throttleAccum.WW = (int32_t)(FIXED_WP_THROTTLE) ;
//			}
//#endif
//		}
//		
//		if ( !flags._.altitude_hold_throttle )
//		{
//			manualThrottle(throttleIn) ;
//		}
//		else if ( flags._.GPS_steering && desired_behavior._.land )
//		{
//			// place a ceiling, in other words, go down, but not up.
//			if ( pitchAltitudeAdjust > 0 )
//			{
//				pitchAltitudeAdjust = 0 ;
//			}
//			
//			throttleFiltered.WW += (((int32_t)(udb_pwTrim[THROTTLE_INPUT_CHANNEL] - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT ) ;
//			set_throttle_control(throttleFiltered._.W1 - throttleIn) ;
//		}
//		else
//		{
//			// Servo reversing is handled in servoMix.c
//			int16_t throttleOut = udb_servo_pulsesat( udb_pwTrim[THROTTLE_INPUT_CHANNEL] + throttleAccum.WW ) ;
//			throttleFiltered.WW += (((int32_t)( throttleOut - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
//			set_throttle_control(throttleFiltered._.W1 - throttleIn) ;
//		}
//		
//		if ( !flags._.altitude_hold_pitch )
//		{
//			pitchAltitudeAdjust = 0 ;
//		}
//		
//		filterManual = true;
//	}
//	else
//	{
//		pitchAltitudeAdjust = 0 ;
//		manualThrottle() ;
//	}

	
	return ;
}



//fractional airspeed_pitch_adjust(void)
//{
//	union longww pitchAccum ;
//	union longww heightError = { 0 } ;
//    fractional airspeedAltitudeAdjust ;
//
//	heightError.WW = speed_height >> 13 ;
//	if ( heightError._.W0 < -height_marginx8 )
//	{
//		airspeedAltitudeAdjust = (int16_t)(pitch_at_max) ;
//	}
//	else if (  heightError._.W0 > height_marginx8 )
//	{
//		airspeedAltitudeAdjust = (int16_t)( pitch_at_zero ) ;
//	}
//	else
//	{
//		pitchAccum.WW = __builtin_mulss( (int16_t)(pitch_height_gain) , - heightError._.W0 - height_marginx8)>>3 ;
//		airspeedAltitudeAdjust = (int16_t)(pitch_at_max) + pitchAccum._.W0 ;
//	}
//
//	return airspeedAltitudeAdjust;
//}

void manualThrottle( fractional throttleIn )
{
	int16_t throttle_control_pre ;
	
	throttleFiltered.WW += (((int32_t)( throttleIn - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
	
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
//void hoverAltitudeCntrl(void)
//{
//	int16_t throttle_control_pre ;
//	int16_t throttleIn = ( udb_flags._.radio_on == 1 ) ? udb_pwIn[THROTTLE_INPUT_CHANNEL] : udb_pwTrim[THROTTLE_INPUT_CHANNEL] ;
//	
//	throttleFiltered.WW += (((int32_t)( throttleIn - throttleFiltered._.W1 )) << THROTTLEFILTSHIFT ) ;
//	
//	if (filterManual) {
//		// Continue to filter the throttle control value in manual mode to avoid large, instant
//		// changes to throttle value, which can burn out a brushed motor.  But after fading over
//		// to the new throttle value, stop applying the filter to the throttle out to allow
//		// faster control.
//		throttle_control_pre = throttleFiltered._.W1 - throttleIn ;
//		if (throttle_control_pre < 10) filterManual = false ;
//	}
//	else {
//		throttle_control_pre = 0 ;
//	}
//	
//	set_throttle_control(throttle_control_pre) ;
//	
//	return ;
//}

