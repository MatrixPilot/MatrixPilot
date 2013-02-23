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

#ifndef _AIRSPEEDPITOT_H_
#define	_AIRSPEEDPITOT_H_

#include "GenericTypeDefs.h"


typedef struct
{
    INT16 zeroOffset;
    float lpf_1_coef;
    float lpf_2_coef;
    float oneMinusLpf_2_coef;
    float scalar;
    INT16 filteredAdcValue;
    INT16 value;
} AirspeedPitot;

void udb_init_pitot(void);
void setAirspeedUsingAdcValue(INT16 adcValue);
void start_Calibration(void);

#endif	// _AIRSPEEDPITOT_H_

