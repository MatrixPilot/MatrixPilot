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


void radioIn_init(void);
boolean radioIn_isFailSafe(void);
uint8_t radioIn_getInput(int16_t* ppm, uint8_t channels);

void udb_servo_record_trims(void);

// called from heartbeat pulse at 20Hz
void radioIn_failsafe_check(void);
// called from heartbeat pulse at 1Hz
void radioIn_failsafe_reset(void);

void udb_callback_radio_did_turn_off(void); // callback
