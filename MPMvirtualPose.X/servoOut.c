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


#include "libUDB_internal.h"
#include "libDCM.h"
#include "options.h"

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE & AUAV2_BOARD)

#define SERVO_OUT_PIN_1			_LATD0
#define SERVO_OUT_PIN_2			_LATD1
#define SERVO_OUT_PIN_3			_LATD2
#define SERVO_OUT_PIN_4			_LATD3
#define SERVO_OUT_PIN_5			_LATD4
#define SERVO_OUT_PIN_6			_LATD5
#define SERVO_OUT_PIN_7			_LATD6
#define SERVO_OUT_PIN_8			_LATD7
#define SERVO_OUT_PIN_9			_LATD7	// 9th Output is not valid on UDB4

#define SCALE_FOR_PWM_OUT(x)	(x)


#else //#if (BOARD_IS_CLASSIC_UDB == 1)

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

#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#define SCALE_FOR_PWM_OUT(x)		((x) << 1)
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#define PWMOUTSCALE					60398	// = 256*256*(3.6864/4)
#define SCALE_FOR_PWM_OUT(x)		(((union longww)(long)__builtin_muluu( (x) ,  PWMOUTSCALE ))._.W1)
#endif

#endif

// Timer 3 for Output Compare module clocks at T3FREQ (= 5MHz with FREQOSC = 80e6)
// 32e6 is the legacy value of FREQOSC
#define PWMOUTSCALE (FREQOSC / 32E6)
// 2e6 is the legacy value of T3FREQ
#define T3FREQ (2000000 * PWMOUTSCALE)
// Timer 3 period is 1 / (ESC_HZ)
#define T3PERIOD (T3FREQ / ESC_HZ)
inline int scale_pwm_out(int channel) {
    union longww pww;
    pww.WW = __builtin_muluu(udb_pwOut[channel], (unsigned int)(65536 * PWMOUTSCALE / 4));
    pww.WW <<= 2;
    return pww._.W1;
}

//	routines to drive the PWM pins for the servos,

int udb_pwOut[NUM_OUTPUTS + 1]; // pulse widths for servo outputs

int outputNum;

#if ((BOARD_TYPE != UDB4_BOARD) && ((BOARD_TYPE & AUAV2_BOARD) == 0))
#error("code modified only for UDB4/AUAV2")
#endif

void udb_init_pwm(void) // initialize the PWM
{
    int i;
    for (i = 0; i <= NUM_OUTPUTS; i++)
        udb_pwOut[i] = FAILSAFE_INPUT_MIN;

    if (NUM_OUTPUTS >= 1) { 
#if ( (BOARD_IS_CLASSIC_UDB == 1 && CLOCK_CONFIG == FRC8X_CLOCK) || BOARD_TYPE == UDB4_BOARD || BOARD_TYPE & AUAV2_BOARD)
        // changed to Timer3 and Output Compare Module for PWM out
        // Since Output Compare mode uses 16 bit registers for both period and duty cycle, the max period at 5MHz Timer3 rate
        // is 65536 / 5e6 = 76.3Hz. At 400Hz, period is 12,500 counts, 1500usec is 7500 counts
        // Initialize and enable Timer3
        T3CONbits.TON = 0; // Disable Timer
        T3CONbits.TCS = 0; // Select internal instruction cycle clock
        T3CONbits.TGATE = 0; // Disable Gated Timer mode
        T3CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 16MHz/8 = 2MHz 40MHz/8 = 5MHz
        TMR3 = 0x00; // Clear timer register
        PR3 = T3PERIOD; // Load the period value
        IEC0bits.T3IE = 0; // disable interrupts
        T3CONbits.TON = 1; // Start timer
#endif
        //		_T4IP = 7 ;							// priority 7
        //		_T4IE = 0 ;							// disable timer 4 interrupt for now (enable for each set of pulses)
    }

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE & AUAV2_BOARD)
    // OC modules 1-8 are used for outputs
    // On the UDB4 and AUAV2_ALPHA2, these are labeled as outputs, on the AUAV2_alpha1 they are labeled I1-I8

    // configure OC1-8 as output pins
    TRISD &= 0xFF00; // clear _TRISD0-7

    // Initialize Output Compare Module
    OC1CONbits.OCM = 0b000; // Disable Output Compare Module
    OC1R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = 250 counts)
    OC1RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
    OC1CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC1CONbits.OCM = 0b110; // Select the Output Compare mode

    OC2CONbits.OCM = 0b000; // Disable Output Compare Module
    OC2R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
    OC2RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
    OC2CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC2CONbits.OCM = 0b110; // Select the Output Compare mode

    OC3CONbits.OCM = 0b000; // Disable Output Compare Module
    OC3R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
    OC3RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
    OC3CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC3CONbits.OCM = 0b110; // Select the Output Compare mode

    OC4CONbits.OCM = 0b000; // Disable Output Compare Module
    OC4R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
    OC4RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
    OC4CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC4CONbits.OCM = 0b110; // Select the Output Compare mode

