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
//#include "interrupt.h"
#include "radioIn.h"
#include "../MatrixPilot/states.h"

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "tim.h"

//#if (FLY_BY_DATALINK_ENABLED == 1)
//#include "fly_by_datalink.h"
//#include "mode_switch.h"
//#endif

// Select which Input Capture pin the PPM device is connected to
// changing this can be useful when using PPM and fitting a UDB into
// very tight airframes, as it allows alternative input pins to be
// assigned for connection to the receiver.
// If not using PPM, then this must be left set to '1'
#ifndef PPM_IC
#define PPM_IC 1
#endif // PPM_IC

#define MAX_NOISE_RATE 5    // up to 5 PWM "glitches" per second are allowed

//#if (MIPS == 64)
//#define TMR_FACTOR 4
//#elif (MIPS == 32)
//#define TMR_FACTOR 1
//#elif (MIPS == 16)
//#define TMR_FACTOR 2
//#else
//#error Invalid MIPS Configuration
//#endif // MIPS

//ToDo: What TMR_FACTOR will be needed by our uC?
#define TMR_FACTOR 4


//#define MIN_SYNC_PULSE_WIDTH (14000/TMR_FACTOR) // 3.5ms
#define MIN_SYNC_PULSE_WIDTH (10000/TMR_FACTOR) // 2.5ms
//#define DEBUG_FAILSAFE_MIN_MAX


// Measure the pulse widths of the servo channel inputs from the radio.
// The dsPIC makes this rather easy to do using its capture feature.

// One of the channels is also used to validate pulse widths to detect loss of radio.

// The pulse width inputs can be directly converted to units of pulse width outputs to control
// the servos by simply dividing by 2. (need to check validity of this statement - RobD)

int16_t udb_pwIn[NUM_INPUTS+1];     // pulse widths of radio inputs
int16_t udb_pwTrim[NUM_INPUTS+1];   // initial pulse widths for trimming

static int16_t failSafePulses = 0;
static int16_t noisePulses = 0;

static boolean isFailSafe = true;

boolean radioIn_isFailSafe(void)
{
	return isFailSafe;
}

uint8_t radioIn_getInput(int16_t* ppm, uint8_t channels)
{
	uint8_t c;

	for (c = 0; c < channels; c++)
	{
		ppm[c+1] = udb_pwIn[c+1];
	}
	return MODE_SWITCH_INPUT_CHANNEL; // make this define specific to each ppm input device
}

//void udb_servo_record_trims(void)
//{
//	int16_t i;
//	for (i = 0; i <= NUM_INPUTS; i++)
//	{
//		udb_pwTrim[i] = udb_pwIn[i];
//	}
//}

void radioIn_init(void) // was called udb_init_capture(void)
{
	int16_t i;

//#if (USE_NV_MEMORY == 1)
//	if (udb_skip_flags.skip_radio_trim == 0)
//#endif
//	{
		for (i = 0; i <= NUM_INPUTS; i++)
//	#if (FIXED_TRIMPOINT == 1)
//			if (i == THROTTLE_OUTPUT_CHANNEL)
//				udb_pwTrim[i] = udb_pwIn[i] = THROTTLE_TRIMPOINT;
//			else
//				udb_pwTrim[i] = udb_pwIn[i] = CHANNEL_TRIMPOINT;
//	#else
			udb_pwTrim[i] = udb_pwIn[i] = 0;
//	#endif
//	}
	MX_TIM5_Init();     //Input Capture CH1 and CH2 timer base
//	MX_TIM4_Init();     //Input Capture CH3 to CH6 timer base
}

// called from heartbeat pulse at 20Hz
void radioIn_failsafe_check(void)
{
	// check to see if at least one valid pulse has been received,
	// and also that the noise rate has not been exceeded
//	if ((failSafePulses == 0) || (noisePulses > MAX_NOISE_RATE))
//	{
//		if (udb_flags._.radio_on == 1)
//		{
//			udb_flags._.radio_on = 0;
//			udb_callback_radio_did_turn_off();
//		}
//		led_off(LED_GREEN);
//		noisePulses = 0; // reset count of noise pulses
//	}
//	else
//	{
//		udb_flags._.radio_on = 1;
//		led_on(LED_GREEN);
//	}
//	failSafePulses = 0;
}

// called from heartbeat pulse at 1Hz
void radioIn_failsafe_reset(void)
{
//	noisePulses = 0;
}

static void set_udb_pwIn(int pwm, int index)
{
//#if (NORADIO != 1)
	pwm = pwm * TMR_FACTOR / 2; // yes we are scaling the parameter up front

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
	}

//#if (FLY_BY_DATALINK_ENABLED == 1)
//	// It's kind of a bad idea to override the radio mode input
//	if (MODE_SWITCH_INPUT_CHANNEL == index)
//	{
//		udb_pwIn[index] = pwm;
//	}
//	else
//	{
//		if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW)
//		{
//			// if mode is in low mode, use pwm values that came in from external source
//			udb_pwIn[index] = get_fbdl_pwm(index);
//		}
//		else
//		{
//			udb_pwIn[index] = pwm;
//		}
//	}
//#else
//	if (FAILSAFE_INPUT_CHANNEL == index)
//	{
//		//DPRINT("FS: %u %u %u\r\n", pwm, failSafePulses, noisePulses);
//		#ifdef DEBUG_FAILSAFE_MIN_MAX
//		{
//			static uint8_t foo = 0;
//			if (!(++foo % 32))
//			{
//				DPRINT("FS: %u\r\n", pwm);
//			}
//		}
//		#endif // DEBUG_FAILSAFE_MIN_MAX
//	}
	udb_pwIn[index] = pwm;
