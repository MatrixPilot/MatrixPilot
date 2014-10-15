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
#include "navigate.h"
#include "behaviour.h"
#include "servoPrepare.h"
#include "altitudeCntrl.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/mathlibNAV.h"

#define ANGLE_90DEG (RMAX/(2*57.3)) // FIXME: never used
#define RTLKICK ((int32_t)(RTL_PITCH_DOWN*(RMAX/57.3)))
#define INVNPITCH ((int32_t)(INVERTED_NEUTRAL_PITCH*(RMAX/57.3)))
#define GRAVITYCMSECSEC ( 981 )
#define RADSTOGYRO ( ( uint16_t ) 48*SCALEGYRO ) // used in the conversion from radians per second to raw gyro units

#define MAX_INPUT ( 1000 ) // maximum input in pwm units

#define MINIMUM_AIRSPEED ( 500 ) // minimum value of airspeed in cm/sec to be used in tilt computation,
								 // mainly used for ground testing of turning tilt, which would go to zero at zero airspeed

int16_t tiltError[3] ;
int16_t desiredRotationRateRadians[3] ;
int16_t rotationRateError[3] ;
// helicalTurnCntrl determines the values of the elements of the bottom row of rmat
// as well as the required rotation rates in the body frame that are required to make a coordinated turn.
// The required values for the bottom row of rmat are placed in the vector desiredTilt.
// Desired tilt is computed from the helical turn parameters from desired climb rate, turn rate, and airspeed.
// The cross product of rmat[6,7,8] with the desiredTilt produces the orientation error.
// The desired rotation rate in the body frame is computed by multiplying desired turn rate times actual tilt vector.
// The rotation rate error is the actual rotation rate vector in the body frame minus the desired rotation rate vector.
// The tilt and rate vectors are then used by roll, pitch, and yaw control to deflect control surfaces.

