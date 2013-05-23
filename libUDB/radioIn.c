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
#include "oscillator.h"
#include "interrupt.h"

#if (FLYBYWIRE_ENABLED == 1)
#include "FlyByWire.h"
#include "mode_switch.h"
#endif


#if (MIPS == 64)
#define TMR_FACTOR 4
#elif (MIPS == 32)
#define TMR_FACTOR 1
#elif (MIPS == 16)
#define TMR_FACTOR 2
#else
#error Invalid MIPS Configuration
#endif // MIPS

#define MIN_SYNC_PULSE_WIDTH (14000/TMR_FACTOR)	// 3.5ms
#define IC_INT_PRI 6
//#define DEBUG_FAILSAFE_MIN_MAX


//	Measure the pulse widths of the servo channel inputs from the radio.
//	The dsPIC makes this rather easy to do using its capture feature.

//	One of the channels is also used to validate pulse widths to detect loss of radio.

//	The pulse width inputs can be directly converted to units of pulse width outputs to control
//	the servos by simply dividing by 2. (need to check validity of this statement - RobD)

int16_t udb_pwIn[NUM_INPUTS+1];	// pulse widths of radio inputs
int16_t udb_pwTrim[NUM_INPUTS+1];	// initial pulse widths for trimming

int16_t failSafePulses = 0;
int16_t noisePulses = 0;


void udb_init_capture(void)
{
	int16_t i;

#if(USE_NV_MEMORY == 1)
	if(udb_skip_flags.skip_radio_trim == 0)
#endif
	{	
		for (i=0; i <= NUM_INPUTS; i++)
	#if (FIXED_TRIMPOINT == 1)
			if(i == THROTTLE_OUTPUT_CHANNEL)
				udb_pwTrim[i] = udb_pwIn[i] = THROTTLE_TRIMPOINT;
			else
				udb_pwTrim[i] = udb_pwIn[i] = CHANNEL_TRIMPOINT;			
	#else
			udb_pwTrim[i] = udb_pwIn[i] = 0;
	#endif
	}
	
	TMR2 = 0; 				// initialize timer
#if (MIPS == 64)
	T2CONbits.TCKPS = 2;	// prescaler = 64 option
#else
	T2CONbits.TCKPS = 1;	// prescaler = 8 option
#endif
	T2CONbits.TCS = 0;		// use the internal clock
	T2CONbits.TON = 1;		// turn on timer 2

#if (NORADIO != 1)
	
#if (BOARD_TYPE == AUAV3_BOARD)
#define IC1VAL 0x0401
#define IC2VAL 0 // SYNCSEL = 0x00: no sync, no trigger, rollover at 0xFFFF
#define IC_INIT(x) \
{ \
	IC##x##CON1 = IC1VAL; \
	IC##x##CON2 = IC2VAL; \
	_IC##x##IP = IC_INT_PRI; \
	_IC##x##IF = 0; \
	_IC##x##IE = 1; \
}
#else
#define IC1VAL 0x0081
#define IC_INIT(x) \
{ \
	IC##x##CON = IC1VAL; \
	_IC##x##IP = IC_INT_PRI; \
	_IC##x##IF = 0; \
	_IC##x##IE = 1; \
}
#endif

    if (NUM_INPUTS > 0) IC_INIT(1);
#if (USE_PPM_INPUT == 0)
    if (NUM_INPUTS > 1) IC_INIT(2);
    if (NUM_INPUTS > 2) IC_INIT(3);
    if (NUM_INPUTS > 3) IC_INIT(4);
    if (NUM_INPUTS > 4) IC_INIT(5);
    if (NUM_INPUTS > 5) IC_INIT(6);
    if (NUM_INPUTS > 6) IC_INIT(7);
    if (NUM_INPUTS > 7) IC_INIT(8);
#endif // USE_PPM_INPUT
#endif // NORADIO
}

