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
#if (WIN == 1 || NIX == 1)
#include "../Tools/MatrixPilot-SIL/SIL-udb.h"
#endif // (WIN == 1 || NIX == 1)

//  math libraray

#define RADIANTOCIRCULAR 10430

//  sine table for angles from zero to pi/2 with an increment of pi/128 radian.
//  sine values are multiplied by 2**14
const int16_t sintab[] = { 0,
	402,   804,   1205,  1606,  2006,  2404,  2801,   3196,  3590,  3981,
	4370,  4756,  5139,  5520,  5897,  6270,  6639,   7005,  7366,  7723,
	8076,  8423,  8765,  9102,  9434,  9760,  10080, 10394, 10702, 11003,
	11297, 11585, 11866, 12140, 12406, 12665, 12916, 13160, 13395, 13623,
	13842, 14053, 14256, 14449, 14635, 14811, 14978, 15137, 15286, 15426,
	15557, 15679, 15791, 15893, 15986, 16069, 16143, 16207, 16261, 16305,
	16340, 16364, 16379, 16384
};


int16_t sine(int8_t angle)
{
	// returns(2**14)*sine(angle), angle measured in units of pi/128 radians
	int16_t angle_int;
	angle_int = angle;

	if (angle_int >= 0)
	{
		if (angle_int > 64)
		{
			return (sintab[128-angle_int]);
		}
		else
		{
			return (sintab[angle_int]);
		}
	}
	else
	{
		angle_int = -angle_int;
		if (angle_int > 64)
		{
			return (-sintab[128 - angle_int]);
		}
		else
		{
			return (-sintab[angle_int]);
		}
	}
}

int8_t arcsine(int16_t y)  // arcsine takes the y coordinate of an x,y point and returns an angle
{
	// returns the inverse sine of y
	// y is in Q2.14 format, 16384 is maximum value
	// returned angle is a byte circular
	int8_t angle = 32;
	int8_t doubleangle = 64;
	int8_t step = 32;
	int8_t sign;

	if (y > 0)
	{
		sign = 1;
	}
	else
	{
		sign = - 1;
		y = - y;
	}
	if (y == 16384)
	{
		return sign * 64;
	}
	while (step > 0)
	{
		angle = doubleangle >> 1;
		if (y == sine(angle))
		{
			return sign * angle;
		}
		else if (y > ((sine(angle) + sine(angle - 1)) >> 1))
		{
			doubleangle += step;
		}
		else
		{
			doubleangle -= step;
		}
		step = step >> 1;
	}
	return sign * (doubleangle >> 1);
}

int16_t cosine(int8_t angle)
{
	return (sine(angle+64));
}

void rotate_2D_vector_by_vector(int16_t vector[2], int16_t rotate[2])
{
	// rotate the vector by the implicit angle of rotate
	// vector[0] is x, vector[1] is y
	// rotate is RMAX*[ cosine(theta), sine(theta) ], theta is the desired rotation angle
	// upon exit, the vector [ x, y ] will be rotated by the angle theta.
	// theta is positive in the counter clockwise direction.
	// This routine can also be used to do a complex multiply, with 1/RMAX scaling,
	// and where vector and rotate are viewed as complex numbers
	int16_t newx, newy;
	union longww accum;

	accum.WW = ((__builtin_mulss(rotate[0], vector[0]) - __builtin_mulss(rotate[1], vector[1])) << 2);
	newx = accum._.W1;
	accum.WW = ((__builtin_mulss(rotate[1], vector[0]) + __builtin_mulss(rotate[0], vector[1])) <<2 );
	newy = accum._.W1;
	vector[0] = newx;
	vector[1] = newy;
}

void rotate_2D_long_vector_by_vector(int32_t vector[2], int16_t rotate[2])
{
	// same as rotate_2D_vector_by_vector, except the first vector is 32 bits
	int32_t newx, newy;

	newx = long_scale(vector[0], rotate[0]) - long_scale(vector[1], rotate[1]);
	newy = long_scale(vector[0], rotate[1]) + long_scale(vector[1], rotate[0]);
	vector[0] = newx;
	vector[1] = newy;
}

void rotate_2D_vector_by_angle(int16_t vector[2], int8_t angle)
{
	// rotate the vector by angle,
	// where vector is [ x, y ], angle is in byte-circular scaling
	int16_t rotate[2];

	rotate[1] = sine(angle);
	rotate[0] = cosine(angle);
	rotate_2D_vector_by_vector(vector, rotate);
}

void rotate_2D(struct relative2D* xy, int8_t angle)
{
	// rotates xy by angle, measured in a counter clockwise sense.
	// A mathematical angle of plus or minus pi is represented digitally as plus or minus 128.
	int16_t cosang, sinang, newx, newy;
	union longww accum;

	sinang = sine(angle);
	cosang = cosine(angle);
	accum.WW = ((__builtin_mulss(cosang, xy->x) - __builtin_mulss(sinang, xy->y)) << 2) + 0x00008000;
	newx = accum._.W1;
	accum.WW = ((__builtin_mulss(sinang, xy->x) + __builtin_mulss(cosang, xy->y)) << 2) + 0x00008000;
	newy = accum._.W1;
	xy->x = newx;
	xy->y = newy;
}

