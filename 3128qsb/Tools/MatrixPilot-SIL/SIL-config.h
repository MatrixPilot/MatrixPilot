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


////////////////////////////////////////////////////////////////////////////////
// Software In the Loop Simulation
// Only set this to 1 when building for simulation directly on your computer instead of
// running on a UDB.
// See the MatrixPilot wiki for more info on using SILSIM.
// Below are settings to configure the simulated UDB UARTs.
// The SERIAL_RC_INPUT settings allow optionally talking over a serial port to a UDB
// passing RC inputs through to the simulated UDB.
#define SILSIM_GPS_RUN_AS_SERVER            0
#define SILSIM_GPS_PORT                     14551       // default port to connect to XPlane HILSIM plugin
#define SILSIM_GPS_HOST                     "127.0.0.1"
#define SILSIM_TELEMETRY_RUN_AS_SERVER      0
#define SILSIM_TELEMETRY_PORT               14550       // default port to connect to QGroundControl
#define SILSIM_TELEMETRY_HOST               "127.0.0.1"
#define SILSIM_SERIAL_RC_INPUT_DEVICE       ""          // i.e. "COM4" or "/dev/cu.usbserial-A600dP4v", or "" to disable
#define SILSIM_SERIAL_RC_INPUT_BAUD         38400
