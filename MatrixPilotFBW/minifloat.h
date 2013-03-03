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

#include <libq.h>

#define MANT_NEG_MIN -256
#define EXP_POS_MAX 63
#define EXP_NEG_MIN -64

typedef struct minifloat_tag
{
    signed int mant : 9;
    signed int exp : 7;
} minifloat;

// Multiply two minifloats
extern minifloat mf_mult(minifloat a, minifloat b);

// Divide two minifloats
extern minifloat mf_div(minifloat num, minifloat den);

extern minifloat mf_sqrt(minifloat num);

//minifloat to long
extern long mftol(minifloat mf);

// Long to minifloat
extern minifloat ltomf(long n);

//minifloat to Q16
//extern Q16 mftoQ16(minifloat mf);

// Q16 to minifloat
//sextern minifloat ltomf(Q16 n);
