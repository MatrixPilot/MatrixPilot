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


#include "dcmTypes.h"
#include "mathlibNAV.h"
#include "mathlib.h"
#if (WIN == 1 || NIX == 1)
#include "../Tools/MatrixPilot-SIL/SIL-udb.h"
#endif // (WIN == 1 || NIX == 1)

#ifndef NULL
#define NULL 0
#endif

//  math libraray

int16_t add_saturate(int16_t x , int16_t y , int16_t margin)
{
	int32_t total = ((int32_t) x );
	total = total + ((int32_t) y) ;
	int32_t maximum = ((int32_t)(32767-margin));
	int32_t minimum = ((int32_t)(-32767+margin));
	if ((total<=maximum)&&(total>=minimum))
	{
		return ((int16_t)total );
	}
	else if ( total > 0 )
	{
		return ((int16_t)maximum) ;
	}
	else
	{
		return ((int16_t)minimum) ;
	}
}

int16_t subtract_saturate(int16_t x , int16_t y , int16_t margin)
{
	int32_t total = ((int32_t) x );
	total = total - ((int32_t) y) ;
	int32_t maximum = ((int32_t)(32767-margin));
	int32_t minimum = ((int32_t)(-32767+margin));
	if ((total<=maximum)&&(total>=minimum))
	{
		return ((int16_t)total );
	}
	else if ( total > 0 )
	{
		return ((int16_t)maximum) ;
	}
	else
	{
		return ((int16_t)minimum) ;
	}
}



#define RADIANTOCIRCULAR 10430

void vect2_16x16_rotate(vect2_16t* vector, const vect2_16t* rotate)
{
	// rotate the vector by the implicit angle of rotate
	// rotate is RMAX*[ cosine(theta), sine(theta) ], theta is the desired rotation angle
	// upon exit, the vector [ x, y ] will be rotated by the angle theta.
	// theta is positive in the counter clockwise direction.
	// This routine can also be used to do a complex multiply, with 1/RMAX scaling,
	// and where vector and rotate are viewed as complex numbers
	union longww accum;
	int16_t newx;

	accum.WW = ((__builtin_mulss(rotate->x, vector->x) - __builtin_mulss(rotate->y, vector->y)) << 2);
	newx = accum._.W1;
	accum.WW = ((__builtin_mulss(rotate->y, vector->x) + __builtin_mulss(rotate->x, vector->y)) << 2);
	vector->y = accum._.W1;
	vector->x = newx;
}

void vect2_32x16_rotate(vect2_32t* vector, const vect2_16t* rotate)
{
	// same as vect2_16x16_rotate, except the first vector is 32 bits
	int32_t newx;

	newx      = long_scale(vector->x, rotate->x) - long_scale(vector->y, rotate->y);
	vector->y = long_scale(vector->x, rotate->y) + long_scale(vector->y, rotate->x);
	vector->x = newx;
}

void vect2_16phi_rotate(vect2_16t* vector, int8_t angle)
{
	// rotate the vector by an angle, phi
	// where vector is [ x, y ], angle is in byte-circular scaling
	vect2_16t rotate;

	rotate.y = sine(angle);
	rotate.x = cosine(angle);
	vect2_16x16_rotate(vector, &rotate);
}

int8_t vect2_polar(polar_16t* polar, const vect2_16t* vector)
{
	// Convert from rectangular to polar coordinates using "CORDIC" arithmetic, which is basically
	// a binary search for the angle.
	// As a by product, the xy is rotated onto the x axis, so that y is driven to zero,
	// and the magnitude of the vector winds up as the x component.
	int16_t scaleShift = 0;
	int8_t theta = 0;
	int8_t delta_theta = 64;
	int8_t theta_rot;
//	int8_t steps = 7;
	vect2_16t xy;

	if (((vector->x) <  255) && ((vector->x) > -255) &&
		((vector->y) <  255) &&	((vector->y) > -255))
	{
		scaleShift = 6;
	}
	xy.x = (vector->x << scaleShift);
	xy.y = (vector->y << scaleShift);
#if 0
	while (steps > 0) // TODO: we could do away with 'steps' and instead test delta_theta for non-zero at the end of a do loop
	{
		theta_rot = delta_theta;
		if (xy.y > 0) theta_rot = -theta_rot;
		vect2_16phi_rotate(&xy, theta_rot);
		theta += theta_rot;
		delta_theta = (delta_theta >> 1);
		steps--;
	}
#else
	do {
		theta_rot = delta_theta;
		if (xy.y > 0) theta_rot = -theta_rot;
		vect2_16phi_rotate(&xy, theta_rot);
		theta += theta_rot;
		delta_theta = (delta_theta >> 1);
	} while (delta_theta);
#endif
	if (xy.y > 0) theta--;
	if (polar != NULL) {
		polar->r = (xy.x >> scaleShift);
//		polar->p = (xy.y >> scaleShift);
		polar->p = -theta;
	}
	return (-theta);
}

