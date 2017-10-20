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
#include "states.h"
#include "config.h"
#include "navigate.h"
#include "behaviour.h"
#include "servoPrepare.h"
#include "airspeedCntrl.h"
#include "altitudeCntrl.h"
#include "helicalTurnCntrl.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/mathlibNAV.h"
#include "../libDCM/rmat.h"
#include <math.h>
#include <stdlib.h> // for declaration of function abs() under gcc
#include "options_airspeed.h" 

//#ifndef RTL_PITCH_DOWN
//#define RTL_PITCH_DOWN (0.0)
//#endif // RLT_PITCH_DOWN

//#ifndef ANGLE_OF_ATTACK_NORMAL
//#define ANGLE_OF_ATTACK_NORMAL (0.0)
//#endif // ANGLE_OF_ATTACK_NORMAL

//#ifndef ANGLE_OF_ATTACK_INVERTED
//#define ANGLE_OF_ATTACK_INVERTED (0.0)
//#endif // ANGLE_OF_ATTACK_INVERTED

//#ifndef ELEVATOR_TRIM_NORMAL
//#define ELEVATOR_TRIM_NORMAL (0.0)
//#endif // ELEVATOR_TRIM_NORMAL

//#ifndef ELEVATOR_TRIM_INVERTED
//#define ELEVATOR_TRIM_INVERTED (0.0)
//#endif // ELEVATOR_TRIM_INVERTED

//#ifndef CRUISE_SPEED
//#define CRUISE_SPEED (12.0)
//#endif // CRUISE_SPEED

#ifndef INVERTED_NEUTRAL_PITCH
#define INVERTED_NEUTRAL_PITCH (0.0)
#endif

#define RTLKICK            ((int32_t)(gains.RtlPitchDown*(RMAX/57.3)))
#define INVNPITCH          ((int32_t)(INVERTED_NEUTRAL_PITCH*(RMAX/57.3)))
#define AOA_NORMAL         ((int16_t)(turns.AngleOfAttackNormal*(RMAX/57.3)))
#define AOA_INVERTED       ((int16_t)(turns.AngleOfAttackInverted*(RMAX/57.3)))
#define ELEV_TRIM_NORMAL   ((int16_t)SERVORANGE*turns.ElevatorTrimNormal)
#define ELEV_TRIM_INVERTED ((int16_t)SERVORANGE*turns.ElevatorTrimInverted)
#define STALL_SPEED_CM_SEC ((uint16_t)turns.RefSpeed*50.0) // assume stall speed approximately 1/2 of reference speed

#define AOA_OFFSET           ((int16_t)((AOA_NORMAL + AOA_INVERTED)/2)) // offset is the average of the two values
#define AOA_SLOPE            ((int16_t)((AOA_NORMAL - AOA_INVERTED) * 4)) // multiply by 4 because base speed is 1/2 of cruise
#define ELEVATOR_TRIM_OFFSET ((int16_t)((ELEV_TRIM_NORMAL + ELEV_TRIM_INVERTED)/2)) // offset is the average of the two values
#define ELEVATOR_TRIM_SLOPE  ((int16_t)((ELEV_TRIM_NORMAL - ELEV_TRIM_INVERTED) * 4)) // multiply by 4 because base speed is 1/2 of cruise

#define GRAVITYCMSECSEC      (981)
#define RADSTOGYRO           ((uint16_t)48*SCALEGYRO) // used in the conversion from radians per second to raw gyro units

#define MAX_INPUT            (1000) // maximum input in pwm units

#define TURN_CALC_MINIMUM_AIRSPEED     (500) // minimum value of airspeed in cm/sec to be used in tilt computation,
                                   // mainly used for ground testing of turning tilt, which would go to zero at zero airspeed

int16_t tiltError[3];
int16_t desiredRotationRateRadians[3];
int16_t rotationRateError[3];
//int16_t angleOfAttack;

static int16_t estimatedLift;
static int16_t relativeLoading;

// Compute estimated wing lift based on orientation.
// This information can be determined directly from the accelerometers,
// but this creates an unstable feedback loop through the elevator.
// It is better to compute the lift as a feed forward term.
// It can be shown that wing lift divided by mass times gravity during a helical turn is
// equal to (Z + X * (X/Z)), where X, Y, and Z are the 3 elements
// of the bottom row of the direction cosine matrix, in real (floating point) values.
// Note: In principle, X/Z can be computed from X and Z, but since X, Z, and X/Z are already available
// in the helical turn control computations, it is more efficient to supply X/Z rather than recompute it.
// The computation of lift can use a mix of actual and desired values of X, Z and X/Z.
// The selection of the mix of actual and desired values for X, Z and X/Z is done when the routine is called.
// The following routine computes 1/16 of the ratio of wing loading divided by mass*gravity.
// The scale factor of 1/16 was selected to handle a fractional representation of a maximum lift/mg of more than 4.

