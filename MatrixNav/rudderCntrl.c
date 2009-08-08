
#include "p30f4011.h"
#include "definesRmat.h"

#include "defines.h"

int yawkp = YAWKP*RMAX ;

int yawkd = YAWKD*RMAX ;

union longww gyroFeedback ;

void rudderCntrl(void)
{
	union longww rudderAccum ;
	union longww dotprod ;
	union longww crossprod ;
	int desiredX ;
	int desiredY ;
	int actualX ;
	int actualY ;

	if ( flags._.radio_on )
	{
		pwrud = pwc7 + waggle ;
	}
	else
	{
		pwrud = ruddtrim + waggle ;
	}
#ifdef TestGains
	flags._.GPS_steering = 1 ;
#endif 
	if ( flags._.GPS_steering )
	{
		gyroFeedback.WW = __builtin_mulss( yawkd , omega[2] ) ;
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
			rudderAccum.WW = __builtin_mulss( crossprod._.W1 , yawkp ) ;
		}
		else
		{
			if ( crossprod._.W1 > 0 )
			{
				rudderAccum._.W1 = RMAX*YAWKP/4 ;
			}
			else
			{
				rudderAccum._.W1 = -RMAX*YAWKP/4 ;
			}
		}
	}
	else
	{
		rudderAccum.WW = 0 ;
		gyroFeedback.WW = 0 ;
	}
	if ( PORTDbits.RD3 )
	{
		rudderAccum.WW = (long)pwrud + (long)rudderAccum._.W1 - (long)gyroFeedback._.W1 ;
		PDC1 = pulsesat( rudderAccum.WW ) ;
	}
	else
	{
		rudderAccum.WW = (long)pwrud - (long)rudderAccum._.W1 + (long)gyroFeedback._.W1 ;
		PDC1 = pulsesat( rudderAccum.WW ) ;
	}	
	return ;
}

