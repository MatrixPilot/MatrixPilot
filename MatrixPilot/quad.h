// This file is part of the MatrixPilotQuad firmware.
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


#ifndef QUAD_H
#define QUAD_H


extern unsigned int tailFlash;
extern unsigned long uptime;
extern boolean sendGains;

int  quad_init(void);
void quad_background_task(void);
void quad_heartbeat_40hz_callback(void); // this was called udb_background_callback_periodic()
void quad_heartbeat_callback(void); // was called dcm_servo_callback_prepare_outputs()

//void dcm_callback_gps_location_updated(void);
//void dcm_servo_callback_prepare_outputs(void);
//void udb_callback_radio_did_turn_off(void);


#endif // QUAD_H
