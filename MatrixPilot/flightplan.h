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

#ifndef FLIGHTPLAN_H
#define FLIGHTPLAN_H

////////////////////////////////////////////////////////////////////////////////
// Waypoint handling

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
#define WAYPOINT_RADIUS         25

#define CAM_VIEW_LAUNCH         { 0, 0, 0 }

// Origin Location
// When using relative waypoints, the default is to interpret those waypoints as relative to the
// plane's power-up location.  Here you can choose to use any specific, fixed 3D location as the
// origin point for your relative waypoints.
//
// USE_FIXED_ORIGIN should be 0 to use the power-up location as the origin for relative waypoints.
// Set it to 1 to use a fixed location as the origin, no matter where you power up.
// FIXED_ORIGIN_LOCATION is the location to use as the origin for relative waypoints.  It uses the
// format { X, Y, Z } where:
// X is Longitude in degrees * 10^7
// Y is Latitude in degrees * 10^7
// Z is altitude above sea level, in meters, as a floating point value.
// 
// If you are using waypoints for an autonomous landing, it is a good idea to set the altitude value
// to be the altitude of the landing point, and then express the heights of all of the waypoints with
// respect to the landing point.
// If you are using OpenLog, an easy way to determine the altitude of your landing point is to
// examine the telemetry after a flight, take a look in the .csv file, it will be easy to spot the
// altitude, expressed in meters.

#define USE_FIXED_ORIGIN        0
//#define FIXED_ORIGIN_LOCATION   { -1219950467, 374124664, 30.0 }    // A point in Baylands Park in Sunnyvale, CA

//#define USE_FIXED_ORIGIN        1
#define FIXED_ORIGIN_LOCATION   { 113480854, 472580108, 578 }    // Innsbruck

//printf("o_lon %li o_lat %li o_alt %li\r\n", o_lon, o_lat, o_alt); // o_lon 113480854 o_lat 472580108 o_alt 57763


boolean use_fixed_origin(void);
vect3_32t get_fixed_origin(void);

void flightplan_init(int16_t flightplanNum);
void flightplan_update(void);
int16_t flightplan_index_get(void);
void flightplan_live_begin(void);
void flightplan_live_received_byte(uint8_t inbyte);
void flightplan_live_commit(void);

#endif // FLIGHTPLAN_H
