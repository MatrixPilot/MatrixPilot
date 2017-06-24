// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
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

#include "libUDB.h"
#include "oscillator.h"
#include "interrupt.h"
#include "servoOut.h"
#include "servoOutPins.h"
#include "cll_io.h"

#if (MIPS == 64)
#define SCALE_FOR_PWM_OUT(x)    (x/2)
#elif (MIPS == 32)
#define SCALE_FOR_PWM_OUT(x)    (x*2)
#elif (MIPS == 16)
#define SCALE_FOR_PWM_OUT(x)    (x)
#else
#error Invalid MIPS Configuration
#endif

int16_t udb_pwOut[NUM_OUTPUTS+1];   // pulse widths for servo outputs
static volatile int16_t outputNum;


// initialize the PWM
void servoOut_init(void) // was called udb_init_pwm()
{
	int16_t i;
	for (i = 0; i <= NUM_OUTPUTS; i++)
	{
		udb_pwOut[i] = 0;
	}

	if (NUM_OUTPUTS >= 1)
	{
		// Set up Timer 4.  Use it to send PWM outputs manually, at high priority.
		T4CON = 0b1000000000000000; // turn on timer 4 with no prescaler
#if (MIPS == 64)
		T4CONbits.TCKPS = 2;        // prescaler 64:1
#else
		T4CONbits.TCKPS = 1;        // prescaler 8:1
#endif
		_T4IP = INT_PRI_T4;         // set interrupt priority
		_T4IE = 0;                  // disable timer 4 interrupt for now (enable for each set of pulses)
	}

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
#if (USE_CASTLE_LINK_THROTTLE != 1)
	_TRISD0 = 0;
#endif
	_TRISD1 = 0;
	_TRISD2 = 0;
	_TRISD3 = 0;
	_TRISD4 = 0;
	_TRISD5 = 0;
	_TRISD6 = 0;
	_TRISD7 = 0;
	if (NUM_OUTPUTS >= 9)  _TRISA4 = 0;
	if (NUM_OUTPUTS >= 10) _TRISA1 = 0;
#elif (BOARD_TYPE == AUAV3_BOARD)
	// port D
	TRISDbits.TRISD7 = 0;       // O4
	// port E
	TRISEbits.TRISE0 = 0;       // O2
	// port F
	TRISFbits.TRISF13 = 0;      // O7
	TRISFbits.TRISF12 = 0;      // O8
	// port G
	TRISGbits.TRISG0 = 0;       // O1
	TRISGbits.TRISG13 = 0;      // O3
	TRISGbits.TRISG14 = 0;      // O5
	TRISGbits.TRISG1 = 0;       // O6
#elif (BOARD_TYPE == AUAV4_BOARD)
#warning here
#else // Classic board
#error Invalid BOARD_TYPE
#endif
}

// saturation logic to maintain pulse width within bounds
// This takes a servo out value, and clips it to be within
// 3000-1000*SERVOSAT and 3000+1000*SERVOSAT (2000-4000 by default).
int16_t udb_servo_pulsesat(int32_t pw)
{
	if (pw > SERVOMAX) pw = SERVOMAX;
	if (pw < SERVOMIN) pw = SERVOMIN;
	return (int16_t)pw;
}

void udb_set_action_state(boolean newValue)
{
	ACTION_OUT_PIN = newValue;
}


void start_pwm_outputs(void)
{
	if (NUM_OUTPUTS > 0)
	{
		outputNum = 0;
		PR4 = SCALE_FOR_PWM_OUT(200);   // set timer to delay 0.1ms
		TMR4 = 0;                       // start timer at 0
		_T4IF = 0;                      // clear the interrupt
		_T4IE = 1;                      // enable timer 4 interrupt
	}
}

#if (RECORD_FREE_STACK_SPACE == 1)
extern uint16_t maxstack;
#endif

// Define HANDLE_SERVO_OUT as a macro to allow passing the pin as an argument
#define HANDLE_SERVO_OUT(channel, pin)                  \
{                                                       \
	if (NUM_OUTPUTS >= channel)                         \
	{                                                   \
		outputNum = channel;                            \
		if (udb_pwOut[channel] > 0)                     \
		{                                               \
			PR4 = SCALE_FOR_PWM_OUT(udb_pwOut[channel]);\
			pin = 1;                                    \
		}                                               \
		else                                            \
		{                                               \
			PR4 = SCALE_FOR_PWM_OUT(100);               \
			pin = 0;                                    \
		}                                               \
		TMR4 = 0;                                       \
	}                                                   \
	else                                                \
	{                                                   \
		_T4IE = 0;                                      \
	}                                                   \
}

