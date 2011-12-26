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
#if (BOARD_IS_CLASSIC_UDB == 1)
	#if (USE_PPM_INPUT != 1 && NUM_INPUTS > 5)
		#error("NUM_INPUTS can't be more than 5 without using PPM Input.")
	#elif (USE_PPM_INPUT == 1 && NUM_INPUTS > 9)
		#error("NUM_INPUTS can't be more than 9 when using PPM Input.")
	#endif
#else
	// UDB4
	#if (USE_PPM_INPUT != 1 && NUM_INPUTS > 8)
		#error("NUM_INPUTS can't be more than 8 without using PPM Input.")
	#elif (USE_PPM_INPUT == 1 && NUM_INPUTS > 9)
		#error("NUM_INPUTS can't be more than 9 when using PPM Input.")
	#endif
#endif

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
#if (BOARD_IS_CLASSIC_UDB == 1)
	#if (USE_PPM_INPUT != 1 && NUM_OUTPUTS > 6)
		#error("NUM_OUTPUTS can't be more than 6 without using PPM Input.")
	#elif (USE_PPM_INPUT == 1 && NUM_OUTPUTS > 9)
		#error("NUM_OUTPUTS can't be more than 9 when using PPM Input.")
	#endif
#else
	// UDB4
	#if (NUM_OUTPUTS > 9)
		#error("NUM_OUTPUTS can't be more than 9.")
	#endif
#endif

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



#if (BOARD_IS_CLASSIC_UDB == 1)

// Check OSD Settings
#if (USE_OSD == 1 && CLOCK_CONFIG == CRYSTAL_CLOCK )
	#error("When using the OSD, CLOCK_CONFIG must be set to FRC8X_CLOCK.")
#endif


// Check MediaTek Settings
#if (UGPS_TYPE == GPS_MTEK && CLOCK_CONFIG == CRYSTAL_CLOCK )
	#error("When using GPS_MTEK, CLOCK_CONFIG must be set to FRC8X_CLOCK.")
#endif


// Check for Analog Sensor Pin Conflicts
#if (NUM_ANALOG_INPUTS >= 1)
	#if (USE_PPM_INPUT == 0)
		#error("Using the Analog Inputs requires using PPM Input.")
	#elif (NUM_ANALOG_INPUTS > 2)
		#error("Only 2 Analog Inputs are available on this UDB model.")
	#elif (PPM_ALT_OUTPUT_PINS != 1 && NUM_OUTPUTS + NUM_ANALOG_INPUTS > 6)
		#error("Using the Analog Inputs with PPM_ALT_OUTPUT_PINS set to 0 requires NUM_OUTPUTS + NUM_ANALOG_INPUTS to be no more than 6.")
	#elif (PPM_ALT_OUTPUT_PINS == 1 && NUM_OUTPUTS + NUM_ANALOG_INPUTS > 9)
		#error("Using the Analog Inputs with PPM_ALT_OUTPUT_PINS set to 1 requires NUM_OUTPUTS + NUM_ANALOG_INPUTS to be no more than 9.")
	#endif
#endif

#else
	// UDB4
	#if (NUM_ANALOG_INPUTS > 4)
		#error("Only 4 extra Analog Inputs are available the UDB4.")
	#endif
#endif


// Check Analog Inputs
#if (ANALOG_CURRENT_INPUT_CHANNEL > NUM_ANALOG_INPUTS)
	#error("ANALOG_CURRENT_INPUT_CHANNEL > NUM_ANALOG_INPUTS.")
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL > NUM_ANALOG_INPUTS)
	#error("ANALOG_VOLTAGE_INPUT_CHANNEL > NUM_ANALOG_INPUTS.")
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL > NUM_ANALOG_INPUTS)
	#error("ANALOG_RSSI_INPUT_CHANNEL > NUM_ANALOG_INPUTS.")
#endif

// Check Magnetometer Options
#if ( MAG_YAW_DRIFT == 1 )
#ifdef MAG_DIRECT
#if ( BOARD_ORIENTATION != ORIENTATION_FORWARDS )
	#error("This board orientation is not yet supported with MAG_DIRECT mag option."
#endif
#endif
#endif

// Check MAVLink Options
#if ( SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK ) && ( BOARD_TYPE != UDB4_BOARD )
	#error("SERIAL_MAVLINK requires use of the UDB4 to ensure sufficient RAM available.")
#endif
#if ( SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK ) && ( FLIGHT_PLAN_TYPE != FP_WAYPOINTS )
	#error("SERIAL_MAVLINK only works with FP_WAYPOINTS in options.h, not FP_LOGO.")
#endif

