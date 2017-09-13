// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2016 MatrixPilot Team
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


#include "libDCM.h"
#include "gpsParseCommon.h"
#include "../libUDB/radioIn.h"
#include "../libUDB/servoOut.h"
#include "hilsim.h"


#if (HILSIM == 1)

static int16_t hil_rc_input_adjust(char *inChannelName, int inChannelIndex, int delta)
{
	udb_pwIn[inChannelIndex] = udb_servo_pulsesat(udb_pwIn[inChannelIndex] + delta);
	if (inChannelIndex == THROTTLE_INPUT_CHANNEL) {
		printf("%s = %d%%\t\r", inChannelName, (udb_pwIn[inChannelIndex]-udb_pwTrim[inChannelIndex])/20);
	}
	else {
		printf("%s = %d%%\t\r", inChannelName, (udb_pwIn[inChannelIndex]-udb_pwTrim[inChannelIndex])/10);
	}
	return udb_pwIn[inChannelIndex];
}

int16_t hilsim_input_adjust(char* inChannelName, int delta)
{
	switch (inChannelName[0]) {
	case 't':
		return hil_rc_input_adjust(inChannelName, THROTTLE_INPUT_CHANNEL, delta);
	case 'r':
		return hil_rc_input_adjust(inChannelName, RUDDER_INPUT_CHANNEL, delta);
	case 'e':
		return hil_rc_input_adjust(inChannelName, ELEVATOR_INPUT_CHANNEL, delta);
	case 'a':
		return hil_rc_input_adjust(inChannelName, AILERON_INPUT_CHANNEL, delta);
	case 'm':
		{
		switch (delta) {
		case 1: // switch mode to manual
			udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = MODE_SWITCH_THRESHOLD_LOW - 1;
			break;
		case 2: // switch mode to stabilised
			udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = MODE_SWITCH_THRESHOLD_LOW + 1;
			break;
		case 3: // switch mode to guided
			udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = MODE_SWITCH_THRESHOLD_HIGH + 1;
			break;
		case 4: // switch mode to failsafe
			udb_pwIn[FAILSAFE_INPUT_CHANNEL] = FAILSAFE_INPUT_MIN - 1;
			break;
		default:
			break;
		}
		return udb_pwIn[MODE_SWITCH_INPUT_CHANNEL];
		}
	case 's': // centre the flight 'stick'
		udb_pwIn[AILERON_INPUT_CHANNEL]  = udb_pwTrim[AILERON_INPUT_CHANNEL];
		udb_pwIn[ELEVATOR_INPUT_CHANNEL] = udb_pwTrim[ELEVATOR_INPUT_CHANNEL];
		udb_pwIn[RUDDER_INPUT_CHANNEL]   = udb_pwTrim[RUDDER_INPUT_CHANNEL];
		printf("\naileron, elevator, rudder = %i, %i, %i\n", udb_pwIn[AILERON_INPUT_CHANNEL], udb_pwIn[ELEVATOR_INPUT_CHANNEL], udb_pwIn[RUDDER_INPUT_CHANNEL]);
		return 0;
	default:
		return 0;
	}
	return 0;
}

#define KEYPRESS_INPUT_DELTA 50

