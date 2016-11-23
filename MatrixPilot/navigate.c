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
#include "servoPrepare.h"
#include "config.h"
#include "states.h"
#include "flightplan.h"
#include "flightplan_waypoints.h"
#include "../libUDB/libUDB.h"
#include "../libDCM/gpsParseCommon.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/estAltitude.h"
#include "../libDCM/estWind.h"
#include "../libDCM/mathlibNAV.h"
#include "../libDCM/mathlib.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/rmat.h"
#include <stdlib.h>

// Compute actual and desired courses.
// Actual course is simply the scaled GPS course over ground information.
// Desired course is a "return home" course, which is simply the negative of the
// angle of the vector from the origin to the location of the plane.

// The origin is recorded as the location of the plane during power up of the control.

uint16_t yawkpail; // only exported for parameter_table
uint16_t yawkprud; // only exported for parameter_table
uint16_t turngainfbw; // fly by wire turn gain
uint16_t turngainnav; // waypoints turn gain

int16_t tofinish_line = 0;
int16_t progress_to_goal = 0;
int8_t desired_dir = 0;
int8_t extended_range = 0;


struct waypointparameters {
	int16_t x;
	int16_t y;
	int16_t cosphi;
	int16_t sinphi;
	int8_t  phi;
	int16_t height;
	int16_t fromHeight;
	int16_t legDist;
};

static struct waypointparameters navgoal;
static int16_t desired_bearing_over_ground_vector[2];

struct relative2D togoal = { 0, 0 };
int8_t desired_bearing_over_ground;

int16_t navigate_get_goal(vect3_16t* _goal)
{
	if (_goal != NULL)
	{
		_goal->x = navgoal.x;
		_goal->y = navgoal.y;
		_goal->z = navgoal.height;
	}
	return navgoal.height;
}

void init_navigation(void)
{
	yawkpail = (uint16_t)(gains.YawKPAileron*RMAX);
	yawkprud = (uint16_t)(gains.YawKPRudder*RMAX);
	turngainnav = (uint16_t)((turns.TurnRateNav/57.3)*RMAX);
	turngainfbw = (uint16_t)((turns.TurnRateFBW/57.3)*RMAX);
}

void save_navigation(void)
{
	gains.YawKPAileron = (float)yawkpail / (RMAX);
	gains.YawKPRudder  = (float)yawkprud / (RMAX);
}

static void setup_origin(void)
{
	if (use_fixed_origin())
	{
		vect3_32t origin = get_fixed_origin();
		dcm_set_origin_location(origin.x, origin.y, origin.z);
	}
	else
	{
		dcm_set_origin_location(lon_gps.WW, lat_gps.WW, alt_sl_gps.WW);
	}
	state_flags._.f13_print_req = 1; // Flag telemetry output that the origin can now be printed.
}

void dcm_callback_gps_location_updated(void)
{
	if (state_flags._.save_origin)
	{
		// capture origin information during power up. much of this is not
		// actually used for anything, but is saved in case you decide to
		// extend this code.
		state_flags._.save_origin = 0;
		setup_origin();
#if (USE_BAROMETER_ALTITUDE == 1)
		altimeter_calibrate();
#endif
	}

// Ideally, navigate should take less than one second.
// For MatrixPilot, navigation takes only a few milliseconds.

// If you rewrite navigation to perform some rather ambitious calculations,
// perhaps using floating point, matrix inversions, Kalman filters, etc,
// you will not cause a stack overflow if you take more than 1 second,
// the interrupt handler will simply skip some of the navigation passes.
}

