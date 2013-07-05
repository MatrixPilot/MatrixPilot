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

//#include "defines.h"
#include "options.h"
#include "libUDB_internal.h"
#include "../libDCM/libDCM.h"
#include "oscillator.h"
#include "interrupt.h"

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == AUAV2_BOARD)

#define SERVO_OUT_PIN_1         _LATD0
#define SERVO_OUT_PIN_2         _LATD1
#define SERVO_OUT_PIN_3         _LATD2
#define SERVO_OUT_PIN_4         _LATD3
#define SERVO_OUT_PIN_5         _LATD4
#define SERVO_OUT_PIN_6         _LATD5
#define SERVO_OUT_PIN_7         _LATD6
#define SERVO_OUT_PIN_8         _LATD7
#define SERVO_OUT_PIN_9         _LATA4
#define SERVO_OUT_PIN_10        _LATA1

#if ((BOARD_TYPE == AUAV2_BOARD) == 0)
#define ACTION_OUT_PIN          SERVO_OUT_PIN_9
#else
#define ACTION_OUT_PIN          SERVO_OUT_PIN_6
#endif

#elif (BOARD_TYPE == AUAV3_BOARD)

#define SERVO_OUT_PIN_1         _LATG0
#define SERVO_OUT_PIN_2         _LATE0
#define SERVO_OUT_PIN_3         _LATG13
#define SERVO_OUT_PIN_4         _LATD7
#define SERVO_OUT_PIN_5         _LATG14
#define SERVO_OUT_PIN_6         _LATG1
#define SERVO_OUT_PIN_7         _LATF13
#define SERVO_OUT_PIN_8         _LATF12
#define SERVO_OUT_PIN_9         _LATF12
#define SERVO_OUT_PIN_10        _LATF12
#define ACTION_OUT_PIN          SERVO_OUT_PIN_8

#if (NUM_OUTPUTS > 8)
#error "max of 8 servo outputs currently supported for AUAV3"
#endif

#else
#error Invalid BOARD_TYPE
#endif

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
// Timer 3 for Output Compare module clocks at 5MHz
#define PWMOUTSCALE (FREQOSC / 32E6)
#define T3FREQ (2000000 * PWMOUTSCALE)
// Timer 3 period is 1 / (ESC_HZ)
#define T3PERIOD (T3FREQ / ESC_HZ)
inline int scale_pwm_out(int channel) {
	union longww pww;
	pww.WW = __builtin_muluu(udb_pwOut[channel], (unsigned int)(65536 * PWMOUTSCALE / 4));
	pww.WW <<= 2;
	return pww._.W1;
}
#endif // AIRFRAME_TYPE

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
int16_t outputNum;


void udb_init_pwm(void) // initialize the PWM
{
	int16_t i;
	for (i=0; i <= NUM_OUTPUTS; i++)
	{
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
		udb_pwOut[i] = FAILSAFE_INPUT_MIN;
#else
		udb_pwOut[i] = 0;
#endif
	}

	if (NUM_OUTPUTS >= 1)
	{
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE & AUAV2_BOARD)
		// changed to Timer3 and Output Compare Module for PWM out
		// Since Output Compare mode uses 16 bit registers for both period and duty cycle, the max period at 5MHz Timer3 rate
		// is 65536 / 5e6 = 76.3Hz. At 400Hz, period is 12,500 counts, 1500usec is 7500 counts
		// Initialize and enable Timer3
		T3CONbits.TON = 0;          // Disable Timer
		T3CONbits.TCS = 0;          // Select internal instruction cycle clock
		T3CONbits.TGATE = 0;        // Disable Gated Timer mode
		T3CONbits.TCKPS = 0b01;     // Select 8:1 Prescaler 16MHz/8 = 2MHz 40MHz/8 = 5MHz
		TMR3 = 0x00;                // Clear timer register
		PR3 = T3PERIOD;             // Load the period value
		IEC0bits.T3IE = 0;          // disable interrupts
		T3CONbits.TON = 1;          // Start timer
#endif

#else // AIRFRAME_TYPE
		// Set up Timer 4.  Use it to send PWM outputs manually, at high priority.
		T4CON = 0b1000000000000000; // turn on timer 4 with no prescaler
#if (MIPS == 64)
		T4CONbits.TCKPS = 2;        // prescaler 64:1
#else
		T4CONbits.TCKPS = 1;        // prescaler 8:1
#endif
		_T4IP = INT_PRI_T4;         // set interrupt priority
		_T4IE = 0;                  // disable timer 4 interrupt for now (enable for each set of pulses)
#endif // AIRFRAME_TYPE
	}

