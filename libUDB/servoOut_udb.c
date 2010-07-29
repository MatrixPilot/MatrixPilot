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


#include "libUDB_internal.h"

#if (BOARD_IS_CLASSIC_UDB == 1)

//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int udb_pwOut[MAX_OUTPUTS+1] ;	// pulse widths for servo outputs

int twentyHertzCounter = 0 ;
int outputNum ;


void setupOutputs( void ) ;


void udb_init_pwm( void )	// initialize the PWM
{
	PDC1 = PDC2 = PDC3 = 0 ;
	
	int i;
	for (i=0; i <= NUM_OUTPUTS; i++)
		udb_pwOut[i] = 0;
	
#if (NORADIO == 1)
	udb_flags._.radio_on = 1 ;
#endif
	
	TRISE = 0b1111111111000000 ;
	PTPER = 25000 ;	// 25 millisecond period at 16 Mz clock, prescale = 4	
	_PTCKPS = 1;	// prescaler = 4

	_PMOD1 = 1 ;	// independent PWM mode
	_PMOD2 = 1 ;
	_PMOD3 = 1 ;
	_PEN1L = 0 ; 	// low pins used as digital I/O
	_PEN2L = 0 ;
	_PEN3L = 0 ;
	
	_PTEN = 1; 		// turn on the PWM 
	_PWMIF = 0 ; 	// clear the PWM interrupt
	_PWMIP = 3 ;    // priority 3
	
	if (NUM_OUTPUTS > 3)
	{
		T4CON = 0b1000000000000000  ;	// turn on timer 4 with no prescaler
		_T4IP = 7 ;						// priority 7
		_T4IE = 0 ;						// disable timer 4 interrupt for now (enable for each set of pulses)
	}
	
	//  note: at this point the PWM is running, so there are pulses going out,
	//	but the PWM interrupt is still off, so no interrupts are coming in yet to compute pulses.
	//  the PWM interrupt is turned on within the A/D interrupt processing
	
	return ;
}


void udb_set_action_state(boolean newValue)
{
	_LATE4 = newValue ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _PWMInterrupt(void)
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
	setupOutputs() ;
	
	_PWMIF = 0 ; /* clear the interrupt */
	
	// interrupt_restore_extended_state ;
	return ;
}


void setupOutputs( void )
{
	PDC1 = udb_pwOut[1] ;
	PDC2 = udb_pwOut[2] ;
	PDC3 = udb_pwOut[3] ;
	
	if (NUM_OUTPUTS > 3)
	{
		outputNum = 4 ;
		if ( udb_pwOut[4] > 0 )
		{
			PR4 = (udb_pwOut[4] << 1) ;	// set timer to the pulse width
			_LATE0 = 1 ;	// start the pulse by setting the E0 pin high (output 4)
		}
		else
		{
			PR4 = 100 ;				// set timer to a short wait
			_LATE0 = 0 ;	// skip the pulse by setting the E0 pin low (output 4)
		}	
		TMR4 = 0 ;				// start timer at 0
		_T4IF = 0 ;		// clear the interrupt
		_T4IE = 1 ;		// enable timer 4 interrupt
	}
	
	return;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _T4Interrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	switch ( outputNum ) {
		case 4:
			_LATE0 = 0 ;		// end the pulse by setting the E0 pin low (output 4)
			if (NUM_OUTPUTS > 4)
			{
				outputNum = 5 ;
				if ( udb_pwOut[5] > 0 )
				{
					PR4 = (udb_pwOut[5] << 1) ;	// set timer to the pulse width
					_LATE2 = 1 ;	// start the pulse by setting the E2 pin high (output 5)
				}
				else
				{
					PR4 = 100 ;				// set timer to the pulse width
					_LATE2 = 0 ;	// skip the pulse by setting the E2 pin low (output 5)
				}	
				TMR4 = 0 ;				// start timer at 0
			}
			else
			{
				_T4IE = 0 ;		// disable timer 4 interrupt
			}
			break ;
		
		case 5:
			_LATE2 = 0 ;		// end the pulse by setting the E2 pin low (output 5)
			if (NUM_OUTPUTS > 5)
			{
				outputNum = 6 ;
				if ( udb_pwOut[6] > 0 )
				{
					PR4 = (udb_pwOut[6] << 1) ;	// set timer to the pulse width
					_LATE4 = 1 ;	// start the pulse by setting the E4 pin high (output 6)
				}
				else
				{
					PR4 = 100 ;				// set timer to the pulse width
					_LATE4 = 0 ;	// start the pulse by setting the E4 pin high (output 6)
				}
				TMR4 = 0 ;				// start timer at 0
			}
			else
			{
				_T4IE = 0 ;		// disable timer 4 interrupt
			}
			break ;
		case 6:
			_LATE4 = 0 ;		// end the pulse by setting the E4 pin low (output 6)
			_T4IE = 0 ;			// disable timer 4 interrupt
			break ;
	}
	
	_T4IF = 0 ;					// clear the interrupt
	
	interrupt_restore_extended_state ;
	return;
}

#endif
