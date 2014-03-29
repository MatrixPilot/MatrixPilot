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
//#include "libUDB_defines.h"

// IIR lowpass filter
union int32_w2 { long  longword; struct {int low; int high;} word; } ;
inline int lp2(int input, union int32_w2 *state, unsigned int lpcb);

// power of 2 length filter not tested in MPQ
// boxBufLen point boxcar low-pass filter
// transfer function is sinc(f)
// N independent data streams
//#define boxBufPow 4
//#define boxBufLen 16
//#define boxBufN 3
//void boxcar_shift(int* index, int *value, long* sum, int* buff, int* result);

// general boxCar filter with length len samples, T=len * sample interval
// transfer function is sinc(f), 1st zero crossing of TF is at 1/T Hz
// filters N data sequences
// caller allocates N*len element integer array buff
// caller allocates N element long array sum
// must call init_boxCarState prior to calling boxcar
struct boxCarState {
    int len; // length of rectangular window
    int N; // number of parallel data streams
    int* buff; // N x len buffer
    long* sum; // N filter sums
    int index; // index into stream buffers
};

void boxcar(int *value, struct boxCarState* state, int* result);
void init_boxCarState(int len, int N, int* buff, long* sum, struct boxCarState* state);
#endif
