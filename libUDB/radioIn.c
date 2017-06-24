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


#include "libUDB.h"
#include "oscillator.h"
#include "interrupt.h"
#include "radioIn.h"
#include "cll_io.h"
#include "../MatrixPilot/states.h"

#if (FLY_BY_DATALINK_ENABLED == 1)
#include "fly_by_datalink.h"
#include "mode_switch.h"
#endif

// Select which Input Capture pin the PPM device is connected to
// changing this can be useful when using PPM and fitting a UDB into
// very tight airframes, as it allows alternative input pins to be
// assigned for connection to the receiver.
// If not using PPM, then this must be left set to '1'
#ifndef PPM_IC
#define PPM_IC 1
#endif // PPM_IC

#define MAX_BAD_PULSE_RATE 5    // up to 5 PWM "glitches" per second are allowed

#if (MIPS == 64)
#define TMR_FACTOR 4
#elif (MIPS == 32)
#define TMR_FACTOR 1
#elif (MIPS == 16)
#define TMR_FACTOR 2
#else
#error Invalid MIPS Configuration
#endif // MIPS

#define MIN_SYNC_PULSE_WIDTH (14000/TMR_FACTOR) // 3.5ms
//#define DEBUG_FAILSAFE_MIN_MAX


// Measure the pulse widths of the servo channel inputs from the radio.
// The dsPIC makes this rather easy to do using its capture feature.

// One of the channels is also used to validate pulse widths to detect loss of radio.

// The pulse width inputs can be directly converted to units of pulse width outputs to control
// the servos by simply dividing by 2. (need to check validity of this statement - RobD)

int16_t udb_pwIn[NUM_INPUTS+1];     // pulse widths of radio inputs
int16_t udb_pwTrim[NUM_INPUTS+1];   // initial pulse widths for trimming

static int16_t goodPWMPulseCount = 0;
static int16_t badPWMPulseCount = 0;


void udb_servo_record_trims(void)
{
	int16_t i;
	for (i = 0; i <= NUM_INPUTS; i++)
	{
		udb_pwTrim[i] = udb_pwIn[i];
	}
}

void radioIn_init(void) // was called udb_init_capture(void)
{
	int16_t i;

#if (USE_NV_MEMORY == 1)
	if (udb_skip_flags.skip_radio_trim == 0)
#endif
	{	
		for (i = 0; i <= NUM_INPUTS; i++)
	#if (FIXED_TRIMPOINT == 1)
			if (i == THROTTLE_OUTPUT_CHANNEL)
				udb_pwTrim[i] = udb_pwIn[i] = THROTTLE_TRIMPOINT;
			else
				udb_pwTrim[i] = udb_pwIn[i] = CHANNEL_TRIMPOINT;
	#else
			udb_pwTrim[i] = udb_pwIn[i] = 0;
	#endif
	}

	TMR2 = 0;               // initialize timer
#if (MIPS == 64)
	T2CONbits.TCKPS = 2;    // prescaler = 64 option
#else
	T2CONbits.TCKPS = 1;    // prescaler = 8 option
#endif
	T2CONbits.TCS = 0;      // use the internal clock
	T2CONbits.TON = 1;      // turn on timer 2

#if (NORADIO != 1)

#if (BOARD_TYPE == AUAV3_BOARD)
#define REGTOK1 N1
#define REGTOK2 N2
#define IC1VAL 0x0401
#else // UDB4 or 5
#define REGTOK1 N
#define REGTOK2 N
#define IC1VAL 0x0081
#endif
#define IC2VAL 0 // SYNCSEL = 0x00: no sync, no trigger, rollover at 0xFFFF

#define _IC_INIT(x, y, z) \
{ \
	IC##x##CO##z = IC2VAL; \
	IC##x##CO##y = IC1VAL; \
	_IC##x##IP = INT_PRI_IC; \
	_IC##x##IF = 0; \
	_IC##x##IE = 1; \
}
#define IC_INIT(x, y, z) _IC_INIT(x, y, z)

	if (NUM_INPUTS > 0) IC_INIT(PPM_IC, REGTOK1, REGTOK2);
#if (USE_PPM_INPUT == 0)
	if (NUM_INPUTS > 1) IC_INIT(2, REGTOK1, REGTOK2);
	if (NUM_INPUTS > 2) IC_INIT(3, REGTOK1, REGTOK2);
	if (NUM_INPUTS > 3) IC_INIT(4, REGTOK1, REGTOK2);
	if (NUM_INPUTS > 4) IC_INIT(5, REGTOK1, REGTOK2);
	if (NUM_INPUTS > 5) IC_INIT(6, REGTOK1, REGTOK2);
	if (NUM_INPUTS > 6) IC_INIT(7, REGTOK1, REGTOK2);

	if (NUM_INPUTS > 7) IC_INIT(8, REGTOK1, REGTOK2);
#endif // USE_PPM_INPUT
#endif // NORADIO
    
#if (USE_CASTLE_LINK_THROTTLE == 1)
    cll_setup();
#endif
}

