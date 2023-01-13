#define MINI5 "UDBmini5 hardware.\r\n"
#define MINI6 "UDBmini6 hardware.\r\n"

#define DATE "rev. 2.2, 1/7/2023\r\n"

// the following defines select what gets sent to the logger. define one of them
//#define LOG_IMU
//#define RECORD_OFFSETS
#define GYRO_OFFSETS
//#define TEST_LOGGER_HZ
//#define GYRO_CALIB
//#define LOG_VELOCITY
//#define GYRO_DRIFT
//#define ROAD_TEST
//#define BUILD_OFFSET_TABLE

// options to override the normal mission logic and always log and/or always remove gyro offsets
//#define ALWAYS_LOG
//#define ALWAYS_SYNC_GYROS

// for IMU log, there are two options related to the gyros. pick one or the other
// LOG_RATE reports the gyro rates, degrees/sec
// LOG_EULER reports roll, pitch and yaw Euler angles in NED
//#define LOG_RATE
#define LOG_EULER
//#define LOG_RATE_AND_EULER
//#define LOG_PITCH_AND_TWO_FORCES


// set the logger hertz, allowable values are 1,2,4,5,10,20,25,40,50,100 or 200
#define LOGGER_HZ	100
#define SLIDE_DET_HZ	10
#define TILT_START	15
#define TILT_STOP	60

#define MINI5_SN12

#ifdef MINI6_SN1
#define BOARD MINI6
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 4
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#define LOG_EULER
#endif // MINI6_SN1

#ifdef MINI6_SN2
#define BOARD MINI6
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 2
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
//#define LOG_EULER
#endif // MINI6_SN2

#ifdef MINI6_SN3
#define BOARD MINI6
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 3
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#define LOG_EULER
#endif // MINI6_SN3

#ifdef MINI6_SN4
#define BOARD MINI6
#define NAME "Tucker West"
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 1
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#define LOG_EULER
#endif // MINI6_SN4

#ifdef MINI6_SN5
#define BOARD MINI6
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 5
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#define LOG_EULER
#endif // MINI6_SN5

#ifdef MINI6_SN6
#define BOARD MINI6
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 6
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#define LOG_EULER
#endif // MINI6_SN6

#ifdef MINI5_SN3
// was sn3
#define BOARD MINI5
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	1
#define SERIAL_NUMBERD3 0
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // MINI5_SN3

#ifdef MINI5_SN4
// was SN4
#define BOARD MINI5
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	1
#define SERIAL_NUMBERD3 1
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // MINI5_SN4

#ifdef MINI5_SN12
#define BOARD MINI5
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	1
#define SERIAL_NUMBERD3 2
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // MINI5_SN12

#ifdef MINI5_SN2
// was SN2
#define BOARD MINI5
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 9
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // MINI5_SN2

#ifdef MINI5_SN1
// was SN1
#define BOARD MINI5
#define SERIAL_NUMBERD1	0
#define SERIAL_NUMBERD2	0
#define SERIAL_NUMBERD3 8
#define ACCEL_RANGE         8
#define GYRO_RANGE	    1000
#endif // MINI5_SN1

#ifdef MINI5_SN1
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 80 )
#define YACCEL_OFFSET	( -17 )
#define ZACCEL_OFFSET	( -288 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_9.h"
//#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_7.h"
#define CALIBRATIONX	1.0159
#define CALIBRATIONY	1.0100
#define CALIBRATIONZ	1.0119
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4089
#define CAL_GRAV_Y	4074
#define CAL_GRAV_Z	4162
#endif //MINI5_SN1

#ifdef MINI5_SN2
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 84 )
#define YACCEL_OFFSET	( -48 )
#define ZACCEL_OFFSET	( 248 )
#error "gyro table not defined"
#define CALIBRATIONX	1.0389
#define CALIBRATIONY	1.0389
#define CALIBRATIONZ	1.0206
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4124
#define CAL_GRAV_Y	4085
#define CAL_GRAV_Z	4140
#endif //MINI5_SN2

