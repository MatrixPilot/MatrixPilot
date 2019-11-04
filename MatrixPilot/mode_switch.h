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


#ifndef MODE_SWITCH_H
#define MODE_SWITCH_H


//inline int16_t flight_mode_switch_manual(void);
//inline int16_t flight_mode_switch_stabilize(void);
//inline int16_t flight_mode_switch_waypoints(void);

// removed 'inline' to keep VC++ happy
int16_t flight_mode_switch_manual(void);
int16_t flight_mode_switch_stabilize(void);
int16_t flight_mode_switch_waypoints(void);

void flight_mode_switch_2pos_poll(void); // this is called at 40 hertz and scans the two postion switch option.
void flight_mode_switch_check_set(void); // this is called at 2 hertz and changes the flight mode if req.
void control_mode_switch_check_set(void); // this is called at 2 hertz and changes the flight mode if req.


#endif // MODE_SWITCH_H
