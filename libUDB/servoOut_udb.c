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
#include "../libDCM/libDCM.h"

#if (BOARD_IS_CLASSIC_UDB == 1)


#if (USE_PPM_INPUT != 1)
	#define EXTRA_OUT_1		_LATE0
	#define EXTRA_OUT_2		_LATE2
	#define EXTRA_OUT_3		_LATE4
	#define EXTRA_OUT_4		_LATE4	// 7th Output is not valid without PPM
	#define EXTRA_OUT_5		_LATE4	// 8th Output is not valid without PPM
	#define EXTRA_OUT_6		_LATE4	// 9th Output is not valid without PPM
#elif (PPM_ALT_OUTPUT_PINS != 1)
	#define EXTRA_OUT_1		_LATD1
	#define EXTRA_OUT_2		_LATB5
	#define EXTRA_OUT_3		_LATB4
	#define EXTRA_OUT_4		_LATE0
	#define EXTRA_OUT_5		_LATE2
	#define EXTRA_OUT_6		_LATE4
#else
	#define EXTRA_OUT_1		_LATE0
	#define EXTRA_OUT_2		_LATE2
	#define EXTRA_OUT_3		_LATE4
	#define EXTRA_OUT_4		_LATD1
	#define EXTRA_OUT_5		_LATB5
	#define EXTRA_OUT_6		_LATB4
#endif


//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int udb_pwOut[NUM_OUTPUTS+1] ;	// pulse widths for servo outputs

int twentyHertzCounter = 0 ;
int outputNum ;


#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#define SCALE_FOR_PWM_OUT(x)		(x)
#define SCALE_FOR_EXTRA_PWM_OUT(x)	((x) << 1)
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#define PWMOUTSCALE					60398	// = 256*256*(3.6864/4)
#define SCALE_FOR_PWM_OUT(x)		(((union longww)(long)__builtin_muluu( (x) ,  PWMOUTSCALE ))._.W1)
#define SCALE_FOR_EXTRA_PWM_OUT(x)	SCALE_FOR_PWM_OUT(x)
#endif


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
#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
	PTPER = 25000 ;	// 25 millisecond period at 16 Mz clock, prescale = 4	
	_PTCKPS = 1;	// prescaler = 4
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
	PTPER = 27126 ;	// 25 millisecond period at 58.982 Mz clock, prescale = 16	
	_PTCKPS = 2;	// prescaler = 16
#endif

	_PMOD1 = 1 ;	// independent PWM mode
	_PMOD2 = 1 ;
	_PMOD3 = 1 ;
	_PEN1L = 0 ; 	// low pins used as digital I/O
	_PEN2L = 0 ;
	_PEN3L = 0 ;
	
	_PTEN = 1; 		// turn on the PWM 
	_PWMIF = 0 ; 	// clear the PWM interrupt
	_PWMIP = 3 ;    // priority 3
	
	if (NUM_OUTPUTS >= 4)
	{
		T4CON = 0b1000000000000000  ;		// turn on timer 4 with no prescaler
#if ( CLOCK_CONFIG == FRC8X_CLOCK )
		T4CONbits.TCKPS = 1 ;				// prescaler 8:1
#endif
		_T4IP = 7 ;							// priority 7
		_T4IE = 0 ;							// disable timer 4 interrupt for now (enable for each set of pulses)
#if (USE_PPM_INPUT == 1)
#if (PPM_ALT_OUTPUT_PINS != 1)
		_TRISD1 = 0 ;						// Set D1 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 5) _TRISB5 = 0 ;	// Set B5 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 6) _TRISB4 = 0 ;	// Set B4 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 7) _TRISE0 = 0 ;	// Set E0 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 8) _TRISE2 = 0 ;	// Set E2 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 9) _TRISE4 = 0 ;	// Set E4 to be an output if we're using PPM
#else
		_TRISE0 = 0 ;						// Set E0 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 5) _TRISE2 = 0 ;	// Set E2 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 6) _TRISE4 = 0 ;	// Set E4 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 7) _TRISD1 = 0 ;	// Set D1 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 8) _TRISB5 = 0 ;	// Set B5 to be an output if we're using PPM
		if (NUM_OUTPUTS >= 9) _TRISB4 = 0 ;	// Set B4 to be an output if we're using PPM
#endif
#endif
	}
	
	//  note: at this point the PWM is running, so there are pulses going out,
	//	but the PWM interrupt is still off, so no interrupts are coming in yet to compute pulses.
	//  the PWM interrupt is turned on within the A/D interrupt processing
	
	return ;
}


void udb_set_action_state(boolean newValue)
{
	EXTRA_OUT_3 = newValue ;
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
	PDC1 = SCALE_FOR_PWM_OUT(udb_pwOut[1]) ;
	PDC2 = SCALE_FOR_PWM_OUT(udb_pwOut[2]) ;
	PDC3 = SCALE_FOR_PWM_OUT(udb_pwOut[3]) ;
	
	if (NUM_OUTPUTS > 3)
	{
		outputNum = 3 ;
		PR4 = SCALE_FOR_EXTRA_PWM_OUT(2000) ;	// set timer to delay 1ms
		
		TMR4 = 0 ;				// start timer at 0
		_T4IF = 0 ;				// clear the interrupt
		_T4IE = 1 ;				// enable timer 4 interrupt
	}
	
	return;
}


#if (RECORD_FREE_STACK_SPACE == 1)
extern unsigned int maxstack ;
#endif

