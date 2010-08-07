// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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

// servo throw can be more than 3 turns - 1080 degrees - so use integers rather than char
const int tan_pitch_in_stabilized_mode = CAM_TAN_PITCH_IN_STABILIZED_MODE ;
const int yaw_in_stabilized_mode   = CAM_YAW_IN_STABILIZED_MODE   * 256.0 / 360.0 ;

const int pitch_offset_centred = CAM_PITCH_OFFSET_CENTRED * 256.0 / 360.0 ;
const int yaw_offset_centred   = CAM_YAW_OFFSET_CENTRED   * 256.0 / 360.0 ;

const int pitch_servo_max = CAM_PITCH_SERVO_MAX  * 256.0 / 360.0 ;
const int pitch_servo_min = CAM_PITCH_SERVO_MIN  * 256.0 / 360.0 ;
const int yaw_servo_max   =  CAM_YAW_SERVO_MAX   * 256.0 / 360.0 ;
const int yaw_servo_min   =  CAM_YAW_SERVO_MIN   * 256.0 / 360.0 ;

// servo_ratios are used to convert degrees of rotation into servo pulse code lengths
// This code is configured for the full throw of the servo to be achieved by a range of
// 2000 units being sent to udb_pwOut. (i.e. min 2000, centered 3000, max 4000 )
const int pitch_servo_ratio = (( 2000.0 / ((CAM_PITCH_SERVO_THROW / 360.0) * 256.0 )) * 256.0 );
const int yaw_servo_ratio   = (( 2000.0 / ((CAM_YAW_SERVO_THROW   / 360.0) * 256.0 )) * 256.0 ) ;


// incremental length of pulse times to create servo travel
int pitch_servo = 0; 
int roll_servo  = 0;
int yaw_servo   = 0;

struct relative3D camera_view = { 0 , 20, 0 };


int pitchServoLimit(int angle)
{
	if ( angle > pitch_servo_max) angle = pitch_servo_max ;
	if ( angle < pitch_servo_min) angle = pitch_servo_min ;
	return (angle) ;
}


int yawServoLimit(int angle)
{
	if ( angle > yaw_servo_max) angle = yaw_servo_max ;
	if ( angle < yaw_servo_min) angle = yaw_servo_min ;
	return (angle) ;
}


// Rmat is the rotation matrix to convert from plane's reference
// to the earth's reference. The camera code needs the inverse function
// to be done regularly. i.e. use the transpose of the rotation matrix to
// rotate a vector from the earth's frame of reference into the plane's
// frame of reference. To save cpu cycles, the software uses this dedicated
// function to do both operations (transpose and multiple) at the same time.


