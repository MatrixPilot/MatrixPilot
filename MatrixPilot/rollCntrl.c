#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

const int yawkdail = YAWKD_AILERON*SCALEGYRO*RMAX ;

const int rollkp = ROLLKP*RMAX ;
const int rollkd = ROLLKD*SCALEGYRO*RMAX ;

const int hoverrollkp = HOVER_ROLLKP*SCALEGYRO*RMAX ;
const int hoverrollkd = HOVER_ROLLKD*SCALEGYRO*RMAX ;

void normalRollCntrl(void) ;
void hoverRollCntrl(void) ;


void rollCntrl(void)
{
	if ( STABILIZE_HOVERING && current_orientation == F_HOVER )
	{
		hoverRollCntrl() ;
	}
	else
	{
		normalRollCntrl() ;
	}
	
	return ;
}


void normalRollCntrl(void)
{
	union longww rollAccum = { 0 } ;
	union longww gyroRollFeedback ;
	union longww gyroYawFeedback ;
	
	fractional rmat6 ;
	fractional omegaAccum2 ;
	
	if ( !STABILIZE_INVERTED_FLIGHT || !desired_behavior._.inverted )
	{
		rmat6 = rmat[6] ;
		omegaAccum2 = omegaAccum[2] ;
	}
	else
	{
		rmat6 = -rmat[6] ;
		omegaAccum2 = -omegaAccum[2] ;
	}
	
#ifdef TestGains
	flags._.GPS_steering = 1 ;
#endif
	if ( AILERON_NAVIGATION && flags._.GPS_steering )
	{
		rollAccum._.W1 = determine_navigation_deflection( 'r' ) ;
	}
	
#ifdef TestGains
	flags._.pitch_feedback = 1 ;
#endif
	
	if ( ROLL_STABILIZATION && flags._.pitch_feedback )
	{
		gyroRollFeedback.WW = __builtin_mulss( rollkd , omegaAccum[1] ) ;
		rollAccum.WW += __builtin_mulss( rmat6 , rollkp ) ;
	}
	else
	{
		gyroRollFeedback.WW = 0 ;
	}
	
	if ( YAW_STABILIZATION_AILERON && (flags._.GPS_steering || flags._.pitch_feedback) )
	{
		gyroYawFeedback.WW = __builtin_mulss( yawkdail , omegaAccum2 ) ;
	}
	else
	{
		gyroYawFeedback.WW = 0 ;
	}
	
	roll_control = (long)rollAccum._.W1 - (long)gyroRollFeedback._.W1 - (long)gyroYawFeedback._.W1 ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}


void hoverRollCntrl(void)
{
	int rollNavDeflection ;
	union longww gyroRollFeedback ;
	
	if ( flags._.pitch_feedback )
	{
		if ( AILERON_NAVIGATION && flags._.GPS_steering )
		{
			rollNavDeflection = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS/2) ? determine_navigation_deflection( 'h' ) : 0 ;
		}
		else
		{
			rollNavDeflection = 0 ;
		}
		
		gyroRollFeedback.WW = __builtin_mulss( hoverrollkd , omegaAccum[1] ) ;
	}
	else
	{
		rollNavDeflection = 0 ;
		gyroRollFeedback.WW = 0 ;
	}
	
	roll_control = rollNavDeflection -(long)gyroRollFeedback._.W1 ;
	
	return ;
}
