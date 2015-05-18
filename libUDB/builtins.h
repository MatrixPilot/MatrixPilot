//
//  builtins.h
//  MatrixPilot
//
//  Created by Robert Dickenson on 15/9/2014.
//  Copyright (c) 2014 MatrixPilot. All rights reserved.
//

#ifndef _BUILTINS_H_
#define _BUILTINS_H_

//#if (WIN == 1 || NIX == 1)
//#else
//#include <dsp.h>
//#endif

// Fake ASM calls
#if 1
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
int32_t  __builtin_mulss(int16_t p0,   int16_t p1);
int32_t  __builtin_mulus(uint16_t p0,  int16_t p1);
int32_t  __builtin_mulsu(int16_t p0,   uint16_t p1);
uint32_t __builtin_muluu(uint16_t p0,  uint16_t p1);
uint16_t __builtin_divud(uint32_t num, uint16_t den);
int16_t  __builtin_divsd(int32_t num,  int16_t den);

// Prototypes from Microchip reference manual
//signed long __builtin_mulss(const signed int p0, const signed int p1);
//signed long __builtin_mulus(const unsigned int p0, const signed int p1);
//signed long __builtin_mulsu(const signed int p0, const unsigned int p1);
//unsigned long __builtin_muluu(const unsigned int p0, const unsigned int p1);
//unsigned int __builtin_divud(const unsigned long num, const unsigned int den);
//int __builtin_divsd(const long num, const int den);
#endif


#endif // _BUILTINS_H_
