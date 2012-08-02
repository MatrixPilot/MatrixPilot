
#include "../MatrixPilot/defines.h"
#include "fbw_options.h"

#if(USE_INPUT_CONTROL == 1)

void cameraServoMix( void )
{
	long temp ;
	int pwManual[NUM_INPUTS+1] ;


	// If radio is off, use udb_pwTrim values instead of the udb_pwIn values
	for (temp = 0; temp <= NUM_INPUTS; temp++)
		if (udb_flags._.radio_on)
			pwManual[temp] = udb_pwIn[temp];
		else
			pwManual[temp] = udb_pwTrim[temp];

	temp = ( pwManual[CAMERA_PITCH_INPUT_CHANNEL] - 3000 ) + REVERSE_IF_NEEDED(CAMERA_PITCH_CHANNEL_REVERSED, 
					cam_pitch_servo_pwm_delta ) ;
	temp = cam_pitchServoLimit(temp) ;
	udb_pwOut[CAMERA_PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp + 3000 ) ;

	temp = ( pwManual[CAMERA_YAW_INPUT_CHANNEL] - 3000 ) + REVERSE_IF_NEEDED(CAMERA_YAW_CHANNEL_REVERSED, 
					cam_yaw_servo_pwm_delta ) ;
	temp = cam_yawServoLimit(temp) ;
	udb_pwOut[CAMERA_YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat( temp + 3000 ) ;
}

#endif // USE_INPUT_CONTROL
