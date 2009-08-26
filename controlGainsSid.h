// control gains.
// YAWKP is the proportional feedback gain for the rudder
// YAWKD is the yaw gyro feedback gain for the rudder
// PITCHGAIN is the pitch proportional feedback gain for the elevator
// PITCHKD is the pitch derivative feedback gain for the elevator

// All four gains should be positive real numbers.
// Typical values are:
//#define YAWKP 0.25
//#define YAWKD 0.25
//#define PITCHGAIN 0.25
//#define PITCHKD 0.25

// maximum allowable values for the gains are 2.0

// experiment with these values to fine tune the performance of the controls in your plane

#define YAWKP .0312
#define YAWKD .250

#define YAWBOOST 1.0

//#define PITCHGAIN 0.25
#define PITCHGAIN 0.250
#define PITCHKD 0.250

#define PITCHBOOST 0.5


// return to launch pitch down in degrees, a real number.
// this is the real angle in degrees that the nose of the plane will pitch downward during a return to launch.
// it is used to increase speed (and wind penetration) during a return to launch.
// set it to zero if you do not want to use this feature.

//#define RTLPITCHDOWN 2.0

#define RTLPITCHDOWN 0.0


// the real number SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want

#define SERVOSAT 1.0

// the following section is for altitude hold

//#define ALTITUDEHOLD	// comment out this line if you are not going to use altitude hold, 
						// to avoid spurious interrupts from the unused PWM channel

//#define HEIGHTMAX 300.0  // maximum height, meters, for altitude hold
#define HEIGHTMAX 150.0

//#define MAXPITCHADJUST 7.5 // maximum pitch adjustment, in degrees, for altitude hold
#define MAXPITCHADJUST 15.0

#define MINIMUMTHROTTLE 0.5 // minimum throttle


// the following define is used to test the above gains and parameters.
// if you define TestGains, there functions will be enabled, even without GPS or Tx turned on.

//#define TestGains		// uncomment this line if you want to test your gains without using GPS



