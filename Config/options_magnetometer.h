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

// Define which magnetometer you are using by uncommenting one
// of the #define lines below.
// HMC5843 is the 3DRobotics HMC5843 (now out of production).
// HMC5883L is the 3DRobotics HMC5883L
// HMC5883L_SF is the SparkFun HMC5883L

//#define HMC5843
#define HMC5883L
//#define HMC5883L_SF

// Define magneticDeclination to be the magnectic declination, in degrees, measured
// clockwise from the north, east is plus, west is minus.
//  Mississauga, ON is Lat 45.58 N and Long 79.65 W, Mag. Decl. therefore is 10deg21' W or -10.35 degrees
//  Bennet Field Springvale, ON is Lat 42deg58' N and Long 80deg9' W, Mag. Decl. therefore is 9deg48' W or -9.48 degrees
  //Salinas, CA Magnetic Variation: 16E (1980), http://www.airport-data.com/airport/SNS/
#define MAGNETICDECLINATION                 0

// Set to 0 for fixed declination angle or 1 for variable declination angle
#define DECLINATIONANGLE_VARIABLE           1


// #define LED_RED_MAG_CHECK 1 if you want the RED LED to indicate the magnetometer is not working.
// #define LED_RED_MAG_CHECK 0 if you want the RED LED to indicate control mode.

#define LED_RED_MAG_CHECK                   0

////////////////////////////////////////////////////////////////////////////////
// Uncomment one of the defines below to specify orientation of the mag.
// The following 4 supported orientations have the mag level with the ground.
// MAG_FORWARDS:  Component-side up,   edge connector front
// MAG_BACKWARDS: Component-side up,   edge connector back
// MAG_INVERTED:  Component-side down, edge connector front
// MAG_FLIPPED:   Component-side down, edge connector back
// MAG_DIRECT:    Magnetometer mounted in an orientation that permits a direct connection to a UDB4 or UDB5
// Note: right now, if MAG_DIRECT is selected, UDB board orientation must be ORIENTATION_FORWARDS
// For 3DRobotics mags, for MAG_DIRECT the mag mounts over the UDB, component side down.
// For SparkFun HMC5883L, for MAG_DIRECT the mag mounts over the UDB, component side up.

//#define MAG_FORWARDS
//#define MAG_BACKWARDS
//#define MAG_INVERTED
#define MAG_FLIPPED
//#define MAG_DIRECT

// Uncomment the following line for using Static Magnetometer Offsets. 
// You will need to manually calibrate your plane's offsets in advance.
// Set SERIAL_OUTPUT_FORMAT to  SERIAL_MAG_CALIBRATE to obtain calibration measurements.
// Turn the plane through all dimensions X,Y,Z  (imagine you are painting a 3D globe by rotating the plane).
// Inspect the last line of the telemetry file that you have created for the following static offsets,
// and set them below.
#define MAG_STATIC_OFFSETS

#define MAG_STATIC_OFFSET_X       0
#define MAG_STATIC_OFFSET_Y       0
#define MAG_STATIC_OFFSET_Z       0

// ************************************************************************
// *** Users should not need to change anything below here ****************
// ************************************************************************



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
// is aligned with the CPU chip on the UDB, with the pin 1 markers in the same orientation


// old 3DRobotics mag
#ifdef HMC5843

#ifdef MAG_FORWARDS
#define MAG_X_AXIS 1
#define MAG_Y_AXIS 0
#define MAG_Z_AXIS 2
#define MAG_X_SIGN -
#define MAG_Y_SIGN -
#define MAG_Z_SIGN -
#endif

#ifdef MAG_DIRECT
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 1
#define MAG_Z_AXIS 2
#define MAG_X_SIGN +
#define MAG_Y_SIGN +
#define MAG_Z_SIGN +
#endif

#ifdef MAG_BACKWARDS
#define MAG_X_AXIS 1
#define MAG_Y_AXIS 0
#define MAG_Z_AXIS 2
#define MAG_X_SIGN +
#define MAG_Y_SIGN +
#define MAG_Z_SIGN -
#endif

#ifdef MAG_INVERTED 
#define MAG_X_AXIS 1
#define MAG_Y_AXIS 0
#define MAG_Z_AXIS 2
#define MAG_X_SIGN +
#define MAG_Y_SIGN -
#define MAG_Z_SIGN +
#endif

#ifdef MAG_FLIPPED
#define MAG_X_AXIS 1
#define MAG_Y_AXIS 0
#define MAG_Z_AXIS 2
#define MAG_X_SIGN -
#define MAG_Y_SIGN +
#define MAG_Z_SIGN +
#endif

#define MAG_GAIN	700.0
#endif

// new 3D Robotics mag
#ifdef HMC5883L

#ifdef MAG_FORWARDS
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN +
#define MAG_Y_SIGN -
#define MAG_Z_SIGN -
#endif

#ifdef MAG_DIRECT
#define MAG_X_AXIS 2
#define MAG_Y_AXIS 0
#define MAG_Z_AXIS 1
#define MAG_X_SIGN +
#define MAG_Y_SIGN -
#define MAG_Z_SIGN +
#endif

#ifdef MAG_BACKWARDS
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN -
#define MAG_Y_SIGN +
#define MAG_Z_SIGN -
#endif

#ifdef MAG_INVERTED
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN -
#define MAG_Y_SIGN -
#define MAG_Z_SIGN +
#endif

#ifdef MAG_FLIPPED
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN +
#define MAG_Y_SIGN +
#define MAG_Z_SIGN +
#endif

#define MAG_GAIN 1300.0
#endif

// SparkFun HMC5883L mag
#ifdef HMC5883L_SF

#ifdef MAG_FORWARDS
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN +
#define MAG_Y_SIGN -
#define MAG_Z_SIGN -
#endif

#ifdef MAG_DIRECT
#define MAG_X_AXIS 2
#define MAG_Y_AXIS 0
#define MAG_Z_AXIS 1
#define MAG_X_SIGN +
#define MAG_Y_SIGN +
#define MAG_Z_SIGN -
#endif

#ifdef MAG_BACKWARDS
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN -
#define MAG_Y_SIGN +
#define MAG_Z_SIGN -
#endif

#ifdef MAG_INVERTED
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN -
#define MAG_Y_SIGN -
#define MAG_Z_SIGN +
#endif

#ifdef MAG_FLIPPED
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 2
#define MAG_Z_AXIS 1
#define MAG_X_SIGN +
#define MAG_Y_SIGN +
#define MAG_Z_SIGN +
#endif

#define MAG_GAIN 1000.0
#endif

#if (HILSIM == 1)
#undef MAG_X_AXIS
#undef MAG_Y_AXIS
#undef MAG_Z_AXIS
#undef MAG_X_SIGN
#undef MAG_Y_SIGN
#undef MAG_Z_SIGN
#undef MAG_GAIN
#define MAG_X_AXIS 0
#define MAG_Y_AXIS 1
#define MAG_Z_AXIS 2
#define MAG_X_SIGN +
#define MAG_Y_SIGN +
#define MAG_Z_SIGN +

#define MAG_GAIN 1000.0
#endif

// Minimum and maximum values expected for the absolute value of the magnetic field.
// These are used to help detect when the magnetometer has stopped working properly due to
// RF interference that may put it into an unknown state.

#define MAGNETICMINIMUM 300
#define MAGNETICMAXIMUM 1500

// The following line computes an internal parameter, do not change it.
#define DECLINATIONANGLE ((int16_t)(MAGNETICDECLINATION *(32767.0 / 180.0)))
