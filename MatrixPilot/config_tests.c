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
#include "options.h"
#include "oscillator.h"
#include "mavlink_options.h"

// This file should generate no code.
// It's just meant for precompiler tests, to catch problems and inconsistencies
// with the user's configuration settings.
//#define COLON :
#define _SELECTED_VALUE(l,v) l ": " #v
#define SELECTED_VALUE(macro) _SELECTED_VALUE(#macro, macro)

#if (BOARD_TYPE == UDB4_BOARD)
#pragma message "BOARD_TYPE: UDB4_BOARD"
#elif (BOARD_TYPE == UDB5_BOARD)
#pragma message "BOARD_TYPE: UDB5_BOARD"
#elif (BOARD_TYPE == AUAV3_BOARD)
#pragma message "BOARD_TYPE: AUAV3_BOARD"
#endif

#pragma message (SELECTED_VALUE(MIPS))

#if (GPS_TYPE == GPS_STD)
#pragma message "GPS_TYPE: GPS_STD"
#elif (GPS_TYPE == GPS_UBX_2HZ)
#pragma message "GPS_TYPE: GPS_UBX_2HZ"
#elif (GPS_TYPE == GPS_UBX_4HZ)
#pragma message "GPS_TYPE: GPS_UBX_4HZ"
#elif (GPS_TYPE == GPS_MTEK)
#pragma message "GPS_TYPE: GPS_MTEK"
#elif (GPS_TYPE == GPS_NMEA)
#pragma message "GPS_TYPE: GPS_MTEK"
#elif (GPS_TYPE == GPS_NONE)
#pragma message "GPS_TYPE: GPS_NONE"
#else
#error "invalid GPS_TYPE"
#endif

#pragma message (SELECTED_VALUE(MAVLINK_BAUD))


// Check RC Inputs
// UDB4
#pragma message (SELECTED_VALUE(USE_PPM_INPUT))
#pragma message (SELECTED_VALUE(NUM_INPUTS))

#if (USE_PPM_INPUT == 0 && NUM_INPUTS > 8)
	#error("NUM_INPUTS can't be more than 8 without using PPM Input.")
#elif (USE_PPM_INPUT != 0 && NUM_INPUTS > 9)
	#error("NUM_INPUTS can't be more than 9 when using PPM Input.")
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
// UDB4
#pragma message (SELECTED_VALUE(NUM_OUTPUTS))

#if (NUM_OUTPUTS > 10)
	#error("NUM_OUTPUTS can't be more than 10.")
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
#pragma message (SELECTED_VALUE(HILSIM))

#if (HILSIM == 1 && GPS_TYPE != GPS_UBX_4HZ)
	#error("When using HILSIM, GPS_TYPE must be set to GPS_UBX_4HZ.")
#endif


// UDB4
#pragma message (SELECTED_VALUE(NUM_ANALOG_INPUTS))
#if (NUM_ANALOG_INPUTS > 4)
	#error("Only 4 extra Analog Inputs are available the UDB4.")
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

#if (GPS_TYPE != GPS_STD && GPS_TYPE != GPS_UBX_2HZ && GPS_TYPE != GPS_UBX_4HZ && GPS_TYPE != GPS_MTEK)
	#error No valid GPS_TYPE specified.
#endif

// Check Magnetometer Options
#pragma message (SELECTED_VALUE(MAG_YAW_DRIFT))
#pragma message (SELECTED_VALUE(SERIAL_OUTPUT_FORMAT))

#if (MAG_YAW_DRIFT == 1)
#ifdef MAG_DIRECT
#if (BOARD_ORIENTATION != ORIENTATION_FORWARDS)
	#error("This board orientation is not yet supported with MAG_DIRECT mag option."
#endif
#endif
#else
#if ((SERIAL_OUTPUT_FORMAT == SERIAL_MAGNETOMETER))
	#error("SERIAL_MAGNETOMETER requires the use of MAG_YAW_DRIFT")
#endif
#endif

// Check flexifunction options
#pragma message (SELECTED_VALUE(USE_FLEXIFUNCTION_MIXING))

#if ((USE_FLEXIFUNCTION_MIXING == 1) && (USE_NV_MEMORY == 0))
	#error("Must use NV memory with flexifunction mixing on UDB4+ only")
#endif

#if ((USE_FLEXIFUNCTION_MIXING == 1) && (SERIAL_OUTPUT_FORMAT != SERIAL_MAVLINK))
	#error("Must use SERIAL_MAVLINK with flexifunction mixing on UDB4+ only")
#endif

// Check that I2C1 drivers are active when using NV memory drivers
#pragma message (SELECTED_VALUE(USE_NV_MEMORY))
#pragma message (SELECTED_VALUE(USE_I2C1_DRIVER))

#if ((USE_NV_MEMORY == 1) && (USE_I2C1_DRIVER == 0))
	#error("NV memory must use I2C1 driver with USE_I2C1_DRIVER = 1")
#endif

// Check that non volatile memory is being used with MAVlink
#if ((USE_NV_MEMORY == 1) && (SERIAL_OUTPUT_FORMAT != SERIAL_MAVLINK))
	#error("Non volatile memory services can only be used with SERIAL_MAVLINK")
#endif

// Check that declination variable is only used with the magnetometer
#if ((DECLINATIONANGLE_VARIABLE == 1) && (MAG_YAW_DRIFT != 1))
	#error("Can't use variable declination angle with no magnetometer. Set MAG_YAW_DRIFT = 1 or DECLINATIONANGLE_VARIABLE = 0")
#endif

#if ((NUM_OUTPUTS >= 9) && (BOARD_TYPE == AUAV3_BOARD))
	#error "max of 8 servo outputs currently supported for AUAV3"
#endif

#pragma message (SELECTED_VALUE(CONSOLE_UART))

#if ((CONSOLE_UART > 2) && (BOARD_TYPE != AUAV3_BOARD))
	#error("Console UART's greater than 2 only supported on AUAV3 board"
#endif

#pragma message (SELECTED_VALUE(USE_TELELOG))

#if ((USE_TELELOG == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("USE_TELELOG only supported on AUAV3 board"
#endif

#pragma message (SELECTED_VALUE(USE_CONFIGFILE))

#if ((USE_CONFIGFILE == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("USE_CONFIGFILE only supported on AUAV3 board"
#endif

#pragma message (SELECTED_VALUE(USE_USB))

#if ((USE_USB == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("USE_USB only supported on AUAV3 board"
#endif

#pragma message (SELECTED_VALUE(USE_MSD))

#if ((USE_MSD == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("USE_MSD only supported on AUAV3 board"
#endif

#pragma message (SELECTED_VALUE(HILSIM_USB))

#if ((HILSIM_USB == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("HILSIM_USB only supported on AUAV3 board"
#endif
