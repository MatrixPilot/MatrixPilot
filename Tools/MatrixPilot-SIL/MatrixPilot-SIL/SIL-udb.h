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

// Fake ASM calls
#define __builtin_mulss(x,y) ((long)((short)(x)*(short)(y)))
#define __builtin_mulus(x,y) ((long)((unsigned short)(x)*(short)(y)))
#define __builtin_mulsu(x,y) ((long)((short)(x)*(unsigned short)(y)))
#define __builtin_muluu(x,y) ((unsigned long)((unsigned short)(x)*(unsigned short)(y)))

#define __builtin_divud(x,y) ((short)((unsigned long)(x)/(short)(y)))
#define __builtin_divsd(x,y) ((short)((long)(x)/(short)(y)))


// LEDs
extern char leds[4];
#define LED_BLUE			leds[0]
#define LED_ORANGE			leds[1]
#define LED_GREEN			leds[2]
#define LED_RED				leds[3]

// There are no hardware toggle switches on the UDB4, so use values of 0
#define HW_SWITCH_1			0
#define HW_SWITCH_2			0
#define HW_SWITCH_3			0

#define MAX_INPUTS			8
#define MAX_OUTPUTS			8


#endif
