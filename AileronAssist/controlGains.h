//YAWKP is the return to launch turning gain, typically around 0.0625

#define YAWKP 0.0625

//ROLLKP and ROLLKD are roll stabilization gains.
//ROLLKP is the proportional gain, approximately 0.25
//ROLLKD is the deriviate (gyro) gain, approximately 0.125

#define ROLLKP 0.25
#define ROLLKD 0.125

// YAWKP, ROLLKP, ROLLKD should all have the same sign,
// such as 0.0625 0.25 0.125 or -0.0625 -.25 -0.125


//PITCHGAIN is the pitch stabilization gain, typically around 0.125

#define PITCHGAIN 0.125

// RTLPITCHDOWN is return to launch pitch down in degrees, a real number.
// this is the real angle in degrees that the nose of the plane will pitch downward during a return to launch.
// it is used to increase speed (and wind penetration) during a return to launch.
// set it to zero if you do want to use this feature.

#define RTLPITCHDOWN 0.0

// the real number SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want

#define SERVOSAT 1.0

// the following define is used to test the above gains and parameters.
// if you define TestGains, there functions will be enabled, even without GPS or Tx turned on.

//#define TestGains



