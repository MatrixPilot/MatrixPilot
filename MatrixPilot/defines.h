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


#define BYTECIR_TO_DEGREE 92160		// (360.0/256 * 2^16)


////////////////////////////////////////////////////////////////////////////////
// states.c
void init_states( void ) ;

extern int16_t waggle ;

// these all moved to states.c as they are purely local defines
//#define CALIB_PAUSE 21		// wait for 10.5 seconds of runs through the state machine
//#define STANDBY_PAUSE 48	// pause for 24 seconds of runs through the state machine
//#define NUM_WAGGLES 4		// waggle 4 times during the end of the standby pause (this number must be less than STANDBY_PAUSE)
//#define WAGGLE_SIZE 300

struct flag_bits {
			uint16_t unused						: 5 ;
			uint16_t save_origin   				: 1 ;
			uint16_t GPS_steering				: 1 ;
			uint16_t pitch_feedback				: 1 ;
			uint16_t altitude_hold_throttle		: 1 ;
			uint16_t altitude_hold_pitch		: 1 ;
			uint16_t man_req					: 1 ;
			uint16_t auto_req					: 1 ;
			uint16_t home_req					: 1 ;
			uint16_t rtl_hold					: 1 ;
			uint16_t f13_print_req				: 1 ;
			uint16_t update_autopilot_state_asap: 1 ;
			} ;
			
union fbts_int { struct flag_bits _ ; int16_t WW ; } ;
extern union fbts_int flags ;



////////////////////////////////////////////////////////////////////////////////
// servoPrepare.c
void init_servoPrepare( void ) ;


////////////////////////////////////////////////////////////////////////////////
// Control code - rollCntrl.c, pitchCntrl.c, yawCntrl.c, altitudeCntrl.c
void rollCntrl( void ) ;
void pitchCntrl( void ) ;
void yawCntrl( void ) ;
void altitudeCntrl( void ) ;
void setTargetAltitude(int16_t targetAlt) ;

void init_yawCntrl(void);void init_rollCntrl(void);void init_pitchCntrl(void);


// wind gain adjustment
uint16_t wind_gain_adjustment(void) ;
extern uint16_t wind_gain ;

extern int16_t pitch_control, roll_control, yaw_control, throttle_control ;
extern union longww throttleFiltered ;
extern int16_t pitchAltitudeAdjust ;

#if ( SPEED_CONTROL == 1)
extern int16_t desiredSpeed ; // Stored in 10ths of meters per second
#endif

// AltitudeHold type
#define AH_NONE				0
#define AH_PITCH_ONLY		1
#define AH_FULL				3



////////////////////////////////////////////////////////////////////////////////
// servoMix.c
void servoMix( void ) ;
void cameraServoMix( void ) ;

// Choose the type of air frame by setting AIRFRAME_TYPE in options.h
// See options.h for a description of each type
#define AIRFRAME_STANDARD			0
#define AIRFRAME_VTAIL				1
#define AIRFRAME_DELTA				2
#define AIRFRAME_HELI				3		// Untested

// Negate VALUE if NEEDS_REVERSING is true
#define REVERSE_IF_NEEDED(NEEDS_REVERSING, VALUE)		((NEEDS_REVERSING) ? (-(VALUE)) : (VALUE))

extern int16_t cam_pitch_servo_pwm_delta ;  
extern int16_t cam_yaw_servo_pwm_delta ;
int32_t cam_pitchServoLimit( int32_t pwm_pulse) ;
int32_t cam_yawServoLimit( int32_t pwm_pulse) ;



////////////////////////////////////////////////////////////////////////////////
// navigation.c
void set_goal( struct relative3D fromPoint , struct relative3D toPoint ) ;
void update_goal_alt( int16_t z ) ;
void compute_bearing_to_goal ( void ) ;
void process_flightplan( void ) ;
int16_t determine_navigation_deflection( char navType ) ;

struct waypointparameters { int16_t x ; int16_t y ; int16_t cosphi ; int16_t sinphi ; int8_t phi ; int16_t height ; int16_t fromHeight; int16_t legDist; } ;
extern struct waypointparameters goal ;

extern struct relative2D togoal ;
extern int16_t tofinish_line ;
extern int16_t progress_to_goal ; // Fraction of the way to the goal in the range 0-4096 (2^12)
extern int8_t	desired_dir ;



////////////////////////////////////////////////////////////////////////////////
// Flight Planning modules - flightplan-waypoints.c and flightplan-logo.c
void init_flightplan( int16_t flightplanNum ) ;
boolean use_fixed_origin( void ) ;
struct absolute3D get_fixed_origin( void ) ;
int32_t get_fixed_altitude( void ) ;
void run_flightplan( void ) ;

