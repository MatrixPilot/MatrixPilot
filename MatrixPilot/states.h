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


struct state_flags_bits {
	uint16_t unused                     : 4;
	uint16_t save_origin                : 1;
	uint16_t GPS_steering               : 1;
	uint16_t pitch_feedback             : 1;
	uint16_t altitude_hold_throttle     : 1;
	uint16_t altitude_hold_pitch        : 1;
	uint16_t man_req                    : 1;
	uint16_t auto_req                   : 1;
	uint16_t home_req                   : 1;
	uint16_t rtl_hold                   : 1;
	uint16_t f13_print_req              : 1;
	uint16_t disable_throttle           : 1;
	uint16_t update_autopilot_state_asap: 1;
};

union state_flags_int { struct state_flags_bits _; int16_t WW; };
extern union state_flags_int state_flags;

extern int16_t waggle;
//extern uint8_t counter;
extern boolean didCalibrate;

void init_states(void);