int8_t rect_to_polar(struct relative2D* xy)
{
	// Convert from rectangular to polar coordinates using "CORDIC" arithmetic, which is basically
	// a binary search for the angle.
	// As a by product, the xy is rotated onto the x axis, so that y is driven to zero,
	// and the magnitude of the vector winds up as the x component.

	int8_t theta = 0;
	int8_t delta_theta = 64;
	int8_t theta_rot;
	int8_t steps = 7;
	int16_t scaleShift;

	if (((xy-> x) <  255) &&
		((xy-> x) > -255) &&
		((xy-> y) <  255) &&
		((xy-> y) > -255))
	{
		scaleShift = 6;
		xy->x = (xy->x << 6);
		xy->y = (xy->y << 6);
	}
	else
	{
		scaleShift = 0;
	}
	while (steps > 0)
	{
		theta_rot = delta_theta;
		if (xy->y  > 0) theta_rot = -theta_rot;
		rotate_2D(xy, theta_rot);
		theta += theta_rot;
		delta_theta = (delta_theta >> 1);
		steps--;
	}
	if (xy->y > 0) theta--;
	xy->x = (xy->x >> scaleShift);
	xy->y = (xy->y >> scaleShift);
	return (-theta);
}

int16_t rect_to_polar16(struct relative2D* xy)
{
	// Convert from rectangular to polar coordinates using "CORDIC" arithmetic, which is basically
	// a binary search for the angle.
	// As a by product, the xy is rotated onto the x axis, so that y is driven to zero,
	// and the magnitude of the vector winds up as the x component.
	// Returns a value as a 16 bit "circular" so that 180 degrees yields 2**15
	int16_t scaleShift;
	int16_t theta16;
	int8_t theta = 0;
	int8_t delta_theta = 64;
	int8_t theta_rot;
	int8_t steps = 7;

	if (((xy-> x) < 255)  &&
		((xy-> x) > -255) &&
		((xy-> y) < 255)  &&
		((xy-> y) > -255))
	{
		scaleShift = 6;
		xy->x = (xy->x << 6);
		xy->y = (xy->y << 6);
	}
	else
	{
		scaleShift = 0;
	}
	while (steps > 0)
	{
		theta_rot = delta_theta;
		if (xy->y  > 0) theta_rot = -theta_rot;
		rotate_2D(xy, theta_rot);
		theta += theta_rot;
		delta_theta = (delta_theta >> 1);
		steps--;
	}
	theta = -theta;
	theta16 = theta << 8;
	if (xy->x > 0)
	{
		theta16 += __builtin_divsd(__builtin_mulss(RADIANTOCIRCULAR, xy->y), xy->x);
	}
	xy->x = (xy->x >> scaleShift);
	xy->y = (xy->y >> scaleShift);
	return (theta16);
}

uint16_t sqrt_int(uint16_t sqr)
{
	// based on Heron's algorithm
	uint16_t binary_point = 0;
	uint16_t result = 255;
	int16_t iterations = 3;

	if (sqr == 0)
	{
		return 0;
	}
	while ((sqr & 0xC000) == 0) // shift left to get a 1 in the 2 MSbits
	{
		sqr = sqr*4;            // shift 2 bits
		binary_point++;         // track half of the number of bits shifted
	}
	sqr = sqr/2;                // for convenience, Herons formula is result = (result + sqr/result) / 2
	while (iterations)
	{
		iterations--;
		result = result/2 + sqr/result;
	}
	result = result >> binary_point; // shift result right to account for shift left of sqr 
	return result;
}

uint16_t sqrt_long(uint32_t sqr)
{
	// based on Heron's algorithm
	uint16_t binary_point = 0;
	uint16_t result = 65535;    // need to start high and work down to avoid overflow in divud
	int16_t iterations = 3;     // thats all you need

	if (sqr < 65536)            // use the 16 bit square root
	{
		return sqrt_int((uint16_t)sqr);
	}
	while ((sqr & 0xC0000000) == 0) // shift left to get a 1 in the 2 MSbits
	{
		sqr = sqr << 2;
		binary_point++;         // track half of the number of bits shifted
	}
	sqr = sqr >> 1;             // for convenience, Herons formula is result = (result + sqr/result) / 2
	while (iterations)
	{
		iterations--;
		result = result/2 + __builtin_divud(sqr, result);
	}
	result = result >> binary_point; // shift result right to account for shift left of sqr 
	return result;
}

uint16_t vector2_mag(int16_t x, int16_t y)
{
	uint32_t magsqr;

	magsqr = __builtin_mulss(x, x) + __builtin_mulss(y, y);
	return sqrt_long(magsqr);
}

