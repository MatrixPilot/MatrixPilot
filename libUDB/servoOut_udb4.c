// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

#define TMR3_PERIOD 50000  //  25 milliseconds

#include "libUDB_internal.h"

#if (BOARD_TYPE == UDB4_BOARD)

int udb_pwOut[MAX_OUTPUTS+1] ;	// pulse widths for servo outputs

int twentyHertzCounter = 0 ;


void udb_init_pwm( void )	// initialize the PWM
{
	int i;
	for (i=0; i <= NUM_OUTPUTS; i++)
		udb_pwOut[i] = 0;
	
#if (NORADIO == 1)
	udb_flags._.radio_on = 1 ;
#endif
	
	OC1CONbits.OCM = 
	OC2CONbits.OCM = 
	OC3CONbits.OCM = 
	OC4CONbits.OCM = 
	OC5CONbits.OCM = 
	OC6CONbits.OCM = 
	OC7CONbits.OCM = 
	OC8CONbits.OCM = 0 ;  // disable
	
	OC1R = OC2R = OC3R = OC4R = OC5R = OC6R = OC7R = OC8R = 0 ;  // no first pulse
	OC1RS = OC2RS = OC3RS = OC4RS = OC5RS = OC6RS = OC7RS = OC8RS = 0 ; // initial pulse
	
	OC1CONbits.OCM = 
	OC2CONbits.OCM = 
	OC3CONbits.OCM = 
	OC4CONbits.OCM = 
	OC5CONbits.OCM = 
	OC6CONbits.OCM = 
	OC7CONbits.OCM = 
	OC8CONbits.OCM = 6 ;  // enable

	OC1CONbits.OCTSEL =
	OC2CONbits.OCTSEL =
	OC3CONbits.OCTSEL =
	OC4CONbits.OCTSEL =
	OC5CONbits.OCTSEL =
	OC6CONbits.OCTSEL =
	OC7CONbits.OCTSEL =
	OC8CONbits.OCTSEL = 1 ;  // timer 3


	TMR3 = 0 ; 				// initialize timer
	PR3 = TMR3_PERIOD ;		// set period register
	T3CONbits.TCKPS = 1 ;	// prescaler = 8 option
	T3CONbits.TCS = 0 ;		// use the internal clock
	_T3IP = 3 ;				//
	_T3IF = 0 ;				// clear the interrupt
	_T3IE = 1 ;				// enable the interrupt
	T3CONbits.TON = 1 ;		// turn on timer 3
	
	return ;
}


void udb_set_action_state(boolean newValue)
{
	_LATE4 = newValue ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _T3Interrupt(void) 
{
	// interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	//	Executes whatever needs to be done every 25 milliseconds, using the PWM clock.
	//	This is a good place to run the A/D digital filters and compute pulse widths for servos.
	//	Also, this is used to wait a few pulses before recording input DC offsets.

#if ( NORADIO == 0 )
	twentyHertzCounter++ ;
	if ( twentyHertzCounter >= 2 )
	{
		if ( failSafePulses == 0 )
		{
			udb_flags._.radio_on = 0 ;
			LED_GREEN = LED_OFF ;
		}
		else if ( failSafePulses >= 2 )
		{
			udb_flags._.radio_on = 1 ;
			LED_GREEN = LED_ON ;
		}
		twentyHertzCounter = 0 ;
		failSafePulses = 0 ;
	}
#endif
	
	udb_servo_callback_prepare_outputs() ;
	
	OC1RS = udb_pwOut[1] ;
	OC2RS = udb_pwOut[2] ;
	OC3RS = udb_pwOut[3] ;
	OC4RS = udb_pwOut[4] ;
	OC5RS = udb_pwOut[5] ;
	OC6RS = udb_pwOut[6] ;
	OC7RS = udb_pwOut[7] ;
	OC8RS = udb_pwOut[8] ;
	
	_T3IF = 0 ;		// clear the interrupt
	
	// interrupt_restore_extended_state ;
	return ;
}

#endif
