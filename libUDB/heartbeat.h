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


#ifndef HEARTBEAT_H
#define HEARTBEAT_H


// number of heartbeats per second

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)

#if (BOARD_TYPE & AUAV2_BOARD) || (DUAL_IMU == 1)
// number of heartbeats per second set by MPU6000 sample rate
#define HEARTBEAT_HZ 200
// frequency of PID loop (HEARTBEAT_HZ / PID_HZ must be an integer)
#define PID_HZ 200
#else
// number of heartbeats per second
#define HEARTBEAT_HZ 400
//#define HEARTBEAT_HZ 40
// frequency of PID loop (HEARTBEAT_HZ / PID_HZ must be an integer)
#define PID_HZ 400
//#define PID_HZ 40
#endif

#else // AIRFRAME_TYPE

// number of heartbeats per second
#define HEARTBEAT_HZ 40
// frequency of PID loop (HEARTBEAT_HZ / PID_HZ must be an integer)
#define PID_HZ 40

#endif // AIRFRAME_TYPE


// number of servo updates per second
#define SERVO_HZ 40

// Read-only value increments with each heartbeat
extern uint16_t udb_heartbeat_counter;


#endif // HEARTBEAT_H
