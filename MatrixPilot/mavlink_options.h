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


////////////////////////////////////////////////////////////////////////////////
// mavlink_options.h
// 

#define MAVLINK_RATE_DEFAULT_RC_CHAN	0
#define MAVLINK_RATE_DEFAULT_IMU_RAW	0

#define     MAVLINK_FREQ_ATTITUDE	 	8   // Be careful if you change this. Requested frequency may not be actual freq.
#define 	MAVLINK_FREQ_GLOBAL_POS		8
#define 	MAVLINK_FREQ_HEARTBEAT		4
#define 	MAVLINK_FREQ_GPS_RAW		0
#define 	MAVLINK_FREQ_SUE			8	// SERIAL_UDB_EXTRA frequency

#define 	MAVLINK_FRAME_FREQUENCY		40
#define 	MAVLINK_WAYPOINT_TIMEOUT 	120 // Dependent on frequency of calling mavlink_output_40hz. 120 is 3 second timeout.

// 19200,38400,57600,115200,230400,460800,921600
// Fixed 19200 for non free running clock
#define MAVLINK_BAUD					57600