// called from heartbeat pulse at 20Hz
void radioIn_failsafe_check(void)
{
	// check to see if at least one valid pulse has been received,
	// and also that the noise rate has not been exceeded
	if ((goodPWMPulseCount == 0) || (badPWMPulseCount > MAX_BAD_PULSE_RATE))
	{
		if (udb_flags._.radio_on == 1)
		{
			udb_flags._.radio_on = 0;
			udb_callback_radio_did_turn_off();
		}
		led_off(LED_GREEN);
		badPWMPulseCount = 0; // reset count of noise pulses
	}
	else
	{
		udb_flags._.radio_on = 1;
		led_on(LED_GREEN);
	}
	goodPWMPulseCount = 0;
}

void radioIn_bad_pulse_count_reset(void)
{
	badPWMPulseCount = 0;
}

#if (NORADIO != 1)
static void set_udb_pwIn(int pwm, int index)
{
	pwm = pwm * TMR_FACTOR / 2; // yes we are scaling the parameter up front

	if (FAILSAFE_INPUT_CHANNEL == index)
	{
		if ((pwm > FAILSAFE_INPUT_MIN) && (pwm < FAILSAFE_INPUT_MAX))
		{
			goodPWMPulseCount++;
		}
		else
		{
			badPWMPulseCount++;
		}
	}

#if (FLY_BY_DATALINK_ENABLED == 1)
	// It's kind of a bad idea to override the radio mode input
	if (MODE_SWITCH_INPUT_CHANNEL == index)
	{
		udb_pwIn[index] = pwm;
	}
	else
	{
		if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
		{
			// if mode is in low mode, use pwm values that came in from external source
			udb_pwIn[index] = get_fbdl_pwm(index);
		}
		else
		{
			udb_pwIn[index] = pwm;
		}
	}
#else
	if (FAILSAFE_INPUT_CHANNEL == index)
	{
		//DPRINT("FS: %u %u %u\r\n", pwm, failSafePulses, noisePulses);
		#ifdef DEBUG_FAILSAFE_MIN_MAX
		{
			static uint8_t foo = 0;
			if (!(++foo % 32)) 
			{
				DPRINT("FS: %u\r\n", pwm);
			}
		}
		#endif // DEBUG_FAILSAFE_MIN_MAX
	}
	udb_pwIn[index] = pwm;
#endif // FLY_BY_DATALINK_ENABLED
}

#if (USE_PPM_INPUT == 0)

