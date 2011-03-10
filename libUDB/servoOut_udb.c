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


#define SERVO_OUT_PIN_1			_LATE1
#define SERVO_OUT_PIN_2			_LATE3
#define SERVO_OUT_PIN_3			_LATE5

#if (USE_PPM_INPUT != 1)
	#define SERVO_OUT_PIN_4		_LATE0
	#define SERVO_OUT_PIN_5		_LATE2
	#define SERVO_OUT_PIN_6		_LATE4
	#define SERVO_OUT_PIN_7		_LATE4	// 7th Output is not valid without PPM
	#define SERVO_OUT_PIN_8		_LATE4	// 8th Output is not valid without PPM
	#define SERVO_OUT_PIN_9		_LATE4	// 9th Output is not valid without PPM
#elif (PPM_ALT_OUTPUT_PINS != 1)
	#define SERVO_OUT_PIN_4		_LATD1
	#define SERVO_OUT_PIN_5		_LATB5
	#define SERVO_OUT_PIN_6		_LATB4
	#define SERVO_OUT_PIN_7		_LATE0
	#define SERVO_OUT_PIN_8		_LATE2
	#define SERVO_OUT_PIN_9		_LATE4
#else
	#define SERVO_OUT_PIN_4		_LATE0
	#define SERVO_OUT_PIN_5		_LATE2
	#define SERVO_OUT_PIN_6		_LATE4
	#define SERVO_OUT_PIN_7		_LATD1
	#define SERVO_OUT_PIN_8		_LATB5
	#define SERVO_OUT_PIN_9		_LATB4
#endif


//	routines to drive the PWM pins for the servos,
//	assumes the use of the 16MHz crystal.

int udb_pwOut[NUM_OUTPUTS+1] ;	// pulse widths for servo outputs

int twentyHertzCounter = 0 ;
int outputNum ;


#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#define SCALE_FOR_PWM_OUT(x)		((x) << 1)
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#define PWMOUTSCALE					60398	// = 256*256*(3.6864/4)
#define SCALE_FOR_PWM_OUT(x)		(((union longww)(long)__builtin_muluu( (x) ,  PWMOUTSCALE ))._.W1)
#endif


void setupOutputs( void ) ;


void udb_init_pwm( void )	// initialize the PWM
{
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
	PTPER = 23040 ;	// 25 millisecond period at 58.982 Mz clock, prescale = 16	
	_PTCKPS = 2;	// prescaler = 16
#endif
	
	_PMOD1 = 1 ;	// independent PWM mode
	_PMOD2 = 1 ;
	_PMOD3 = 1 ;
	_PEN1L = 0 ; 	// low pins used as digital I/O
	_PEN2L = 0 ;
	_PEN3L = 0 ;
	_PEN1H = 0 ; 	// high pins used as digital I/O
	_PEN2H = 0 ;
	_PEN3H = 0 ;
	
	_PTEN = 1; 		// turn on the PWM 
	_PWMIF = 0 ;	// clear the PWM interrupt
	_PWMIP = 3 ;	// priority 3
	_PWMIE = 1 ;	// enable the PWM interrupt
	
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
	
	return ;
}


void udb_set_action_state(boolean newValue)
{
	SERVO_OUT_PIN_6 = newValue ;
}


void __attribute__((__interrupt__,__auto_psv__)) _PWMInterrupt(void)
{
	indicate_loading_inter ;
	// interrupt_save_extended_state ;

	//	Executes whatever needs to be done every 25 milliseconds, using the PWM clock.
	//	This is a good place to compute pulse widths for servos.
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
	if (NUM_OUTPUTS > 0)
	{
		outputNum = 0 ;
		PR4 = SCALE_FOR_PWM_OUT(200) ;	// set timer to delay 0.1ms
		
		TMR4 = 0 ;				// start timer at 0
		_T4IF = 0 ;				// clear the interrupt
		_T4IE = 1 ;				// enable timer 4 interrupt
	}
	
	return;
}


#if (RECORD_FREE_STACK_SPACE == 1)
extern unsigned int maxstack ;
#endif


// Define HANDLE_SERVO_OUT as a macro to allow passing the pin as an argument
#define HANDLE_SERVO_OUT(channel, pin) \
	if (NUM_OUTPUTS >= channel) \
	{ \
		outputNum = channel ; \
		if ( udb_pwOut[channel] > 0 ) \
		{ \
			PR4 = SCALE_FOR_PWM_OUT(udb_pwOut[channel]) ; \
			pin = 1 ;	\
		} \
		else \
		{ \
			PR4 = SCALE_FOR_PWM_OUT(100) ; \
			pin = 0 ; \
		} \
		TMR4 = 0 ; \
	} \
	else \
	{ \
		_T4IE = 0 ; \
	}


void __attribute__((__interrupt__,__auto_psv__)) _T4Interrupt(void)
{
	indicate_loading_inter ;
//	interrupt_save_extended_state ;
	
	switch ( outputNum ) {
		case 0:
			HANDLE_SERVO_OUT(1, SERVO_OUT_PIN_1) ;
			break ;
		case 1:
			SERVO_OUT_PIN_1 = 0 ;
			HANDLE_SERVO_OUT(2, SERVO_OUT_PIN_2) ;
			break ;
		case 2:
			SERVO_OUT_PIN_2 = 0 ;
			HANDLE_SERVO_OUT(3, SERVO_OUT_PIN_3) ;
			break ;
		case 3:
			SERVO_OUT_PIN_3 = 0 ;
			HANDLE_SERVO_OUT(4, SERVO_OUT_PIN_4) ;
			break ;
		case 4:
			SERVO_OUT_PIN_4 = 0 ;
			HANDLE_SERVO_OUT(5, SERVO_OUT_PIN_5) ;
			break ;
		case 5:
			SERVO_OUT_PIN_5 = 0 ;
			HANDLE_SERVO_OUT(6, SERVO_OUT_PIN_6) ;
			break ;
		case 6:
			SERVO_OUT_PIN_6 = 0 ;
			HANDLE_SERVO_OUT(7, SERVO_OUT_PIN_7) ;
			break ;
		case 7:
			SERVO_OUT_PIN_7 = 0 ;
			HANDLE_SERVO_OUT(8, SERVO_OUT_PIN_8) ;
			break ;
		case 8:
			SERVO_OUT_PIN_8 = 0 ;
			HANDLE_SERVO_OUT(9, SERVO_OUT_PIN_9) ;
			break ;
		case 9:
			SERVO_OUT_PIN_9 = 0 ;	// end the pulse by setting the SERVO_OUT_PIN_9 pin low
			_T4IE = 0 ;				// disable timer 4 interrupt
			break ;
	}
	
	_T4IF = 0 ;						// clear the interrupt
	
#if (RECORD_FREE_STACK_SPACE == 1)
	unsigned int stack = WREG15 ;
	if ( stack > maxstack )
	{
		maxstack = stack ;
	}
#endif
	
//	interrupt_restore_extended_state ;
	return;
}

#endif
