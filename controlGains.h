// control gains.
// YAWKP is the proportional feedback gain for the rudder
// YAWKD is the yaw gyro feedback gain for the rudder
// YAWBOOST is the additional gain multiplier for the manually commanded rudder deflection
// PITCHGAIN is the pitch proportional feedback gain for the elevator
// RUDDERELEVMIX is the degree of elevator adjustment for rudder and banking

// All gains should be positive real numbers.
// Typical values for the red board are:
//#define YAWKP 0.100
//#define YAWKD (0.15*SCALEGYRO)
//#define YAWBOOST 1.0
//#define PITCHGAIN 0.150
//#define PITCHKD (0.15*SCALEGYRO)
//#define PITCHBOOST 0.5
//#define RUDDERELEVMIX 1.0

// maximum allowable values for the gains are 2.0
// experiment with these values to fine tune the performance of the controls in your plane

#define YAWKP 0.10	
#define YAWKD (.15*SCALEGYRO)
#define YAWBOOST 1.0

#define PITCHGAIN 0.15
#define PITCHKD (0.15*SCALEGYRO)
#define PITCHBOOST 0.5

#define RUDDERELEVMIX 1.0

// the real number SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want

#define SERVOSAT 1.0

// the following section is for altitude hold

#define ALTITUDEHOLD	// comment out this line if you are not going to use altitude hold, 
						// to avoid spurious interrupts from the unused PWM channel

#define HEIGHTMAX 100.0	// maximum target height in meters

#define MINIMUMTHROTTLE 0.35 // minimum throttle

#define PITCHATMINTHROTTLE 0.0 // target pitch angle in degrees at minimum throttle
#define PITCHATMAXTHROTTLE 15.0 // target pitch angle in degrees at maximum throttle
#define PITCHATZEROTHROTTLE 0.0 // target pitch angle in degrees while gliding

// the following define is used to test the above gains and parameters.
// if you define TestGains, there functions will be enabled, even without GPS or Tx turned on.

//#define TestGains		// uncomment this line if you want to test your gains without using GPS



