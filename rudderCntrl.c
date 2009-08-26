
#include "p30f4011.h"
#include "definesRmat.h"

#include "defines.h"

int yawkp = YAWKP*RMAX ;

int yawkd = YAWKD*RMAX ;

int yawbgain = (int) (8.0*YAWBOOST) ;

long yawboost = 0 ;

union longww gyroFeedback ;

int rudderDeflection ;

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
	}
	if ( flags._.GPS_steering || flags._.pitch_feedback )
	{
		gyroFeedback.WW = __builtin_mulss( yawkd , omegaAccum[2] ) ;
		yawboost = ( __builtin_mulss( yawbgain , ( pwrud - ruddtrim ) ))>>3 ;
	}
	else
	{
		gyroFeedback.WW = 0 ;
		yawboost = 0 ;
	}
	if ( PORTDbits.RD3 )
	{
		rudderAccum.WW = (long)pwrud + (long)rudderAccum._.W1 - (long)gyroFeedback._.W1 + yawboost ;
		PDC1 = pulsesat( rudderAccum.WW ) ;
		rudderDeflection = ruddtrim - PDC1 ;

	}
	else
	{
		rudderAccum.WW = (long)pwrud - (long)rudderAccum._.W1 + (long)gyroFeedback._.W1 + yawboost ;
		PDC1 = pulsesat( rudderAccum.WW ) ;		
		rudderDeflection = PDC1 - ruddtrim ;
	}	
	return ;
}

