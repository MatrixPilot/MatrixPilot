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
#include "heartbeat.h"
#include "servoOut.h"
#include "servoOutPins.h"
#include "cll_io.h"
#include "../Config/options_multicopter.h"
#if AIRFRAME_TYPE != AIRFRAME_QUAD
#if (MIPS == 64)
#define SCALE_FOR_PWM_OUT(x)    (x/2)
#elif (MIPS == 32)
#define SCALE_FOR_PWM_OUT(x)    (x*2)
#elif (MIPS == 16)
#define SCALE_FOR_PWM_OUT(x)    (x)
#else
#error Invalid MIPS Configuration
#endif

#else
    //AIRFRAMETYPE=QUAD
// Timer 3 for Output Compare module clocks at T3FREQ (= 5MHz with FREQOSC = 80e6)
// 32e6 is the legacy value of FREQOSC
#if (MIPS == 64)
#define PWMOUTSCALE (FREQOSC / 64E6)
#elif (MIPS == 40)
#define PWMOUTSCALE (FREQOSC / 40E6)
#elif (MIPS == 32)
#define PWMOUTSCALE (FREQOSC / 32E6)
#elif (MIPS == 16)
#define PWMOUTSCALE (FREQOSC / 16E6)
#else
#error Invalid MIPS Configuration
#endif // MIPS
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
#endif

//	routines to drive the PWM pins for the servos,

int16_t udb_pwOut[NUM_OUTPUTS+1];   // pulse widths for servo outputs
static volatile int16_t outputNum;


// initialize the PWM
void servoOut_init(void) // was called udb_init_pwm()
{
#if AIRFRAME_TYPE != AIRFRAME_QUAD
    #if (MIPS == 64)
    #define SCALE_FOR_PWM_OUT(x)    (x/2)
    #elif (MIPS == 32)
    #define SCALE_FOR_PWM_OUT(x)    (x*2)
    #elif (MIPS == 16)
    #define SCALE_FOR_PWM_OUT(x)    (x)
    #else
    #error Invalid MIPS Configuration
    #endif
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

#else

    int i;
    for (i = 1; i <= NUM_OUTPUTS; i++)
        udb_pwOut[i] = FAILSAFE_INPUT_MIN;

    if (NUM_OUTPUTS >= 1) {
#if ( (BOARD_IS_CLASSIC_UDB == 1 && CLOCK_CONFIG == FRC8X_CLOCK) || BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD || BOARD_TYPE & AUAV2_BOARD)
        // changed to Timer3 and Output Compare Module for PWM out
        // Since Output Compare mode uses 16 bit registers for both period and duty cycle, the max period at 5MHz Timer3 rate
        // is 65536 / 5e6 = 76.3Hz. At 400Hz, period is 12,500 counts, 1500usec is 7500 counts
        // Initialize and enable Timer3
        T3CONbits.TON = 0; // Disable Timer
        T3CONbits.TCS = 0; // Select internal instruction cycle clock
        T3CONbits.TGATE = 0; // Disable Gated Timer mode
        T3CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 16MHz/8 = 2MHz 40MHz/8 = 5MHz 32Mhz/8=4MHz
        TMR3 = 0x00; // Clear timer register
        PR3 = T3PERIOD; // Load the period value (gfm calcul:4000/8 at 32 MIPS = 500 µs)
        IEC0bits.T3IE = 1; // disable interrupts
        T3CONbits.TON = 1; // Start timer
        //		_T4IP = 7 ;							// priority 7
        //		_T4IE = 0 ;							// disable timer 4 interrupt for now (enable for each set of pulses)
#endif
    

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD || BOARD_TYPE & AUAV2_BOARD)
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
// gfm quadcopter tabking account Trigger_Action
#if (TRIGGER_TYPE == TRIGGER_TYPE_SERVO)
    OC5CONbits.OCM = 0b000; // Disable Output Compare Module
    OC5R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
    OC5RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
    OC5CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC5CONbits.OCM = 0b110; // Select the Output Compare mode
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
    
//    return;
#endif
    }
}
static int dc_mod = 0;
void udb_set_dc()
{
    #if !CHANNEL_1_REDUCED_RATE
        OC1RS = scale_pwm_out(1);
    #endif
    #if !CHANNEL_2_REDUCED_RATE
        OC2RS = scale_pwm_out(2);
    #endif
    #if !CHANNEL_3_REDUCED_RATE
        OC3RS = scale_pwm_out(3);
    #endif
    #if !CHANNEL_4_REDUCED_RATE
        OC4RS = scale_pwm_out(4);
    #endif
    #if (NUM_ROTORS > 4)
    #if !CHANNEL_5_REDUCED_RATE
        OC5RS = scale_pwm_out(5);
    #endif
    #if !CHANNEL_6_REDUCED_RATE
        OC6RS = scale_pwm_out(6);
    #endif
    #endif
// gfm quadcopter taking into account Trigger_Action
    #if (TRIGGER_TYPE == TRIGGER_TYPE_SERVO)
        OC5RS = scale_pwm_out(5);
    #endif
    if (dc_mod++ >= REDUCED_RATE_SKIPS)
    {
        dc_mod = 0;
        #if CHANNEL_1_REDUCED_RATE
            OC1RS = scale_pwm_out(1);
        #endif
        #if CHANNEL_2_REDUCED_RATE
            OC2RS = scale_pwm_out(2);
        #endif
        #if CHANNEL_3_REDUCED_RATE
            OC3RS = scale_pwm_out(3);
        #endif
        #if CHANNEL_4_REDUCED_RATE
            OC4RS = scale_pwm_out(4);
        #endif
        #if (NUM_ROTORS > 4)
        #if CHANNEL_5_REDUCED_RATE
            OC5RS = scale_pwm_out(5);
        #endif
        #if CHANNEL_6_REDUCED_RATE
            OC6RS = scale_pwm_out(6);
        #endif
        #endif
    } else
    {
        #if CHANNEL_1_REDUCED_RATE
            OC1RS = 0;
        #endif
        #if CHANNEL_2_REDUCED_RATE
            OC2RS = 0;
        #endif
        #if CHANNEL_3_REDUCED_RATE
            OC3RS = 0;
        #endif
        #if CHANNEL_4_REDUCED_RATE
            OC4RS = 0;
        #endif
        #if (NUM_ROTORS > 4)
        #if CHANNEL_5_REDUCED_RATE
            OC5RS = 0;
        #endif
        #if CHANNEL_6_REDUCED_RATE
            OC6RS = 0;
        #endif
        #endif
    }
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
#if AIRFRAME_TYPE != AIRFRAME_QUAD
	ACTION_OUT_PIN = newValue;
#else
    SERVO_OUT_PIN_6 = newValue;
#endif
}
#if AIRFRAME_TYPE != AIRFRAME_QUAD
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
#else
// Modif gfm Quadcopter : _T3Interrupt use in mpu6000.c only
// All following lines are so DE_commented

void __attribute__((__interrupt__, __no_auto_psv__)) _T3Interrupt(void)
{
    indicate_loading_inter;
    interrupt_save_set_corcon;
    _T3IF = 0; // clear the interrupt
    // set the motor PWM values; these are sent to all ESCs continuously at ESC_HZ
    udb_set_dc();

    interrupt_restore_corcon;
    return;
}
 
#warning("synchronous PWM outputs using OC capability: not sequential")
#endif

