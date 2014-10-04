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


#include "libUDB.h"
#include "oscillator.h"
#include "interrupt.h"
#include "lrsIn.h"

#if (USE_LRS_RX == 1)
#else
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static boolean isFailSafe = true;
static int16_t lrsIn[NUM_INPUTS+1];     // pulse widths of radio inputs
static int16_t lrsTrim[NUM_INPUTS+1];   // initial pulse widths for trimming

void lrsIn_init(void)
{
}

boolean lrsIn_isFailSafe(void)
{
	return isFailSafe;
}

uint8_t lrsIn_getInput(int16_t* ppm, uint8_t channels)
{
	uint8_t c;

	for (c = 0; c < channels; c++)
	{
		ppm[c+1] = lrsIn[c+1];
	}
	return MODE_SWITCH_INPUT_CHANNEL; // make this define specific to each ppm input device
}

void lrsIn_record_trims(void)
{
	int16_t i;
	for (i = 0; i <= NUM_INPUTS; i++)
	{
		lrsTrim[i] = lrsIn[i];
	}
}

// ALL THE FUNCTIONS BELOW ARE CALLED FROM THE LRS DRIVER
void lrsIn_setInput(volatile uint16_t PPM[], uint8_t channels)
{
	uint8_t c;

//	for (c = 0; c < channels, c++)8_t
//	for (c = 0; c < MIN((NUM_INPUTS + 1), channels); c++)
	for (c = 0; c < MIN(NUM_INPUTS, channels); c++)
	{
		lrsIn[c+1] = PPM[c] * 2 + 1977; // hence pwIn values range from 1977 thru 4023
	}
/*
	static int cnt = 0;
	if (cnt++ > 50)
	{
		cnt = 0;
		DPRINT("ch: ");
//		for (c = 0; c < MIN(NUM_INPUTS, channels); c++)
//		for (c = 0; c < 4; c++)
		for (c = 4; c < MIN(11, channels); c++)
		{
			DPRINT("%u ", PPM[c]);
//			DPRINT("%u ", lrsIn[c+1]);
		}
		DPRINT("\r\n");
	}
 */
}

void lrsIn_setFailsafe(boolean active) // called from low level driver
{
	if (active != isFailSafe)
	{
		DPRINT("lrsIn_setFailsafe(%u)\r\n", active);
	}
	isFailSafe = active;
}
