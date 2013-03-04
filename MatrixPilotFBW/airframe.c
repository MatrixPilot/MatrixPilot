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


#include "../MatrixPilot/defines.h"
#include "fbw_options.h"
#include "airframe.h"
//#include "airspeedCntrlFBW.h"
#include "../libDCM/libDCM.h"
#include "minifloat.h"

#define AFRM_MULT1		2.0
#define AFRM_MULT2		128.0

#define INVERSE_GLIDE_RATIO (RMAX / CRUISE_GLIDE_RATIO)

#define AFRM_EFFECTIVE_AREA (AFRM_WING_AREA * AFRM_EFFECTIVE_AREA_RATIO)



// This constant is directly related to wing loading so has limited dynamic range
#define AFRM_CL_CALC_CONST	(2.0 * AFRM_AIRCRAFT_MASS / (AFRM_AIR_DENSITY * AFRM_EFFECTIVE_AREA))

// Adjust the calcualtion constant from g to m/s**2 and to RMAX with MULT1 scaling
#define AFRM_CL_CALC_CONST_SCALED (RMAX * (AFRM_MULT1 * AFRM_CL_CALC_CONST * AFRM_GRAVITY / GRAVITY))

#define AFRM_ACCN_CALC_CONST_SCALED (RMAX / ( AFRM_CL_CALC_CONST * AFRM_GRAVITY ))

#define AFRM_CL_CALC_CONST_G (AFRM_CL_CALC_CONST / GRAVITY)

int successive_interpolation(int X, int X1, int X2, int Y1, int Y2)
{
	int X1temp = X1;
	int X2temp = X2;
	int Y1temp = Y1;
	int Y2temp = Y2;
	int Xtemp;

	// Test for out of limit.  Return limit if so.
	if( (X2-X1) > 0)
	{
		if(X > X2) return Y2;
		if(X < X1) return Y1;
	}
	else
	{
		if(X < X2) return Y2;
		if(X > X1) return Y1;
	}

	// Repeat approximation until magnitude difference between X estiamtes is <= 1
	while( ((X2temp - X1temp) >> 1) != 0)
	{ 
		int deltaX = (X2temp - X1temp) >> 1;
		int deltaY = (Y2temp - Y1temp) >> 1;
		Xtemp  = X - X1temp;

		if(deltaX > 0)
		{
			if(Xtemp > deltaX)
			{
				X1temp += deltaX;
				Y1temp += deltaY;
			}
			else
			{
				X2temp -= deltaX;
				Y2temp -= deltaY;
			}
		}
		else
		{
			if(Xtemp < deltaX)
			{
				X1temp -= deltaX;
				Y1temp -= deltaY;
			}
			else
			{
				X2temp += deltaX;
				Y2temp += deltaY;
			}
		}

	}

	// Last selection is when |X1-X2| <= 1
	if(X == X1temp)
		return Y1temp;
	else
		return Y2temp;
}


minifloat afrm_aspdcm_to_m(int airspeedCm)
{
	minifloat aspdmf = ltomf(airspeedCm);
	minifloat tempmf = {164, -6};		// 0.01
	aspdmf = mf_mult(aspdmf, tempmf);
	return aspdmf;
}


// Get the required lift coefficient for the airspeed
// airspeed in cm/s
// load in GRAVITY scale
minifloat afrm_get_required_Cl_mf(int airspeed, int load)
{
	minifloat Clmf = {0,0};
	union longww temp;

	// If airsped is lower than 1m/s then don't try this calculation
	if(airspeed < 100) return Clmf;

	minifloat aspdmf = afrm_aspdcm_to_m(airspeed);
	minifloat aspd2mf = mf_sqr(aspdmf);				// Airspeed^2

	// Acceleration is in GRAVITY units.  Rescale to 2048*2^16 = 2^27
	temp.WW = __builtin_mulss( load , (RMAX * 2048.0/GRAVITY) ) << 2;
	minifloat loadmf = ltomf(temp.WW);
	loadmf.exp -= 27;								// Rescale the exponent against the previous 2^27

	temp.WW = (long) (AFRM_CL_CALC_CONST_G * RMAX);
	minifloat constmf = ltomf(temp.WW);
	constmf.exp -= 14;		// Rescale the exponent against the previous RMAX scale

	Clmf = mf_mult(loadmf, constmf);		// load * Cl calc constant
		
	Clmf = mf_div(Clmf, aspd2mf);					// Cl = load * Cl calc constant / aispeed^2

	return Clmf;
}

