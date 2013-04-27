#include "../libUDB/libUDB.h"

#ifndef GAIN_VARIABLES_H
#define GAIN_VARIABLES_H


#define GAINS_VARIABLE			1

// Variable altitude and airspeed
// BEWARE: This uses an alternative library for altitude control
// Your aircraft will not behave the same way as when using non variable gains. 
#define ALTITUDE_GAINS_VARIABLE 0

// Aileron/Roll Control Gains
// ROLLKP is the proportional gain, approximately 0.25
// ROLLKD is the derivative (gyro) gain, approximately 0.125
// YAWKP_AILERON is the proportional feedback gain for ailerons in response to yaw error
// YAWKD_AILERON is the derivative feedback gain for ailerons in response to yaw rotation
// AILERON_BOOST is the additional gain multiplier for the manually commanded aileron deflection

#if ((SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || ( GAINS_VARIABLE == 1 ))
   // MAVLINK, QGROUND CONTROL (Ground Control Station) can change these variables 
	extern uint16_t rollkp;
	extern uint16_t rollkd;
	extern uint16_t yawkpail;
	extern uint16_t yawkdail;
#else
	extern const uint16_t rollkp;
	extern const uint16_t rollkd;
	extern const uint16_t yawkpail;
	extern const uint16_t yawkdail;
#endif
//#define AILERON_BOOST						1.0

// Elevator/Pitch Control Gains
// PITCHGAIN is the pitch stabilization gain, typically around 0.125
// PITCHKD feedback gain for pitch damping, around 0.0625
// RUDDER_ELEV_MIX is the degree of elevator adjustment for rudder and banking
// AILERON_ELEV_MIX is the degree of elevator adjustment for aileron
// ELEVATOR_BOOST is the additional gain multiplier for the manually commanded elevator deflection
#if ((SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || ( GAINS_VARIABLE == 1 ))
	extern uint16_t pitchgain;
	extern uint16_t pitchkd;
	extern uint16_t rudderElevMixGain;
	extern uint16_t rollElevMixGain;
#else
	extern const uint16_t pitchgain;
	extern const uint16_t pitchkd;
	extern const uint16_t rudderElevMixGain;
	extern const uint16_t rollElevMixGain;
#endif
//#define ELEVATOR_BOOST						0.5

// Neutral pitch angle of the plane (in degrees) when flying inverted
// Use this to add extra "up" elevator while the plane is inverted, to avoid losing altitude.
//#define INVERTED_NEUTRAL_PITCH				8.0

// Rudder/Yaw Control Gains
// YAWKP_RUDDER is the proportional feedback gain for rudder navigation
// YAWKD_RUDDER is the yaw gyro feedback gain for the rudder in reponse to yaw rotation
// ROLLKP_RUDDER is the feedback gain for the rudder in response to the current roll angle
// ROLLKD_RUDDER is the feedback gain for the rudder in response to the rate of roll
// MANUAL_AILERON_RUDDER_MIX is the fraction of manual aileron control to mix into the rudder when
// in stabilized or waypoint mode.  This mainly helps aileron-initiated turning while in stabilized.
// RUDDER_BOOST is the additional gain multiplier for the manually commanded rudder deflection
#if ((SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || ( GAINS_VARIABLE == 1 ))
	extern uint16_t yawkprud;
	extern uint16_t yawkdrud;
	extern uint16_t rollkprud;
	extern uint16_t rollkdrud;
#else
	extern const uint16_t yawkprud;
	extern const uint16_t yawkdrud;
	extern const uint16_t rollkprud;
	extern const uint16_t rollkdrud;
#endif
//extern uint16_t MANUAL_AILERON_RUDDER_MIX			0.20
//#define RUDDER_BOOST						1.0

// Gains for Hovering
// Gains are named based on plane's frame of reference (roll means ailerons)
// HOVER_ROLLKP is the roll-proportional feedback gain applied to the ailerons while navigating a hover
// HOVER_ROLLKD is the roll gyro feedback gain applied to ailerons while stabilizing a hover
// HOVER_PITCHGAIN is the pitch-proportional feedback gain applied to the elevator while stabilizing a hover
// HOVER_PITCHKD is the pitch gyro feedback gain applied to elevator while stabilizing a hover
// HOVER_PITCH_OFFSET is the neutral pitch angle for the plane (in degrees) while stabilizing a hover
// HOVER_YAWKP is the yaw-proportional feedback gain applied to the rudder while stabilizing a hover
// HOVER_YAWKD is the yaw gyro feedback gain applied to rudder while stabilizing a hover
// HOVER_YAW_OFFSET is the neutral yaw angle for the plane (in degrees) while stabilizing a hover
// HOVER_PITCH_TOWARDS_WP is the max angle in degrees to pitch the nose down towards the WP while navigating
// HOVER_NAV_MAX_PITCH_RADIUS is the radius around a waypoint in meters, within which the HOVER_PITCH_TOWARDS_WP
//                            value is proportionally scaled down.

#if ((SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || ( GAINS_VARIABLE == 1 ))
	extern uint16_t hoverrollkp;
	extern uint16_t hoverrollkd;
	extern uint16_t hoverpitchgain;
	extern uint16_t hoverpitchkd;
	extern uint16_t hoveryawkp;
	extern uint16_t hoveryawkd;
#else
	extern const uint16_t hoverrollkp;
	extern const uint16_t hoverrollkd;
	extern const uint16_t hoverpitchgain;
	extern const uint16_t hoverpitchkd;
	extern const uint16_t hoveryawkp;
	extern const uint16_t hoveryawkd;
#endif

//#define HOVER_PITCH_OFFSET					0.0		// + leans towards top, - leans towards bottom
//#define HOVER_YAW_OFFSET					0.0
//#define HOVER_PITCH_TOWARDS_WP			   30.0
//#define HOVER_NAV_MAX_PITCH_RADIUS		   20


/*
// servo throw can be more than 3 turns - 1080 degrees - so use integers rather than char
const int16_t tan_pitch_in_stabilized_mode;
const int16_t yaw_in_stabilized_mode;

const int16_t pitch_offset_centred;
const int16_t yaw_offset_centred;

const int16_t pitch_servo_max;
const int16_t pitch_servo_min;
const int16_t yaw_servo_max;
const int16_t yaw_servo_min;

// servo_ratios are used to convert degrees of rotation into servo pulse code lengths
// This code is configured for the full throw of the servo to be achieved by a range of
// 2000 units being sent to udb_pwOut. (i.e. min 2000, centered 3000, max 4000 )
const int16_t pitch_servo_ratio;
const int16_t yaw_servo_ratio;

*/

#endif 	//GAIN_VARIABLES_H
