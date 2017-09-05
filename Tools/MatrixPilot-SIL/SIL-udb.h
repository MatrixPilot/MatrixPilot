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

/*
// Fake ASM calls
#if 1
#define __builtin_mulss(x,y) ((( int32_t)(x))*( int32_t)(y))
#define __builtin_mulus(x,y) (((uint32_t)(x))*( int32_t)(y))
#define __builtin_mulsu(x,y) ((( int32_t)(x))*(uint32_t)(y))
#define __builtin_muluu(x,y) (((uint32_t)(x))*(uint32_t)(y))
#define __builtin_divud(x,y) ((uint16_t)(((uint32_t)(x))/(uint16_t)(y)))
#define __builtin_divsd(x,y) (( int16_t)((( int32_t)(x))/(int16_t)(y)))
#else
static inline int32_t  __builtin_mulss(const int16_t p0,   const int16_t p1)   { return (p0 * p1); }
static inline int32_t  __builtin_mulus(const uint16_t p0,  const int16_t p1)   { return (p0 * p1); }
static inline int32_t  __builtin_mulsu(const int16_t p0,   const uint16_t p1)  { return (p0 * p1); }
static inline uint32_t __builtin_muluu(const uint16_t p0,  const uint16_t p1)  { return (p0 * p1); }
static inline uint16_t __builtin_divud(const uint32_t num, const uint16_t den) { return (num / den); }
static inline int16_t  __builtin_divsd(const int32_t num,  const int16_t den)  { return (num / den); }

// Prototypes from Microchip reference manual
//signed long __builtin_mulss(const signed int p0, const signed int p1);
//signed long __builtin_mulus(const unsigned int p0, const signed int p1);
//signed long __builtin_mulsu(const signed int p0, const unsigned int p1);
//unsigned long __builtin_muluu(const unsigned int p0, const unsigned int p1);
//unsigned int __builtin_divud(const unsigned long num, const unsigned int den);
//int __builtin_divsd(const long num, const int den);
#endif
 */
// LEDs
//extern uint8_t leds[5];
//#define LED_RED         leds[0]
//#define LED_GREEN       leds[1]
//#define LED_ORANGE      leds[2]
//#define LED_BLUE        leds[3]
//#define LED_TAIL_LIGHT  leds[4]

#define LED_RED         0
#define LED_GREEN       1
#define LED_ORANGE      2
#define LED_BLUE        3
#define LED_TAIL_LIGHT  4

#define MAX_INPUTS      8
#define MAX_OUTPUTS     8

extern UDBSocket gpsSocket;
extern UDBSocket telemetrySocket;
extern UDBSocket serialSocket;
extern uint8_t sil_radio_on;

extern volatile uint16_t trap_flags;
extern volatile uint32_t trap_source;
extern volatile uint16_t osc_fail_count;

uint16_t get_reset_flags(void);
void sil_reset(void);

uint16_t get_current_milliseconds(void);
void sleep_milliseconds(uint16_t ms);

void sil_telemetry_input(uint8_t* buffer, int32_t bytesRead);
void mavlink_start_sending_data(void);

#endif
