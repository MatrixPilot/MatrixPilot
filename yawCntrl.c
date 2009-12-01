#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

int yawkprud = YAWKP_RUDDER*RMAX ;
int yawkdrud = YAWKD_RUDDER*SCALEGYRO*RMAX ;


void yawCntrl(void)
{
	union longww yawAccum ;
	union longww dotprod ;
	union longww crossprod ;
	union longww gyroYawFeedback ;
	int desiredX ;
	int desiredY ;
	int actualX ;
	int actualY ;

#ifdef TestGains
	flags._.GPS_steering = 1 ;
#endif 
	if ( RUDDER_NAVIGATION && flags._.GPS_steering )
	{
#ifdef TestGains
		desiredX = -cosine ( 0 ) ;
		desiredY = sine ( 0 ) ;
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
			yawAccum.WW = __builtin_mulss( crossprod._.W1 , yawkprud ) ;
		}
		else
		{
			if ( crossprod._.W1 > 0 )
			{
				yawAccum._.W1 = yawkprud/4 ;
			}
			else
			{
				yawAccum._.W1 = -yawkprud/4 ;
			}
		}
	}
	else
	{
		yawAccum.WW = 0 ;
	}
	
	if ( YAW_STABILIZATION_RUDDER && (flags._.GPS_steering || flags._.pitch_feedback) )
	{
		gyroYawFeedback.WW = __builtin_mulss( yawkdrud , omegaAccum[2] ) ;
	}
	else
	{
		gyroYawFeedback.WW = 0 ;
	}
	
	yaw_control = (long)yawAccum._.W1 - (long)gyroYawFeedback._.W1 ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}