static int16_t wingLift(int16_t X, int16_t Z, int16_t XoverZ)
{
	// compute (1/16*(Z + X *(X/Z)))*2^16
	union longww lift;
	union longww accum;

	// compute the first term. X is already scaled by 1/4, so apply another 1/4:

	accum._.W1 = Z;
	accum._.W0 = 0;
	accum.WW = accum.WW >> 2; // divide by 4
	lift.WW = accum.WW; // Z/16

	// compute the second term in result, X *(X/Z)
	// X has been divided by 4
	// X/Z has been divided by 8
	// so we need to multiply by 2
	accum.WW = __builtin_mulss(X, XoverZ);
	accum.WW += accum.WW; // multiply by 2

	// add the two terms
	lift.WW += accum.WW;

	return lift._.W1;
}

// Compute relative wing loading, 2**15*(Load/(mass*Gravity))*(V0/V)**2
// This number ranges from -2**15 to +2**15. Either extreme represents stall conditions.
// Typical values for relative wing loading under normal conditions are around 1/8 to 1/4 of the stall value.
// V0 is stall speed in centimeters per second during level flight, V is airspeed in centimeters per second.
// Load is wing loading in acceleration units, G is gravity.
// Relative wing loading is 2**15 when airspeed is equal to stall speed during level, unaccelerated flight
//
// Implement as 8*((2**16)*(a/(16g))*(V0/V)**2
//

static int16_t relativeWingLoading(int16_t wingLoad, uint16_t airSpeed)
{
	// wingLoad is(2**16)*((wing_load / mass*gravity) / 16)
	// stallSpeed is the stall speed in centimeters per second
	// airSpeed is the air speed in centimeters per second

	uint16_t stallSpeed = STALL_SPEED_CM_SEC;
	int16_t result = 0;
	uint32_t long_unsigned_accum;
	union longww long_signed_accum;
	uint16_t unsigned_accum;

	// if airspeed is less than or equal to stall speed, return zero
	if (airSpeed <= stallSpeed)
	{
		return 0;
	}

	long_unsigned_accum = (uint32_t)stallSpeed;
	long_unsigned_accum = long_unsigned_accum << 16; //(2**16)*V0, 32 bits unsigned
	unsigned_accum = __builtin_divud(long_unsigned_accum, airSpeed); //(2**16)*(V0/V), 16 bits unsigned
	long_unsigned_accum = __builtin_muluu(unsigned_accum, unsigned_accum); //(2**32)*(V0/V)**2, 32 bits unsigned
	unsigned_accum = long_unsigned_accum >> 16; //(2**16)*(V0/V)**2, 16 bits unsigned
	long_signed_accum.WW = __builtin_mulus(unsigned_accum, wingLoad); //(2**32)*(a/16g)*(V0/V)**2, 32 bits unsigned
	if (abs(long_signed_accum._.W1) < 4095)
	{
		long_signed_accum.WW = long_signed_accum.WW << 3; // multiply by 8
		result = long_signed_accum._.W1;
	}
	else
	{
		if (wingLoad > 0)
		{
			result = 32767;
		}
		else
		{
			result = -32767;
		}
	}	
	return result;
}


// helicalTurnCntrl determines the values of the elements of the bottom row of rmat
// as well as the required rotation rates in the body frame that are required to make a coordinated turn.
// The required values for the bottom row of rmat are placed in the vector desiredTilt.
// Desired tilt is computed from the helical turn parameters from desired climb rate, turn rate, and airspeed.
// The cross product of rmat[6,7,8] with the desiredTilt produces the orientation error.
// The desired rotation rate in the body frame is computed by multiplying desired turn rate times actual tilt vector.
// The rotation rate error is the actual rotation rate vector in the body frame minus the desired rotation rate vector.
// The tilt and rate vectors are then used by roll, pitch, and yaw control to deflect control surfaces.