void set_udb_pwIn(int pwm, int index)
{
	pwm = pwm * TMR_FACTOR / 2;	// yes we are scaling the parameter up front

#if (FLYBYWIRE_ENABLED == 1)
	// It's kind of a bad idea to override the radio mode input
	if (MODE_SWITCH_INPUT_CHANNEL == index)
	{
		udb_pwIn[index] = pwm;	
	}
	else
	{
		if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
		{
			udb_pwIn[index] = get_fbw_pwm(index);	
		}
		else
		{
			udb_pwIn[index] = pwm;	
		}
	}
#else
	if (FAILSAFE_INPUT_CHANNEL == index)
	{
		if ((pwm > FAILSAFE_INPUT_MIN) && (pwm < FAILSAFE_INPUT_MAX))
		{
			failSafePulses++;
		}
		else
		{
			noisePulses++;
		}
		//printf("FS: %u %u %u\r\n", pwm, failSafePulses, noisePulses);
		#ifdef DEBUG_FAILSAFE_MIN_MAX
		{
			static uint8_t foo = 0;
			if (!(++foo % 32)) 
			{
				printf("FS: %u\r\n", pwm);
			}
		}
		#endif // DEBUG_FAILSAFE_MIN_MAX
	}
	udb_pwIn[index] = pwm;	
#endif // FLYBYWIRE_ENABLED
}

#if (USE_PPM_INPUT == 0)

#if (BOARD_TYPE == AUAV3_BOARD)
#define IC_HANDLER(x, y) \
void __attribute__((__interrupt__,__no_auto_psv__)) _IC##x##Interrupt(void) \
{ \
	indicate_loading_inter; \
	interrupt_save_set_corcon; \
	static uint16_t rise = 0; \
	uint16_t time = 0; \
	_IC##x##IF = 0; \
	while (IC##x##CON1bits.ICBNE) \
		time = IC##x##BUF; \
	if (y) \
		rise = time; \
	else \
		set_udb_pwIn(time - rise, x); \
	interrupt_restore_corcon; \
}
#else
#define IC_HANDLER(x, y) \
void __attribute__((__interrupt__,__no_auto_psv__)) _IC##x##Interrupt(void) \
{ \
	indicate_loading_inter; \
	interrupt_save_set_corcon; \
	static uint16_t rise = 0; \
	uint16_t time = 0; \
	_IC##x##IF = 0; \
	while (IC##x##CONbits.ICBNE) \
		time = IC##x##BUF; \
	if (y) \
		rise = time; \
	else \
		set_udb_pwIn(time - rise, x); \
	interrupt_restore_corcon; \
}
#endif

IC_HANDLER(1, IC_PIN1);
IC_HANDLER(2, IC_PIN2);
IC_HANDLER(3, IC_PIN3);
IC_HANDLER(4, IC_PIN4);
IC_HANDLER(5, IC_PIN5);
IC_HANDLER(6, IC_PIN6);
IC_HANDLER(7, IC_PIN7);
IC_HANDLER(8, IC_PIN8);

#else // (USE_PPM_INPUT != 0)

#if (PPM_SIGNAL_INVERTED == 1)
#define PPM_PULSE_VALUE 0
#else
#define PPM_PULSE_VALUE 1
#endif

// PPM Input on Channel 1
void __attribute__((__interrupt__,__no_auto_psv__)) _IC1Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;

	static uint16_t rise_ppm = 0;
	static uint8_t ppm_ch = 0;
	uint16_t time = 0;	

	_IC1IF = 0;
#if (BOARD_TYPE == AUAV3_BOARD)
	while (IC1CON1bits.ICBNE)
#else
	while (IC1CONbits.ICBNE)
#endif
	{
		time = IC1BUF;
	}
#if (USE_PPM_INPUT == 1)
	if (IC_PIN1 == PPM_PULSE_VALUE)
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

	if (IC_PIN1 == PPM_PULSE_VALUE)
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
}

#endif // USE_PPM_INPUT
