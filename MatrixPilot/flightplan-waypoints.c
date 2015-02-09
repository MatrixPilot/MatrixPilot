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
#include "navigate.h"
#include "behaviour.h"
#include "cameraCntrl.h"
#include "flightplan_waypoints.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/gpsData.h"
#include "../MAVLink/MAVLink.h"
#include "../MAVLink/MAVMission.h"
#include <stdlib.h>


#ifdef USE_EXTENDED_NAV
struct relWaypointDef { struct relative3D_32 loc; int16_t flags; struct relative3D viewpoint; };
#else
struct relWaypointDef { struct relative3D loc; int16_t flags; struct relative3D viewpoint; };
#endif // USE_EXTENDED_NAV
struct waypointDef { struct waypoint3D loc; int16_t flags; struct waypoint3D viewpoint; };

#include "waypoints.h"

#define STD_WAYPOINTS_COUNT (sizeof(stdWaypoints_default) / sizeof(struct waypointDef))
#define RTL_WAYPOINTS_COUNT (sizeof(rtlWaypoints_default) / sizeof(struct waypointDef))

//uint16_t number_of_waypoints = STD_WAYPOINTS_COUNT;
static int16_t waypointIndex = 0;

#ifdef USE_DYNAMIC_WAYPOINTS
static struct waypointDef WaypointSet[MAX_WAYPOINTS];
static struct waypointDef* currentWaypointSet = (struct waypointDef*)WaypointSet;
static int16_t numPointsInCurrentSet = 0;
#else
static const struct waypointDef* currentWaypointSet = (struct waypointDef*)stdWaypoints_default;
static int16_t numPointsInCurrentSet = STD_WAYPOINTS_COUNT;
#endif

static struct relWaypointDef current_waypoint;
static struct waypointDef wp_inject;
static uint8_t wp_inject_pos = 0;
#define WP_INJECT_READY 255
const uint8_t wp_inject_byte_order[] = {3, 2, 1, 0, 7, 6, 5, 4, 9, 8, 11, 10, 15, 14, 13, 12, 19, 18, 17, 16, 21, 20 };

int16_t waypoint_count(void)
{
	return numPointsInCurrentSet;
}

// For a relative waypoint, wp_to_relative() just passes the relative
// waypoint location through unchanged.
// For an absolute waypoint, wp_to_relative() converts the waypoint's
// location from absolute to relative.
static struct relWaypointDef wp_to_relative(struct waypointDef wp)
{
	struct relWaypointDef rel;

	if (wp.flags & F_ABSOLUTE)
	{
#ifdef USE_EXTENDED_NAV
		rel.loc = dcm_absolute_to_relative_32(wp.loc);
#else
		rel.loc = dcm_absolute_to_relative(wp.loc);
#endif // USE_EXTENDED_NAV
		rel.viewpoint = dcm_absolute_to_relative(wp.viewpoint);
		rel.flags = wp.flags - F_ABSOLUTE;
	}
	else
	{
		rel.loc.x = wp.loc.x;
		rel.loc.y = wp.loc.y;
		rel.loc.z = wp.loc.z;
		rel.viewpoint.x = wp.viewpoint.x;
		rel.viewpoint.y = wp.viewpoint.y;
		rel.viewpoint.z = wp.viewpoint.z;
		rel.flags = wp.flags;
	}
	return rel;
}

//struct relWaypointDef wp_to_absolute(struct waypointDef wp)

//struct waypoint3D wp_to_absolute(struct waypointDef wp)
static vect3_32t wp_to_absolute_coords(struct waypointDef wp)
{
	vect3_32t v;

	if (wp.flags & F_ABSOLUTE)
	{
		v.x = wp.loc.x;
		v.y = wp.loc.y;
		v.z = wp.loc.z;
	}
	else
	{
		v.x = wp.loc.x;
		v.y = wp.loc.y;
		v.z = wp.loc.z;
		v = dcm_rel2abs(v);
	}
	return v;
}

void clear_flightplan(void)
{
	numPointsInCurrentSet = 0;
}

// X is Longitude in degrees * 10^7
// Y is Latitude in degrees * 10^7
// Z is altitude above sea level, in meters, as a floating point value.

void add_waypoint(struct waypoint3D wp, int16_t flags)
{
#ifdef USE_DYNAMIC_WAYPOINTS
	DPRINT("add_waypoint(%li, %li, %i\r\n", wp.x, wp.y, wp.z);
	if (numPointsInCurrentSet < MAX_WAYPOINTS)
	{
		currentWaypointSet[numPointsInCurrentSet].loc.x = wp.x;
		currentWaypointSet[numPointsInCurrentSet].loc.y = wp.y;
		currentWaypointSet[numPointsInCurrentSet].loc.z = wp.z;
		currentWaypointSet[numPointsInCurrentSet].flags = flags;
		numPointsInCurrentSet++;
	}
	else
	{
		DPRINT("waypoints list full, new point not added\r\n");
	}
#else
	DPRINT("Must define USE_DYNAMIC_WAYPOINTS in order to add waypoints\r\n");

#endif // USE_DYNAMIC_WAYPOINTS
}
/*
void add_waypoint(int32_t x, int32_t y, int16_t z, int16_t flags)
{
	DPRINT("add_waypoint(%li, %li, %li\r\n", x, y, z);
	if (numPointsInCurrentSet < MAX_WAYPOINTS)
	{
		currentWaypointSet[numPointsInCurrentSet].loc.x = x;
		currentWaypointSet[numPointsInCurrentSet].loc.y = y;
		currentWaypointSet[numPointsInCurrentSet].loc.z = z;
		currentWaypointSet[numPointsInCurrentSet].flags = flags;
		numPointsInCurrentSet++;
	}
	else
	{
		DPRINT("waypoints list full, new point not added\r\n");
	}
}
 */