#ifdef USE_EXTENDED_NAV
void navigate_set_goal(struct relative3D_32 fromPoint, struct relative3D_32 toPoint)
#else
void navigate_set_goal(struct relative3D fromPoint, struct relative3D toPoint)
#endif // USE_EXTENDED_NAV
{
	struct relative2D courseLeg;
	int16_t courseDirection[2];

#ifdef USE_EXTENDED_NAV
	union longww from_to_x;
	union longww from_to_y;
	int16_t from_to_z;
	int16_t first_one_bit_location_min;
	int16_t first_one_bit_location_x;
	int16_t first_one_bit_location_y;

	from_to_x.WW = toPoint.x - fromPoint.x;
	from_to_y.WW = toPoint.y - fromPoint.y;
	from_to_z    = toPoint.z - fromPoint.z;

	first_one_bit_location_x = find_first_bit_int32(from_to_x.WW);
	first_one_bit_location_y = find_first_bit_int32(from_to_y.WW);

	if (first_one_bit_location_x < first_one_bit_location_y)
	{
		first_one_bit_location_min = first_one_bit_location_x;
	}
	else
	{
		first_one_bit_location_min = first_one_bit_location_y;
	}

	if (first_one_bit_location_min < 18)
	{
		from_to_x.WW = (from_to_x.WW) >> (18 - first_one_bit_location_min);
		from_to_y.WW = (from_to_y.WW) >> (18 - first_one_bit_location_min);
		from_to_z    = (from_to_z)    >> (18 - first_one_bit_location_min);

		toPoint.x = fromPoint.x + from_to_x.WW;
		toPoint.y = fromPoint.y + from_to_y.WW;
		toPoint.z = fromPoint.z + from_to_z;

		extended_range = 1;
	}
	else
	{
		extended_range = 0;
	}
#endif // USE_EXTENDED_NAV

	navgoal.x = toPoint.x;
	navgoal.y = toPoint.y;
	navgoal.height = toPoint.z;
	navgoal.fromHeight = fromPoint.z;

	courseLeg.x = toPoint.x - fromPoint.x;
	courseLeg.y = toPoint.y - fromPoint.y;

	courseDirection[0] = courseLeg.x;
	courseDirection[1] = courseLeg.y;

//  The following is the old way to define the goal angle and distance.
//  It is left in for now because other computations, such as OSD, require
//  an angle, and also the leg distance is required.
//  But leg distance is produced as a by product of vector2_normalize.
//  TODO: revise the following two lines.
	navgoal.phi = rect_to_polar(&courseLeg); // binary angle (0 - 256 = 360 degrees)
	navgoal.legDist = courseLeg.x;

//struct waypointparameters { int16_t x; int16_t y; int16_t cosphi; int16_t sinphi; int8_t phi; int16_t height; int16_t fromHeight; int16_t legDist; };
//extern struct waypointparameters navgoal;
//	DPRINT("navigate_set_goal(..) x %i y %i phi %i height %i dist %i\r\n", navgoal.x, navgoal.y, navgoal.phi, navgoal.height, navgoal.legDist);

//  New method for computing cosine and sine of course direction
	vector2_normalize(&courseDirection[0], &courseDirection[0]);
	navgoal.cosphi = courseDirection[0];
	navgoal.sinphi = courseDirection[1];
}

void navigate_set_goal_height(int16_t z)
{
	navgoal.height = z;
}

void navigate_process_flightplan(void)
{
	if (gps_nav_valid() && state_flags._.GPS_steering)
	{
		navigate_compute_bearing_to_goal();
		flightplan_update(); // was called run_flightplan();
		compute_camera_view();
	}
}

///////////////////////////////////////////////////////////////////////////////

static int16_t compute_progress_to_goal(int16_t totalDist, int16_t remainingDist)
{
	// progress is the fraction of the distance from the start to the finish of
	// the current waypoint leg, that is still remaining. it ranges from 0 - 1<<12 (4096).
	int16_t progress;

	if (totalDist > 0)
	{
		progress = (((int32_t)totalDist - remainingDist) << 12) / totalDist;
		if (progress < 0)
		{
			progress = 0;
		}
		if (progress > (int32_t)1 << 12)
		{
			progress = (int32_t)1 << 12;
		}
	}
	else
	{
		progress = (int32_t)1 << 12;
	}
	return progress;
}

int16_t navigate_desired_height(void)
{
	int16_t height;

	if (desired_behavior._.takeoff || desired_behavior._.altitude)
	{
		height = navgoal.height;
	}
	else
	{
		int16_t progress_to_goal; // Fraction of the way to the goal in the range 0-4096 (2^12)
		progress_to_goal = compute_progress_to_goal(navgoal.legDist, tofinish_line);
		height = navgoal.fromHeight + (((navgoal.height - navgoal.fromHeight) * (int32_t)progress_to_goal) >> 12);
	}
	return height;
}

