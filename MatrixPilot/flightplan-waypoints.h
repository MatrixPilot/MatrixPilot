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


#ifndef FLIGHTPLAN_WAYPOINTS_H
#define FLIGHTPLAN_WAYPOINTS_H


//#define USE_DYNAMIC_WAYPOINTS
#define MAX_WAYPOINTS 20

extern int16_t waypointIndex;
vect3_32t getWaypoint3D(uint16_t wp);
int16_t waypoint_count(void);
void set_waypoint(int16_t index);
void clear_flightplan(void);
void add_waypoint(struct waypoint3D wp, int16_t flags);

void flightplan_waypoints_init(void);
void flightplan_waypoints_begin(int16_t flightplanNum);
void flightplan_waypoints_update(void);
int16_t flightplan_waypoints_index_get(void);
void flightplan_waypoints_live_begin(void);
void flightplan_waypoints_live_received_byte(uint8_t inbyte);
void flightplan_waypoints_live_commit(void);


#endif // FLIGHTPLAN_WAYPOINTS_H
