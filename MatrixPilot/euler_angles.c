 // This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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

// Implements a set of functions that return vehicle euler_angles.
// Provides centralized logic for commonly needed code.

#include "defines.h"
#include "euler_angles.h"

/**
 * Returns the aircraft heading angle (a.k.a., yaw angle) in degrees relative
 * to geographic north.
 * Values returned range from 0 - 360 degrees, positive clockwise.
 */
uint16_t get_geo_heading_angle() {
    struct relative2D matrix_accum ;
    matrix_accum.x = rmat[4] ;
    matrix_accum.y = rmat[1] ;
    int16_t accum = rect_to_polar(&matrix_accum) ;	// binary angle (0 to 180, -1 to -179 for complete 360 degrees)
    int16_t angle = (accum * 180 + 64) >> 7 ;	// Angle measured counter clockwise, 0=Geographic North
    angle = -angle ;				// Angle measure clockwise, 0=Geographic North
    if (angle > 360 ) {
        angle = angle - 360 ;
    } else if (angle < 0   ) {
        angle = angle + 360 ;
    }
    return angle;	// Aircraft heading in degrees from geographic north
}

/**
 * Returns the aircraft magnetic heading angle (a.k.a., yaw angle) in degrees relative
 * to magnetic north.
 * Values returned range from 0 - 360 degrees, positive clockwise.
 */
uint16_t get_mag_heading_angle() {
    uint16_t angle = get_geo_heading_angle() - MAGNETICDECLINATION;
    if (angle > 360 ) {
        angle = angle - 360 ;
    } else if (angle < 0   ) {
        angle = angle + 360 ;
    }
    return angle;	// Aircraft heading in degrees from magnetic north
}

euler_struct get_current_aircraft_orientation()
{
  int32_t earth_pitch; // pitch in binary angles ( 0-255 is 360 degreres)
  int32_t earth_roll; // roll of the plane with respect to earth frame
  struct relative2D matrix_accum;
  euler_struct orientation;

  //  Roll
  //  Earth Frame of Reference
  matrix_accum.x = rmat[8];
  matrix_accum.y = rmat[6];
  earth_roll = rect_to_polar(&matrix_accum); // binary angle (0 - 256 = 360 degrees)
  earth_roll = (-earth_roll * BYTECIR_TO_DEGREE) >> 16; // switch polarity, convert to -180 - 180 degrees

  //  Pitch
  //  Earth Frame of Reference
  //  Note that we are using the matrix_accum.x
  //  left over from previous rect_to_polar in this calculation.
  //  so this Pitch calculation must follow the Roll calculation
  matrix_accum.y = rmat[7];
  earth_pitch = rect_to_polar(&matrix_accum); // binary angle (0 - 256 = 360 degrees)
  earth_pitch = (-earth_pitch * BYTECIR_TO_DEGREE) >> 16; // switch polarity, convert to -180 - 180 degrees

  orientation.yaw = get_mag_heading_angle(); // yaw (magnetic)
  orientation.pitch = earth_pitch;
  orientation.roll = earth_roll;

  return orientation;
}