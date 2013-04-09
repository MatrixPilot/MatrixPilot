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

// Airframe data

#ifndef AIRFRAME_H
#define AIRFRAME_H

#include "airframe_options.h"
#include "minifloat.h"
#include <libq.h>


// Update the status of the airframe including wing polars and aoa estimates
extern void airframeStateUpdate( void );

// Defines unity coefficient of lift as RMAX/2
// This gives headroom for wing sections with Cl above 2.
//#define AFRM_CL_SCALE	(RMAX / 4)
#define AFRM_GRAVITY 	9.81
//#define DEG_TO_CIRCULAR_SCALE (RMAX / 90.0)

#define AFRM_Q16_SCALE	65536.0

/*************************************************************/
// AIRFRAME DATA
// This section describes the airfram data sctructures

typedef struct polar_point_tag
{
	_Q16			alpha;	// Wing angle of attack in degrees
	_Q16 			Cl;		// Lift coefficient
	_Q16			Cd;		// Drag coefficient
	_Q16			Cm;		// 1/4 chord moment coefficient (is it needed with Cp?)
//	fractional		Cp;		// Centre of pressure as percentage of wing chord
//	int16_t		ClCdx10;
} polar_point;


typedef struct polar
{
	int16_t 		airspeed;
	polar_point	points[AFRM_OPT_POINTS_PER_POLAR];
} polar;

//extern const polar normal_polars[AFRM_OPT_POLARS_PER_CONDITION];


typedef struct polar2_tag
{
	const uint16_t				airspeed;
	const _Q16					flap;
	const uint16_t				point_count;
	const polar_point* const	ppoints;
	const uint16_t				maxCl_index;
} polar2;

typedef struct op_point_tag
{
	uint16_t				aairspeed;
	_Q16					flap;
	_Q16					alpha;
	_Q16					Cl;
	_Q16					Cd;
	_Q16					Cm;
	_Q16					Clmax;
} op_point;


// Data arrays containing polars and poalr reference data

// Array of polars organised as [flap][aspd]
extern const polar2 afrm_ppolars[];
extern const _Q16 afrm_polar_flap_settings[];
extern const _Q16 afrm_polar_aspd_settings[];



// All airspeeds in cm/s
// best lift/drag in ratio * 10
// sink rate in cm/s
typedef struct tag_aero_data
{
	int16_t airspeed_stall;
	int16_t airspeed_vne;
	int16_t airspeed_cruise;
	int16_t cruise_ld;
} aero_data;

typedef struct tag_aero_condition_point
{
	int16_t					condition_point;
	aero_data			data_point;
} aero_condition_point;

extern const aero_condition_point camber_aero_data[];
extern const int16_t camber_aero_datapoints;


typedef struct control_surface_angle_tag
{
	fractional ap_control;
	_Q16 surface_deflection;
} control_surface_angle;

extern const control_surface_angle elevator_angles[];
extern const int16_t elevator_angle_points;

extern const control_surface_angle rudder_angles[];
extern const int16_t rudder_angle_points;

// Decribes delta aileron angles.  Will be replaced by polars
extern const control_surface_angle aileron_angles[];
extern const int16_t aileron_angle_points;

// Decribes flap angles
extern const control_surface_angle flap_angles[];
extern const int16_t flap_angle_points;


/*************************************************************/
// AIRFRAME FUNCTIONS
// This section describes functions for calculating aircraft
// aerodynamics and flight surface movements

// Get the required lift coefficient for the airspeed
minifloat afrm_get_required_Cl_mf(int16_t airspeed, minifloat load);

// Get the maximum acceleration avaiable at a given airspeed and Clmax
fractional afrm_get_max_accn_mf(int16_t airspeed, minifloat Clmax);

// Get the required angle of attack (alpha) from a given airspeed and Cl
// Returns RMAX if the required Cl is not acheivable - TODO, not yet!
//minifloat afrm_get_required_alpha(int16_t airspeed, minifloat Cl);
minifloat afrm_get_required_alpha_mf(int16_t airspeed, minifloat Clmf);

// Calculate necessary elevator Cl to balance wing pitch moment
minifloat afrm_get_tail_required_Cl_mf(minifloat wing_aoa);

// Turn tail required Cl into tail pitch against airflow.
minifloat afrm_get_tail_required_alpha(minifloat Clmf_tail);

// Convert elevator aoa into elevator command
fractional lookup_elevator_control( minifloat pitch );

// Find the Cl required from the rudder for the yaw moment
// Yaw moment in Nm
minifloat afrm_get_rudd_required_Cl(int16_t airspeed, minifloat yaw_moment);

// Convert rudder aoa into rudder command
fractional lookup_rudder_control( minifloat aoa );

// Find the aoa delta required to give a required roll rate
minifloat afrm_get_roll_rate_required_aoa_delta(int16_t airspeed, minifloat roll_rate);

// Find the aileron deflection required for the aoa change required
minifloat afrm_get_aileron_deflection(minifloat aoa_delta);

// Lookup the aileron control required for the aileron angle
fractional afrm_lookup_aileron_control( minifloat angle );

int16_t successive_interpolation(int16_t X, int16_t X1, int16_t X2, int16_t Y1, int16_t Y2);
_Q16 successive_interpolation_Q16(_Q16 X, _Q16 X1, _Q16 X2, _Q16 Y1, _Q16 Y2);

extern _Q16 afrm_calc_flap_angle(fractional flap_setting);

extern void afrm_find_working_polar(int16_t airspeedCm, fractional camber);

extern minifloat afrm_get_estimated_aoa(int16_t airspeed, minifloat load);

extern int16_t expected_glide_descent_rate(int16_t airspeed, minifloat aoa);

extern minifloat afrm_get_opp_sink_rate(uint16_t airspeed);

// Calculate the expected climb rate depending on a throttle setting and airspeed
extern int16_t feedforward_climb_rate(fractional throttle, int16_t glide_descent_rate, int16_t airspeed);

// Return load that is limited to airfame constraints
extern minifloat afrm_load_limit(minifloat requested_load);

//typedef struct tag_aero_condition_list
//{
//	char listName[15];
//	const aero_condition_point*	pcond_list;
//	
//} aero_condition_list
//
//typedef struct tagAirframeDataMap
//{
//} AirframeDataMap;
//
//extern int16_t condition_list_count[];
//extern int16_t condition_list_sizes[];


#endif  // AIRFRAME_H

