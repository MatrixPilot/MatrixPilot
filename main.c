#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	main program for testing the IMU.
//	Select device options:

_FOSC( CSW_FSCM_OFF & HS ) ; 	// external high speed crystal
_FWDT( WDT_OFF ) ;				// no watchdog timer
_FBORPOR( 	PBOR_OFF & // brown out off
			MCLR_EN &  // enable MCLR
			RST_PWMPIN & // pwm pins as pwm
			PWMxH_ACT_HI & // PWMH is active high
			PWMxL_ACT_HI ) ; // PMWL is active high
_FGS( CODE_PROT_OFF ) ; // no protection
_FICD( 0xC003 ) ;		// normal use of debugging port

void init_all(void)
{
	TRISFbits.TRISF0 = 0 ;
	init_capture() ; 	// initialize capture module
	init_pwm() ;		//  set up the PWM
	init_ADC() ;		//  initialze the AD
	init_GPS2() ;    	//  set up communications to the GPS
	init_clock() ; 		//  turn on the clock
	init_T3()  ;		//  enable the T3 internally triggered interrupt
	init_states() ;
	return ;
}

int main (void)
{	//	Initialize all modules and turn on the interrupts.
	init_all() 		;   // initialize interrupt handlers
	SRbits.IPL = 0 ;	// turn on all interrupt priorities
	while (1)			//  nothing else to do...entirely interrupt driven
	{

	}
	return 0 ;
}
	
