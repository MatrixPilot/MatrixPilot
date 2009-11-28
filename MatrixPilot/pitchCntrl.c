#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	If the state machine selects pitch feedback, compute it from the pitch gyro and accelerometer.

int rtlkick = 0 ;

#define RTLKICK RMAX*(RTL_PITCH_DOWN/57.3)

int pitchgain = (int)(PITCHGAIN*RMAX) ;
int pitchrate = 0 ;
int pitchkd = (int) (PITCHKD*SCALEGYRO*RMAX) ;

int rudderElevMixGain = (int)(RMAX*RUDDER_ELEV_MIX) ;
int rollElevMixGain = (int)(RMAX*ROLL_ELEV_MIX) ;
int navElevMix ;

int elevInput ;

void pitchCntrl(void)
{
	union longww pitchAccum ;
	
#ifdef TestGains
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
#endif
	
	navElevMix = 0;
	if ( RUDDER_NAVIGATION && flags._.pitch_feedback )
	{
		pitchAccum.WW = __builtin_mulss( rmat[6] , rudderElevMixGain ) ;
		pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 , yaw_control ) << 4 ;
		navElevMix += pitchAccum._.W1 ;
	}
	if ( AILERON_NAVIGATION && flags._.pitch_feedback )
	{
		pitchAccum.WW = __builtin_mulss( rmat[6] , rollElevMixGain ) << 1 ;
		pitchAccum.WW = __builtin_mulss( pitchAccum._.W1 , rmat[6] )  >> 3 ;
		navElevMix += pitchAccum._.W1 ;
	}

	pitchAccum.WW = (	__builtin_mulss( rmat[8] , omegagyro[0] )
					-	__builtin_mulss( rmat[6] , omegagyro[2] ))<<1 ;
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
		pitchAccum.WW = 	__builtin_mulss( rmat[7] + pitchAltitudeAdjust, pitchgain ) 
						+	__builtin_mulss( pitchkd , pitchrate ) ;
	}
	else
	{
		pitchAccum.WW = 0 ;
	}
	
	pitch_control = (long)pitchAccum._.W1 + navElevMix ;
	// Servo reversing is handled in servoMix.c
	
	return ;
}
