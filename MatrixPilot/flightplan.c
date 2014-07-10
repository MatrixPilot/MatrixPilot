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
#include "flightplan.h"
#include "flightplan_logo.h"
#include "flightplan_waypoints.h"

boolean use_fixed_origin(void)
{
#if (USE_FIXED_ORIGIN == 1)
	return 1;
#else
	return 0;
#endif
}

vect3_32t get_fixed_origin(void)
{
	struct fixedOrigin3D origin = FIXED_ORIGIN_LOCATION;

	vect3_32t standardizedOrigin;
	standardizedOrigin.x = origin.x;
	standardizedOrigin.y = origin.y;
	standardizedOrigin.z = (int32_t)(origin.z * 100);
	return standardizedOrigin;
}

static boolean flightplan_logo_active = false;

void flightplan_init(void)
{
#if (FLIGHT_PLAN_TYPE != FP_NONE)
	DPRINT("flightplan_init() - %s\r\n", flightplan_logo_active ? "LOGO" : "WAYPOINTS");
#if (FLIGHT_PLAN_TYPE == FP_LOGO)
	flightplan_logo_active = true;
#endif
	flightplan_logo_init();
	flightplan_waypoints_init();
#endif // FLIGHT_PLAN_TYPE
}

void flightplan_begin(int16_t flightplanNum)
{
#if (FLIGHT_PLAN_TYPE != FP_NONE)
	DPRINT("flightplan_begin(%u)\r\n", flightplanNum);

	if (flightplan_logo_active) {
		flightplan_logo_begin(flightplanNum);
	} else {
		flightplan_waypoints_begin(flightplanNum);
	}
#endif // FLIGHT_PLAN_TYPE
}

void flightplan_update(void)
{
#if (FLIGHT_PLAN_TYPE != FP_NONE)
	if (flightplan_logo_active) {
		flightplan_logo_update();
	} else {
		flightplan_waypoints_update();
	}	
#endif // FLIGHT_PLAN_TYPE
}

int16_t flightplan_index_get(void)
{
#if (FLIGHT_PLAN_TYPE != FP_NONE)
	if (flightplan_logo_active) {
		return flightplan_logo_index_get();
	} else {
		return flightplan_waypoints_index_get();
	}
#endif // FLIGHT_PLAN_TYPE
}

void flightplan_live_begin(void)
{
#if (FLIGHT_PLAN_TYPE != FP_NONE)
	if (flightplan_logo_active) {
		flightplan_logo_live_begin();
	} else {
		flightplan_waypoints_live_begin();
	}
#endif // FLIGHT_PLAN_TYPE
}

void flightplan_live_received_byte(uint8_t inbyte)
{
#if (FLIGHT_PLAN_TYPE != FP_NONE)
	if (flightplan_logo_active) {
		flightplan_logo_live_received_byte(inbyte);
	} else {
		flightplan_waypoints_live_received_byte(inbyte);
	}
#endif // FLIGHT_PLAN_TYPE
}

void flightplan_live_commit(void)
{
#if (FLIGHT_PLAN_TYPE != FP_NONE)
	if (flightplan_logo_active) {
		flightplan_logo_live_commit();
	} else {
		flightplan_waypoints_live_commit();
	}
#endif // FLIGHT_PLAN_TYPE
}
