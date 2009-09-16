#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	Perform elevator control.
//	Use the radio to determine the baseline pulse width if the radio is on.
//	Otherwise, use the "trim" pulse width measured during power up.

//	If the state machine selects pitch feedback, compute it from the pitch gyro and accelerometer.


int pitchgain = (int) (PITCHGAIN*RMAX) ;
int pitchrate = 0 ;
int pitchkd = (int) (PITCHKD*RMAX) ;
int pitchbgain = (int) (8.0*PITCHBOOST) ;
long pitchboost = 0 ;

int rudderElevMixGain = (int) (RMAX*RUDDERELEVMIX) ;
int rudderElevMix ;

int pitchError ;
int elevInput ;

void elevatorCntrl(void)
{
	union longww elevAccum ;

	if ( flags._.pitch_feedback )
	{
		elevAccum.WW = __builtin_mulss( rmat[6] , rudderElevMixGain ) ;
		elevAccum.WW = __builtin_mulss( elevAccum._.W1 , rudderDeflection ) << 4 ;
		rudderElevMix = elevAccum._.W1 ;
	}
	else
	{
		rudderElevMix = 0 ;
	}

	elevAccum.WW = 	(	__builtin_mulss( rmat[8] , omegagyro[0] )
					-	__builtin_mulss( rmat[6] , omegagyro[2] ))<<1 ;
	pitchrate = elevAccum._.W1 ;
	if ( flags._.radio_on )
	{
		pwele = pwc8 ;
	}
	else
	{
		pwele = elevtrim ;
	}
#ifdef TestGains
	flags._.GPS_steering = 1 ;
	flags._.pitch_feedback = 1 ;
#endif 
	if ( flags._.pitch_feedback )
	{
		if( PORTDbits.RD2 )
		{
			elevInput = pwele - elevtrim ;
		}
		else
		{
			elevInput = elevtrim - pwele ;
		}

		elevAccum.WW = 		__builtin_mulss( rmat[7] + pitchAltitudeAdjust , pitchgain ) 
						+	__builtin_mulss( pitchkd , pitchrate ) ;
		pitchboost =  (__builtin_mulss(pitchbgain , (	elevInput ))>>3) ;
	}
	else
	{
		elevAccum.WW = 0 ;
		pitchboost = 0 ;
	}

	//	use channel 2 switch to reverse the polarity of the control feedback
	if ( PORTDbits.RD2 )
	{
		elevAccum.WW = (long)pwele + (long)elevAccum._.W1+(long)rudderElevMix + pitchboost ;
		PDC2 = pulsesat( elevAccum.WW ) ;
	}
	else
	{
		elevAccum.WW = (long)pwele - (long)elevAccum._.W1 -(long)rudderElevMix - pitchboost ;
		PDC2 = pulsesat( elevAccum.WW ) ;
	}
	return ;
}
