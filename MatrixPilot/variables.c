// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"


//	Variables.

unsigned int rise[MAX_INPUTS+1] ;	// rising edge clock capture for radio inputs
int pwIn[MAX_INPUTS+1] ;	// pulse widths of radio inputs
int pwTrim[MAX_INPUTS+1] ;	// initial pulse widths for trimming
int pwOut[MAX_OUTPUTS+1] ;	// pulse widths for servo outputs

int pitch_control, roll_control, yaw_control, altitude_control ;

int waggle = 0 ;
int calib_timer, standby_timer ;
int pulsesselin = 0 ;
int gps_data_age;

boolean timer_5_on = 0 ;
boolean needSaveExtendedState = 0 ;
int defaultCorcon = 0 ;
