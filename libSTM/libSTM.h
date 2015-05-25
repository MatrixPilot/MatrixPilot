//
//  libSTM.h
//  MatrixPilot
//
//  Created by Robert Dickenson on 17/6/2014.
//  Copyright (c) 2014 MatrixPilot. All rights reserved.
//

#ifndef _LIBSTM_H_
#define _LIBSTM_H_

#include "dsp.h"

// Fake ASM calls
#if 0
#define __builtin_mulss(x,y) ((( int32_t)(x))*( int32_t)(y))
#define __builtin_mulus(x,y) (((uint32_t)(x))*( int32_t)(y))
#define __builtin_mulsu(x,y) ((( int32_t)(x))*(uint32_t)(y))
#define __builtin_muluu(x,y) (((uint32_t)(x))*(uint32_t)(y))
#define __builtin_divud(x,y) ((uint16_t)(((uint32_t)(x))/(uint16_t)(y)))
#define __builtin_divsd(x,y) (( int16_t)((( int32_t)(x))/(int16_t)(y)))
#else
/*
static inline int32_t  __builtin_mulss(const int16_t p0,   const int16_t p1)   { return (p0 * p1); }
static inline int32_t  __builtin_mulus(const uint16_t p0,  const int16_t p1)   { return (p0 * p1); }
static inline int32_t  __builtin_mulsu(const int16_t p0,   const uint16_t p1)  { return (p0 * p1); }
static inline uint32_t __builtin_muluu(const uint16_t p0,  const uint16_t p1)  { return (p0 * p1); }
static inline uint16_t __builtin_divud(const uint32_t num, const uint16_t den) { return (num / den); }
static inline int16_t  __builtin_divsd(const int32_t num,  const int16_t den)  { return (num / den); }
 */
/*
int32_t  __builtin_mulss(int16_t p0,   int16_t p1);
int32_t  __builtin_mulus(uint16_t p0,  int16_t p1);
int32_t  __builtin_mulsu(int16_t p0,   uint16_t p1);
uint32_t __builtin_muluu(uint16_t p0,  uint16_t p1);
uint16_t __builtin_divud(uint32_t num, uint16_t den);
int16_t  __builtin_divsd(int32_t num,  int16_t den);
 */
// Prototypes from Microchip reference manual
//signed long __builtin_mulss(const signed int p0, const signed int p1);
//signed long __builtin_mulus(const unsigned int p0, const signed int p1);
//signed long __builtin_mulsu(const signed int p0, const unsigned int p1);
//unsigned long __builtin_muluu(const unsigned int p0, const unsigned int p1);
//unsigned int __builtin_divud(const unsigned long num, const unsigned int den);
//int __builtin_divsd(const long num, const int den);
#endif

// LEDs
//extern uint8_t leds[4];
//#define LED_RED         leds[0]
//#define LED_GREEN       leds[1]
//#define LED_ORANGE      leds[2]
//#define LED_BLUE        leds[3]


//#define MAX_INPUTS      6
//#define MAX_OUTPUTS     6


extern uint8_t sil_radio_on;

extern volatile uint16_t trap_flags;
extern volatile uint32_t trap_source;
extern volatile uint16_t osc_fail_count;

void udb_magnetometer_callback(void);
uint16_t get_reset_flags(void);


#endif // _LIBSTM_H_

