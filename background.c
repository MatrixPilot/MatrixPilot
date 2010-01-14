#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

void init_clock(void)	/* initialize timer 1 and LEDs */
{
	TRISF = 0b1111111111101100 ;

	TMR1 = 0 ; 				// initialize timer
	PR1 = tmr1_period ;		// set period register
	T1CONbits.TCKPS = 3 ;	// prescaler = 256 option
	T1CONbits.TCS = 0 ;		// use the crystal to drive the clock
	IPC0bits.T1IP = 1 ;		// low priority, background is not time critical
	IFS0bits.T1IF = 0 ;		// clear the interrupt
	IEC0bits.T1IE = 1 ;		// enable the interrupt

	T1CONbits.TON = 1 ;		// turn on timer 1
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _T1Interrupt(void) 
// excute whatever needs to run in the background, once every 0.5 seconds
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
//	run the state machine 
	state_machine() ;

	IFS0bits.T1IF = 0 ;			// clear the interrupt
	
	interrupt_restore_extended_state ;
	return ;
}
