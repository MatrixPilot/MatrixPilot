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


#ifndef RMAT_H
#define RMAT_H

//		These are the routines for maintaining a direction cosine matrix
//		that can be used to transform vectors between the earth and plane
//		coordinate systems. The 9 direction cosines in the matrix completely
//		define the orientation of the plane with respect to the earth.
//		The inverse of the matrix is equal to its transpose. This defines
//		the so-called orthogonality conditions, which impose 6 constraints on
//		the 9 elements of the matrix.

//	All numbers are stored in 2.14 format.
//	Vector and matrix libraries work in 1.15 format.
//	This combination allows values of matrix elements between -2 and +2.
//	Multiplication produces results scaled by 1/2.


// Outside of libDCM, these should all be treated as read-only

#if (MAG_YAW_DRIFT == 1)
extern fractional magFieldEarth[3];
void udb_magnetometer_callback(void);
#endif

extern fractional rmat[];			//	gyro rotation vector:
extern fractional omegaAccum[];		//	accumulator for computing adjusted omega:
extern fractional omegagyro[];
extern fractional accelEarth[];		//  acceleration, as measured in GPS earth coordinate system
extern fractional gplane[];
extern fractional dirovergndHGPS[];	//	horizontal velocity over ground, as measured by GPS (Vz = 0 )
extern fractional dirovergndHRmat[];//	horizontal direction over ground, as indicated by Rmatrix

extern union intbb dcm_declination_angle;	// Declination +-32767 = +-360deg

void dcm_init_rmat(void);
void dcm_run_imu_step(void);

void read_gyros(void);
void read_accel(void);


#endif // RMAT_H
