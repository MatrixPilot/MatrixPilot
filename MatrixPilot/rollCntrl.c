#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

int yawkpail = YAWKP_AILERON*RMAX ;
int yawkdail = YAWKD_AILERON*SCALEGYRO*RMAX ;

int rollkp = ROLLKP*RMAX ;
int rollkd = ROLLKD*SCALEGYRO*RMAX ;


void rollCntrl(void)
{
	union longww rollAccum ;
	union longww dotprod ;
	union longww crossprod ;
	union longww gyroRollFeedback ;
	union longww gyroYawFeedback ;
	int desiredX ;
	int desiredY ;
	int actualX ;
	int actualY ;
	
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
#ifdef TestGains
		desiredX = -cosine ( 64 ) ;
		desiredY = sine ( 64 ) ;
#else
		desiredX = -cosine( desired_dir ) ;
		desiredY = sine( desired_dir ) ;
#endif
		actualX = rmat[1] ;
		actualY = rmat[4] ;
		dotprod.WW = __builtin_mulss( actualX , desiredX ) + __builtin_mulss( actualY , desiredY ) ;
		crossprod.WW = __builtin_mulss( actualX , desiredY ) - __builtin_mulss( actualY , desiredX ) ;
		crossprod.WW = crossprod.WW<<2 ;
		if ( dotprod._.W1 > 0 )
		{
			rollAccum.WW = __builtin_mulss( crossprod._.W1 , yawkpail ) ;
		}
		else
		{
			if ( crossprod._.W1 > 0 )
			{
				rollAccum._.W1 = yawkpail/4 ;
			}
			else
			{
				rollAccum._.W1 = -yawkpail/4 ;
			}
		}
	}
	else
	{
		rollAccum.WW = 0 ;
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
