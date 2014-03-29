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


#include "defines.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/mathlibNAV.h"
#include "../libUDB/libUDB.h"

#include "../libDCM/libDCM_internal.h"
//#include "gpsParseCommon.h"
//#include "../libUDB/heartbeat.h"b
//#include "../libUDB/magnetometer.h"
//#include "../libUDB/barometer.h"


union dcm_fbts_word dcm_flags;

//void udb_heartbeat_40hz_callback(void) {}
//void udb_heartbeat_callback(void) {}
//void dcm_heartbeat_callback(void) {}
void udb_heartbeat_callback(void) {}

void udb_callback_read_sensors(void)
{
}

void udb_servo_callback_prepare_outputs(void)
{
}

void udb_background_callback_triggered(void)
{
}

void udb_gps_callback_received_byte(uint8_t rxchar)
{
}

int16_t udb_gps_callback_get_byte_to_send(void)
{
}
