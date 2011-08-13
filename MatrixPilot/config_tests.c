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

#include "defines.h"


// This file should generate no code.
// It's just meant for precompiler tests, to catch problems and inconsistencies
// with the user's configuration settings.


// Check RC Inputs
#if (THROTTLE_INPUT_CHANNEL > NUM_INPUTS)
	#error("THROTTLE_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (AILERON_INPUT_CHANNEL > NUM_INPUTS)
	#error("AILERON_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (ELEVATOR_INPUT_CHANNEL > NUM_INPUTS)
	#error("ELEVATOR_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (RUDDER_INPUT_CHANNEL > NUM_INPUTS)
	#error("RUDDER_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (MODE_SWITCH_INPUT_CHANNEL > NUM_INPUTS)
	#error("MODE_SWITCH_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (CAMERA_PITCH_INPUT_CHANNEL > NUM_INPUTS)
	#error("CAMERA_PITCH_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (CAMERA_YAW_INPUT_CHANNEL > NUM_INPUTS)
	#error("CAMERA_YAW_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (OSD_MODE_SWITCH_INPUT_CHANNEL > NUM_INPUTS)
	#error("OSD_MODE_SWITCH_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (PASSTHROUGH_A_INPUT_CHANNEL > NUM_INPUTS)
	#error("PASSTHROUGH_A_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (PASSTHROUGH_B_INPUT_CHANNEL > NUM_INPUTS)
	#error("PASSTHROUGH_B_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (PASSTHROUGH_C_INPUT_CHANNEL > NUM_INPUTS)
	#error("PASSTHROUGH_C_INPUT_CHANNEL > NUM_INPUTS.")
#endif

#if (PASSTHROUGH_D_INPUT_CHANNEL > NUM_INPUTS)
	#error("PASSTHROUGH_D_INPUT_CHANNEL > NUM_INPUTS.")
#endif



// Check RC Outputs

#if (THROTTLE_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("THROTTLE_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (AILERON_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("AILERON_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (ELEVATOR_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("ELEVATOR_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (RUDDER_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("RUDDER_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (AILERON_SECONDARY_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("AILERON_SECONDARY_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (CAMERA_PITCH_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("CAMERA_PITCH_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (CAMERA_YAW_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("CAMERA_YAW_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (TRIGGER_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("TRIGGER_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (PASSTHROUGH_A_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("PASSTHROUGH_A_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (PASSTHROUGH_B_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("PASSTHROUGH_B_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (PASSTHROUGH_C_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("PASSTHROUGH_C_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif

#if (PASSTHROUGH_D_OUTPUT_CHANNEL > NUM_OUTPUTS)
	#error("PASSTHROUGH_D_OUTPUT_CHANNEL > NUM_OUTPUTS.")
#endif



// Check HILSIM Settings
#if (HILSIM == 1 && GPS_TYPE != GPS_UBX_4HZ)
	#error("When using HILSIM, GPS_TYPE must be set to GPS_UBX_4HZ.")
#endif



// Check OSD Settings
#if (USE_OSD == 1 && CLOCK_CONFIG == CRYSTAL_CLOCK )
	#error("When using the OSD, CLOCK_CONFIG must be set to FRC8X_CLOCK.")
#endif



#if (BOARD_IS_CLASSIC_UDB == 1)

// Check for Current Sensor Pin Conflicts
#if (USE_CURRENT_SENSOR == 1)
	#if (USE_PPM_INPUT == 0)
		#error("Using the Current Sensor requires using PPM Input.")
	#elif (PPM_ALT_OUTPUT_PINS != 1 && NUM_OUTPUTS > 5)
		#error("Using the Current Sensor with PPM_ALT_OUTPUT_PINS set to 0 requires NUM_OUTPUTS to be less than 6.")
	#elif (NUM_OUTPUTS > 8)
		#error("Using the Current Sensor with PPM_ALT_OUTPUT_PINS set to 0 requires NUM_OUTPUTS to be less than 9.")
	#endif
#endif

// Check for RSSI Input Pin Conflicts
#if (USE_RSSI_INPUT == 1)
	#if (USE_PPM_INPUT == 0)
		#error("Using the RSSI Input requires using PPM Input.")
	#elif (PPM_ALT_OUTPUT_PINS != 1 && NUM_OUTPUTS > 4)
		#error("Using the RSSI Input with PPM_ALT_OUTPUT_PINS set to 0 requires NUM_OUTPUTS to be less than 5.")
	#elif (NUM_OUTPUTS > 7)
		#error("Using the RSSI Input with PPM_ALT_OUTPUT_PINS set to 0 requires NUM_OUTPUTS to be less than 8.")
	#endif
#endif

#endif
