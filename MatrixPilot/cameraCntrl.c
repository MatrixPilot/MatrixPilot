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
#include "camera_config.h"

// servo_ratios are used to convert degrees of rotation into servo pulse code lengths
// This code is configured for the full throw of the servo to be achieved by a range of
// 2000 units being sent to udb_pwOut. (i.e. min deflection 2000, centered 3000, max deflection 4000)
#define PITCH_SERVO_HIGH_RATIO  ((2000.0 / ((CAM_PITCH_SERVO_THROW / 360.0) * 65536.0)) * 65536.0)
#define YAW_SERVO_HIGH_RATIO    ((2000.0 / ((CAM_YAW_SERVO_THROW   / 360.0) * 65536.0)) * 65536.0)
#define PITCH_SERVO_RATIO        (2000.0 / ((CAM_PITCH_SERVO_THROW / 360.0) * 65536.0)) 
#define YAW_SERVO_RATIO          (2000.0 / ((CAM_YAW_SERVO_THROW   / 360.0) * 65536.0))

int16_t cam_pitch_servo_pwm_delta = 0;  // Change in PWM pulse value from centred value (3000) to send to camera pitch servo
int16_t cam_yaw_servo_pwm_delta   = 0;  // Change in PWM pulse value from centred value (3000) to send to camera yaw servo

const int16_t pitch_servo_high_ratio = PITCH_SERVO_HIGH_RATIO;
const int16_t yaw_servo_high_ratio   = YAW_SERVO_HIGH_RATIO;

// Note that most angles in cameraCntrl.c are 16 bit quantities
// For example, 90 degrees is represented as 16384 (65536 / 4)
const int16_t tan_pitch_in_stabilized_mode = CAM_TAN_PITCH_IN_STABILIZED_MODE;

const int16_t pitch_offset_centred_pwm = (CAM_PITCH_OFFSET_CENTRED * 65536.0 / 360.0) * PITCH_SERVO_RATIO;
const int16_t yaw_offset_centred_pwm   = (CAM_YAW_OFFSET_CENTRED   * 65536.0 / 360.0) * YAW_SERVO_RATIO  ;

const int16_t pitch_servo_pwm_max = ((CAM_PITCH_SERVO_MAX - CAM_PITCH_OFFSET_CENTRED) * 65536.0 / 360.0) * PITCH_SERVO_RATIO;					;
const int16_t pitch_servo_pwm_min = ((CAM_PITCH_SERVO_MIN - CAM_PITCH_OFFSET_CENTRED) * 65536.0 / 360.0) * PITCH_SERVO_RATIO;
const int16_t yaw_servo_pwm_max   = ((CAM_YAW_SERVO_MAX   - CAM_YAW_OFFSET_CENTRED  ) * 65536.0 / 360.0) * YAW_SERVO_RATIO;
const int16_t yaw_servo_pwm_min   = ((CAM_YAW_SERVO_MIN   - CAM_YAW_OFFSET_CENTRED  ) * 65536.0 / 360.0) * YAW_SERVO_RATIO;

struct relative3D view_location = { 0 , 20 , 0 };
struct relative3D camera_view   = { 0 ,  0 , 0 };

#if (CAM_TESTING_OVERIDE == 1)  // Used to test that Camera swings by correct angles when camera control gains.
#define CAM_TEST_TIMER 200      // e.g. value of 200 means 5 seconds (200 decremented 40 times / second until zero).
int16_t cam_test_yaw            = CAM_TESTING_YAW_ANGLE    * 65536.0 / 360.0;
int16_t cam_testing_pitch_angle = CAM_TESTING_PITCH_ANGLE  * 65536.0 / 360.0;
int16_t cam_test_timer          = CAM_TEST_TIMER;
#endif

#if (CAM_PITCH_TEST_GRANULARITY == 1) // Used to test the smallest movement possible by pitch servo.
int16_t pitch_servo_out = 500;
uint8_t counter_slow_down = 40;
#endif

int32_t cam_pitchServoLimit(int32_t pwm_pulse)
{
	if (pwm_pulse > pitch_servo_pwm_max) pwm_pulse = pitch_servo_pwm_max;
	if (pwm_pulse < pitch_servo_pwm_min) pwm_pulse = pitch_servo_pwm_min;
	return(pwm_pulse);
}

