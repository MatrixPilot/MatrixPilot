// Define magneticDeclination to be the magnectic declination, in degrees, measured
// clockwise from the north, east is plus, west is minus.

#define MAGNETICDECLINATION 0
#define DECLINATIONANGLE ((signed char)(MAGNETICDECLINATION*128/180))

// #define LED_RED_MAG_CHECK 1 if you want the RED LED to indicate the magnetometer is not working.
// #define LED_RED_MAG_CHECK 0 if you want the RED LED to indicate control mode.

#define LED_RED_MAG_CHECK	0

// Define the alignment of magnetometer with the UDB X, Y, and Z axis.
// MAG_X_AXIS, MAG_Y_AXIS, MAG_Y_AXIS refer to the UDB X, Y, and Z axis.
// 0, 1, 2 refer to the magnetometer X, Y, and Z axis.
// Each axis entry pairs a UDB axis with a magnetometer axis.
// Each sign definition expresses the relative sign of alignment with a UDB axis

// The following definitions are for Jordi's breakout board.
// It assumes that the X and Y axis labels on the breakout board align
// with the X and Y labels on the UDB, and that the breakout board is mounted with
// the magnetometer on top, the same as for the UDB.

// If you are using a different HMC5843 magnetometer breakout board, just make sure the magnetometer
// is aligned with the CPU chip on the UDB, with the pin 1 markers in the same orientation.

#define MAG_X_AXIS 1
#define MAG_Y_AXIS 0
#define MAG_Z_AXIS 2

#define MAG_X_SIGN -
#define MAG_Y_SIGN -
#define MAG_Z_SIGN -

// Minimum and maximum values expected for the absolute value of the magnetic field.
// These are used to help detect when the magnetometer has stopped working properly due to
// RF interference that may put it into an unknown state.

#define MAGNETICMINIMUM 500
#define MAGNETICMAXIMUM 1000

