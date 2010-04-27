#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

#define HOVERYOFFSET ((long)(HOVER_YAW_OFFSET*(RMAX/57.3)))

const int yawkdrud = YAWKD_RUDDER*SCALEGYRO*RMAX ;

const int hoveryawkp = HOVER_YAWKP*RMAX ;
const int hoveryawkd = HOVER_YAWKD*SCALEGYRO*RMAX ;

void normalYawCntrl(void) ;
void hoverYawCntrl(void) ;


void yawCntrl(void)
{
	if ( STABILIZE_HOVERING && current_orientation == F_HOVER )
	{
		hoverYawCntrl() ;
	}
	else
	{
		normalYawCntrl() ;
	}
	
	return ;
}


void normalYawCntrl(void)
{
	int yawNavDeflection ;
	union longww gyroYawFeedback ;

#ifdef TestGains
	flags._.GPS_steering = 1 ;
#endif 
	if ( RUDDER_NAVIGATION && flags._.GPS_steering )
	{
		yawNavDeflection = determine_navigation_deflection( 'y' ) ;
		
		if ( STABILIZE_INVERTED_FLIGHT && current_orientation == F_INVERTED )
		{
			yawNavDeflection = -yawNavDeflection ;
		}
	}
	else
	{
		yawNavDeflection = 0 ;
	}
	
	if ( YAW_STABILIZATION_RUDDER && flags._.pitch_feedback )
	{
		gyroYawFeedback.WW = __builtin_mulss( yawkdrud , omegaAccum[2] ) ;
	}
	else
	{
		gyroYawFeedback.WW = 0 ;
	}
	
	yaw_control = (long)yawNavDeflection - (long)gyroYawFeedback._.W1 ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}


void hoverYawCntrl(void)
{
	union longww yawAccum ;
	union longww gyroYawFeedback ;
	
	if ( flags._.pitch_feedback )
	{
		gyroYawFeedback.WW = __builtin_mulss( hoveryawkd , omegaAccum[2] ) ;
		
		int yawInput = ( flags._.radio_on == 1 ) ? REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, pwIn[RUDDER_INPUT_CHANNEL] - pwTrim[RUDDER_INPUT_CHANNEL]) : 0 ;
		int manualYawOffset = yawInput * (int)(RMAX/2000);
		
		yawAccum.WW = __builtin_mulss( rmat[6] + HOVERYOFFSET + manualYawOffset , hoveryawkp ) ;
	}
	else
	{
		gyroYawFeedback.WW = 0 ;
		yawAccum.WW = 0 ;
	}
	
	yaw_control = (long)yawAccum._.W1 - (long)gyroYawFeedback._.W1 ;
	
	return ;
}
