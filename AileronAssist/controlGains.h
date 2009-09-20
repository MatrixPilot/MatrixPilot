// control gains.

// maximum allowable values for the gains are 2.0

// experiment with these values to fine tune the performance of the controls in your plane


#define YAWKP 0.1	// yaw proportional feedback gain for steering, around 0.1

#define ROLLKP 0.25		// roll proportional feedback gain for roll leveling, around 0.25
#define ROLLKD (0.125*SCALEGYRO)	// roll rate feedback gain for roll damping, around 0.125

#define PITCHGAIN 0.125	// pitch proportional feedback gain for pitch leveling, around 0.125
#define PITCHKD (0.0625*SCALEGYRO) // pitch rate feedback gain for pitch damping, around 0.0625
#define PITCHBOOST 0.0	// pitch boost (optional, I do not use it myself), around 0.25

// return to launch pitch down in degrees, a real number.
// this is the real angle in degrees that the nose of the plane will pitch downward during a return to launch.
// it is used to increase speed (and wind penetration) during a return to launch.
// set it to zero if you do not want to use this feature.

#define RTLPITCHDOWN 0.0

// the real number SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want

#define SERVOSAT 1.0

// the following section is for altitude hold

#define ALTITUDEHOLD	// comment out this line if you are not going to use altitude hold, 
						// to avoid spurious interrupts from the unused PWM channel

#define HEIGHTMAX 100.0 // maximum target height in meters

#define MINIMUMTHROTTLE 0.35 // minimum throttle

#define PITCHATMINTHROTTLE 0.0  // target pitch angle in degrees at minimum throttle
#define PITCHATMAXTHROTTLE 15.0 // target pitch angle in degrees at maximum throttle
#define PITCHATZEROTHROTTLE 0.0 // target pitch angle in degrees while gliding


// the following define is used to test the above gains and parameters.
// if you define TestGains, there functions will be enabled, even without GPS or Tx turned on.

//#define TestGains		// uncomment this line if you want to test your gains without using GPS