// Get the required lift coefficient for the airspeed
// airspeed in cm/s
// acceleration in GRAVITY scale
fractional afrm_get_required_Cl(int airspeed, int acceleration)
{
	union longww temp;
	
	// calculate airspeed squared after scaling to m/s
	int aspd2;			
	temp.WW = __builtin_mulss( 0.01 * RMAX , airspeed) << 2;
	if(temp._.W0 & 0x8000) temp._.W1++;					// Correct for underflow.
	temp.WW = __builtin_mulss( temp._.W1 , temp._.W1 );
	aspd2 = temp._.W0;

	// If airspeed is zero, return error.
	if(aspd2 == 0) return 0x7FFF;

	// RMAX / airspeed**2
	temp.WW = RMAX;
	temp._.W1 = __builtin_divsd(temp.WW, aspd2);
	
	// Multiply by ( acceleration / multiplier 2)
	temp._.W0 = acceleration >> 6;
	temp.WW = __builtin_mulss( temp._.W1 , temp._.W0 );

	// Scale by constant defined by wing area, mass and air density
	temp._.W1 = AFRM_CL_CALC_CONST_SCALED;		// TODO - remove test code
	temp.WW = __builtin_mulsu(temp._.W0, AFRM_CL_CALC_CONST_SCALED) << (6-1);	// scale by Mult2/Mult1

	return temp._.W1;	
}

// Get the required lift coefficient for the airspeed
// airspeed in cm/s
// acceleration in 
fractional afrm_get_max_accn(int airspeed, fractional Clmax)
{
	union longww temp;

	// calculate airspeed squared after scaling to m/s
	int aspd2;			
	temp.WW = __builtin_mulss( 0.01 * RMAX , airspeed) << 2;
	if(temp._.W0 & 0x8000) temp._.W1++;					// Correct for underflow.
	temp.WW = __builtin_mulss( temp._.W1 , temp._.W1 );
	aspd2 = temp._.W0;

	// mass,wing area,density constant * Clmax
	temp._.W0 = AFRM_ACCN_CALC_CONST_SCALED;
	temp._.W0 <<= 4;
	temp.WW = __builtin_mulss( Clmax , temp._.W0 );

	// *aspd2
	temp.WW = __builtin_mulss( temp._.W1 , aspd2 ) >> (4 - 1);

	if(temp.WW > RMAX) return RMAX;
	if(temp.WW < -RMAX) return -RMAX;
	
	return temp._.W0;
}


fractional afrm_get_required_alpha(int airspeed, fractional Cl)
{
	return successive_interpolation(Cl, 
			normal_polars[0].points[0].Cl, 
			normal_polars[0].points[1].Cl, 
			normal_polars[0].points[0].alpha, 
			normal_polars[0].points[1].alpha);
}

// Calculate the expected descent rate in cm/s at the given airspeed in cm/s
// if the aircraft were gliding.  This is a measure of expected energy loss.
// It is also known as Cl/Cd or the Lift/Drag coefficent.
// Output is positive for descending.
int expected_glide_descent_rate(int airspeed)
{
	// Get the expected descent rate
	union longww temp;
	temp.WW = __builtin_muluu( air_speed_3DIMU, INVERSE_GLIDE_RATIO);
	temp.WW <<= 2;
	return temp._.W1;
}

// Calculate the expected climb rate depending on a throttle setting and airspeed
// glide_ratio.  rate and airspeed in cm/s.  Descent rate is positive falling.
// Returned value is positive rising in cm/s
int feedforward_climb_rate(fractional throttle, int glide_descent_rate, int airspeed)
{
	// TODO - Correct the assumption that scale changes with descent rate.
	int rateScale = (MAX_THROTTLE_CLIMB_RATE * 100) + glide_descent_rate;

	union longww temp;
	temp.WW = __builtin_mulss( throttle, rateScale);
	temp.WW <<= 2;
	temp._.W1 -= glide_descent_rate;
	return temp._.W1;
}


