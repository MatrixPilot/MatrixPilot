#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

int camera_rollkp= CAMERA_ROLLKP*RMAX ;
int camera_pitchkp= CAMERA_PITCHKP*RMAX ;
int camera_yawkp = CAMERA_YAWKP*RMAX ;

// Basic camera stabilization control.
// Adjust the camera roll/pitch/yaw gains in controlGains.h.
// We may need to also take the rotational velocities into account.
// After getting stabilization working, look into aiming the camera at a specific location on the ground.


long lpFilteredCameraYawControl = 0 ;


void cameraCntrl( void )
{
#if ( USE_CAMERA_STABILIZATION == 1 )

	union longww cam ;
	
	// If the radio is off, pwIn[] values will already be set to pwTrim[] values in servoMix()
	
	// Adjust the roll of the camera proportionally to the current roll of the plane
	cam.WW = REVERSE_IF_NEEDED(CAMERA_ROLL_CHANNEL_REVERSED, __builtin_mulss( rmat[6] , camera_rollkp )) ;
	pwOut[CAMERA_ROLL_OUTPUT_CHANNEL] = pulsesat(pwIn[CAMERA_ROLL_INPUT_CHANNEL] - cam._.W1) ;
	
	
	// Adjust the pitch of the camera proportionally to the current pitch of the plane
	cam.WW = REVERSE_IF_NEEDED(CAMERA_PITCH_CHANNEL_REVERSED, __builtin_mulss( rmat[7] , camera_pitchkp )) ;
	pwOut[CAMERA_PITCH_OUTPUT_CHANNEL] = pulsesat(pwIn[CAMERA_PITCH_INPUT_CHANNEL] - cam._.W1) ;
	
	
	// High pass filter the current yaw of the plane, and adjust the yaw of the camera proportionally to that
	// Bill or Pete, maybe you can improve this filtering...
	cam.WW = __builtin_mulss( rmat[1] , camera_yawkp ) ;
	lpFilteredCameraYawControl -= lpFilteredCameraYawControl >> 5 ;
	lpFilteredCameraYawControl += cam._.W1 >> 5 ;
	long hpFilteredCameraYawControl = (long)cam._.W1 - lpFilteredCameraYawControl ;
	pwOut[CAMERA_YAW_OUTPUT_CHANNEL] = pulsesat(pwIn[CAMERA_YAW_INPUT_CHANNEL] -
		REVERSE_IF_NEEDED(CAMERA_YAW_CHANNEL_REVERSED, hpFilteredCameraYawControl)) ;

#endif
}
