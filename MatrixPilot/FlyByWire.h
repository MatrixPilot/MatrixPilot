// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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


#ifndef _FLYBYWIRE_H_
#define _FLYBYWIRE_H_

#define LENGTH_OF_HEADER	(3)
#define LENGTH_OF_PAYLOAD	(10)
#define LENGTH_OF_PACKET	(LENGTH_OF_HEADER + LENGTH_OF_PAYLOAD)

uint8_t get_fbw_pos(void);
void fbw_live_begin(void );
void fbw_live_commit(void);
void fbw_live_commit_buf(uint8_t* buf);
boolean fbw_live_received_byte(uint8_t inbyte);
int16_t get_fbw_pwm(int16_t index);


#endif // _FLYBYWIRE_H_
