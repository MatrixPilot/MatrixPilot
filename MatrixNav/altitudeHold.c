#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"
#include "options.h"

int pwthrottleIn = 0 ;
int throttleIn = 0 ;
int throttleOut = 0 ;
int desiredHeight = 0 ;

union longww throttleFiltered = { 0 } ;

#define THROTTLEFILTSHIFT 12

#define HEIGHTMARGIN 10 // full throttle until height is margin below target
						// min when height is margin above target

#define DEADBAND 150

#define MAXTHROTTLE ((int) 2.0*SERVORANGE*SERVOSAT  )

#define THROTTLEHEIGHTGAIN ( (int ) ( ( (1.0 - MINIMUMTHROTTLE ) * MAXTHROTTLE ) / ( HEIGHTMARGIN*2 ) ) )

#define PITCHATMAX ((long)PITCHATMAXTHROTTLE)*((long)RMAX)/((long)57.3)
#define PITCHATMIN ((long)PITCHATMINTHROTTLE)*((long)RMAX)/((long)57.3)
#define PITCHATZERO ((long)PITCHATZEROTHROTTLE)*((long)RMAX)/((long)57.3)

#define PITCHHEIGHTGAIN ( ( (PITCHATMAX - PITCHATMIN) / ( ( long )(HEIGHTMARGIN*2  )  ) ) )

#define HEIGHTTHROTTLEGAIN ( (int )  ( ((long) (1.5*HEIGHTMAX)*(long) 1024 ) / ((long) SERVORANGE*(long)SERVOSAT ) ))

int pitchAltitudeAdjust = 0 ;

union longww throttleAccum ;
						
extern struct waypointparameters goal ;

void throttleCntrl(void)
{
#ifdef ALTITUDEHOLD

	
	if ( flags._.radio_on )
	{
		pwthrottleIn = pwc2 ;
	}
	else
	{
		pwthrottleIn = throttleIdle ;
	}

	if ( flags._.altitude_hold )
	{
		throttleIn = pwthrottleIn - throttleIdle ;
		if ( PORTFbits.RF6 ) throttleIn = - throttleIn ;
		if ( flags._.use_waypoints == 1 )
		{
			desiredHeight = goal.height ;
		}
		else
		{
			desiredHeight =(( __builtin_mulss(  HEIGHTTHROTTLEGAIN, throttleIn ))>>11) ;
		}
		if ( throttleIn < DEADBAND )
		{
			pitchAltitudeAdjust = 0 ;
			throttleOut = throttleIdle ;
			throttleFiltered.WW += (((long)( throttleOut - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT );
			PDC3 = throttleFiltered._.W1 ;
		}
		else
		{
			if ( height < (desiredHeight - HEIGHTMARGIN) )
			{
				throttleAccum.WW = MAXTHROTTLE ;
				pitchAltitudeAdjust = PITCHATMAX ;
			}
			else if ( height > (desiredHeight+ HEIGHTMARGIN ) )
			{
				throttleAccum.WW = 0 ;
				pitchAltitudeAdjust = PITCHATZERO ;
			}
			else
			{
				throttleAccum.WW = MAXTHROTTLE 
						+ __builtin_mulss( THROTTLEHEIGHTGAIN, ( desiredHeight - height - HEIGHTMARGIN ) );
				pitchAltitudeAdjust = PITCHATMAX + PITCHHEIGHTGAIN*( desiredHeight - height - HEIGHTMARGIN ) ;
			}

			if ( PORTFbits.RF6 )
			{
				throttleOut = pulsesat( - throttleAccum.WW + throttleIdle ) ;
				throttleFiltered.WW += (((long)( throttleOut - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT );
				PDC3 = throttleFiltered._.W1 ;
			}
			else
			{
				throttleOut = pulsesat( throttleAccum.WW + throttleIdle ) ;
				throttleFiltered.WW += (((long)( throttleOut - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT );
				PDC3 = throttleFiltered._.W1 ;
			}
		}
	}
	else
	{
		throttleFiltered.WW += (((long)( pwthrottleIn - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT );
		PDC3 = throttleFiltered._.W1 ;
		pitchAltitudeAdjust = 0 ;
	}
#else
	PDC3 = 3000 ;
	pitchAltitudeAdjust = 0 ;
#endif
	return ;
}
