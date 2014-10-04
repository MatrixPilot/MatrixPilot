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
#include "selectIn.h"
#include "radioIn.h"
#include "lrsIn.h"
#include "../MatrixPilot/mode_switch.h"


void datalinkIn_init(void) {}
boolean datalinkIn_isFailSafe(void) { return true; }
uint8_t datalinkIn_getInput(int16_t* ppm, uint8_t channels) { return 0; }


typedef boolean (*selectIn_isFailSafe_fptr_t)(void);typedef uint8_t (*selectIn_getInput_fptr_t)(int16_t*, uint8_t );
typedef struct inputDev {
	selectIn_isFailSafe_fptr_t isFailSafe;
	selectIn_getInput_fptr_t getInput;
} inputDev_t;

#define MAX_INPUT_DEVICES 3

static inputDev_t inputDevs[MAX_INPUT_DEVICES] = {
	{ 0x00, 0x00 },
	{ 0x00, 0x00 },
	{ 0x00, 0x00 },
};
static uint8_t activeDev = 0;
static boolean isFailSafe = true;
static uint8_t mode = MODE_MANUAL;
static uint8_t prev_mode = MODE_MANUAL;

void selectIn_init(void)
{
	inputDevs[0].isFailSafe = lrsIn_isFailSafe;
	inputDevs[0].getInput = lrsIn_getInput;

	inputDevs[1].isFailSafe = radioIn_isFailSafe;
	inputDevs[1].getInput = radioIn_getInput;

	inputDevs[2].isFailSafe = datalinkIn_isFailSafe;
	inputDevs[2].getInput = datalinkIn_getInput;

	lrsIn_init();
	radioIn_init();
	datalinkIn_init();
}

boolean selectIn(int16_t* pwIn, int8_t pwIn_len)
{
	uint8_t mode_channel;

	for (activeDev = 0; activeDev < MAX_INPUT_DEVICES; activeDev++)
	{
		assert(inputDevs[activeDev].isFailSafe);
		if (!inputDevs[activeDev].isFailSafe())
		{
			assert(inputDevs[activeDev].getInput);
			mode_channel = inputDevs[activeDev].getInput(pwIn, pwIn_len);
			mode = mode_switch_check_set(pwIn[mode_channel]); // call at 40Hz
			if (mode != prev_mode)
			{
				prev_mode = mode;
				switch (mode) {
					case MODE_MANUAL:
						DPRINT("MODE_MANUAL\r\n");
						break;
					case MODE_STABILISE:
						DPRINT("MODE_STABILISE\r\n");
						break;
					case MODE_AUTOPILOT:
						DPRINT("MODE_AUTOPILOT\r\n");
						break;
					default:
						break;
				}
			}
			isFailSafe = false;
		}
	}
//	udb_activate_failsafe(isFailSafe);
	return isFailSafe;
}

boolean selectIn_chkMode(uint8_t mode)
{
	switch (mode) {
		case MODE_MANUAL:
			return (mode == MODE_MANUAL);
		case MODE_STABILISE:
			return (mode == MODE_STABILISE);
		case MODE_AUTOPILOT:
			return (mode == MODE_AUTOPILOT);
		case MODE_FAILSAFE:
			return isFailSafe;
		default:
			assert(0); // throw an assertion failure as we should never get here
			break;
	}
	return false; // we should never get here
}
