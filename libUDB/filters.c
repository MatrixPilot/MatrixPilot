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

#include "filters.h"

inline int lp2(int input, union int32_w2 *state, unsigned int lpcb) {
    state->longword -= __builtin_mulus(lpcb, state->word.high);
    state->longword += __builtin_mulus(lpcb, input);
    return (state->word.high);
}

void init_boxCarState(int len, int N, int* buff, long* sum, struct boxCarState* state)
{
    int i;
    state->N = N;
    state->len = len;
    state->index = 0;
    state->buff = buff;
    for (i=0; i<N*len; i++) buff[i] = 0;
    state->buff = buff;
    state->sum = sum;
    for (i=0; i<N; i++) sum[i] = 0;
}

void boxcar(int* value, struct boxCarState* state, int* result)
{
    int i, offset;
    state->index++;  // index of oldest value in buffer
    if (state->index >= state->len) // wrap circular
        state->index = 0;
    for (i = 0; i < state->N; i++)
    {
        offset = i * state->len + state->index;
        state->sum[i] -= state->buff[offset]; // subtract oldest value
        state->buff[offset] = value[i]; // save new value
        state->sum[i] += value[i]; // add in new value
        result[i] = state->sum[i] / state->len; // divide by number of samples
    }
}

// not tested in MPQ
//void boxcar_shift(int* index, int* value, long* sum, int* buff, int* result)
//{
//    int i, offset;
//    *index = (boxBufLen - 1) & (*index + 1); // index of oldest value in buffer
//    for (i = 0; i < boxBufN; i++)
//    {
//        offset = i * boxBufLen + *index;
//        sum[i] -= buff[offset]; // subtract oldest value
//        buff[offset] = value[i]; // save new value
//        sum[i] += value[i]; // add in new value
//        result[i] = sum[i] >> boxBufPow; // divide by number of samples
//    }
//}

