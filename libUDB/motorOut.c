// This file is part of the MatrixPilotQuad firmware.
//
//	http://code.google.com/p/gentlenav/
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


#include "../MatrixPilot/defines.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/oscillator.h"
#include "../libUDB/servoOut.h"


//int16_t udb_pwOut[NUM_OUTPUTS+1]; // pulse widths for servo outputs
extern int16_t udb_pwOut[]; // pulse widths for servo outputs


// Timer 3 for Output Compare module clocks at 5MHz
#define PWMOUTSCALE (FREQOSC / 32E6)
#define T3FREQ (2000000 * PWMOUTSCALE)
// Timer 3 period is 1 / (ESC_HZ)
#define T3PERIOD (T3FREQ / ESC_HZ)

static inline int scale_pwm_out(int channel)
{
	union longww pww;
	pww.WW = __builtin_muluu(udb_pwOut[channel], (unsigned int)(65536 * PWMOUTSCALE / 4));
	pww.WW <<= 2;
	return pww._.W1;
}

// initialize the PWM
void init_motorOut(void) // was called udb_init_pwm()
{
	int16_t i;

	for (i = 0; i <= NUM_OUTPUTS; i++)
	{
		udb_pwOut[i] = FAILSAFE_INPUT_MIN;
	}

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

#ifdef __dsPIC33EP512MU810__
#define OC1CONbits OC1CON1bits
#define OC2CONbits OC2CON1bits
#define OC3CONbits OC3CON1bits
#define OC4CONbits OC4CON1bits
#endif

	// OC modules 1-8 are used for outputs
	// On the UDB4, these are labeled as outputs, on the AUAV2_alpha1 they are labeled I1-I8

	// configure OC1-8 as output pins
	TRISD &= 0xFF00; // clear _TRISD0-7

	// Initialize Output Compare Module
	OC1CONbits.OCM = 0b000; // Disable Output Compare Module
	OC1R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = 250 counts)
	OC1RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
	OC1CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
	OC1R = 100; // Load the Compare Register Value
	OC1CONbits.OCM = 0b110; // Select the Output Compare mode

	OC2CONbits.OCM = 0b000; // Disable Output Compare Module
	OC2R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
	OC2RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
	OC2CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
	OC2R = 100; // Load the Compare Register Value
	OC2CONbits.OCM = 0b110; // Select the Output Compare mode

	OC3CONbits.OCM = 0b000; // Disable Output Compare Module
	OC3R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
	OC3RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
	OC3CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
	OC3R = 100; // Load the Compare Register Value
	OC3CONbits.OCM = 0b110; // Select the Output Compare mode

	OC4CONbits.OCM = 0b000; // Disable Output Compare Module
	OC4R = FAILSAFE_INPUT_MIN; // Write the duty cycle for the first PWM pulse (1msec = FAILSAFE_INPUT_MIN counts)
	OC4RS = FAILSAFE_INPUT_MIN; // Write the duty cycle for the second PWM pulse
	OC4CONbits.OCTSEL = 1; // Select Timer 3 as output compare time base
	OC4R = 100; // Load the Compare Register Value
	OC4CONbits.OCM = 0b110; // Select the Output Compare mode
}

void start_esc_outputs(void)    // was called udb_set_dc()
{
	OC1RS = scale_pwm_out(1);
	OC2RS = scale_pwm_out(2);
	OC3RS = scale_pwm_out(3);
	OC4RS = scale_pwm_out(4);
}
