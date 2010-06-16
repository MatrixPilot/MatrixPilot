// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

unsigned int cpu_timer = 0 ;
boolean skip_timer_reset = 1 ; 

void init_clock(void)	/* initialize timer 1 and LEDs */
{
	TRISF = 0b1111111111101100 ;

	TMR1 = 0 ; 				// initialize timer
	PR1 = tmr1_period ;		// set period register
	T1CONbits.TCKPS = 3 ;	// prescaler = 256 option
	T1CONbits.TCS = 0 ;		// use the crystal to drive the clock
	IPC0bits.T1IP = 3 ;		// Set to 3 to improve accuracy of cpu timing
	IFS0bits.T1IF = 0 ;		// clear the interrupt
	IEC0bits.T1IE = 1 ;		// enable the interrupt
	
	T1CONbits.TON = 1 ;		// turn on timer 1

	// Timer 5 is used to measure time spent per second in interrupt routines
	// which enables the calculation of the CPU loading.
	TMR5 = 0 ; 				// initialize timer
	T5CONbits.TCKPS = 3 ;	// prescaler = 256 option
	T5CONbits.TCS = 0 ;	    // use the chrstal to drive the clock
	IEC1bits.T5IE = 0 ;		// disable the interrupt
	
	// Timer 5 will be turned on in interrupt routines and turned off in main()
	T5CONbits.TON = 0 ;		// turn off timer 5
	timer_5_on = 0;
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _T1Interrupt(void) 
// excute whatever needs to run in the background, once every 0.5 seconds
{
	// interrupt_save_extended_state ;
	
	indicate_loading_inter ;

	// capture cpu_timer once per second.
	if (skip_timer_reset)
	{
		// catch another second 1/2 second in timer 5
		skip_timer_reset = 0;
	}
	else
	{
		cpu_timer = TMR5 ;
		T5CONbits.TON = 0 ;		// turn off timer 5 
		TMR5 = 0 ;				// reset timer 5 to 0
		T5CONbits.TON = 1 ;		// turn on timer 5
		timer_5_on = 1;
		skip_timer_reset = 1;
	}

	//	run the state machine 
	state_machine() ;
	
	IFS0bits.T1IF = 0 ;			// clear the interrupt
	
	// interrupt_restore_extended_state ;
	return ;
}