void flightplan_live_begin( void ) ;
void flightplan_live_received_byte( uint8_t inbyte ) ;
void flightplan_live_commit( void ) ;

// Failsafe Type
#define FAILSAFE_RTL					1
#define FAILSAFE_MAIN_FLIGHTPLAN		2

#define FP_WAYPOINTS					1
#define FP_LOGO							2



////////////////////////////////////////////////////////////////////////////////
// behavior.c
void init_behavior( void ) ;
void setBehavior( int16_t newBehavior ) ;
void updateBehavior( void ) ;
void updateTriggerAction( void ) ;
boolean canStabilizeInverted( void ) ;
boolean canStabilizeHover( void ) ;

struct behavior_flag_bits {
			uint16_t takeoff		: 1 ;	// disable altitude interpolation for faster climbout
			uint16_t inverted		: 1 ;	// fly iverted
			uint16_t hover			: 1 ;	// hover the plane
			uint16_t rollLeft		: 1 ;				// unimplemented
			uint16_t rollRight		: 1 ;				// unimplemented
			uint16_t trigger		: 1 ;	// trigger action
			uint16_t loiter			: 1 ;	// stay on the current waypoint
			uint16_t land			: 1 ;	// throttle off
			uint16_t absolute		: 1 ;	// absolute waypoint
			uint16_t altitude		: 1 ;	// climb/descend to goal altitude
			uint16_t cross_track	: 1 ;	// use cross-tracking navigation
			uint16_t unused			: 5 ;
			} ;

#define F_NORMAL						   0
#define F_TAKEOFF						   1
#define F_INVERTED						   2
#define F_HOVER							   4
#define F_ROLL_LEFT						   8
#define F_ROLL_RIGHT					  16
#define F_TRIGGER						  32
#define F_LOITER						  64
#define F_LAND							 128
#define F_ABSOLUTE						 256
#define F_ALTITUDE_GOAL					 512
#define F_CROSS_TRACK					1024

union bfbts_word { struct behavior_flag_bits _ ; int16_t W; } ;

extern int16_t current_orientation ;
extern union bfbts_word desired_behavior ;

#define TRIGGER_TYPE_NONE				 0
#define TRIGGER_TYPE_SERVO				 1
#define TRIGGER_TYPE_DIGITAL			 2

#define TRIGGER_PULSE_HIGH				 4
#define TRIGGER_PULSE_LOW				 8
#define TRIGGER_TOGGLE					16
#define TRIGGER_REPEATING				32



////////////////////////////////////////////////////////////////////////////////
// serialIO.c
void init_serial( void ) ;
void serial_output( char* format, ... ) ;
void serial_output_8hz( void ) ;
void mavlink_output_40hz( void ) ;

// Serial Output Format
#define SERIAL_NONE			0	// No serial data is sent
#define SERIAL_DEBUG		1	// UAV Dev Board debug info
#define SERIAL_ARDUSTATION	2	// Compatible with ArduStation
#define SERIAL_UDB			3	// Pete's efficient UAV Dev Board format
#define SERIAL_OSD_REMZIBI	4	// Output data formatted to use as input to a Remzibi OSD (only works with GPS_UBX)
#define SERIAL_OSD_IF		5	// Output data formatted to use as input to a IF OSD (only works with GPS_UBX)
#define SERIAL_MAGNETOMETER	6	// Debugging the magnetometer
#define SERIAL_UDB_EXTRA	7	// Extra Telemetry beyond that provided by SERIAL_UDB for higher bandwidth connections
#define SERIAL_CAM_TRACK	8	// Output Location in a format usable by a 2nd UDB to target its camera at this plane
#define SERIAL_MAVLINK		9	// The Micro Air Vehicle Link protocol from the PixHawk Project


////////////////////////////////////////////////////////////////////////////////
// cameraCntrl.c
void set_camera_view( struct relative3D current_view ) ;
void compute_camera_view( void ) ;
void cameraCntrl( void ) ;

void camera_live_begin( void ) ;
void camera_live_received_byte( uint8_t inbyte ) ;
void camera_live_commit( void ) ;
void camera_live_commit_values(const struct relative3D target);

#define CAM_VIEW_LAUNCH					{ 0, 0, 0 }



////////////////////////////////////////////////////////////////////////////////
// mp_osd.c
void osd_run_step( void );

#define OSD_NTSC			0
#define OSD_PAL				1

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