#ifdef USE_DYNAMIC_WAYPOINTS

static void load_flightplan(const struct waypointDef* waypoints, int count)
{
	int i;

//struct waypoint3D  { int32_t x; int32_t y; int16_t z; };
//struct waypointDef { struct waypoint3D loc; int16_t flags; struct waypoint3D viewpoint; };

	struct waypointDef* dst_wp;
	const struct waypointDef* src_wp;

	DPRINT("load_flightplan(..., %u)\r\n", count);
	for (i = 0; i < count; i++)
	{
		dst_wp = &currentWaypointSet[i];
		src_wp = &waypoints[i];

		dst_wp->loc.x = src_wp->loc.x;
		dst_wp->loc.y = src_wp->loc.y;
		dst_wp->loc.z = src_wp->loc.z;
		dst_wp->flags = src_wp->flags;
		dst_wp->viewpoint = src_wp->viewpoint;
	}
	numPointsInCurrentSet = count;
}

#else

static void load_flightplan(const struct waypointDef* waypoints, int count)
{
	currentWaypointSet = waypoints;
	numPointsInCurrentSet = count;
}

#endif

int16_t flightplan_waypoints_index_get(void)
{
	return waypointIndex;
}

const struct waypointDef* rtlWaypoints = rtlWaypoints_default;
const struct waypointDef* stdWaypoints = stdWaypoints_default;
uint16_t rtlWaypoints_count = RTL_WAYPOINTS_COUNT;
uint16_t stdWaypoints_count = STD_WAYPOINTS_COUNT;

void flightplan_waypoints_init(void)
{
	DPRINT("flightplan_waypoints_init()\r\n");

#ifdef USE_DYNAMIC_WAYPOINTS
	load_flightplan(rtlWaypoints_default, RTL_WAYPOINTS_COUNT);
	load_flightplan(stdWaypoints_default, STD_WAYPOINTS_COUNT);
#else
	rtlWaypoints = rtlWaypoints_default;
	stdWaypoints = stdWaypoints_default;
	rtlWaypoints_count = RTL_WAYPOINTS_COUNT;
	stdWaypoints_count = STD_WAYPOINTS_COUNT;
#endif // USE_DYNAMIC_WAYPOINTS
}

// In the future, we could include more than 2 waypoint sets...
// flightplanNum is 0 for main waypoints, and 1 for RTL waypoints
void flightplan_waypoints_begin(int16_t flightplanNum)
{
	DPRINT("flightplan_waypoints_begin(%i)\r\n", flightplanNum);

	if (flightplanNum == 1)         // RTL waypoint set
	{
//		currentWaypointSet = (struct waypointDef*)rtlWaypoints;
		currentWaypointSet = rtlWaypoints;
		numPointsInCurrentSet = RTL_WAYPOINTS_COUNT;
	}
	else if (flightplanNum == 0)    // Main waypoint set
	{
//		currentWaypointSet = (struct waypointDef*)stdWaypoints;
		currentWaypointSet = stdWaypoints;
		numPointsInCurrentSet = STD_WAYPOINTS_COUNT;
	}

	if (numPointsInCurrentSet != 0)
	{
		waypointIndex = 0;
		current_waypoint = wp_to_relative(currentWaypointSet[0]);
		navigate_set_goal(GPSlocation, current_waypoint.loc);
		set_camera_view(current_waypoint.viewpoint);
		setBehavior(current_waypoint.flags);
		// udb_background_trigger();    // trigger navigation immediately
	}
	else
	{
		DPRINT("No waypoints are set\r\n");
	}
}

//struct waypoint3D getWaypoint3D(uint16_t wp)
vect3_32t getWaypoint3D(uint16_t wp)
{
//struct waypoint3D    { int32_t x; int32_t y; int16_t z; };
//	struct waypoint3D = currentWaypointSet[wp].loc;

	vect3_32t v;
	v = wp_to_absolute_coords(currentWaypointSet[wp]);
	return v;

//	return currentWaypointSet[wp].loc;
}

