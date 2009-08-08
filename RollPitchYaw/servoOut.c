#include "p30f4011.h"
#include "defines.h"

//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int gpscount ; // counter used to initialize gps

void init_pwm( void )	// initialize the PWM
{
	PDC1 = PDC2 = PDC3 = 3000 ;

	firstsamp = 1;	// flag for the first sample
	calibcount = 400 ;	// wait 10 seconds for A/D filters to settle
	gpscount = 1000 ;  // wait 25 seconds for GPS to initialize

	TRISE = 0b1111111011000000 ;

	PTPER = 25000 ;			// 25 millisecond period at 16 Mz clock, prescale = 4	
	PTCONbits.PTCKPS = 1;	// prescaler = 4

	PWMCON1bits.PMOD1 = 1 ; // independent PWM mode
	PWMCON1bits.PMOD2 = 1 ;
	PWMCON1bits.PMOD3 = 1 ;
	PWMCON1bits.PEN1L = 0 ; // low pins used as digital I/O
	PWMCON1bits.PEN2L = 0 ;
	PWMCON1bits.PEN3L = 0 ;

	PTCONbits.PTEN = 1; 	// turn on the PWM 
	IFS2bits.PWMIF = 0 ; 	// clear the PWM interrupt
	IPC9bits.PWMIP = 3 ;    // priority 3

	//  note: at this point the PWM is running, so there are pulses going out,
	//	but the PWM interrupt is still off, so no interrupts are coming in yet to compute pulses.
	//  the PWM interrupt is turned on within the A/D interrupt processing
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _PWMInterrupt(void)
{
	//	Executes whatever needs to be done every 20 milliseconds, using the PWM clock.
	//	This is a good place to run the A/D digital filters and compute pulse widths for servos.
	//	Also, this is used to wait a few pulses before recording input DC offsets.
	indicate_loading_inter ;
	switch (calibcount ) {

	case 0 :
	{
		imu_demo() ;
		break ;
	}

	case 1: {
		// almost ready to turn the control on, save the input offsets
		xaccel.offset = xaccel.value ;
		xrate.offset = xrate.value ;
		yaccel.offset = yaccel.value ;
		yrate.offset = yrate.value ;
		zaccel.offset = zaccel.value - 2*GRAVITY ; // GRAVITY is measured in A-D/2 units
		zrate.offset = zrate.value ;
#ifdef VREF
		vref.offset = vref.value ;
#endif	
		break ;
	}

	default: {
	}
		// otherwise, there is not anything to do
		break ;
	}
	// count down the startup counter to 0
	if ( calibcount > 0 ) calibcount-- ;

	switch (gpscount ) {

	case 8:
	{
		U2BRG = 12 ;
	}
	case 10:
	{
		set_baud() ; //	change the baud rate if it was used at 4800 binary
		break ;
	}
	case 12:
	{
		set_bin() ;	// put into binary mode, if it is fresh from the factory
		break ;
	}
	default: {
	}
		// otherwise, there is not anything to do
		break ;
	}
	// count down the startup counter to 0
	if ( gpscount > 0 ) gpscount-- ;

	IFS2bits.PWMIF = 0 ; /* clear the interrupt */

	return ;
}