void helicalTurnCntrl(void)
{
	union longww accum;
	int16_t pitchAdjustAngleOfAttack;
	int16_t rollErrorVector[3];
	int16_t rtlkick;
	int16_t desiredPitch;
	int16_t steeringInput;
	int16_t desiredTurnRateRadians;
	int16_t desiredTiltVector[3];
	int16_t desiredRotationRateGyro[3];
	uint16_t airSpeed;
	union longww desiredTilt;
	int16_t desiredPitchVector[2];
	int16_t desiredPerpendicularPitchVector[2];
	int16_t actualPitchVector[2];
	int16_t pitchDot;
	int16_t pitchCross;
	int16_t pitchError;
	int16_t pitchEarthBodyProjection[2];
	int16_t angleOfAttack;
#ifdef TestGains
	state_flags._.GPS_steering = 0;   // turn off navigation
	state_flags._.pitch_feedback = 1; // turn on stabilization
	airSpeed = 981; // for testing purposes, an airspeed is needed
#else
	airSpeed = air_speed_3DIMU;
	if (airSpeed < TURN_CALC_MINIMUM_AIRSPEED) airSpeed = TURN_CALC_MINIMUM_AIRSPEED;
#endif

	// determine the desired turn rate as the sum of navigation and fly by wire.
	// this allows the pilot to override navigation if needed.
	steeringInput = 0 ; // just in case no airframe type is specified or radio is off
	if (udb_flags._.radio_on == 1)
	{
#if ( (AIRFRAME_TYPE == AIRFRAME_STANDARD) || (AIRFRAME_TYPE == AIRFRAME_GLIDER) )
		if (AILERON_INPUT_CHANNEL != CHANNEL_UNUSED)  // compiler is smart about this
		{
			steeringInput = udb_pwIn[ AILERON_INPUT_CHANNEL ] - udb_pwTrim[ AILERON_INPUT_CHANNEL ];
			steeringInput = REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, steeringInput);
		}
		else if (RUDDER_INPUT_CHANNEL != CHANNEL_UNUSED)
		{
			steeringInput = udb_pwIn[ RUDDER_INPUT_CHANNEL ] - udb_pwTrim[ RUDDER_INPUT_CHANNEL ];
			steeringInput = REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, steeringInput);
		}
		else
		{
			steeringInput = 0;
		}
#endif // AIRFRAME_STANDARD

#if (AIRFRAME_TYPE == AIRFRAME_VTAIL)
		// use aileron channel if it is available, otherwise use rudder
		if (AILERON_INPUT_CHANNEL != CHANNEL_UNUSED)  // compiler is smart about this
		{
			steeringInput = udb_pwIn[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL];
			steeringInput = REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, steeringInput);
		}
		else if (RUDDER_INPUT_CHANNEL != CHANNEL_UNUSED)
		{
			// unmix the Vtail
			int16_t rudderInput  = REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, (udb_pwIn[ RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]));
			int16_t elevatorInput = REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, (udb_pwIn[ ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]));
			steeringInput = (-rudderInput + elevatorInput);
		}
		else
		{
			steeringInput = 0;
		}
#endif // AIRFRAME_VTAIL

#if (AIRFRAME_TYPE == AIRFRAME_DELTA)
		// delta wing must have an aileron input, so use that
		// unmix the elevons
		int16_t aileronInput  = REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, (udb_pwIn[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]));
		int16_t elevatorInput = REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, (udb_pwIn[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]));
		steeringInput = REVERSE_IF_NEEDED(ELEVON_VTAIL_SURFACES_REVERSED, ((elevatorInput - aileronInput)));
