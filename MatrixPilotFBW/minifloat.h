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

#ifndef MINIFLOAT_H
#define MINIFLOAT_H

#include <libq.h>
#include <math.h>

#define MANT_NEG_MIN -256
#define EXP_POS_MAX 63
#define EXP_NEG_MIN -64

typedef struct minifloat_tag
{
    int16_t mant : 9;
    int16_t exp : 7;
} minifloat;

// Multiply two minifloats
extern minifloat mf_mult(minifloat a, minifloat b);

// Divide two minifloats
extern minifloat mf_div(minifloat num, minifloat den);

// Square root
extern minifloat mf_sqrt(minifloat num);

// Square - simplification of multiply
extern minifloat mf_sqr(minifloat num);

// Add minifloats
extern minifloat mf_add(minifloat a, minifloat b);

// Subtract minifloat a-b
extern minifloat mf_sub(minifloat a, minifloat b);

// Floating point to minifloat
extern minifloat ftomf(float num);

// Minifloat to float
extern float mftof(minifloat num);

// RMAX scale to minifloat
extern minifloat RMAXtomf(int16_t num);

//minifloat to long
extern int32_t mftol(minifloat mf);

// Long to minifloat
extern minifloat ltomf(int32_t n);

// Compare b to a
// If greater, return 1.  If less return -1
// If equal return 0
extern int16_t mf_compare_mag(minifloat a, minifloat b);

// Get magnitude of minifloat
extern minifloat mf_mag(minifloat num);

// Check if b is larger than a
extern int16_t mf_larger(minifloat a, minifloat b);

// Invert
extern minifloat mf_inv(minifloat num);

//minifloat to Q16 in longww union
// ._.W0 is underflow fractional
// ._.W1 is integer
extern _Q16 mftoQ16(minifloat mf);

// Q16 to minifloat
extern minifloat Q16tomf(_Q16 n);


#endif 	// MINIFLOAT_H