static void cross_track(void)
{
// INPUTS: navgoal, IMUlocation, IMUintegralAcceleration
// OUTPUT: desired_bearing_over_ground_vector
//

//struct ww   { int16_t W0; int16_t W1; };
//union longww     { int32_t WW;  struct ww _; };

	// Using Cross Tracking
	// CROSS_TRACK_MARGIN is the value of cross track error in meters
	// beyond which cross tracking correction saturates at 45 degrees 
#if (CROSS_TRACK_MARGIN >= 1024)
#error ("CTMARGIN is too large, it must be less than 1024")
#endif
	union longww crossVector[2];
	int16_t cross_rotate[2];
	int16_t crosstrack;

	// cross_rotate is a vector parallel to the desired course track
	cross_rotate[0] =  navgoal.cosphi;
	cross_rotate[1] = -navgoal.sinphi;

	// cross_vector is a weighted sum of cross track distance error and cross velocity.
	// IMU velocity is in centimeters per second, so right shifting by 4 produces
	// about 6 times the IMU velocity in meters per second.
	// This sets the time constant of the exponential decay to about 6 seconds
	crossVector[0]._.W1 = navgoal.x;
	crossVector[1]._.W1 = navgoal.y;
	crossVector[0].WW -= IMUlocationx.WW + ((IMUintegralAccelerationx.WW) >> 4);
	crossVector[1].WW -= IMUlocationy.WW + ((IMUintegralAccelerationy.WW) >> 4);

	// The following rotation transforms the cross track error vector into the
	// frame of the desired course track
	rotate_2D_long_vector_by_vector(&crossVector[0].WW, cross_rotate);
	crosstrack = crossVector[1]._.W1;

	// Compute the adjusted desired bearing over ground.
	// Start with the straight line between waypoints.
	desired_bearing_over_ground_vector[0] = navgoal.cosphi;
	desired_bearing_over_ground_vector[1] = navgoal.sinphi;

	// Determine if the crosstrack error is within saturation limit.
	// If so, then multiply by 64 to pick up an extra 6 bits of resolution.
	if (abs(crosstrack) < ((uint16_t)(CROSS_TRACK_MARGIN)))
	{
		crossVector[1].WW <<= 6;
		cross_rotate[1] = crossVector[1]._.W1;
		cross_rotate[0] = 64*((uint16_t)(CROSS_TRACK_MARGIN));
		vector2_normalize(cross_rotate, cross_rotate);
		// At this point, the implicit angle of the cross correction rotation
		// is atan of (the cross error divided by the cross margin).
		// Rotate the base course by the cross correction
		rotate_2D_vector_by_vector(desired_bearing_over_ground_vector, cross_rotate);
	}
	else
	{
		if (crosstrack > 0)
		{
			rotate_2D_vector_by_angle(desired_bearing_over_ground_vector, (int8_t) (32));
		}
		else
		{
			rotate_2D_vector_by_angle(desired_bearing_over_ground_vector, (int8_t) (- 32));
		}
	}
}

void navigate_compute_bearing_to_goal(void)
{
	struct relative2D togoal;
	union longww temporary;

	// compute the goal vector from present position to waypoint target in meters:
#if (DEADRECKONING == 1)
	togoal.x = navgoal.x - IMUlocationx._.W1;
	togoal.y = navgoal.y - IMUlocationy._.W1;
#else
	togoal.x = navgoal.x - GPSlocation.x;
	togoal.y = navgoal.y - GPSlocation.y;
#endif

	// project the goal vector onto the direction vector between waypoints
	// to get the distance to the "finish" line:
	temporary.WW = (__builtin_mulss(togoal.x, navgoal.cosphi)
	              + __builtin_mulss(togoal.y, navgoal.sinphi))<<2;
	tofinish_line = temporary._.W1;

	// Determine if aircraft is making forward progress.
	// If not, do not apply cross track correction.
	// This is done to prevent "waggles" during a 180 degree turn.
	temporary.WW = (__builtin_mulss(IMUintegralAccelerationx._.W1, navgoal.cosphi)
	              + __builtin_mulss(IMUintegralAccelerationy._.W1, navgoal.sinphi));
	if ((desired_behavior._.cross_track) && (temporary._.W1 > 0))
	{
		cross_track();
	}
	else
	{
			// If not using Cross Tracking
			// the desired bearing unit vector is simply the normalized to goal vector
			desired_bearing_over_ground_vector[0] = togoal.x;
			desired_bearing_over_ground_vector[1] = togoal.y;
			vector2_normalize(desired_bearing_over_ground_vector, desired_bearing_over_ground_vector);
	}
	if (state_flags._.GPS_steering)   // return to home or waypoints state
	{
		desired_dir = navgoal.phi;
	}
	else
	{
		if (current_orientation != F_HOVER)
		{
			desired_dir = calculated_heading;
		}
	}
}

