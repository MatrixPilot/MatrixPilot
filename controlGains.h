// control gains.
// YAWKP is the proportional feedback gain for the rudder
// YAWKD is the yaw gyro feedback gain for the rudder
// PITCHGAIN is the proportional feedback gain for the elevator

// All three gains should be positive real numbers.
// Typical values are:
//#define YAWKP 0.0625
//#define YAWKD 0.25
//#define PITCHGAIN 0.5

// maximum allowable values for the gains are 2.0
// to avoid gyro saturation, YAWKD should be bigger than YAWKP.

// experiment with these values to fine tune the performance of the controls in your plane

#define YAWKP .0625
#define YAWKD .25

#define PITCHGAIN 0.5

// return to launch pitch down in degrees, a real number.
// this is the real angle in degrees that the nose of the plane will pitch downward during a return to launch.
// it is used to increase speed (and wind penetration) during a return to launch.
// set it to zero if you do not want to use this feature.

#define RTLPITCHDOWN 2.0

// the real number SERVOSAT limits servo throw by controlling pulse width saturation.
// set it to 1.0 if you want full servo throw, otherwise set it to the portion that you want

#define SERVOSAT 1.0

// the following define is used to test the above gains and parameters.
// if you define TestGains, there functions will be enabled, even without GPS or Tx turned on.

//#define TestGains



