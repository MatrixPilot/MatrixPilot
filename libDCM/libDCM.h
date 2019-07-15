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


#ifndef LIB_DCM_H
#define LIB_DCM_H


#include "../libUDB/libUDB.h"
#include "libDCM_defines.h"
#include "../libUDB/heartbeat.h"

////////////////////////////////////////////////////////////////////////////////
// libDCM.h defines the API for accessing the location and orientation
// information from the DCM algorithm and GPS.
// 
// Requires libUDB.
// 
// This library is designed to use libUDB, but to remain independent of the 
// specifics of the MatrixPilot application.


void SetAofA(int16_t AofA);
int16_t GetAofA(void);

////////////////////////////////////////////////////////////////////////////////
// Functions
void dcm_init(void);
void dcm_calibrate(void);
void dcm_set_origin_location(int32_t o_long, int32_t o_lat, int32_t o_alt);


// Implement this callback to prepare the pwOut values.
// It is called at 40Hz (once every 25ms).
//void dcm_servo_callback_prepare_outputs(void);  // Callback
void dcm_heartbeat_callback(void);

// Convert an absolute location to relative
struct relative3D dcm_absolute_to_relative(struct waypoint3D absolute);
struct relative3D_32 dcm_absolute_to_relative_32(struct waypoint3D absolute);

vect3_32t dcm_rel2abs(vect3_32t rel);


////////////////////////////////////////////////////////////////////////////////
// Vars
extern union dcm_fbts_word { struct dcm_flag_bits _; int16_t W; } dcm_flags;

// Calibrate for 10 seconds before moving servos
#define DCM_CALIB_COUNT  10*PID_HZ    // 10 seconds at 40 Hz
#define DCM_GPS_COUNT    25*PID_HZ   // 25 seconds at 40 Hz

#endif // LIB_DCM_H
