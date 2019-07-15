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


#ifndef DEADRECKONING_H
#define DEADRECKONING_H


extern uint16_t air_speed_3DIMU;
extern int16_t total_energy;
extern fractional locationErrorEarth[3];

extern union longww IMUlocationx, IMUlocationy, IMUlocationz;
extern union longww IMUvelocityx, IMUvelocityy, IMUvelocityz;
extern union longww IMUintegralAccelerationx;
extern union longww IMUintegralAccelerationy;
// Modif gfm #4 fusion
extern union longww IMUintegralAccelerationz;
// fin modif gfm #4 fusion

extern int16_t forward_ground_speed;

#define IMUheight IMUlocationz._.W1


void dead_reckon(void);


#endif // DEADRECKONING_H