// This is called by the UBX GPS message parser and contians keystroke data from X-Plane
void hilsim_handle_key_input(char c)
{
	switch (c) {
		case 107: // Numpad +
			hilsim_input_adjust("throttle", KEYPRESS_INPUT_DELTA*2);
			break;
		case 109: // Numpad -
			hilsim_input_adjust("throttle", -KEYPRESS_INPUT_DELTA*2);
			break;
		case 97:  // Numpad 1
			hilsim_input_adjust("rudder", KEYPRESS_INPUT_DELTA);
			break;
		case 99:  // Numpad 3
			hilsim_input_adjust("rudder", -KEYPRESS_INPUT_DELTA);
			break;
		case 104: // Numpad 8
			hilsim_input_adjust("elevator", KEYPRESS_INPUT_DELTA);
			break;
		case 98:  // Numpad 2
			hilsim_input_adjust("elevator", -KEYPRESS_INPUT_DELTA);
			break;
		case 100: // Numpad 4
			hilsim_input_adjust("aileron", KEYPRESS_INPUT_DELTA);
			break;
		case 102: // Numpad 6
			hilsim_input_adjust("aileron", -KEYPRESS_INPUT_DELTA);
			break;
		case 101: // Numpad 5
			hilsim_input_adjust("stick", 0);
			break;
		case 35:  // '1' Numpad End (switch mode to manual)
			hilsim_input_adjust("mode", 1);
			break;
		case 111: // '2' Numpad / (switch mode to stabilised)
			hilsim_input_adjust("mode", 2);
			break;
		case 106: // '3' Numpad * (switch mode to guided)
			hilsim_input_adjust("mode", 3);
			break;
		case 36:  // '4' Numpad Home (switch mode to failsafe)
			hilsim_input_adjust("mode", 4);
			break;
		default:
			break;
	}
}
#endif // (HILSIM == 1)

#if (HILSIM == 1)
#if (USE_VARIABLE_HILSIM_CHANNELS != 1)
static uint8_t SIMservoOutputs[] = {
	0xFF, 0xEE, //sync
	0x03, 0x04, //S1
	0x05, 0x06, //S2
	0x07, 0x08, //S3
	0x09, 0x0A, //S4
	0x0B, 0x0C, //S5
	0x0D, 0x0E, //S6
	0x0F, 0x10, //S7
	0x11, 0x12, //S8
	0x13, 0x14  //checksum
};
#define HILSIM_NUM_SERVOS 8
#else
#define HILSIM_NUM_SERVOS NUM_OUTPUTS
static uint8_t SIMservoOutputs[(NUM_OUTPUTS*2) + 5] = {
	0xFE, 0xEF, // sync
	0x00        // output count
	            // Two checksum on the end
};
#endif // USE_VARIABLE_HILSIM_CHANNELS


void send_HILSIM_outputs(void)
{
	// Setup outputs for HILSIM
	int16_t i;
	uint8_t CK_A = 0;
	uint8_t CK_B = 0;
	union intbb TempBB;

#if (USE_VARIABLE_HILSIM_CHANNELS != 1)
	for (i = 1; i <= NUM_OUTPUTS; i++)
	{
		TempBB.BB = udb_pwOut[i];
		SIMservoOutputs[2*i] = TempBB._.B1;
		SIMservoOutputs[(2*i)+1] = TempBB._.B0;
	}

	for (i = 2; i < HILSIM_NUM_SERVOS*2+2; i++)
	{
		CK_A += SIMservoOutputs[i];
		CK_B += CK_A;
	}
	SIMservoOutputs[i] = CK_A;
	SIMservoOutputs[i+1] = CK_B;

	// Send HILSIM outputs
	gpsoutbin(HILSIM_NUM_SERVOS*2+4, SIMservoOutputs);
#else
	for (i = 1; i <= NUM_OUTPUTS; i++)
	{
		TempBB.BB = udb_pwOut[i];
		SIMservoOutputs[(2*i)+1] = TempBB._.B1;
		SIMservoOutputs[(2*i)+2] = TempBB._.B0;
	}

	SIMservoOutputs[2] = NUM_OUTPUTS;

	// Calcualte checksum
	for (i = 3; i < (NUM_OUTPUTS*2)+3; i++)
	{
		CK_A += SIMservoOutputs[i];
		CK_B += CK_A;
	}
	SIMservoOutputs[i] = CK_A;
	SIMservoOutputs[i+1] = CK_B;

	// Send HILSIM outputs
	gpsoutbin((HILSIM_NUM_SERVOS*2)+5, SIMservoOutputs);
#endif // USE_VARIABLE_HILSIM_CHANNELS
}

#endif // HILSIM
