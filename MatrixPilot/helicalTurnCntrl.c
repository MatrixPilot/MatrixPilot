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
#include "../libDCM/mathlibNAV.h"

int16_t tiltError[3] ;
//int16_t desiredTiltVector[3] = { RMAX , 0 , RMAX } ;  // test case, desired tilt is 45 degrees to the left
//int16_t desiredTiltVector[3] = { RMAX , 0 , 0 } ;  // test case, desired tilt is 90 degrees to the left
static int16_t desiredTiltVector[3] = { 0 , 0 , RMAX } ;  // test case, level
static int16_t desiredRotationRateVector[3] = { 0 , 0 , 0 } ;
//int16_t actualRotationRate[3] = { 0 , 0 , 0 } ;
int16_t rotationRateError[3] = { 0 , 0 , 0 } ;

static uint16_t airSpeed = 1000 ;

int16_t desiredTurnRateRadians = RMAX ;
int16_t desiredTurnRateGyro ;

static union longww desiredTilt ;

#define GRAVITYCMSECSEC ( 981 )
#define RADSTOGYRO ( ( uint16_t ) 96*SCALEGYRO ) // used in the conversion from radians per second to raw gyro units

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

	// compute the desired tilt

	// desiredTilt is the ratio -rmat[6]/rmat[8] required for the turn
	// desiredTilt = desiredTurnRate * airSpeed / gravity
	// desiredTilt = RMAX*"desired tilt"
	// desiredTurnRate = RMAX*"desired turn rate", desired turn rate in radians per second
	// airSpeed is air speed centimeters per second
	// gravity is 981 centimeters per second per second 

	desiredTilt.WW = __builtin_mulsu( desiredTurnRateRadians , airSpeed ) ;
	desiredTilt.WW /= GRAVITYCMSECSEC ;

	// limit the lateral acceleration to +- 2 times gravity, total wing loading approximately 2.25 times gravity

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

	VectorScale( 3, desiredRotationRateVector , &rmat[6] , accum._.W0 ) ;

	// build the desired tilt vector and tilt error

	desiredTiltVector[0] = -desiredTilt._.W0 ;
	desiredTiltVector[1] =  0 ;
	desiredTiltVector[2] = RMAX ;

	vector3_normalize( desiredTiltVector , desiredTiltVector ) ; // make sure tilt vector has magnitude RMAX
	VectorCross( tiltError , &rmat[6] , desiredTiltVector ) ; // compute tilt orientation error
	if ( VectorDotProduct( 3 , &rmat[6] , desiredTiltVector ) < 0 ) // more than 90 degree error
	{
		vector3_normalize( tiltError , tiltError ) ; // for more than 90 degrees, make the tilt error vector parallel to desired axis, with magnitude RMAX
	}

	// compute the rotation rate error vector

//	VectorSubtract( 3 , rotationRateError , omegaAccum , desiredRotationRateVector ) ;
	rotationRateError[0] = desiredRotationRateVector[0] ;
	rotationRateError[1] = desiredRotationRateVector[1] ;
	rotationRateError[2] = desiredRotationRateVector[2] ;

}