#ifdef MINI5_SN3
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 158 )
#define YACCEL_OFFSET	( -29 )
#define ZACCEL_OFFSET	( -287 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_8.h"
#define CALIBRATIONX	1.0120
#define CALIBRATIONY	1.0185
#define CALIBRATIONZ	1.0090
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4103
#define CAL_GRAV_Y	4082
#define CAL_GRAV_Z	4174
#endif //MINI5_SN3

#ifdef MINI5_SN4
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 132 )
#define YACCEL_OFFSET	( 56 )
#define ZACCEL_OFFSET	( -330 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_11.h"
#define CALIBRATIONX	1.0072
#define CALIBRATIONY	1.0057
#define CALIBRATIONZ	1.0039
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4123
#define CAL_GRAV_Y	4096
#define CAL_GRAV_Z	4198
#endif //MINI5_SN4

#ifdef MINI5_SN12
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 0 )
#define YACCEL_OFFSET	( 0 )
#define ZACCEL_OFFSET	( 0 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_12.h"
#define CALIBRATIONX	1.0000
#define CALIBRATIONY	1.0000
#define CALIBRATIONZ	1.0000
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4096
#define CAL_GRAV_Y	4096
#define CAL_GRAV_Z	4096
#endif //MINI5_SN12

#ifdef MINI6_SN1
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 83 )
#define YACCEL_OFFSET	( -15 )
#define ZACCEL_OFFSET	( -16 )
#error "gyro table not defined"
#define CALIBRATIONX	1.0109
#define CALIBRATIONY	1.0171
#define CALIBRATIONZ	1.0066
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4101
#define CAL_GRAV_Y	4099
#define CAL_GRAV_Z	4153
#endif // MINI6_SN1

#ifdef MINI6_SN2
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 72 )
#define YACCEL_OFFSET	( -42 )
#define ZACCEL_OFFSET	( 26 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_4.h"
#define CALIBRATIONX	1.0141
#define CALIBRATIONY	1.0056
#define CALIBRATIONZ	1.0127
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4103
#define CAL_GRAV_Y	4103
#define CAL_GRAV_Z	4108
#endif // MINI6_SN2

#ifdef MINI6_SN3
#define CUSTOM_OFFSETS
#define XACCEL_OFFSET	( 58 )
#define YACCEL_OFFSET	( -34 )
#define ZACCEL_OFFSET	( -77 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_10.h"
#define CALIBRATIONX	1.0135
#define CALIBRATIONY	1.0009
#define CALIBRATIONZ	1.0067
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4100
#define CAL_GRAV_Y	4104
#define CAL_GRAV_Z	4148
#endif // MINI6_SN3

#ifdef MINI6_SN4
#define XACCEL_OFFSET	( 83 )
#define YACCEL_OFFSET	( -50 )
#define ZACCEL_OFFSET	( -21 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_1.h"
#define CALIBRATIONX	1.0112
#define CALIBRATIONY	1.0043
#define CALIBRATIONZ	1.0067
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4105
#define CAL_GRAV_Y	4098
#define CAL_GRAV_Z	4122
#endif // MINI6_SN4

#ifdef MINI6_SN5
#define XACCEL_OFFSET	( 58 )
#define YACCEL_OFFSET	( -30 )
#define ZACCEL_OFFSET	( -104 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_5.h"
#define CALIBRATIONX	1.0146
#define CALIBRATIONY	1.0009
#define CALIBRATIONZ	1.0061
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4097
#define CAL_GRAV_Y	4106
#define CAL_GRAV_Z	4124
#endif // MINI6_SN5

#ifdef MINI6_SN6
#define XACCEL_OFFSET	( 135 )
#define YACCEL_OFFSET	( -37 )
#define ZACCEL_OFFSET	( -100 )
#define GYRO_OFFSET_TABLE "../libUDB/gyro_tables/table_6.h"
#define CALIBRATIONX	1.0145
#define CALIBRATIONY	1.0037
#define CALIBRATIONZ	1.0055
#define CALIB_GRAVITY	4096
#define CAL_GRAV_X	4098
#define CAL_GRAV_Y	4108
#define CAL_GRAV_Z	4134
#endif // MINI6_SN6