#endif // AIRFRAME_DELTA
	}

	if (steeringInput > MAX_INPUT) steeringInput = MAX_INPUT;
	if (steeringInput < - MAX_INPUT) steeringInput = - MAX_INPUT;

	// note that total steering is the sum of pilot input and waypoint navigation,
	// so that the pilot always has some say in the matter

	accum.WW = __builtin_mulsu(steeringInput, turngainfbw) /(2*MAX_INPUT);

	if ((settings._.AileronNavigation || settings._.RudderNavigation) && state_flags._.GPS_steering)
	{
		accum.WW +=(int32_t) navigate_determine_deflection('t');
	}

	if (accum.WW >(int32_t) 2*(int32_t) RMAX - 1) accum.WW =(int32_t) 2*(int32_t) RMAX - 1;
	if (accum.WW <  -(int32_t) 2*(int32_t) RMAX + 1) accum.WW = -(int32_t) 2*(int32_t) RMAX + 1;

	desiredTurnRateRadians = accum._.W0;

	// compute the desired tilt from desired turn rate and air speed
	// range for acceleration is plus minus 4 times gravity
	// range for turning rate is plus minus 4 radians per second

	// desiredTilt is the ratio(-rmat[6]/rmat[8]), times RMAX/2 required for the turn
	// desiredTilt = desiredTurnRate * airSpeed / gravity
	// desiredTilt = RMAX/2*"real desired tilt"
	// desiredTurnRate = RMAX/2*"real desired turn rate", desired turn rate in radians per second
	// airSpeed is air speed centimeters per second
	// gravity is 981 centimeters per second per second 

	desiredTilt.WW = - __builtin_mulsu(desiredTurnRateRadians, airSpeed);
	desiredTilt.WW /= GRAVITYCMSECSEC;

	// limit the lateral acceleration to +- 4 times gravity, total wing loading approximately 4.12 times gravity

	if (desiredTilt.WW > (int32_t)2 * (int32_t)RMAX - 1)
	{
		desiredTilt.WW = (int32_t)2 * (int32_t)RMAX - 1;
		accum.WW = __builtin_mulsu(-desiredTilt._.W0, GRAVITYCMSECSEC);
		accum.WW /= airSpeed;
		desiredTurnRateRadians = accum._.W0;
	}
	else if (desiredTilt.WW < -(int32_t)2 * (int32_t)RMAX + 1)
	{
		desiredTilt.WW = -(int32_t)2 * (int32_t)RMAX + 1;
		accum.WW = __builtin_mulsu(-desiredTilt._.W0, GRAVITYCMSECSEC);
		accum.WW /= airSpeed;
		desiredTurnRateRadians = accum._.W0;
	}

	// Compute the amount of lift needed to perform the desired turn
	// Tests show that the best estimate of lift is obtained using
	// actual values of rmat[6] and rmat[8], and the commanded value of their ratio
	estimatedLift = wingLift(rmat[6], rmat[8], desiredTilt._.W0);

	// compute angle of attack and elevator trim based on relative wing loading.
	// relative wing loading is the ratio of wing loading divided by the stall wing loading, as a function of air speed
	// both angle of attack and trim are computed by a linear approximation as a function of relative loading:
	// y = (2m)*(x/2) + b, y is either angle of attack or elevator trim.
	// x is relative wing loading. (x/2 is computed instead of x)
	// 2m and b are determined from values of angle of attack and trim at stall speed, normal and inverted.
	// b =  (y_normal + y_inverted) / 2.
	// 2m = (y_normal - y_inverted).

	// If airspeed is greater than stall speed, compute angle of attack and elevator trim,
	// otherwise set AoA and trim to zero.

	if (air_speed_3DIMU > STALL_SPEED_CM_SEC)
	{
		// compute "x/2", the relative wing loading
		relativeLoading = relativeWingLoading(estimatedLift, air_speed_3DIMU);

		// multiply x/2 by 2m for angle of attack
		accum.WW = __builtin_mulss(AOA_SLOPE, relativeLoading);
		// add mx to b
		angleOfAttack = AOA_OFFSET + accum._.W1;

		// project angle of attack into the earth frame
		accum.WW =(__builtin_mulss(angleOfAttack, rmat[8])) << 2;
		pitchAdjustAngleOfAttack = accum._.W1;

		// similarly, compute elevator trim
		accum.WW = __builtin_mulss(ELEVATOR_TRIM_SLOPE, relativeLoading);
		elevatorLoadingTrim = ELEVATOR_TRIM_OFFSET + accum._.W1;
	}
	else
	{
		angleOfAttack = 0;
		pitchAdjustAngleOfAttack = 0;
		elevatorLoadingTrim = 0;
	}
//	SetAofA(angleOfAttack); // removed by helicalTurns

	// convert desired turn rate from radians/second to gyro units

	accum.WW = (((int32_t)desiredTurnRateRadians) << 4);  // desired turn rate in radians times 16 to provide resolution for the divide to follow
	accum.WW = accum.WW / RADSTOGYRO; // at this point accum._.W0 has 2 times the required gyro signal for the turn.

	// compute desired rotation rate vector in body frame, scaling is same as gyro signal

	VectorScale(3, desiredRotationRateGyro, &rmat[6], accum._.W0); // this operation has side effect of dividing by 2

	// compute desired rotation rate vector in body frame, scaling is in RMAX/2*radians/sec

	VectorScale(3, desiredRotationRateRadians, &rmat[6], desiredTurnRateRadians); // this produces half of what we want
	VectorAdd(3, desiredRotationRateRadians, desiredRotationRateRadians, desiredRotationRateRadians); // double

	// incorporate roll into desired tilt vector

	desiredTiltVector[0] = desiredTilt._.W0;
	desiredTiltVector[1] =  0;
	desiredTiltVector[2] = RMAX/2; // the divide by 2 is to account for the RMAX/2 scaling in both tilt and rotation rate
	vector3_normalize(desiredTiltVector, desiredTiltVector); // make sure tilt vector has magnitude RMAX

	// incorporate pitch into desired tilt vector
	// compute return to launch pitch down kick for unpowered RTL
	if (!udb_flags._.radio_on && state_flags._.GPS_steering)
	{
		rtlkick = RTLKICK;
	}
	else
	{
		rtlkick = 0;
	}

	// Compute Matt's glider pitch adjustment