#ifdef __dsPIC33EP512MU810__
#define OC1CONbits OC1CON1bits
#define OC2CONbits OC2CON1bits
#define OC3CONbits OC3CON1bits
#define OC4CONbits OC4CON1bits
#endif

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
	// OC modules 1-8 are used for outputs
	// On the UDB4, these are labeled as outputs, on the AUAV2_alpha1 they are labeled I1-I8

	// configure OC1-8 as output pins
	TRISD &= 0xFF00;                // clear _TRISD0-7

	// Initialize Output Compare Module
	OC1CONbits.OCM = 0b000;         // Disable Output Compare Module
	OC1R = FAILSAFE_INPUT_MIN;      // Write the duty cycle for the first PWM pulse (1msec = 250 counts)
	OC1RS = FAILSAFE_INPUT_MIN;     // Write the duty cycle for the second PWM pulse
	OC1CONbits.OCTSEL = 1;          // Select Timer 3 as output compare time base
	OC1R = 100;                     // Load the Compare Register Value
	OC1CONbits.OCM = 0b110;         // Select the Output Compare mode

	OC2CONbits.OCM = 0b000;         // Disable Output Compare Module
	OC2R = FAILSAFE_INPUT_MIN;      // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
	OC2RS = FAILSAFE_INPUT_MIN;     // Write the duty cycle for the second PWM pulse
	OC2CONbits.OCTSEL = 1;          // Select Timer 3 as output compare time base
	OC2R = 100;                     // Load the Compare Register Value
	OC2CONbits.OCM = 0b110;         // Select the Output Compare mode

	OC3CONbits.OCM = 0b000;         // Disable Output Compare Module
	OC3R = FAILSAFE_INPUT_MIN;      // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
	OC3RS = FAILSAFE_INPUT_MIN;     // Write the duty cycle for the second PWM pulse
	OC3CONbits.OCTSEL = 1;          // Select Timer 3 as output compare time base
	OC3R = 100;                     // Load the Compare Register Value
	OC3CONbits.OCM = 0b110;         // Select the Output Compare mode

	OC4CONbits.OCM = 0b000;         // Disable Output Compare Module
	OC4R = FAILSAFE_INPUT_MIN;      // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
	OC4RS = FAILSAFE_INPUT_MIN;     // Write the duty cycle for the second PWM pulse
	OC4CONbits.OCTSEL = 1;          // Select Timer 3 as output compare time base
	OC4R = 100;                     // Load the Compare Register Value
	OC4CONbits.OCM = 0b110;         // Select the Output Compare mode

#else // AIRFRAME_TYPE

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == AUAV2_BOARD)
	_TRISD0 = 0; _TRISD1 = 0; _TRISD2 = 0; _TRISD3 = 0; _TRISD4 = 0; _TRISD5 = 0; _TRISD6 = 0; _TRISD7 = 0;
#endif
#if (BOARD_TYPE == UDB4_BOARD)
	if (NUM_OUTPUTS >= 9)  _TRISA4 = 0;
	if (NUM_OUTPUTS >= 10) _TRISA1 = 0;
#endif

#endif // AIRFRAME_TYPE
}

void udb_set_action_state(boolean newValue)
{
	ACTION_OUT_PIN = newValue;
}


#if (AIRFRAME_TYPE == AIRFRAME_QUAD)

#warning("synchronous PWM outputs using OC capability: not sequential");

void udb_set_dc(void)
{
	OC1RS = scale_pwm_out(1);
	OC2RS = scale_pwm_out(2);
	OC3RS = scale_pwm_out(3);
	OC4RS = scale_pwm_out(4);
}

#else // AIRFRAME_TYPE

// Call this to start sending out pulses to all the PWM output channels sequentially
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

void __attribute__((__interrupt__,__no_auto_psv__)) _T4Interrupt(void)
{
	indicate_loading_inter;
	// interrupt_save_set_corcon;

	switch (outputNum) {
		case 0:
			HANDLE_SERVO_OUT(1, SERVO_OUT_PIN_1);
			break;
		case 1:
			SERVO_OUT_PIN_1 = 0;
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
#if ((BOARD_TYPE & AUAV2_BOARD) == 0)
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
#endif
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

#endif // AIRFRAME_TYPE
