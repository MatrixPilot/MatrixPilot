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


#include "libDCM_internal.h"
#include "gpsParseCommon.h"


#if (HILSIM == 1)
#if (USE_VARIABLE_HILSIM_CHANNELS != 1)
uint8_t SIMservoOutputs[] = {
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
uint8_t SIMservoOutputs[(NUM_OUTPUTS*2) + 5] = {
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
