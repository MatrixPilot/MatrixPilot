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
#include <libq.h>

int16_t successive_interpolation(int16_t X, int16_t X1, int16_t X2, int16_t Y1, int16_t Y2)
{
	int16_t X1temp = X1;
	int16_t X2temp = X2;
	int16_t Y1temp = Y1;
	int16_t Y2temp = Y2;
	int16_t Xtemp;

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
		int16_t deltaX = (X2temp - X1temp) >> 1;
		int16_t deltaY = (Y2temp - Y1temp) >> 1;
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


// Succesive interpolation between X inputs of Y outputs
_Q16 successive_interpolation_Q16(_Q16 X, _Q16 X1, _Q16 X2, _Q16 Y1, _Q16 Y2)
{
	_Q16 X1temp = X1;
	_Q16 X2temp = X2;
	_Q16 Y1temp = Y1;
	_Q16 Y2temp = Y2;
	_Q16 Xtemp;

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
		_Q16 deltaX = (X2temp - X1temp) >> 1;
		_Q16 deltaY = (Y2temp - Y1temp) >> 1;
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
