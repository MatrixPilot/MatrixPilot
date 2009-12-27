#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"
#include "options.h"

union longww throttleFiltered = { 0 } ;

#define THROTTLEFILTSHIFT 12

#define DEADBAND 150

#define MAXTHROTTLE ((int) 2.0*SERVORANGE*SERVOSAT*ALT_HOLD_THROTTLE_MAX  )

#define THROTTLEHEIGHTGAIN ( (int ) ( ( (1.0 - ALT_HOLD_THROTTLE_MIN ) * MAXTHROTTLE ) / ( HEIGHT_MARGIN ) ) )

#define PITCHATMAX ((long)(ALT_HOLD_PITCH_MAX*(RMAX/57.3)))
#define PITCHATMIN ((long)(ALT_HOLD_PITCH_MIN*(RMAX/57.3)))
#define PITCHATZERO ((long)(ALT_HOLD_PITCH_HIGH*(RMAX/57.3)))

#define PITCHHEIGHTGAIN ( ( (PITCHATMAX - PITCHATMIN) / ( ( long )(HEIGHT_MARGIN*2) ) ) )

#define HEIGHTTHROTTLEGAIN ( (int)  ( ((long) (1.5*HEIGHT_TARGET_MAX)*(long) 1024 ) / ((long) SERVORANGE*(long)SERVOSAT ) ))

int pitchAltitudeAdjust = 0 ;
boolean filterManual = false;

union longww throttleAccum ;

int desiredHeight ;

extern struct waypointparameters goal ;


void altitudeCntrl(void)
{
	int throttleIn ;
	int throttleInOffset ;
	
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
	
	if ( flags._.altitude_hold )
	{
		if ( THROTTLE_CHANNEL_REVERSED ) throttleInOffset = - throttleInOffset ;
		
		if ( flags._.use_waypoints == 1 )
		{
			desiredHeight = goal.height ;
		}
		else
		{
			desiredHeight =(( __builtin_mulss( HEIGHTTHROTTLEGAIN, throttleInOffset ))>>11) ;
			if (desiredHeight < HEIGHT_TARGET_MIN) desiredHeight = HEIGHT_TARGET_MIN ;
		}
		
		if ( throttleInOffset < DEADBAND )
		{
			pitchAltitudeAdjust = 0 ;
			throttleFiltered.WW += (((long)(pwTrim[THROTTLE_INPUT_CHANNEL] - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT ) ;
			altitude_control = throttleFiltered._.W1 - throttleIn ;
		}
		else
		{
			if ( height < (desiredHeight - HEIGHT_MARGIN) )
			{
				throttleAccum.WW = MAXTHROTTLE ;
				pitchAltitudeAdjust = PITCHATMAX ;
			}
			else if ( height > (desiredHeight + HEIGHT_MARGIN) )
			{
				throttleAccum.WW = 0 ;
				pitchAltitudeAdjust = PITCHATZERO ;
			}
			else
			{
				throttleAccum.WW = MAXTHROTTLE + __builtin_mulss( THROTTLEHEIGHTGAIN, ( desiredHeight - height - HEIGHT_MARGIN ) );
				if ( throttleAccum.WW > MAXTHROTTLE ) throttleAccum.WW = MAXTHROTTLE ;
				pitchAltitudeAdjust = PITCHATMAX + PITCHHEIGHTGAIN*( desiredHeight - height - HEIGHT_MARGIN ) ;
			}
			
#if (RACING_MODE == 1)
			if ( flags._.GPS_steering )
			{
				throttleAccum.WW = MAXTHROTTLE ;
			}
#endif
			
			// Servo reversing is handled in servoMix.c
			int throttleOut = pulsesat( pwTrim[THROTTLE_INPUT_CHANNEL] + throttleAccum.WW ) ;
			throttleFiltered.WW += (((long)( throttleOut - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT ) ;
			altitude_control = throttleFiltered._.W1 - throttleIn ;
		}
		filterManual = true;
		
		if ( flags._.GPS_steering && desired_behavior._.land )
		{
			pitchAltitudeAdjust = 0 ;
			throttleFiltered.WW += (((long)(pwTrim[THROTTLE_INPUT_CHANNEL] - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT ) ;
			altitude_control = throttleFiltered._.W1 - throttleIn ;
		}
	}
	else
	{
		pitchAltitudeAdjust = 0 ;
		throttleFiltered.WW += (((long)( throttleIn - throttleFiltered._.W1 ))<<THROTTLEFILTSHIFT ) ;
		
		if (filterManual) {
			// Continue to filter the throttle control value in manual mode to avoid large, instant
			// changes to throttle value, which can burn out a brushed motor.  But after fading over
			// to the new throttle value, stop applying the filter to the throttle out to allow
			// faster control.
			altitude_control = throttleFiltered._.W1 - throttleIn ;
			if (altitude_control < 10) filterManual = false ;
		}
		else {
			altitude_control = 0 ;
		}
	}
	
	return ;
}
