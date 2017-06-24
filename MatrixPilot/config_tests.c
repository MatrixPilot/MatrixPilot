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
#include "behaviour.h"
#include "options_ports.h"


// This file should generate no code.
// It's just meant for precompiler tests, to catch problems and inconsistencies
// with the user's configuration settings.


// Check RC Inputs
// UDB4
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


// Check Castle Link Live Settings
#if (USE_CASTLE_LINK_THROTTLE == 1 && THROTTLE_OUTPUT_CHANNEL != 1)
	#error("THROTTLE_OUTPUT_CHANNEL has to be 1 when using USE_CASTLE_LINK_THROTTLE.")
#endif

#if (USE_CASTLE_LINK_THROTTLE == 1 && NUM_INPUTS >= 8 && USE_PPM_INPUT == 0)
	#error("Please use fewer than 8 Inputs, or use PPM, when using USE_CASTLE_LINK_THROTTLE.")
#endif

#if (USE_CASTLE_LINK_THROTTLE == 1 && PPM_IC == 8)
	#error("Please choose a different PPM_IC (not 8) when using USE_CASTLE_LINK_THROTTLE.")
#endif


// Check HILSIM Settings
#if (HILSIM == 1 && GPS_TYPE != GPS_UBX_4HZ)
	#error("When using HILSIM, GPS_TYPE must be set to GPS_UBX_4HZ.")
#endif


// UDB4
#if ((NUM_ANALOG_INPUTS > 4) && (BOARD_TYPE != AUAV3_BOARD))
	#error("Only 4 extra Analog Inputs are available the UDB.")
#endif

// Check Analog Inputs
#if (ANALOG_CURRENT_INPUT_CHANNEL > NUM_ANALOG_INPUTS)
//	#error("ANALOG_CURRENT_INPUT_CHANNEL > NUM_ANALOG_INPUTS.")
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL > NUM_ANALOG_INPUTS)
//	#error("ANALOG_VOLTAGE_INPUT_CHANNEL > NUM_ANALOG_INPUTS.")
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL > NUM_ANALOG_INPUTS)
//	#error("ANALOG_RSSI_INPUT_CHANNEL > NUM_ANALOG_INPUTS.")
#endif

#if (GPS_TYPE != GPS_STD && GPS_TYPE != GPS_UBX_2HZ && \
     GPS_TYPE != GPS_UBX_4HZ && GPS_TYPE != GPS_MTEK && \
     GPS_TYPE != GPS_NMEA && GPS_TYPE != GPS_NONE && \
     GPS_TYPE != GPS_ALL)
	#error No valid GPS_TYPE specified.
#endif

// Check Magnetometer Options
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
#if ((USE_FLEXIFUNCTION_MIXING == 1) && (USE_NV_MEMORY == 0))
	#error("Must use NV memory with flexifunction mixing on UDB4+ only")
#endif

#if ((USE_FLEXIFUNCTION_MIXING == 1) && (SERIAL_OUTPUT_FORMAT != SERIAL_MAVLINK))
	#error("Must use SERIAL_MAVLINK with flexifunction mixing on UDB4+ only")
#endif

// Check that I2C1 drivers are active when using NV memory drivers
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

#if ((CONSOLE_UART > 2) && (BOARD_TYPE != AUAV3_BOARD) && (SILSIM == 0))
//	#error("Console UART's greater than 2 only supported on AUAV3 board"
#endif

#if ((USE_TELELOG == 1) && (USE_FILESYS == 0))
	#error("USE_TELELOG requires USE_FILESYS"
#endif

#if ((USE_USB == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("USE_USB only supported on AUAV3 board"
#endif

#if ((USE_MSD == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("USE_MSD only supported on AUAV3 board"
#endif

#if ((HILSIM_USB == 1) && (BOARD_TYPE != AUAV3_BOARD))
	#error("HILSIM_USB only supported on AUAV3 board"
#endif

#ifdef INVERTED_NEUTRAL_PITCH
#ifdef ANGLE_OF_ATTACK_INVERTED
#error ( "Both INVERTED_NEUTRAL_PITCH and ANGLE_OF_ATTACK_INVERTED are being used. Use only one or the other."
#endif // ANGLE_OF_ATTACK_INVERTED
#endif // INVERTED_NEUTRAL_PITCH

#if ( AIRFRAME_TYPE == AIRFRAME_HELI )
	#error("Helical controls does not support AIRFRAME_HELI."
#endif // AIRFRAME_HELI

#if ( AIRFRAME_TYPE == AIRFRAME_GLIDER && ( GAINS_VARIABLE == 0 || ALTITUDE_GAINS_VARIABLE == 0 ) )
	#warning("GAINS_VARIABLE and ALTITUDE_GAINS_VARIABLE in gain_variables.h recommended for AIRFRAME_GLIDER"
#endif // AIRFRAME_GLIDER