int32_t cam_yawServoLimit(int32_t pwm_pulse)
{
	if (pwm_pulse > yaw_servo_pwm_max) pwm_pulse = yaw_servo_pwm_max;
	if (pwm_pulse < yaw_servo_pwm_min) pwm_pulse = yaw_servo_pwm_min;
	return (pwm_pulse);
}

void set_camera_view(struct relative3D current_view)
{
#if (CAM_USE_EXTERNAL_TARGET_DATA != 1)
	view_location.x = current_view.x;
	view_location.y = current_view.y;
	view_location.z = current_view.z;
#endif
}

void compute_camera_view (void)
{
#if (DEADRECKONING == 1)
	camera_view.x = view_location.x - IMUlocationx._.W1;
	camera_view.y = view_location.y - IMUlocationy._.W1;
	camera_view.z = view_location.z - IMUlocationz._.W1;
#else
	camera_view.x = view_location.x - GPSlocation.x;
	camera_view.y = view_location.y - GPSlocation.y;
	camera_view.z = view_location.z - GPSlocation.z;
#endif
}

void cameraCntrl(void)
{
#if (USE_CAMERA_STABILIZATION == 1)
	union longbbbb cam;
	int16_t cam_pitch16 = 0;    // pitch accumalator in 16 bit byte circular.
	int16_t cam_yaw16   = 0;    // yaw   accumalator in 16 bit byte circular.
	int8_t cam_yaw8 = 0;        // An 8 bit version of cam_yaw to use with sine(), cosine()

	struct relative2D matrix_accum  = { 0, 0 };     // Temporary variable to keep intermediate results of functions.
	fractional cam_vector_ground[]  = { 0, 0 ,0 };  // Vector to camera target from within ground coordinate reference.
	fractional cam_vector_plane[]   = { 0, 0, 0 };  // Vector to camera target from within plane's coordinate reference
	fractional rmat_transpose[]     = {RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX};

	// In Manual Mode 
#if (CAMERA_MODE_INPUT_CHANNEL ==	CHANNEL_UNUSED)
	if (flags._.GPS_steering == 0 && flags._.pitch_feedback == 0)
#else
	if (udb_pwIn[CAMERA_MODE_INPUT_CHANNEL] < CAMERA_MODE_THRESHOLD_LOW)
#endif
	{
		// set camera to default position
		// Pitch Servo
		cam_pitch_servo_pwm_delta =  - pitch_offset_centred_pwm;		
		// Yaw Servo	
		cam_yaw_servo_pwm_delta = - yaw_offset_centred_pwm ;
	}
	else
	{
		// Stabilised Mode
#if (CAMERA_MODE_INPUT_CHANNEL ==	CHANNEL_UNUSED)
		if (flags._.GPS_steering == 0 && flags._.pitch_feedback == 1)
#else
		if ((udb_pwIn[CAMERA_MODE_INPUT_CHANNEL] > CAMERA_MODE_THRESHOLD_LOW) && \
		    (udb_pwIn[CAMERA_MODE_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_HIGH))
#endif
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

			// Yaw
			// "Aviation Convention - Ground" frame of reference
			matrix_accum.x =   rmat[4];
			matrix_accum.y =   rmat[1];
			cam_yaw8 =  rect_to_polar(&matrix_accum); //

			// camera_view uses the "UAV Devboard - Earth" reference
			// The next lines overide waypoint camera views for stablized mode.
			//cam_yaw8 = (char) (cam_yaw16 >> 8);
			camera_view.x =   - sine(cam_yaw8);
			camera_view.y =     cosine(cam_yaw8);
			camera_view.z =   - tan_pitch_in_stabilized_mode;
		}

		// Waypoint Mode and Stabilized Mode (and RTL)

		// The maths here is as follows. 
		// Take a vector defined in the earth reference (camera_view),
		// and rotate into the plane's reference. This requires the use of the inverse
		// of rmat which is also the transpose of the rmat matrix.
		// Then calculate each of the angles for yaw and pitch in the plane's reference. (roll not implemented at this time).
		// Finally, convert camera angles in yaw and pitch to servo rotation angles.

		// Convert externally requested camera view into a structure of type fractional 
		// Convert from "UAV Devboard - Ground" convention to "Aviation Convention - Ground"
		cam_vector_ground[0] = - camera_view.x;
		cam_vector_ground[1] =   camera_view.y;
		cam_vector_ground[2] = - camera_view.z; 

		// Rotate camera vector from ground reference into plane reference
		MatrixTranspose(3, 3, rmat_transpose, rmat);	
		// It does not matter that the result of the following operation is not the expected magnitude
		// because the code only uses the ratios of X,Y,Z relative to each other to calculate angles.
		MatrixMultiply(3 , 3 , 1 , cam_vector_plane , rmat_transpose , cam_vector_ground);

		// Convert camera vector which is now in plane's coordinate reference, to a Yaw angle with respect to front of plane.
		matrix_accum.x =   cam_vector_plane[0] ;
		matrix_accum.y =   cam_vector_plane[1] ; 
		cam_yaw16 = rect_to_polar16(&matrix_accum) - 16384; // subtract 90 degrees so yaw measured in line with fuselage

#if (CAM_TESTING_OVERIDE == 1)
		cam_test_timer--;
		if (cam_test_timer <= 0)
		{
			cam_test_timer = CAM_TEST_TIMER;
			cam_test_yaw = cam_test_yaw * -1; // reverse the angle of test
		}
		cam_yaw16 = cam_test_yaw;
#endif

		// Convert camera vector (which is in plane's coordinaet reference) to a pitch angle.
		matrix_accum.y = cam_vector_plane[2];
		cam_pitch16 = rect_to_polar16(&matrix_accum); // Note matrix_accum.x is the left over result of yaw call to rect_to_polar16

#if (CAM_TESTING_OVERIDE == 1)
		cam_pitch16 = cam_testing_pitch_angle; 
#endif

		// One day, insert special logic for when camera nearly pointing straight down 
		// to prevent large movements of camera on yaw for small changes in roll and pitch.
		// if (pitch > 15000 || pitch < -15000) // 16383 is close to 90 degrees.
		
		// Calculate signal to send to pitch servo
#if (CAM_PITCH_TEST_GRANULARITY == 1)
		// Move camera by smallest pitch granular amount, continuously. (testing for blurring of camera picture)
		if (counter_slow_down-- == 0)
		{
			if (pitch_servo_out-- < -500) pitch_servo_out = 500;  // A 22.5 degree movement either side of servo centre for a 90 degree servo
			counter_slow_down = 40;
		}
		cam_pitch_servo_pwm_delta = pitch_servo_out;		
#else
		cam.WW = __builtin_mulss(cam_pitch16, pitch_servo_high_ratio) + 0x8000;
		cam_pitch_servo_pwm_delta = cam._.W1 - pitch_offset_centred_pwm;
#endif
		// Calculate signal to send to yaw servo
		cam.WW = __builtin_mulss(cam_yaw16 , yaw_servo_high_ratio) + 0x8000; 		
		cam_yaw_servo_pwm_delta = cam._.W1 - yaw_offset_centred_pwm; 	
	}
#endif // USE_CAMERA_STABILIZATION
}