#if (TEST_INTERRUPT_PRIORITY_TIMINGS == 1)
// When testing interrupt priorities, all servo output timings are sent to pin 8 
// Pins 1-7 for are then indicating the timing of  interrupt priorities
// No Servos should be attached to the board outputs, only a logic analyser.
void __attribute__((__interrupt__,__no_auto_psv__)) _T4Interrupt(void)
{
	indicate_loading_inter;
	set_ipl_on_output_pin;
	// interrupt_save_set_corcon;

	switch (outputNum) {
		case 0:
			HANDLE_SERVO_OUT(1, SERVO_OUT_PIN_8);
			break;
		case 1:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(2, SERVO_OUT_PIN_8);
			break;
		case 2:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(3, SERVO_OUT_PIN_8);
			break;
		case 3:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(4, SERVO_OUT_PIN_8);
			break;
		case 4:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(5, SERVO_OUT_PIN_8);
			break;
		case 5:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(6, SERVO_OUT_PIN_8);
			break;
		case 6:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(7, SERVO_OUT_PIN_8);
			break;
		case 7:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(8, SERVO_OUT_PIN_8);
			break;
		case 8:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(9, SERVO_OUT_PIN_8);
			break;
#ifdef SERVO_OUT_PIN_10
		case 9:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(10, SERVO_OUT_PIN_8);
			break;
		case 10:
			SERVO_OUT_PIN_8 = 0;   // end the pulse by setting the SERVO_OUT_PIN_10 pin low
			_T4IE = 0;              // disable timer 4 interrupt
			break;
#else
		case 9:
			SERVO_OUT_PIN_9 = 0;
			_T4IE = 0;              // disable timer 4 interrupt
			break;
#endif // SERVO_OUT_PIN_10
	}

	_T4IF = 0;                      // clear the interrupt

#if (RECORD_FREE_STACK_SPACE == 1)
	// Check stack space here because it's a high-priority ISR
	// which may have interrupted a whole chain of other ISRs,
	// So available stack space can get lowest here.
	uint16_t stack = SP_current();
	if (stack > maxstack)
	{
		maxstack = stack;
	}
#endif

	// interrupt_restore_corcon;
	unset_ipl_on_output_pin;
}
#else

void __attribute__((__interrupt__,__no_auto_psv__)) _T4Interrupt(void)
{
	indicate_loading_inter;
	// "set_ipl_on_output_pin" macro not here as _T4 Interrupt disabled when testing Interrupt Priorities
	// interrupt_save_set_corcon;

	switch (outputNum) {
		case 0:
#if (USE_CASTLE_LINK_THROTTLE != 1)
			HANDLE_SERVO_OUT(1, SERVO_OUT_PIN_1);
#else
			cll_start_servo_out(SCALE_FOR_PWM_OUT(udb_pwOut[1]), SCALE_FOR_PWM_OUT(100));
            outputNum++; // usually handled in HANDLE_SERVO_OUT()
#endif
			break;
		case 1:
#if (USE_CASTLE_LINK_THROTTLE != 1)
			SERVO_OUT_PIN_1 = 0;
#else
			cll_start_listening();
			if (outputNum >= NUM_OUTPUTS) _T4IE = 0;  // usually handled in HANDLE_SERVO_OUT()
#endif
			HANDLE_SERVO_OUT(2, SERVO_OUT_PIN_2);
			break;
		case 2:
			SERVO_OUT_PIN_2 = 0;
			HANDLE_SERVO_OUT(3, SERVO_OUT_PIN_3);
			break;
		case 3:
			SERVO_OUT_PIN_3 = 0;
			HANDLE_SERVO_OUT(4, SERVO_OUT_PIN_4);
			break;
		case 4:
			SERVO_OUT_PIN_4 = 0;
			HANDLE_SERVO_OUT(5, SERVO_OUT_PIN_5);
			break;
		case 5:
			SERVO_OUT_PIN_5 = 0;
			HANDLE_SERVO_OUT(6, SERVO_OUT_PIN_6);
			break;
		case 6:
			SERVO_OUT_PIN_6 = 0;
			HANDLE_SERVO_OUT(7, SERVO_OUT_PIN_7);
			break;
		case 7:
			SERVO_OUT_PIN_7 = 0;
			HANDLE_SERVO_OUT(8, SERVO_OUT_PIN_8);
			break;
		case 8:
			SERVO_OUT_PIN_8 = 0;
			HANDLE_SERVO_OUT(9, SERVO_OUT_PIN_9);
			break;
#ifdef SERVO_OUT_PIN_10
		case 9:
			SERVO_OUT_PIN_9 = 0;
			HANDLE_SERVO_OUT(10, SERVO_OUT_PIN_10);
			break;
		case 10:
			SERVO_OUT_PIN_10 = 0;   // end the pulse by setting the SERVO_OUT_PIN_10 pin low
			_T4IE = 0;              // disable timer 4 interrupt
			break;
#else
		case 9:
			SERVO_OUT_PIN_9 = 0;
			_T4IE = 0;              // disable timer 4 interrupt
			break;
#endif // SERVO_OUT_PIN_10
	}

	_T4IF = 0;                      // clear the interrupt

#if (RECORD_FREE_STACK_SPACE == 1)
	// Check stack space here because it's a high-priority ISR
	// which may have interrupted a whole chain of other ISRs,
	// So available stack space can get lowest here.
	uint16_t stack = SP_current();
	if (stack > maxstack)
	{
		maxstack = stack;
	}
#endif

	// interrupt_restore_corcon;
}
#endif

