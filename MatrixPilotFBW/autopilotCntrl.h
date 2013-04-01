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

// Responsible for synthesizing motion demands according to inputs and limits

#ifndef AUTOPILOTCNTRL_H
#define AUTOPILOTCNTRL_H


// Transform demand positions and rotations into 
void autopilotCntrl(void);

void get_demand_rmat(int* prmat);

struct relative2D get_auto_rollDemand(void);
struct relative2D get_auto_pitchDemand(void);
int get_auto_rollRate(void);
int get_auto_pitchRate(void);

//struct relative2D 

#endif