void cameraCntrl( void )
{

#if ( USE_CAMERA_STABILIZATION == 1 )

	union longbbbb cam ;
	union longbbbb accum ;

	int delta_pitch_servo ;
	int delta_yaw_servo ;

	signed char pitch = 0;		// pitch accumalator in byte circular.
	signed char yaw   = 0;		// yaw accumalator in byte circular.

	struct relative2D matrix_accum = { 0 , 0 } ;
	fractional cam_vector_ground[]  = { 0 ,0 , 0 } ;

	fractional cam_vector_plane[]  = { 0, 0 ,0 } ;
	fractional rmat_transpose[] = {RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX} ;
	
	// In Manual Mode 
	
	if ( flags._.GPS_steering == 0 && flags._.pitch_feedback == 0 )
	{
		// set camera to default position once, and leave there
		// Pitch Servo
		cam.WW = __builtin_mulss((pitchServoLimit( - pitch_offset_centred)), pitch_servo_ratio) ;
		accum.__.B1 = cam.__.B2 ;
		accum.__.B0 = cam.__.B1 ;
		pitch_servo = ( accum._.W0 + 0x80 );
		udb_pwOut[CAMERA_PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + REVERSE_IF_NEEDED(CAMERA_PITCH_CHANNEL_REVERSED, pitch_servo)) ;
		
		// Roll Servo
		// Not implemented
		
		// Yaw Servo
		cam.WW = __builtin_mulss((yawServoLimit( - yaw_offset_centred)), yaw_servo_ratio) ; 
		accum.__.B1 = cam.__.B2 ;
		accum.__.B0 = cam.__.B1 ;
		yaw_servo = ( accum._.W0 + 0x80 ) ;	
		udb_pwOut[CAMERA_YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + REVERSE_IF_NEEDED(CAMERA_YAW_CHANNEL_REVERSED, yaw_servo)) ;
	}
	else
	{
	 	// Stabilised Mode
		if ( flags._.GPS_steering == 0 && flags._.pitch_feedback == 1 )
		{
			// Stabilised mode is actually the most complex translation. It requires a 
			// a mix of settings from the ground reference and the plane's reference.
			// Against the ground reference, the camera has to stay at a constant pitch to the horizontal
			// Against the plane reference, the camera has to keep at a constant yaw. e.g. straight ahead.
			// Maths: 
			// Calculate the Yaw angle of the Plane in the ground reference. 
			// Use the desired pitch of the camera, to create a camera view vector with reference to the ground
			// Convert the camera view vector back to the plane's frame of reference.
			// Calculate pitch and yaw for the camera servos.
			
			// Roll
			// Ground frame of reference
			//matrix_accum.x = rmat[8] ;
			//matrix_accum.y = rmat[6] ;
			//roll = rect_to_polar(&matrix_accum);
			// Pitch
			// Ground frame of Reference
			// This pitch calculation must come after the Roll
			// caclulation as it mathematically uses the result
			// left over from rect_to_polar in matrix_accum.x
			//matrix_accum.y = rmat[7] ;
			//pitch = rect_to_polar(&matrix_accum);
			// Yaw
			// "Aviation Convention - Ground" frame of reference
			matrix_accum.x =   rmat[4] ;
			matrix_accum.y =   rmat[1] ;
			yaw =  rect_to_polar(&matrix_accum); //
			
			// camera_view uses the "UAV Devboard - Earth" reference
			camera_view.x =   - sine(yaw) ;
			camera_view.y =     cosine(yaw) ;
			camera_view.z =   - tan_pitch_in_stabilized_mode ;
		}
		
		
		// Waypoint Mode and Stabilized Mode (and RTL)
		
		// The maths here is as follows. 
		// Take a vector defined in the earth reference (camera_view),
		// and rotate into the plane's reference. This requires the use of the inverse
		// of rmat which is also the transpose of the rmat matrix.
		// Then calculate each of the angles for yaw and pitch in the plane's reference. (roll not implemented at this time).
		// Convert camera angles in yaw and pitch to servo rotation angles.
		
		// Convert externally requested camera view into fractional structure
		// Convert from "UAV Devboard - Ground" convention to "Aviation Convention - Ground"
		cam_vector_ground[0] = - camera_view.x ;
		cam_vector_ground[1] =   camera_view.y ;
		cam_vector_ground[2] = - camera_view.z ; 
		// Rotate camera vector from ground reference into plane reference
		udb_setDSPLibInUse(true) ;
		MatrixTranspose(3, 3, rmat_transpose, rmat ) ;
		// It does not matter that the result of this operation is not the expected magnitude
		// because the code only uses the ratios of X,Y,Z relative to each other to calculate angles.
		MatrixMultiply( 3 , 3 , 1 , cam_vector_plane , rmat_transpose , cam_vector_ground ) ;
		udb_setDSPLibInUse(false) ;
		
		// Yaw 
		matrix_accum.x =   cam_vector_plane[0] ;
		matrix_accum.y =   cam_vector_plane[1] ; 
		yaw = rect_to_polar(&matrix_accum) - 64; // subtract 90 degrees so yaw measured in line with fuselage
		
		// Pitch
		matrix_accum.y = cam_vector_plane[2] ;
		pitch = rect_to_polar(&matrix_accum) ;
		
		// Need to insert special logic for when camera nearly pointing straight down 
		// to prevent large movements of camera on yaw for small changes in roll and pitch.
		// if ( pitch > 60 || pitch < -60 ) // 64 bytecircular is 90 degrees
		
		// Calculate signal to send to pitch servo
		cam.WW = __builtin_mulss((pitchServoLimit(pitch - pitch_offset_centred)), pitch_servo_ratio) ;
		accum.__.B1 = cam.__.B2 ;
		accum.__.B0 = cam.__.B1 ;
		// 0x80 deals with rounding error (128)
		delta_pitch_servo = ( accum._.W0 + 0x80 ) - pitch_servo;
		pitch_servo = pitch_servo + (delta_pitch_servo >> 3);
		udb_pwOut[CAMERA_PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + REVERSE_IF_NEEDED(CAMERA_PITCH_CHANNEL_REVERSED, pitch_servo)) ;
		
		// Code for driving roll servo left here for future use
		//cam.WW = __builtin_mulss((rollServoLimit(roll - roll_offset_centred)), roll_servo_ratio) ;
		//accum.__.B1 = cam.__.B2 ;
		//accum.__.B0 = cam.__.B1 ;
		// // 0x80 deals with rounding error (128)
		//delta_roll_servo = ( accum._.W0 + 0x80 ) - roll_servo;
		//roll_servo = roll_servo + (delta_roll_servo >> 3);
		//udb_pwOut[CAMERA_ROLL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + REVERSE_IF_NEEDED(CAMERA_ROLL_CHANNEL_REVERSED, roll_servo)) ;
		
		// Calculate signal to send to yaw servo
		cam.WW = __builtin_mulss((yawServoLimit(yaw - yaw_offset_centred)), yaw_servo_ratio) ; 
		accum.__.B1 = cam.__.B2 ;
		accum.__.B0 = cam.__.B1 ;
		// 0x80 deals with rounding error (128)
		delta_yaw_servo = ( accum._.W0 + 0x80 ) - yaw_servo;
		yaw_servo = yaw_servo + (delta_yaw_servo >> 3);	
		udb_pwOut[CAMERA_YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + REVERSE_IF_NEEDED(CAMERA_YAW_CHANNEL_REVERSED, yaw_servo)) ;
	}
#endif
}

