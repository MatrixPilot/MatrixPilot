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


////////////////////////////////////////////////////////////////////////////////
// Control code - rollCntrl.c, pitchCntrl.c, yawCntrl.c, altitudeCntrl.c

extern int16_t pitch_control;
extern int16_t roll_control;
extern int16_t yaw_control;
extern int16_t throttle_control;
//extern union longww throttleFiltered;
//extern int16_t pitchAltitudeAdjust;

void rollCntrl(void);
void pitchCntrl(void);
void yawCntrl(void);
void altitudeCntrl(void);
//void setTargetAltitude(int16_t targetAlt);

void init_yawCntrl(void);
void init_rollCntrl(void);
void init_pitchCntrl(void);
void init_altitudeCntrl(void);
void init_altitudeCntrlVariable(void);

void save_yawCntrl(void);
void save_rollCntrl(void);
void save_pitchCntrl(void);
void save_altitudeCntrl(void);
void save_altitudeCntrlVariable(void);