//#endif // FLY_BY_DATALINK_ENABLED
//#endif // NOARADIO !=1
}
//
//#if (NORADIO != 1)

/* In global HAL ISR for TIMER, before check the source of Interrupt, it call this function */
// There is just one IC_HANDLER callback for any IC
#if (USE_PPM_INPUT == 0)

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

	uint16_t time = 0;
	static uint16_t rise=0;

    if( htim->Instance == TIM5 )
	{
		/* CHANNEL 1 called ISR */
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			// Get the Captured timer
		//	time = HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_1);
			// Is it ok? I remove a warning passing htim instead of &htim, but it is ok?
			time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if (IC_PIN1) //Rising edge?
			{
				// There is a problem with this approach. if previous capture were 65000 and current capture is 23 so
				// 23-65000 -> overflow. I think that this is filtered out by noise filter function
//
// NOTE: i hope not! if this condition can exist then we should detect the counter rollover
//       and perform the arithmatic appropriately.. (RobD)
//
				rise = time;
			}
			else // falling edge
			{
				set_udb_pwIn(rise-time, 1); // Return captured timer
			}
		}
		/* CHANNEL 2 called ISR */
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			if (IC_PIN2)
			{
				rise = time;
			}
			else
			{
				set_udb_pwIn(rise-time, 2);
			}
		}
	}
	else if (htim->Instance == TIM4)
	{
		/* CHANNEL 1 called ISR */
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
			if (IC_PIN3)
			{
				rise = time;
			}
			else
			{
				set_udb_pwIn(rise-time, 3);
			}
		}
		/* CHANNEL 2 called ISR */
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			if (IC_PIN4)
			{
				rise = time;
			}
			else
			{
				set_udb_pwIn(rise-time, 4);
			}
		}
			/* CHANNEL 3 called ISR */
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_3);
			if (IC_PIN5)
			{
				rise = time;
			}
			else
			{
				set_udb_pwIn(rise-time, 5);
			}
		}
		/* CHANNEL 4 called ISR */
		else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
			if (IC_PIN6)
			{
				rise = time;
			}
			else
			{
				set_udb_pwIn(rise-time, 6);
			}
		}
	}
}
#else // (USE_PPM_INPUT != 0)

	#if (PPM_SIGNAL_INVERTED == 1)
	#define PPM_PULSE_VALUE 0
	#else
	#define PPM_PULSE_VALUE 1
	#endif

/*
elgarbe: On my receiver I've PPM1 signal, but Toff is the part that change and Ton is fixd at 0.5mSeg
is it PPM1 with PPM_PULSE_VALUE = 1 I think not
PPM_3

       1     2       3        4      5       6      7
   ___   ___   ___       ___    ___     ___     ___
  |   | |   | |   |     |   |  |   |   |   |   |   |
  |   | |   | |   |     |   |  |   |   |   |   |   |
__|   |_|   |_|   |_____|   |__|   |___|   |___|   |____


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
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){

//	extern int one_hertz_flag;

//	indicate_loading_inter;
//	interrupt_save_set_corcon;

	static uint16_t rise_ppm = 0;
	static uint8_t ppm_ch = 0;
	uint16_t time = 0;

	if (htim->Instance == TIM5)
	{

		/* CHANNEL 1 called ISR */
		if( htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
		{
			time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
#if (USE_PPM_INPUT == 1)
			// ToDo: Use PPM_IC to get IC_PINx
            if ((IC_PIN2) == PPM_PULSE_VALUE)
			{
				uint16_t pulse = time - rise_ppm;
				rise_ppm = time;

				if (pulse > MIN_SYNC_PULSE_WIDTH)
				{
		//			if (one_hertz_flag)
		//			{
		//				one_hertz_flag = 0;
		//				DPRINT("**: %u %u\r\n", pulse, MIN_SYNC_PULSE_WIDTH);
		//			}
					ppm_ch = 1;
				}
				else
				{
		//			if (one_hertz_flag)
		//			{
		//				one_hertz_flag = 0;
		//				DPRINT("--: %u\r\n", pulse);
		//			}
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
			else
			{
		//		if (one_hertz_flag)
		//		{
		//			one_hertz_flag = 0;
		//			DPRINT("DIS %u\r\n", time);
		//		}
			}
        }
#elif (USE_PPM_INPUT == 2)


			uint16_t pulse = time - rise_ppm;
			rise_ppm = time;
			//ToDo: Use PPM_IC to get IC_PINx
			if ((IC_PIN2) == PPM_PULSE_VALUE)
			{
				if (pulse > MIN_SYNC_PULSE_WIDTH)   //Looking for long pulse time to get synchronized with CH1
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

        }


#else // USE_PPM_INPUT > 2
#error Invalid USE_PPM_INPUT setting
#endif // USE_PPM_INPUT
//	interrupt_restore_corcon;
	}
}
#endif

//#endif // NORADIO !=1
