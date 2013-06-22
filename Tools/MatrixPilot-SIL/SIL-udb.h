//
//  SIL-udb.h
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/1/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#ifndef MatrixPilot_SIL_dsp_compat_h
#define MatrixPilot_SIL_dsp_compat_h

#include "SIL-dsp.h"
#include "UDBSocket.h"


// Fake ASM calls
#define __builtin_mulss(x,y) ((( int32_t)(x))*( int32_t)(y))
#define __builtin_mulus(x,y) (((uint32_t)(x))*( int32_t)(y))
#define __builtin_mulsu(x,y) ((( int32_t)(x))*(uint32_t)(y))
#define __builtin_muluu(x,y) (((uint32_t)(x))*(uint32_t)(y))

#define __builtin_divud(x,y) ((uint16_t)(((uint32_t)(x))/(uint16_t)(y)))
#define __builtin_divsd(x,y) (( int16_t)((( int32_t)(x))/(int16_t)(y)))


// LEDs
extern uint8_t leds[4];
#define LED_RED				leds[0]
#define LED_GREEN			leds[1]
#define LED_ORANGE			leds[2]
#define LED_BLUE			leds[3]

// There are no hardware toggle switches on the UDB4, so use values of 0
#define HW_SWITCH_1			0
#define HW_SWITCH_2			0
#define HW_SWITCH_3			0

#define MAX_INPUTS			8
#define MAX_OUTPUTS			8


extern UDBSocket gpsSocket;
extern UDBSocket telemetrySocket;
extern UDBSocket serialSocket;
extern uint8_t sil_radio_on;

extern volatile int16_t trap_flags ;
extern volatile int32_t trap_source ;
extern volatile int16_t osc_fail_count ;

void udb_magnetometer_callback(void);
uint16_t get_reset_flags(void) ;
void sil_reset(void);

uint16_t get_current_milliseconds();
void sleep_milliseconds(uint16_t ms);


#endif