#if (CAM_USE_EXTERNAL_TARGET_DATA == 1)

struct relative3D cam_inject; // Camera view location received on the serial port
uint8_t cam_inject_pos = 0;

void camera_live_begin(void)
{
	cam_inject_pos = 0;
}

void camera_live_received_byte(uint8_t inbyte)
{
	if (cam_inject_pos < sizeof(cam_inject))
	{
		if (cam_inject_pos % 2 == 0)
		{
			((uint8_t*)(&cam_inject))[cam_inject_pos++ +1] = inbyte;
		}
		else
		{
			((uint8_t*)(&cam_inject))[cam_inject_pos++ -1] = inbyte;
		}
	}
	else if (cam_inject_pos == sizeof(cam_inject))
	{
		cam_inject_pos++;
	}
}

void camera_live_commit(void)
{
	if (cam_inject_pos == sizeof(cam_inject))
	{
		view_location.x = cam_inject.x;
		view_location.y = cam_inject.y;
		view_location.z = cam_inject.z;
	}
	cam_inject_pos = 0;
}

void camera_live_commit_relative_position(const struct relative3D target)
{
	view_location.x = target.x ; //relative position towards the east
	view_location.y = target.y ; //relative position towards the north
	view_location.z = target.z ; //relative position vertically up
}

#endif // CAM_USE_EXTERNAL_TARGET_DATA
