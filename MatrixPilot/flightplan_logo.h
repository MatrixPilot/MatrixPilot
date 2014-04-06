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


#ifndef FLIGHTPLAN_LOGO_H
#define FLIGHTPLAN_LOGO_H


void flightplan_logo_init(void);
void flightplan_logo_begin(int16_t flightplanNum);
void flightplan_logo_update(void);
int16_t flightplan_logo_index_get(void);
void flightplan_logo_live_begin(void);
void flightplan_logo_live_received_byte(uint8_t inbyte);
void flightplan_logo_live_commit(void);


#endif // FLIGHTPLAN_LOGO_H
