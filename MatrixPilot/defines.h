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

#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "../libDCM/libDCM.h"


#define BYTECIR_TO_DEGREE 92160 // (360.0/256 * 2^16)


////////////////////////////////////////////////////////////////////////////////
// states.c
void init_states(void);

extern int16_t waggle;

// these all moved to states.c as they are purely local defines
//#define CALIB_PAUSE 21        // wait for 10.5 seconds of runs through the state machine
//#define STANDBY_PAUSE 48      // pause for 24 seconds of runs through the state machine
//#define NUM_WAGGLES 4         // waggle 4 times during the end of the standby pause (this number must be less than STANDBY_PAUSE)
//#define WAGGLE_SIZE 300

struct flag_bits {
	uint16_t unused                     : 4;
	uint16_t save_origin                : 1;
	uint16_t GPS_steering               : 1;
	uint16_t pitch_feedback             : 1;
	uint16_t altitude_hold_throttle     : 1;
	uint16_t altitude_hold_pitch        : 1;
	uint16_t man_req                    : 1;
	uint16_t auto_req                   : 1;
	uint16_t home_req                   : 1;
	uint16_t rtl_hold                   : 1;
	uint16_t f13_print_req              : 1;
	uint16_t disable_throttle           : 1;
	uint16_t update_autopilot_state_asap: 1;
};

union fbts_int { struct flag_bits _; int16_t WW; };
extern union fbts_int flags;


////////////////////////////////////////////////////////////////////////////////
// Control code - helicalTurnCntrl.c,rollCntrl.c, pitchCntrl.c, yawCntrl.c, altitudeCntrl.c
void helicalTurnCntrl(void);
void rollCntrl(void);
void pitchCntrl(void);
void yawCntrl(void);
void altitudeCntrl(void);
void setTargetAltitude(int16_t targetAlt);

void init_yawCntrl(void);
void init_rollCntrl(void);
void init_pitchCntrl(void);
void init_altitudeCntrl(void);
void init_altitudeCntrlVariable(void);

void save_yawCntrl(void);
void save_rollCntrl(void);
void save_pitchCntrl(void);
void save_altitudeCntrl(void);
void save_altitudeCntrlVariable(void);


void calculate_sonar_height_above_ground(void);


// AltitudeHold type
#define AH_NONE             0
#define AH_PITCH_ONLY       1
#define AH_FULL             3



////////////////////////////////////////////////////////////////////////////////
// servoMix.c
void servoMix(void);
void cameraServoMix(void);

// Choose the type of air frame by setting AIRFRAME_TYPE in options.h
// See options.h for a description of each type
//#define AIRFRAME_STANDARD   0
//#define AIRFRAME_VTAIL      1
//#define AIRFRAME_DELTA      2
//#define AIRFRAME_HELI       3    // Untested
//#define AIRFRAME_QUAD       4    // Under development

#define AIRFRAME_NONE       0
#define AIRFRAME_STANDARD   1
#define AIRFRAME_VTAIL      2
#define AIRFRAME_DELTA      3
#define AIRFRAME_HELI       4    // Untested
#define AIRFRAME_QUAD       5    // Under development

// Negate VALUE if NEEDS_REVERSING is true
#define REVERSE_IF_NEEDED(NEEDS_REVERSING, VALUE) ((NEEDS_REVERSING) ? (-(VALUE)) : (VALUE))

extern int16_t cam_pitch_servo_pwm_delta;  
extern int16_t cam_yaw_servo_pwm_delta;
int32_t cam_pitchServoLimit(int32_t pwm_pulse);
int32_t cam_yawServoLimit(int32_t pwm_pulse);


////////////////////////////////////////////////////////////////////////////////
// Failsafe Type
#define FAILSAFE_RTL                1
#define FAILSAFE_MAIN_FLIGHTPLAN    2

#define FP_WAYPOINTS                1
#define FP_LOGO                     2


////////////////////////////////////////////////////////////////////////////////
// serialIO.c
void init_serial(void);
void serial_output(char* format, ...);
void serial_output_8hz(void);
void mavlink_output_40hz(void);

// Serial Output Format
#define SERIAL_NONE         0    // No serial data is sent
#define SERIAL_DEBUG        1    // UAV Dev Board debug info
#define SERIAL_ARDUSTATION  2    // Compatible with ArduStation
#define SERIAL_UDB          3    // Pete's efficient UAV Dev Board format
#define SERIAL_OSD_REMZIBI  4    // Output data formatted to use as input to a Remzibi OSD (only works with GPS_UBX)
#define SERIAL_OSD_IF       5    // Output data formatted to use as input to a IF OSD (only works with GPS_UBX)
#define SERIAL_MAGNETOMETER 6    // Debugging the magnetometer
#define SERIAL_UDB_EXTRA    7    // Extra Telemetry beyond that provided by SERIAL_UDB for higher bandwidth connections
#define SERIAL_CAM_TRACK    8    // Output Location in a format usable by a 2nd UDB to target its camera at this plane
#define SERIAL_MAVLINK      9    // The Micro Air Vehicle Link protocol from the PixHawk Project


////////////////////////////////////////////////////////////////////////////////
// mp_osd.c
void osd_run_step(void);


#include "gain_variables.h"

// GNU compiler specific macros for specifically marking variables as unused
// If not using GNU, then macro makes no alteration to the code
#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

#endif // _DEFINES_H_
