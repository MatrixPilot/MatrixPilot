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


boolean mode_switch_manual_chk(void);
boolean mode_switch_stabilize_chk(void);
boolean mode_switch_waypoints_chk(void);
boolean mode_switch_rtl_hold_chk(void);
void mode_switch_rtl_hold_set(void);

void mode_switch_2pos_poll(void); // this is called at 40 hertz and scans the two postion switch option.
void mode_switch_check_set(boolean radio_on); // this is called at 2 hertz and changes the flight mode if req.


#endif // MODE_SWITCH_H
