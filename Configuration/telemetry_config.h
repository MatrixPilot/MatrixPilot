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
// Serial Output Format (Can be SERIAL_NONE, SERIAL_DEBUG, SERIAL_ARDUSTATION, SERIAL_UDB,
// SERIAL_UDB_EXTRA,SERIAL_MAVLINK, SERIAL_CAM_TRACK, or SERIAL_OSD_REMZIBI)
// This determines the format of the output sent out the spare serial port.
// Note that SERIAL_OSD_REMZIBI only works with a ublox GPS.
// SERIAL_UDB_EXTRA will add additional telemetry fields to those of SERIAL_UDB.
// SERIAL_UDB_EXTRA can be used with the OpenLog without characters being dropped.
// SERIAL_UDB_EXTRA may result in dropped characters if used with the XBEE wireless transmitter.
// SERIAL_CAM_TRACK is used to output location data to a 2nd UDB, which will target its camera at this plane.
// SERIAL_MAVLINK is a bi-directional binary format for use with QgroundControl, HKGCS or MAVProxy (Ground Control Stations.)
// SERIAL_MAVLINK is only supported on the UDB4 to ensure that sufficient RAM is available.
// Note that SERIAL_MAVLINK defaults to using a baud rate of 57600 baud (other formats default to 19200)
//#define SERIAL_OUTPUT_FORMAT                SERIAL_MAVLINK
#define SERIAL_OUTPUT_FORMAT                SERIAL_UDB_EXTRA
//#define SERIAL_OUTPUT_FORMAT                SERIAL_NONE


////////////////////////////////////////////////////////////////////////////////
// Serial Output BAUD rate for either standard telemetry streams or MAVLink
//  19200, 38400, 57600, 115200, 230400, 460800, 921600 // yes, it really will work at this rate
//#define SERIAL_BAUDRATE                     19200 // default
#define SERIAL_BAUDRATE                     115200


///////////////////////////////////////////////////////////////////////////////////
// MAVLink requires an aircraft Identifier (I.D) as it is deaigned to control multiple aircraft
// Each aircraft in the sky will need a unique I.D. in the range from 0-255
#define MAVLINK_SYSID                       55


///////////////////////////////////////////////////////////////////////////////////
// Vehicle and Pilot Identification

// Once you are flying your plane and swapping flights and telemetry with other's across
// the world, you may like to fill in some of the fields below. This will be embedded in your
// telemetry, and used to make more interesting flights in Google Earth.
// ID_VEHICLE_MODEL_NAME provides indication of what model of plane, quad, car etc you are using
// ID_VEHICLE_REGISTRATION should be short (less than 12 continuous characters with no space
// it will be used in Google Earth as the folder name containing your flights.
// ID_LEAD_PILOT is your lead pilot flight name or alias e.g. "UAV Flight Director"
// ID_DIY_DRONES_URL should be the URL of your member page on DIY Drones.
// That will allow Google Earth viewers of your flights to click straight through to your latest discussions.
// EXAMPLE:-
//#define ID_VEHICLE_MODEL_NAME "Multiplex Twinstar 2"
//#define ID_VEHICLE_REGISTRATION "TW2-PDH-UK"
//#define ID_LEAD_PILOT "Pete Hollands"
//#define ID_DIY_DRONES_URL "http://www.diydrones.com/profile/PeterHollands"
#define ID_VEHICLE_MODEL_NAME               "Not Defined"
#define ID_VEHICLE_REGISTRATION             "Not Defined"
#define ID_LEAD_PILOT                       "Not Defined"
#define ID_DIY_DRONES_URL                   "http://www.diydrones.com"