int16_t vect2_polar_16(polar_32t* polar, const vect2_16t* vector)
{
	// Convert from rectangular to polar coordinates using "CORDIC" arithmetic, which is basically
	// a binary search for the angle.
	// As a by product, the xy is rotated onto the x axis, so that y is driven to zero,
	// and the magnitude of the vector winds up as the x component.
	// Returns a value as a 16 bit "circular" so that 180 degrees yields 2**15
	int16_t scaleShift = 0;
	int16_t theta16;
	int8_t theta = 0;
	int8_t delta_theta = 64;
	int8_t theta_rot;
//	int8_t steps = 7;
	vect2_16t xy;

	if (((vector->x) <  255) && ((vector->x) > -255) &&
		((vector->y) <  255) &&	((vector->y) > -255))
	{
		scaleShift = 6;
	}
	xy.x = (vector->x << scaleShift);
	xy.y = (vector->y << scaleShift);
#if 0
	while (steps > 0) // TODO: we could do away with 'steps' and instead test delta_theta for non-zero at the end of a do loop
	{
		theta_rot = delta_theta;
		if (xy.y > 0) theta_rot = -theta_rot;
		vect2_16phi_rotate(&xy, theta_rot);
		theta += theta_rot;
		delta_theta = (delta_theta >> 1);
		steps--;
	}
#else
	do {
		theta_rot = delta_theta;
		if (xy.y > 0) theta_rot = -theta_rot;
		vect2_16phi_rotate(&xy, theta_rot);
		theta += theta_rot;
		delta_theta = (delta_theta >> 1);
	} while (delta_theta);
#endif
	theta = -theta;
	theta16 = theta << 8;
	if (xy.x > 0)
	{
		theta16 += __builtin_divsd(__builtin_mulss(RADIANTOCIRCULAR, xy.y), xy.x);
	}
	if (polar != NULL) {
		polar->r = (xy.x >> scaleShift);
//		polar->p = (xy.y >> scaleShift);
		polar->p = theta16;
	}
	return (theta16);
}

uint16_t vect2_16_mag(const vect2_16t* v)
{
	uint32_t magsqr = __builtin_mulss(v->x, v->x) + __builtin_mulss(v->y, v->y);
	return sqrt_long(magsqr);
}

uint16_t vect3_16_mag(const vect3_16t* v)
{
	uint32_t magsqr = __builtin_mulss(v->x, v->x) + __builtin_mulss(v->y, v->y) + __builtin_mulss(v->z, v->z);
	return sqrt_long(magsqr);
}

uint16_t vect2_16_norm(vect2_16t* result, const vect2_16t* v)
{
	uint16_t mag = vect2_16_mag(v);
	if (mag > 0)
	{
		result->x = __builtin_divsd(__builtin_mulss(RMAX, v->x), mag);
		result->y = __builtin_divsd(__builtin_mulss(RMAX, v->y), mag);
	}
	else
	{
		result->x = result->y = 0;
	}
	return mag;
}

uint16_t vect3_16_norm(vect3_16t* result, const vect3_16t* v)
{
	uint16_t mag = vect3_16_mag(v);
	if (mag > 0)
	{
		result->x = __builtin_divsd(__builtin_mulss(RMAX, v->x), mag);
		result->y = __builtin_divsd(__builtin_mulss(RMAX, v->y), mag);
		result->z = __builtin_divsd(__builtin_mulss(RMAX, v->z), mag);
	}
	else
	{
		result->x = result->y = result->z = 0;
	}
	return mag;
}