#ifdef GYRO_CALIB
#undef LOGGER_HZ
#define LOGGER_HZ 10
#undef ALWAYS_LOG
#undef TILT_STOP
#define TILT_STOP 500
#define UPSIDE_DOWN
#endif // GYRO_CALIB

#ifdef GYRO_OFFSETS
#undef LOGGER_HZ
#define LOGGER_HZ 4
#undef ALWAYS_LOG
#define ALWAYS_LOG
#undef ALWAYS_SYNC_GYROS
//#define ALWAYS_SYNC_GYROS
#undef TILT_STOP
#define TILT_STOP 600
#endif // GYRO_OFFSETS

#ifdef GYRO_DRIFT
#undef LOGGER_HZ
#define LOGGER_HZ 10
#undef ALWAYS_LOG
#define ALWAYS_LOG
#undef ALWAYS_SYNC_GYROS
//#define ALWAYS_SYNC_GYROS
#undef TILT_STOP
#define TILT_STOP 60
#endif // GYRO_DRIFT

#ifdef RECORD_OFFSETS
#undef LOGGER_HZ
#define LOGGER_HZ 10
#ifndef ALWAYS_LOG
#define ALWAYS_LOG
#endif // ALWAYS_LOG
#endif // RECORD_OFFSETS

#ifdef LOG_VELOCITY
#undef LOGGER_HZ
#define LOGGER_HZ 20
#undef ALWAYS_LOG
#endif // LOG_VELOCITY

#ifdef TEST_GYRO_LOCK 
#define ALWAYS_SYNC_GYROS
#define ALWAYS_LOG
#undef LOGGER_HZ
#define LOGGER_HZ 1
#endif // TEST_GYRO_LOCK

#ifdef LOG_PITCH_AND_TWO_FORCES
#undef LOGGER_HZ
#define LOGGER_HZ 200
#endif // LOG_PITCH_AND_TWO_FORCES

#ifdef ROAD_TEST
#undef ALWAYS_LOG
#undef LOGGER_HZ
//#define ALWAYS_LOG
#define LOGGER_HZ 40
#endif // ROAD_TEST

#ifdef BUILD_OFFSET_TABLE
#define ALWAYS_LOG
#define BUILD_OFFSET_HZ 200
#endif // 

#define FILTERING "Force data is filtered by averaging pairs of 200 Hz samples.\r\n"

#define TEST_LIDAR 0

#define DR_TAU 2.5

#define GPS_TYPE						GPS_NONE
#define NO_RADIO						1
#define MAG_YAW_DRIFT 						0
#define DEBUG_MAG						0

#define NUM_INPUTS	7
#define NUM_OUTPUTS	4

#define CONSOLE_UART            0
#define HILSIM			0

////////////////////////////////////////////////////////////////////////////////
// Use board orientation to change the mounting direction of the board.
// The following 4 orientations have the board parallel with the ground.
// ORIENTATION_FORWARDS:  Component-side up,   GPS connector front
// ORIENTATION_BACKWARDS: Component-side up,   GPS connector back
// ORIENTATION_INVERTED:  Component-side down, GPS connector front
// ORIENTATION_FLIPPED:   Component-side down, GPS connector back
// The following 2 orientations are "knife edge" mountings
// ORIENTATION_ROLLCW: Rick's picture #9, board rolled 90 degrees clockwise,
//		from point of view of the pilot
// ORIENTATION_ROLLCW180: Rick's pitcure #11, board rolled 90 degrees clockwise,
//		from point of view of the pilot, then rotate the board 180 around the Z axis of the plane,
//		so that the GPS connector points toward the tail of the plane
// ********** NOTE: orientations are withrespect to the front motor for + configuration,  *******
// or with respect to left front motor, for X configuration

#ifdef UPSIDE_DOWN
#define BOARD_ORIENTATION	ORIENTATION_FLIPPED
#else
#define BOARD_ORIENTATION	ORIENTATION_FORWARDS
#endif // 

