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

// Defines a set of functions that return vehicle euler_angles.
// Provides centralized logic for commonly needed code.


#ifndef EULER_ANGLES_H
#define	EULER_ANGLES_H


/**
 * Returns the aircraft heading angle (a.k.a., yaw angle) in degrees relative
 * to geographic north.
 * Values returned range from 0 - 360 degrees, positive clockwise.
 */
uint16_t get_geo_heading_angle();


#endif	// EULER_ANGLES_H