#if (GLIDE_AIRSPEED_CONTROL == 1)
	fractional aspd_pitch_adj = gliding_airspeed_pitch_adjust();
#endif

	// Compute total desired pitch
#if (GLIDE_AIRSPEED_CONTROL == 1)
	desiredPitch = - rtlkick + aspd_pitch_adj + pitchAltitudeAdjust;
#else
	desiredPitch = - rtlkick + pitchAltitudeAdjust;
#endif

	// Adjustment for inverted flight
	if (!canStabilizeInverted() || !desired_behavior._.inverted)
	{
		// normal flight
		desiredTiltVector[1] =  - desiredPitch - pitchAdjustAngleOfAttack;
	}
	else
	{
		// inverted flight
		desiredTiltVector[0] = - desiredTiltVector[0];
		desiredTiltVector[1] = - desiredPitch - pitchAdjustAngleOfAttack - INVNPITCH; // only one of the adjustments is not zero
		desiredTiltVector[2] = - desiredTiltVector[2];
	}

	vector3_normalize(desiredTiltVector, desiredTiltVector); // make sure tilt vector has magnitude RMAX

	// compute roll error

	VectorCross(rollErrorVector, &rmat[6], desiredTiltVector); // compute tilt orientation error
	if (VectorDotProduct(3, &rmat[6], desiredTiltVector) < 0) // more than 90 degree error
	{
		vector3_normalize(rollErrorVector, rollErrorVector); // for more than 90 degrees, make the tilt error vector parallel to desired axis, with magnitude RMAX
	}
	
	tiltError[1] = rollErrorVector[1];

	// compute pitch error

	// start by computing the projection of earth frame pitch error to body frame

	pitchEarthBodyProjection[0] = rmat[6];
	pitchEarthBodyProjection[1] = rmat[8];

	// normalize the projection vector and compute the cosine of the actual pitch as a side effect 

	actualPitchVector[1] =(int16_t) vector2_normalize(pitchEarthBodyProjection, pitchEarthBodyProjection);

	// complete the actual pitch vector

	actualPitchVector[0] = rmat[7];

	// compute the desired pitch vector

	desiredPitchVector[0] = - desiredPitch;
	desiredPitchVector[1] = RMAX;
	vector2_normalize(desiredPitchVector, desiredPitchVector);

	// rotate desired pitch vector by 90 degrees to be able to compute cross product using VectorDot

	desiredPerpendicularPitchVector[0] = desiredPitchVector[1];
	desiredPerpendicularPitchVector[1] = - desiredPitchVector[0];

	// compute pitchDot, the dot product of actual and desired pitch vector
	// (the 2* that appears in several of the following expressions is a result of the Q2.14 format)

	pitchDot = 2*VectorDotProduct(2, actualPitchVector, desiredPitchVector);

	// compute pitchCross, the cross product of the actual and desired pitch vector

	pitchCross = 2*VectorDotProduct(2, actualPitchVector, desiredPerpendicularPitchVector);

	if (pitchDot > 0)
	{
		pitchError = pitchCross;
	}
	else
	{
		if (pitchCross > 0)
		{
			pitchError = RMAX;
		}
		else
		{
			pitchError = - RMAX;
		}
	}

	// multiply the normalized rmat[6], rmat[8] vector by the pitch error
	VectorScale(2, pitchEarthBodyProjection, pitchEarthBodyProjection, pitchError);
	tiltError[0] =   2*pitchEarthBodyProjection[1];
	tiltError[2] = - 2*pitchEarthBodyProjection[0];

	// compute the rotation rate error vector
	VectorSubtract(3, rotationRateError, omegaAccum, desiredRotationRateGyro);
}
