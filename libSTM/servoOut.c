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
#include "../libDCM/libDCM.h"
#include "oscillator.h"
#include "interrupt.h"
#include "servoOut.h"
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include "tim.h"

#if (BOARD_TYPE == PX4_BOARD)

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
#define ACTION_OUT_PIN          SERVO_OUT_PIN_9

#elif (BOARD_TYPE == AUAV3_BOARD)

#if (NUM_OUTPUTS > 8)
#error "max of 8 servo outputs currently supported for AUAV3"
#endif

#else
#error Invalid BOARD_TYPE
#endif

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
extern TIM_HandleTypeDef htim3;    // TIM base for PWM CH1 to CH4

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
              // disable timer 4 interrupt for now (enable for each set of pulses)
	}

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == PX4_BOARD)

#elif (BOARD_TYPE == AUAV3_BOARD)

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
//	ACTION_OUT_PIN = newValue;
}

// Call this to start sending out pulses to all the PWM output channels sequentially
void start_pwm_outputs(void)
{
//	if (NUM_OUTPUTS > 0)
//	{
//
//	}

    /*##-3- Start PWM signals generation #######################################*/
    /* Start channel 1 */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    /* Start channel 1 */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    /* Start channel 1 */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    /* Start channel 1 */
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
}