void helicalTurnCntrl( void )
{
	union longww accum ;
	int16_t rollErrorVector[3] ;
	int16_t rtlkick ;
	int16_t desiredPitch ;
	int16_t steeringInput ;
	int16_t desiredTurnRateRadians ;
	int16_t desiredTiltVector[3] ;
	int16_t desiredRotationRateGyro[3] ;
	uint16_t airSpeed ;
	union longww desiredTilt ;
	int16_t desiredPitchVector[2] ;
	int16_t desiredPerpendicularPitchVector[2] ;
	int16_t actualPitchVector[2] ;
	int16_t pitchDot ;
	int16_t pitchCross ;
	int16_t pitchError ;
	int16_t pitchErrorBody[2] ;	
#ifdef TestGains
	flags._.GPS_steering = 0; // turn off navigation
	flags._.pitch_feedback = 1; // turn on stabilization
	airSpeed = 981 ; // for testing purposes, an airspeed is needed
#else
	airSpeed = air_speed_3DIMU ;
	if ( airSpeed < MINIMUM_AIRSPEED ) airSpeed = MINIMUM_AIRSPEED ;
#endif

	// determine the desired turn rate as the sum of navigation and fly by wire.
	// this allows the pilot to override navigation if needed.

	if (udb_flags._.radio_on == 1)
	{
		if ( AILERON_INPUT_CHANNEL != CHANNEL_UNUSED )  // compiler is smart about this
		{
			steeringInput = udb_pwIn[ AILERON_INPUT_CHANNEL ] - udb_pwTrim[ AILERON_INPUT_CHANNEL ] ;
			steeringInput = REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, steeringInput) ;
		}
		else if ( RUDDER_INPUT_CHANNEL != CHANNEL_UNUSED )
		{
			steeringInput = udb_pwIn[ RUDDER_INPUT_CHANNEL ] - udb_pwTrim[ RUDDER_INPUT_CHANNEL ] ;
			steeringInput = REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, steeringInput) ;
		}
		else
		{
			steeringInput = 0 ;
		}
	}
	else
	{
		steeringInput = 0 ;
	}

	if ( steeringInput > MAX_INPUT ) steeringInput = MAX_INPUT ;
	if ( steeringInput < - MAX_INPUT ) steeringInput = - MAX_INPUT ;

	accum.WW = __builtin_mulsu( steeringInput , turngainfbw ) / ( 2*MAX_INPUT) ;

	if ((AILERON_NAVIGATION||RUDDER_NAVIGATION) && flags._.GPS_steering)
	{
		accum.WW += (int32_t) determine_navigation_deflection('t');		
	}

	if ( accum.WW > (int32_t) 2* (int32_t ) RMAX - 1 ) accum.WW = (int32_t) 2* (int32_t ) RMAX - 1  ;
	if ( accum.WW <  - (int32_t) 2* (int32_t ) RMAX + 1 ) accum.WW = - (int32_t) 2* (int32_t ) RMAX + 1 ;

	desiredTurnRateRadians = accum._.W0 ;

	// compute the desired tilt from desired turn rate and air speed
	// range for acceleration is plus minus 4 times gravity
	// range for turning rate is plus minus 4 radians per second

	// desiredTilt is the ratio (-rmat[6]/rmat[8]), times RMAX/2 required for the turn
	// desiredTilt = desiredTurnRate * airSpeed / gravity
	// desiredTilt = RMAX/2*"real desired tilt"
	// desiredTurnRate = RMAX/2*"real desired turn rate", desired turn rate in radians per second
	// airSpeed is air speed centimeters per second
	// gravity is 981 centimeters per second per second 

	desiredTilt.WW = - __builtin_mulsu( desiredTurnRateRadians , airSpeed ) ;
	desiredTilt.WW /= GRAVITYCMSECSEC ;

	// limit the lateral acceleration to +- 4 times gravity, total wing loading approximately 4.12 times gravity

	if ( desiredTilt.WW > (int32_t) 2* (int32_t ) RMAX - 1)
	{
		desiredTilt.WW = (int32_t) 2* (int32_t ) RMAX - 1 ;
		accum.WW = __builtin_mulsu( desiredTilt._.W0 , GRAVITYCMSECSEC ) ;
		accum.WW /= airSpeed ;
		desiredTurnRateRadians = accum._.W0 ;
	}
	else if ( desiredTilt.WW < - (int32_t) 2* (int32_t ) RMAX + 1 )
	{
		desiredTilt.WW = - (int32_t) 2* (int32_t ) RMAX + 1 ;
		accum.WW = __builtin_mulsu( desiredTilt._.W0 , GRAVITYCMSECSEC ) ;
		accum.WW /= airSpeed ;
		desiredTurnRateRadians = accum._.W0 ;
	}	

	// convert desired turn rate from radians/second to gyro units

	accum.WW = ( ( ( int32_t ) desiredTurnRateRadians ) << 4 );  // desired turn rate in radians times 16 to provide resolution for the divide to follow
	accum.WW = accum.WW / RADSTOGYRO ; // at this point accum._.W0 has 2 times the required gyro signal for the turn.

	// compute desired rotation rate vector in body frame, scaling is same as gyro signal

	VectorScale( 3, desiredRotationRateGyro , &rmat[6] , accum._.W0 ) ; // this operation has side effect of dividing by 2

	// compute desired rotation rate vector in body frame, scaling is in RMAX/2*radians/sec

	VectorScale( 3, desiredRotationRateRadians , &rmat[6] , desiredTurnRateRadians ) ; // this produces half of what we want
	VectorAdd( 3 , desiredRotationRateRadians , desiredRotationRateRadians , desiredRotationRateRadians ) ; // double

	// incorporate roll into desired tilt vector

	desiredTiltVector[0] = desiredTilt._.W0 ;
	desiredTiltVector[1] =  0 ;
	desiredTiltVector[2] = RMAX/2 ; // the divide by 2 is to account for the RMAX/2 scaling in both tilt and rotation rate
	vector3_normalize( desiredTiltVector , desiredTiltVector ) ; // make sure tilt vector has magnitude RMAX

	// incorporate pitch into desired tilt vector
	// compute return to launch pitch down kick for unpowered RTL
	if (!udb_flags._.radio_on && flags._.GPS_steering)
	{
		rtlkick = RTLKICK;
	}
	else
	{
		rtlkick = 0;
	}

	//	Compute Matt's glider pitch adjustment
