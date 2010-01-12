#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	If the state machine selects pitch feedback, compute it from the pitch gyro and accelerometer.

int rtlkick = 0 ;

#define RTLKICK ((long)(RTL_PITCH_DOWN*(RMAX/57.3)))
#define INVNPITCH ((long)(INVERTED_NEUTRAL_PITCH*(RMAX/57.3)))
#define HOVERPOFFSET ((long)(HOVER_PITCH_OFFSET*(RMAX/57.3)))

int pitchgain = (int)(PITCHGAIN*RMAX) ;
int pitchkd = (int) (PITCHKD*SCALEGYRO*RMAX) ;

int hoverpitchgain = (int)(HOVER_PITCHGAIN*RMAX) ;
int hoverpitchkd = (int) (HOVER_PITCHKD*SCALEGYRO*RMAX) ;

int rudderElevMixGain = (int)(RMAX*RUDDER_ELEV_MIX) ;
int rollElevMixGain = (int)(RMAX*ROLL_ELEV_MIX) ;

int pitchrate ;
int navElevMix ;
int elevInput ;

void normalPitchCntrl(void) ;
void hoverPitchCntrl(void) ;


void pitchCntrl(void)
{
	if ( STABILIZE_HOVERING && desired_behavior._.hover )
	{
		hoverPitchCntrl() ;
	}
	else
	{
		normalPitchCntrl() ;
	}
	
	return ;
}


void normalPitchCntrl(void)
{
	union longww pitchAccum ;
	
#ifdef TestGains
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
#endif
	
	fractional rmat6 ;
	fractional rmat7 ;
	fractional rmat8 ;
	
	if ( !STABILIZE_INVERTED_FLIGHT || current_orientation != F_INVERTED )
	{
		rmat6 = rmat[6] ;
		rmat7 = rmat[7] ;
		rmat8 = rmat[8] ;
	}
	else
	{
		rmat6 = -rmat[6] ;
		rmat7 = -rmat[7] ;
		rmat8 = -rmat[8] ;
		pitchAltitudeAdjust = -pitchAltitudeAdjust - INVNPITCH ;
	}
	
	navElevMix = 0 ;
	if ( RUDDER_NAVIGATION && flags._.pitch_feedback )
	{
		pitchAccum.WW = __builtin_mulss( rmat6 , rudderElevMixGain ) << 1 ;
		pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 , yaw_control ) << 3 ;
		navElevMix += pitchAccum._.W1 ;
	}
	if ( AILERON_NAVIGATION && flags._.pitch_feedback )
	{
		pitchAccum.WW = __builtin_mulss( rmat6 , rollElevMixGain ) << 1 ;
		pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 , rmat[6] ) >> 3 ;
		navElevMix += pitchAccum._.W1 ;
	}

	pitchAccum.WW = ( __builtin_mulss( rmat8 , omegagyro[0] )
					- __builtin_mulss( rmat6 , omegagyro[2] )) << 1 ;
	pitchrate = pitchAccum._.W1 ;
	
	if ( (RUDDER_NAVIGATION || AILERON_NAVIGATION) && flags._.GPS_steering )
	{
		rtlkick = RTLKICK ;
	}
	else
	{
		rtlkick = 0 ;
	}
	
	if ( PITCH_STABILIZATION && flags._.pitch_feedback )
	{
		pitchAccum.WW = __builtin_mulss( rmat7 + pitchAltitudeAdjust, pitchgain ) 
					  + __builtin_mulss( pitchkd , pitchrate ) ;
	}
	else
	{
		pitchAccum.WW = 0 ;
	}
	
	pitch_control = (long)pitchAccum._.W1 + navElevMix ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}


void hoverPitchCntrl(void)
{
	union longww pitchAccum ;
	
	if ( flags._.pitch_feedback )
	{
		pitchAccum.WW = ( __builtin_mulss( -rmat[7] , omegagyro[0] )
						- __builtin_mulss( rmat[6] , omegagyro[1] )) << 1 ;
		pitchrate = pitchAccum._.W1 ;
		
		int elevInput = ( flags._.radio_on == 1 ) ? REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, pwIn[ELEVATOR_INPUT_CHANNEL] - pwTrim[ELEVATOR_INPUT_CHANNEL]) : 0 ;
		int manualPitchOffset = elevInput * (int)(RMAX/600);
		
		pitchAccum.WW = __builtin_mulss( rmat[8] + HOVERPOFFSET + manualPitchOffset , hoverpitchgain ) 
					  + __builtin_mulss( hoverpitchkd , pitchrate ) ;
	}
	else
	{
		pitchAccum.WW = 0 ;
	}
	
	pitch_control = (long)pitchAccum._.W1 ;
	
	return ;
}
