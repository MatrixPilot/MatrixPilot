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

#include "defines.h"
#include "airframe.h"
#include "airframe_options.h"
#include "inputCntrl.h"

#if(USE_AIRFRAME == 1)

aero_condition_point camber_aero_data[] = 
	{
	{-RMAX,	{1000, 2000, 1200, 0}},
	{0,		{1200, 3000, 1500, 0}},
	{RMAX,	{1500, 5000, 2500, 0}},
	};

int camber_aero_datapoints = (sizeof(camber_aero_data) / sizeof(aero_condition_point));

#endif	//USE_AIRFRAME == 1