uint16_t vector3_mag(int16_t x, int16_t y, int16_t z)
{
	uint32_t magsqr;

	magsqr = __builtin_mulss(x, x) + __builtin_mulss(y, y) + __builtin_mulss(z, z);
	return sqrt_long(magsqr);
}

uint16_t vector2_normalize(int16_t result[], int16_t input[])
{
	uint16_t half_magnitude;
	uint16_t magnitude;

	magnitude = vector2_mag(input[0], input[1]);
	half_magnitude = magnitude/2 ; // needed because divsd requires two signed values
	if (half_magnitude > 0)
	{
		result[0] = __builtin_divsd(__builtin_mulss(RMAX, input[0]/2), half_magnitude);
		result[1] = __builtin_divsd(__builtin_mulss(RMAX, input[1]/2), half_magnitude);
	}
	else
	{
		result[0]=result[1]=0;
	}
	return magnitude;
}

uint16_t vector3_normalize(int16_t result[], int16_t input[])
{
	uint16_t half_magnitude;
	uint16_t magnitude;

	magnitude = vector3_mag(input[0], input[1], input[2]);
	half_magnitude = magnitude/2; // needed because divsd requires two signed values
	if (half_magnitude > 0)
	{
		result[0] = __builtin_divsd(__builtin_mulss(RMAX, input[0]/2), half_magnitude);
		result[1] = __builtin_divsd(__builtin_mulss(RMAX, input[1]/2), half_magnitude);
		result[2] = __builtin_divsd(__builtin_mulss(RMAX, input[2]/2), half_magnitude);
	}
	else
	{
		result[0] = result[1] = result[2] = 0;
	}
	return magnitude;
}

int32_t long_scale(int32_t arg1, int16_t arg2)
{
	// returns arg1*arg2/RMAX
	// usually this is used where arg2 is a Q14.2 fractional number
	int8_t sign_result = 1;
	int32_t product;
	union longww accum;
	union longww arg1ww;

	arg1ww.WW = arg1;
	if (arg1ww._.W1 < 0)
	{
		sign_result = -sign_result;
		arg1ww.WW = -arg1ww.WW;
	}
	if (arg2 < 0)
	{
		sign_result = -sign_result;
		arg2 = -arg2;
	}
	product  = __builtin_muluu(arg2, arg1ww._.W1);
	product <<= 2;
	accum.WW = __builtin_muluu(arg2, arg1ww._.W0);
	accum.WW <<= 2;
	accum._.W0 = accum._.W1;
	accum._.W1 = 0;
	product += accum.WW;
	if (sign_result > 0)
	{
		return product;
	}
	else
	{
		return -product;
	}
}

int16_t find_first_bit_int16(int16_t argument)
{
	// finds first bit position in the absolute value of a 16 bit signed integer
	// returns the position of the first significant bit
	// MSB position is 0
	// LSB position is 15
	// returns 16 for input=0 and 0 for input=0x8000H
	int16_t input;

	if (argument == 0)
	{
		return 16;
	}
	if (argument < 0)
	{
		input = - argument;
	}
	else
	{
		input = argument;
	}
	return FindFirstBitFromLeft(input) - 1;
}

int16_t find_first_bit_int32(int32_t argument)
{
	// finds first bit position in the absolute value of a 32 bit signed integer
	// returns the position of the first significan bit
	// MSB position is 0
	// LSB position is 31
	// returns 32 for input=0 and 0 for input=0x80000000H
	union longww input;
	int16_t binw0;
	int16_t binw1;

	if (argument == 0)
	{
		return 32;
	}
	if (argument < 0)
	{
		input.WW = -argument;
	}
	else
	{
		input.WW = argument;
	}
	binw1 = FindFirstBitFromLeft(input._.W1);
	binw0 = FindFirstBitFromLeft(input._.W0);
	if (binw1 == 0)
	{
		return binw0 + 15;
	}
	else
	{
		return binw1 - 1;
	}
}

void VectorCross(int16_t * dest, int16_t * src1, int16_t * src2)
{
	// Implement the cross product. *dest = *src1X*src2;
	union longww crossaccum;
	crossaccum.WW = __builtin_mulss(src1[1], src2[2]);
	crossaccum.WW -= __builtin_mulss(src1[2], src2[1]);
	crossaccum.WW *= 4;
	dest[0] = crossaccum._.W1;
	crossaccum.WW = __builtin_mulss(src1[2], src2[0]);
	crossaccum.WW -= __builtin_mulss(src1[0], src2[2]);
	crossaccum.WW *= 4;
	dest[1] = crossaccum._.W1;
	crossaccum.WW = __builtin_mulss(src1[0], src2[1]);
	crossaccum.WW -= __builtin_mulss(src1[1], src2[0]);
	crossaccum.WW *= 4;
	dest[2] = crossaccum._.W1;
}
