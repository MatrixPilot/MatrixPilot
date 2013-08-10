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

#ifndef _FLIGHTPLAN_WAYPOINTS_H_
#define _FLIGHTPLAN_WAYPOINTS_H_

#include "../libDCM/libDCM_defines.h"

struct relWaypointDef { struct relative3D loc ; int16_t flags ; struct relative3D viewpoint ; } ;
struct waypointDef { struct waypoint3D loc ; int16_t flags ; struct waypoint3D viewpoint ; } ;

extern struct waypointDef getWaypoint(uint16_t index);

#endif