#if (NUM_ROTORS > 4)
    OC5CONbits.OCM = 0b000; // Disable Output Compare Module
    OC5R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
    OC5RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
    OC5CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC5CONbits.OCM = 0b110; // Select the Output Compare mode

    OC6CONbits.OCM = 0b000; // Disable Output Compare Module
    OC6R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
    OC6RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
    OC6CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC6CONbits.OCM = 0b110; // Select the Output Compare mode
#endif
#else // Classic board
    TRISE = 0b1111111111000000;

    if (NUM_OUTPUTS >= 1) {
#if (USE_PPM_INPUT == 1)
#if (PPM_ALT_OUTPUT_PINS != 1)
        _TRISD1 = 0; // Set D1 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 5) _TRISB5 = 0; // Set B5 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 6) _TRISB4 = 0; // Set B4 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 7) _TRISE0 = 0; // Set E0 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 8) _TRISE2 = 0; // Set E2 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 9) _TRISE4 = 0; // Set E4 to be an output if we're using PPM
#else
        _TRISE0 = 0; // Set E0 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 5) _TRISE2 = 0; // Set E2 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 6) _TRISE4 = 0; // Set E4 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 7) _TRISD1 = 0; // Set D1 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 8) _TRISB5 = 0; // Set B5 to be an output if we're using PPM
        if (NUM_OUTPUTS >= 9) _TRISB4 = 0; // Set B4 to be an output if we're using PPM
#endif
#endif
    }
#endif

    return;
}

#ifndef SERVO_HACK
void udb_set_dc() {
        OC1RS = scale_pwm_out(1);
        OC2RS = scale_pwm_out(2);
        OC3RS = scale_pwm_out(3);
        OC4RS = scale_pwm_out(4);
#if (NUM_ROTORS > 4)
        OC5RS = scale_pwm_out(5);
        OC6RS = scale_pwm_out(6);
#endif
}
#else
static int dc_mod = 0;
void udb_set_dc() {
        OC1RS = scale_pwm_out(1);
        OC2RS = scale_pwm_out(2);
        OC3RS = scale_pwm_out(3);
    if (dc_mod++ >= 3) {
        dc_mod = 0;
        OC4RS = scale_pwm_out(4);
    } else {
        OC1RS = 0;
        OC2RS = 0;
        OC3RS = 0;
        OC4RS = 0;
    }
}
#endif

void udb_set_action_state(boolean newValue) {
    SERVO_OUT_PIN_6 = newValue;
}


#warning("synchronous PWM outputs using OC capability: not sequential")
#if 0
// Call this to start sending out pulses to all the PWM output channels sequentially

void start_pwm_outputs(void) {
    if (NUM_OUTPUTS > 0) {
        outputNum = 0;
        PR4 = SCALE_FOR_PWM_OUT(200); // set timer to delay 0.1ms

        TMR4 = 0; // start timer at 0
        _T4IF = 0; // clear the interrupt
        _T4IE = 1; // enable timer 4 interrupt
    }

    return;
}


#if (RECORD_FREE_STACK_SPACE == 1)
extern unsigned int maxstack;
#endif

// Define HANDLE_SERVO_OUT as a macro to allow passing the pin as an argument
#define HANDLE_SERVO_OUT(channel, pin)						\
{															\
	if (NUM_OUTPUTS >= channel)								\
	{														\
		outputNum = channel ;								\
		if ( udb_pwOut[channel] > 0 )						\
		{													\
			PR4 = SCALE_FOR_PWM_OUT(udb_pwOut[channel]) ;	\
			pin = 1 ;										\
		}													\
		else												\
		{													\
			PR4 = SCALE_FOR_PWM_OUT(100) ;					\
			pin = 0 ;										\
		}													\
		TMR4 = 0 ;											\
	}														\
	else													\
	{														\
		_T4IE = 0 ;											\
	}														\
}

void __attribute__((__interrupt__, __no_auto_psv__)) _T4Interrupt(void) {
    indicate_loading_inter;
    // interrupt_save_set_corcon ;

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
        case 7:
            SERVO_OUT_PIN_7 = 0;
            HANDLE_SERVO_OUT(8, SERVO_OUT_PIN_8);
            break;
        case 8:
            SERVO_OUT_PIN_8 = 0;
            HANDLE_SERVO_OUT(9, SERVO_OUT_PIN_9);
            break;
        case 9:
            SERVO_OUT_PIN_9 = 0; // end the pulse by setting the SERVO_OUT_PIN_9 pin low
            _T4IE = 0; // disable timer 4 interrupt
            break;
    }

    _T4IF = 0; // clear the interrupt

#if (RECORD_FREE_STACK_SPACE == 1)
    // Check stack space here because it's a high-priority ISR
    // which may have interrupted a whole chain of other ISRs,
    // So available stack space can get lowest here.
    unsigned int stack = WREG15;
    if (stack > maxstack) {
        maxstack = stack;
    }
#endif

    // interrupt_restore_corcon ;
    return;
}
#endif