void set_waypoint(int16_t index)
{
	DPRINT("set_waypoint(%u)\r\n", index);

	if (index < numPointsInCurrentSet)
	{
		waypointIndex = index;
#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
		mavlink_waypoint_changed(waypointIndex);
#endif
		if (waypointIndex == 0)
		{
			if (numPointsInCurrentSet > 1)
			{
				struct relWaypointDef previous_waypoint = wp_to_relative(currentWaypointSet[numPointsInCurrentSet-1]);
				current_waypoint  = wp_to_relative(currentWaypointSet[0]);
				navigate_set_goal(previous_waypoint.loc, current_waypoint.loc);
			}
			else
			{
				current_waypoint = wp_to_relative(currentWaypointSet[0]);
				navigate_set_goal(GPSlocation, current_waypoint.loc);
			}
			set_camera_view(current_waypoint.viewpoint);
			setBehavior(currentWaypointSet[0].flags);
		}
		else
		{
			struct relWaypointDef previous_waypoint = wp_to_relative(currentWaypointSet[waypointIndex-1]);
			current_waypoint = wp_to_relative(currentWaypointSet[waypointIndex]);
			navigate_set_goal(previous_waypoint.loc, current_waypoint.loc);
			set_camera_view(current_waypoint.viewpoint);
			setBehavior(current_waypoint.flags);
		}
#if (DEADRECKONING == 0)
		navigate_compute_bearing_to_goal();
#endif
	}
}

static void next_waypoint(void)
{
	if (extended_range == 0)
	{
#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
		mavlink_waypoint_reached(waypointIndex);
#endif
		waypointIndex++;
		if (waypointIndex >= numPointsInCurrentSet) waypointIndex = 0;

		DPRINT("next_waypoint(%u)\r\n", waypointIndex);
		set_waypoint(waypointIndex);
/*
#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
		mavlink_waypoint_changed(waypointIndex);
#endif
		if (waypointIndex == 0)
		{
			if (numPointsInCurrentSet > 1)
			{
				struct relWaypointDef previous_waypoint = wp_to_relative(currentWaypointSet[numPointsInCurrentSet-1]);
				current_waypoint  = wp_to_relative(currentWaypointSet[0]);
				navigate_set_goal(previous_waypoint.loc, current_waypoint.loc);
				set_camera_view(current_waypoint.viewpoint);
			}
			else
			{
				current_waypoint = wp_to_relative(currentWaypointSet[0]);
				navigate_set_goal(GPSlocation, current_waypoint.loc);
				set_camera_view(current_waypoint.viewpoint);
			}
			setBehavior(currentWaypointSet[0].flags);
		}
		else
		{
			struct relWaypointDef previous_waypoint = wp_to_relative(currentWaypointSet[waypointIndex-1]);
			current_waypoint = wp_to_relative(currentWaypointSet[waypointIndex]);
			navigate_set_goal(previous_waypoint.loc, current_waypoint.loc);
			set_camera_view(current_waypoint.viewpoint);
			setBehavior(current_waypoint.flags);
		}
 */
	}
	else
	{
		navigate_set_goal(GPSlocation, current_waypoint.loc);
#if (DEADRECKONING == 0)
		navigate_compute_bearing_to_goal();
#endif
	}
}

void flightplan_waypoints_update(void)
{
	// first run any injected wp from the serial port
	if (wp_inject_pos == WP_INJECT_READY)
	{
		current_waypoint = wp_to_relative(wp_inject);
		navigate_set_goal(GPSlocation, current_waypoint.loc);
		set_camera_view(current_waypoint.viewpoint);
		setBehavior(current_waypoint.flags);
		navigate_compute_bearing_to_goal();
		wp_inject_pos = 0;
		return;
	}

	// steering is based on cross track error.
	// waypoint arrival is detected computing distance to the "finish line".

	// note: locations are measured in meters
	//       velocities are in centimeters per second

	// locations have a range of +-32000 meters (20 miles) from origin
	
	if (desired_behavior._.altitude)
	{
		if (abs(IMUheight - navigate_get_goal(NULL)) < ((int16_t)HEIGHT_MARGIN))
		{
			DPRINT("flightplan_wayoints() IMUheight %i goal.height %i\r\n", IMUheight, navigate_get_goal(NULL));
			next_waypoint();
		}
	}
	else
	{
		if (tofinish_line < WAYPOINT_RADIUS) // crossed the finish line
		{
			DPRINT("flightplan_waypoints() tofinish_line %i\r\n", tofinish_line);
			if (desired_behavior._.loiter)
			{
				navigate_set_goal(GPSlocation, wp_to_relative(currentWaypointSet[waypointIndex]).loc);
			}
			else
			{
				next_waypoint();
			}
		}
	}
}

void flightplan_waypoints_live_begin(void)
{
	wp_inject_pos = 0;
}

void flightplan_waypoints_live_received_byte(uint8_t inbyte)
{
	if (wp_inject_pos < sizeof(wp_inject_byte_order))
	{
		((uint8_t*)(&wp_inject))[wp_inject_byte_order[wp_inject_pos++]] = inbyte;
	}
	else if (wp_inject_pos == sizeof(wp_inject_byte_order))
	{
		wp_inject_pos++;
	}
}

void flightplan_waypoints_live_commit(void)
{
	if (wp_inject_pos == sizeof(wp_inject_byte_order))
	{
		wp_inject_pos = WP_INJECT_READY;
	}
	else
	{
		wp_inject_pos = 0;
	}
}