uint16_t wind_gain_adjustment(void)
{
#if (WIND_GAIN_ADJUSTMENT == 1)
	uint16_t horizontal_air_speed;
	uint16_t horizontal_ground_speed_over_2;
	uint16_t G_over_2A;
	uint16_t G_over_2A_sqr;
	uint32_t temporary_long;

	horizontal_air_speed = vector2_mag(IMUvelocityx._.W1 - estimatedWind[0],
	                                   IMUvelocityy._.W1 - estimatedWind[1]);
	horizontal_ground_speed_over_2 = vector2_mag(IMUvelocityx._.W1,
	                                             IMUvelocityy._.W1) >> 1;
	if (horizontal_ground_speed_over_2 >= horizontal_air_speed)
	{
		return 0xFFFF;
	}
	else if (horizontal_air_speed > 0)
	{
		temporary_long = ((uint32_t)horizontal_ground_speed_over_2) << 16;
		G_over_2A = __builtin_divud(temporary_long, horizontal_air_speed);
		temporary_long = __builtin_muluu(G_over_2A, G_over_2A);
		G_over_2A_sqr = temporary_long >> 16;
		if (G_over_2A_sqr > 0x4000)
		{
			return (G_over_2A_sqr);
		}
		else
		{
			return (0x4000);
		}
	}
	else
	{
		return 0x4000;
	}
#else
	return 0x4000;
#endif
}

// Values for navType:
// 'y' = yaw/rudder, 'a' = aileron/roll, 'h' = aileron/hovering
int16_t navigate_determine_deflection(char navType)
{
	union longww deflectionAccum;
	union longww dotprod;
	union longww crossprod;
	int16_t desiredX;
	int16_t desiredY;
	int16_t actualX;
	int16_t actualY;
	int16_t actualXY[2];
	uint16_t yawkp;

	// If plane is flying, and is making forward progress over the ground,
	// use course over ground to navigate, otherwise, use attitude.
	// Forward ground speed must be greater than 1/8 of the airspeed, plus a fixed margin
	if (forward_ground_speed > ((air_speed_magnitudeXY >> 2) + WIND_NAV_AIR_SPEED_MIN))
	{
		// The following uses IMU values to get actual course over ground
		actualXY[0] = -IMUintegralAccelerationx._.W1;
		actualXY[1] =  IMUintegralAccelerationy._.W1;
		vector2_normalize(actualXY, actualXY);
		actualX = actualXY[0];
		actualY = actualXY[1];
		if (navType == 't')
		{
			yawkp = turngainnav;
		}
		else if (navType == 'h')
		{
			yawkp = yawkpail;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (navType == 't')
		{
			yawkp = turngainnav;
			actualX = rmat[1];
			actualY = rmat[4];
		}
		else if (navType == 'h')
		{
			yawkp = yawkpail;
			actualX = rmat[2];
			actualY = rmat[5];
		}
		else
		{
			return 0;
		}
	}
#ifdef TestGains
	desiredX = -cosine((navType == 'y') ? 0 : 64);
	desiredY =    sine((navType == 'y') ? 0 : 64);
#else
	desiredX = -desired_bearing_over_ground_vector[0];
	desiredY =  desired_bearing_over_ground_vector[1];
#endif
	dotprod.WW   = __builtin_mulss(actualX, desiredX) + __builtin_mulss(actualY, desiredY);
	crossprod.WW = __builtin_mulss(actualX, desiredY) - __builtin_mulss(actualY, desiredX);
	crossprod.WW = crossprod.WW << 2; // at this point, we have 1/4 of the cross product
	                                  // cannot go any higher than that, could get overflow
	if (dotprod._.W1 > 0)
	{
		deflectionAccum.WW = (__builtin_mulsu(crossprod._.W1, yawkp) << 1);
	}
	else
	{
		if (crossprod._.W1 > 0)
		{
			deflectionAccum._.W1 = (yawkp / 2);
		}
		else
		{
			deflectionAccum._.W1 = -(yawkp / 2);    // yawkp is unsigned, must divide and then negate
		}
	}
	if (navType == 'h')
	{
		deflectionAccum.WW = -deflectionAccum.WW;
	}

#if (WIND_GAIN_ADJUSTMENT == 1)
#error ( "wind gain adjustment is under construction and is not working at this time.")
	deflectionAccum.WW = __builtin_mulsu(deflectionAccum._.W1, wind_gain);
#endif // WIND_GAIN_ADJUSTMENT

	return deflectionAccum._.W1;
}