void __attribute__((__interrupt__,__no_auto_psv__)) _T4Interrupt(void)
{
	interrupt_save_extended_state ;
	
	indicate_loading_inter ;
	
	switch ( outputNum ) {
		case 3:
			if (NUM_OUTPUTS > 3)
			{
				outputNum = 4 ;
				if ( udb_pwOut[4] > 0 )
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(udb_pwOut[4]) ;	// set timer width
					EXTRA_OUT_1 = 1 ;			// start the pulse by setting the EXTRA_OUT_1 pin high (output 4)
				}
				else
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(100) ;	// set timer width
					EXTRA_OUT_1 = 0 ;			// skip the pulse by setting the EXTRA_OUT_1 pin low (output 4)
				}	
				TMR4 = 0 ;						// start timer at 0
			}
			else
			{
				_T4IE = 0 ;						// disable timer 4 interrupt
			}
			break ;
		
		case 4:
			EXTRA_OUT_1 = 0 ;					// end the pulse by setting the EXTRA_OUT_1 pin low (output 4)
			if (NUM_OUTPUTS > 4)
			{
				outputNum = 5 ;
				if ( udb_pwOut[5] > 0 )
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(udb_pwOut[5]) ;	// set timer width
					EXTRA_OUT_2 = 1 ;			// start the pulse by setting the EXTRA_OUT_2 pin high (output 5)
				}
				else
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(100) ;	// set timer width
					EXTRA_OUT_2 = 0 ;			// skip the pulse by setting the EXTRA_OUT_2 pin low (output 5)
				}	
				TMR4 = 0 ;						// start timer at 0
			}
			else
			{
				_T4IE = 0 ;						// disable timer 4 interrupt
			}
			break ;
		
		case 5:
			EXTRA_OUT_2 = 0 ;					// end the pulse by setting the EXTRA_OUT_2 pin low (output 5)
			if (NUM_OUTPUTS > 5)
			{
				outputNum = 6 ;
				if ( udb_pwOut[6] > 0 )
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(udb_pwOut[6]) ;	// set timer width
					EXTRA_OUT_3 = 1 ;			// start the pulse by setting the EXTRA_OUT_3 pin high (output 6)
				}
				else
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(100) ;	// set timer width
					EXTRA_OUT_3 = 0 ;			// start the pulse by setting the EXTRA_OUT_3 pin high (output 6)
				}
				TMR4 = 0 ;						// start timer at 0
			}
			else
			{
				_T4IE = 0 ;						// disable timer 4 interrupt
			}
			break ;
		
		case 6:
			EXTRA_OUT_3 = 0 ;					// end the pulse by setting the EXTRA_OUT_3 pin low (output 6)
			if (NUM_OUTPUTS > 6)
			{
				outputNum = 7 ;
				if ( udb_pwOut[7] > 0 )
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(udb_pwOut[7]) ;	// set timer width
					EXTRA_OUT_4 = 1 ;			// start the pulse by setting the EXTRA_OUT_4 pin high (output 7)
				}
				else
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(100) ;	// set timer width
					EXTRA_OUT_4 = 0 ;			// start the pulse by setting the EXTRA_OUT_4 pin high (output 7)
				}
				TMR4 = 0 ;						// start timer at 0
			}
			else
			{
				_T4IE = 0 ;						// disable timer 4 interrupt
			}
			break ;
		
		case 7:
			EXTRA_OUT_4 = 0 ;					// end the pulse by setting the EXTRA_OUT_4 pin low (output 7)
			if (NUM_OUTPUTS > 7)
			{
				outputNum = 8 ;
				if ( udb_pwOut[8] > 0 )
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(udb_pwOut[8]) ;	// set timer width
					EXTRA_OUT_5 = 1 ;			// start the pulse by setting the EXTRA_OUT_5 pin high (output 8)
				}
				else
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(100) ;	// set timer width
					EXTRA_OUT_5 = 0 ;			// start the pulse by setting the EXTRA_OUT_5 pin high (output 8)
				}
				TMR4 = 0 ;						// start timer at 0
			}
			else
			{
				_T4IE = 0 ;						// disable timer 4 interrupt
			}
			break ;
		
		case 8:
			EXTRA_OUT_5 = 0 ;					// end the pulse by setting the EXTRA_OUT_5 pin low (output 8)
			if (NUM_OUTPUTS > 8)
			{
				outputNum = 9 ;
				if ( udb_pwOut[9] > 0 )
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(udb_pwOut[9]) ;	// set timer width
					EXTRA_OUT_6 = 1 ;			// start the pulse by setting the EXTRA_OUT_6 pin high (output 9)
				}
				else
				{
					PR4 = SCALE_FOR_EXTRA_PWM_OUT(100) ;	// set timer width
					EXTRA_OUT_6 = 0 ;			// start the pulse by setting the EXTRA_OUT_6 pin high (output 9)
				}
				TMR4 = 0 ;						// start timer at 0
			}
			else
			{
				_T4IE = 0 ;						// disable timer 4 interrupt
			}
			break ;
		
		case 9:
			EXTRA_OUT_6 = 0 ;					// end the pulse by setting the EXTRA_OUT_6 pin low (output 9)
			_T4IE = 0 ;							// disable timer 4 interrupt
			break ;
	}
	
	_T4IF = 0 ;					// clear the interrupt
	
#if (RECORD_FREE_STACK_SPACE == 1)
	unsigned int stack = WREG15 ;
	if ( stack > maxstack )
	{
		maxstack = stack ;
	}
#endif
	
	interrupt_restore_extended_state ;
	return;
}

#endif
