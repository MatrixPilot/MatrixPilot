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

#ifndef FILTERS_H
#define FILTERS_H

#include <stdlib.h>

// 3dB frequency about 45Hz at 2 KHz sample rate
#define LPCB_45_HZ (unsigned int)(65536 / 12)

union int32_w2 { int32_t  longword; struct {int16_t low; int16_t high;} word; } ;

// IIR lowpass recursive filter
// longword = (1 - lpcb)*longword + lpcb*input
static inline int16_t lp2(int16_t input, union int32_w2 *state, uint16_t lpcb) {
    state->longword -= __builtin_mulus(lpcb, state->word.high);
    state->longword += __builtin_mulus(lpcb, input);
    return (state->word.high);
}

#endif