#define _IC_HANDLER(x, y, z) \
void __attribute__((__interrupt__,__no_auto_psv__)) _IC##x##Interrupt(void) \
{ \
	indicate_loading_inter; \
	set_ipl_on_output_pin; \
	interrupt_save_set_corcon; \
	static uint16_t rise = 0; \
	uint16_t time = 0; \
	_IC##x##IF = 0; \
	while (IC##x##CO##y##bits.ICBNE) \
		time = IC##x##BUF; \
	if (z) \
		rise = time; \
	else \
		set_udb_pwIn(time - rise, x); \
	interrupt_restore_corcon; \
	unset_ipl_on_output_pin; \
}
#define IC_HANDLER(x, y, z) _IC_HANDLER(x, y, z)

IC_HANDLER(1, REGTOK1, IC_PIN1);
IC_HANDLER(2, REGTOK1, IC_PIN2);
IC_HANDLER(3, REGTOK1, IC_PIN3);
IC_HANDLER(4, REGTOK1, IC_PIN4);
IC_HANDLER(5, REGTOK1, IC_PIN5);
IC_HANDLER(6, REGTOK1, IC_PIN6);
IC_HANDLER(7, REGTOK1, IC_PIN7);
#if (USE_SONAR_INPUT != 8 && USE_CASTLE_LINK_THROTTLE != 1)
IC_HANDLER(8, REGTOK1, IC_PIN8);
#endif // USE_SONAR_INPUT

#else // (USE_PPM_INPUT != 0)

#if (PPM_SIGNAL_INVERTED == 1)
#define PPM_PULSE_VALUE 0
#else
#define PPM_PULSE_VALUE 1
#endif

//#if (BOARD_TYPE == AUAV3_BOARD)
//#define ICBNE(x) IC##x##CON1bits.ICBNE
//#else
//#define ICBNE(x) IC##x##CONbits.ICBNE
//#endif

/*
PPM_2

    1   2  3  4   5  6  7   
   ___     _     ___   ___    
  |   |   | |   |   | |   |   
  |   |   | |   |   | |   |   
__|   |___| |___|   |_|   |____________________


PPM_1

    1     2    3      4     5    6      7
   ___   ___   _     ___   ___   _     ___
  |   | |   | | |   |   | |   | | |   |   |
  |   | |   | | |   |   | |   | | |   |   |
__|   |_|   |_| |___|   |_|   |_| |___|   |____

 */

//#define REGTOK1 N1
#define ICBNE(x, y) IC##x##CO##y##bits.ICBNE

#define _IC_TIME(x, y) \
static inline uint16_t ic_time(void) \
{ \
	uint16_t time = 0; \
	_IC##x##IF = 0; \
	while (ICBNE(x, y)) time = IC##x##BUF; \
	return time; \
}
#define IC_TIME(x, y) _IC_TIME(x, y)

IC_TIME(PPM_IC, REGTOK1);

#define _IC_INTERRUPT(x) _IC##x##Interrupt(void)
#define IC_INTERRUPT(x) _IC_INTERRUPT(x)

#define _IC_PIN(x) IC_PIN##x
#define __IC_PIN(x) _IC_PIN(x)

// PPM Input on Channel PPM_IC
void __attribute__((__interrupt__,__no_auto_psv__)) IC_INTERRUPT(PPM_IC)
{
	indicate_loading_inter;
	set_ipl_on_output_pin;
	interrupt_save_set_corcon;

	static uint16_t rise_ppm = 0;
	static uint8_t ppm_ch = 0;
	uint16_t time = 0;

	time = ic_time();

#if (USE_PPM_INPUT == 1)
	if (__IC_PIN(PPM_IC) == PPM_PULSE_VALUE)
	{
		uint16_t pulse = time - rise_ppm;
		rise_ppm = time;

		if (pulse > MIN_SYNC_PULSE_WIDTH)
		{
			ppm_ch = 1;
		}
		else
		{
			if (ppm_ch > 0 && ppm_ch <= PPM_NUMBER_OF_CHANNELS)
			{
				if (ppm_ch <= NUM_INPUTS)
				{
					set_udb_pwIn(pulse, ppm_ch);
				}
				ppm_ch++;
			}
		}
	}
#elif  (USE_PPM_INPUT == 2)
	uint16_t pulse = time - rise_ppm;
	rise_ppm = time;

	if (__IC_PIN(PPM_IC) == PPM_PULSE_VALUE)
	{
		if (pulse > MIN_SYNC_PULSE_WIDTH)
		{
			ppm_ch = 1;
		}
	}
	else
	{
		if (ppm_ch > 0 && ppm_ch <= PPM_NUMBER_OF_CHANNELS)
		{
			if (ppm_ch <= NUM_INPUTS)
			{
				set_udb_pwIn(pulse, ppm_ch);
			}
			ppm_ch++;
		}
	}
#else  // USE_PPM_INPUT > 2
#error Invalid USE_PPM_INPUT setting
#endif // USE_PPM_INPUT
	interrupt_restore_corcon;
	unset_ipl_on_output_pin;
}

#endif // USE_PPM_INPUT
#endif // NORADIO !=1
