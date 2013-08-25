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


#ifndef UDB_TYPES_H
#define UDB_TYPES_H

#include <stdint.h>

// UDB Types
struct bb { uint8_t B0; uint8_t B1; };
struct bbbb { uint8_t B0; uint8_t B1; uint8_t B2; uint8_t B3; };
struct ww { int16_t W0; int16_t W1; };
struct wwww { int16_t W0; int16_t W1; int16_t W2; int16_t W3; };
struct LL { int32_t L0; int32_t L1; };

union intbb { int16_t BB; struct bb _; };
union longbbbb { int32_t WW; struct ww _; struct bbbb __; };
union longww { int32_t  WW; struct ww _; };
union longlongLL { int64_t LL; struct LL _; struct wwww __; };

// UDB Constants
#define RMAX                    16384//0b0100000000000000       // 1.0 in 2.14 fractional format

//#if (SILSIM != 1)

// Device header file
#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif // __C30__
#endif // __XC16__

//#endif // SILSIM

#if defined(WIN)
// Fake ASM calls
#define __builtin_mulss(x,y) ((( int32_t)(x))*( int32_t)(y))
#define __builtin_mulus(x,y) (((uint32_t)(x))*( int32_t)(y))
#define __builtin_mulsu(x,y) ((( int32_t)(x))*(uint32_t)(y))
#define __builtin_muluu(x,y) (((uint32_t)(x))*(uint32_t)(y))
#define __builtin_divud(x,y) ((uint16_t)(((uint32_t)(x))/(uint16_t)(y)))
#define __builtin_divsd(x,y) (( int16_t)((( int32_t)(x))/(int16_t)(y)))
#endif // WIN

#endif // UDB_TYPES_H