#if (GLIDE_AIRSPEED_CONTROL == 1)
	fractional aspd_pitch_adj = gliding_airspeed_pitch_adjust();
#endif

	//	Compute total desired pitch
#if (GLIDE_AIRSPEED_CONTROL == 1)
	desiredPitch =  - rtlkick + aspd_pitch_adj + pitchAltitudeAdjust ;
#else
	desiredPitch =  - rtlkick + pitchAltitudeAdjust  ;
#endif

	//	Adjustment for inverted flight
	if (!canStabilizeInverted() || !desired_behavior._.inverted )
	{
		// normal flight
		desiredTiltVector[1] =  - desiredPitch ;
	}
	else
	{
		desiredPitch += INVNPITCH ;
		// inverted flight, flip the desired tilt vector
		desiredTiltVector[0] = - desiredTiltVector[0] ;
		desiredTiltVector[1] = - desiredPitch  ;
		desiredTiltVector[2] = - desiredTiltVector[2] ;
	}

	vector3_normalize( desiredTiltVector , desiredTiltVector ) ; // make sure tilt vector has magnitude RMAX

	// compute roll error

	VectorCross( rollErrorVector , &rmat[6] , desiredTiltVector ) ; // compute tilt orientation error
	if ( VectorDotProduct( 3 , &rmat[6] , desiredTiltVector ) < 0 ) // more than 90 degree error
	{
		vector3_normalize( rollErrorVector , rollErrorVector ) ; // for more than 90 degrees, make the tilt error vector parallel to desired axis, with magnitude RMAX
	}
	
	tiltError[1] = rollErrorVector[1] ;

	// compute pitch error

	// start by computing the projection of earth frame pitch error to body frame

	pitchErrorBody[0] = rmat[6] ;
	pitchErrorBody[1] = rmat[8] ;

	// normalize the projection vector and compute the cosine of the actual pitch as a side effect 

	actualPitchVector[1] = ( int16_t ) vector2_normalize( pitchErrorBody , pitchErrorBody ) ;

	// complete the actual pitch vector

	actualPitchVector[0] = rmat[7] ;

	// compute the desired pitch vector

	desiredPitchVector[0] = - desiredPitch ;
	desiredPitchVector[1] = RMAX ;
	vector2_normalize ( desiredPitchVector , desiredPitchVector ) ;

	// rotate desired pitch vector by 90 degrees to be able to compute cross product using VectorDot

	desiredPerpendicularPitchVector[0] = desiredPitchVector[1] ;
	desiredPerpendicularPitchVector[1] = - desiredPitchVector[0] ;

	// compute pitchDot, the dot product of actual and desired pitch vector
	// (the 2* that appears in several of the following expressions is a result of the Q2.14 format)

	pitchDot = 2*VectorDotProduct( 2 , actualPitchVector , desiredPitchVector ) ;

	// compute pitchCross, the cross product of the actual and desired pitch vector

	pitchCross = 2*VectorDotProduct( 2 , actualPitchVector , desiredPerpendicularPitchVector ) ;

	if( pitchDot > 0 )
	{
		pitchError = pitchCross ;
	}
	else
	{
		if ( pitchCross > 0 )
		{
			pitchError = RMAX ;
		}
		else
		{
			pitchError = - RMAX ;
		}
	}

	// multiply the normalized rmat[6] , rmat[8] vector by the pitch error

	VectorScale( 2 , pitchErrorBody , pitchErrorBody , pitchError ) ;

	tiltError[0] = 2*pitchErrorBody[1] ;
	tiltError[2] = - 2*pitchErrorBody[0] ;

	// compute the rotation rate error vector

	VectorSubtract( 3 , rotationRateError , omegaAccum , desiredRotationRateGyro ) ;

}
