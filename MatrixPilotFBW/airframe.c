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


#include "../MatrixPilot/defines.h"
#include "fbw_options.h"
#include "airframe.h"
//#include "airspeedCntrlFBW.h"
#include "../libDCM/libDCM.h"
#include "minifloat.h"
#include "inputCntrl.h"		// For limitRMAX()
#include "polar_data.h"
#include "airspeedCntrlFBW.h"

// CONSTANTS

#define INVERSE_GLIDE_RATIO (RMAX / CRUISE_GLIDE_RATIO)

#define AFRM_EFFECTIVE_AREA (AFRM_WING_AREA * AFRM_EFFECTIVE_AREA_RATIO)

// This constant is directly related to wing loading so has limited dynamic range
#define AFRM_CL_CALC_CONST	(2.0 * AFRM_AIRCRAFT_MASS / (AFRM_AIR_DENSITY * AFRM_EFFECTIVE_AREA))

// Inverse of constant used to calculate sink rate
// p*A*v^2/2
#define AFRM_AERO_CONST (1 / AFRM_CL_CALC_CONST )

#define AFRM_SINKRATE_CONST (AFRM_AERO_CONST / AFRM_GRAVITY)

// CL calculation constant for minifloat based calculation only
#define AFRM_CL_CALC_CONST_G (AFRM_CL_CALC_CONST * AFRM_GRAVITY)

//  Effective tail volume for calculating elevator Cl
#define AFRM_TAIL_VOLUME	((AFRM_TAIL_AREA/AFRM_WING_AREA) * (AFRM_TAIL_MOMENT / AFRM_MAC))

//  Inverse tail volume for easier calculation
#define AFRM_INV_TAIL_VOLUME	(1.0 / AFRM_TAIL_VOLUME)

// Constant used to calculate requried rudder Cl
// This is false since it uses aircraft mass as a scaling.
// It should use rotational moments
#define AFRM_RUDD_CL_CALC_CONST (2.0 * AFRM_AIRCRAFT_MASS / (AFRM_AIR_DENSITY * AFRM_FIN_AREA * AFRM_FIN_MOMENT))



// VARIABLES

// the indexes and gains describing the working polar in terms of stored polars
// index is for the lower value corner of the square to interpolate
// Gain is the ratio to interpolate between two reference polars
int16_t afrm_aspd_index  = 0;
int16_t afrm_flap_index = 0;
_Q16 afrm_flap_interp_gain = 0;
_Q16 afrm_aspd_interp_gain = 0;

_Q16 maxG_positive = MAX_G_POSITIVE * AFRM_Q16_SCALE;
_Q16 maxG_negative = MAX_G_NEGATIVE * AFRM_Q16_SCALE;

op_point afrm_opp;		// Current operating point for required Cl at aspd and flap.

// Glide ratio computed from working polar
minifloat afrm_glide_ratio = {0,0};


// FUNCTION DECLARATIONS

// Assisting functions
int16_t successive_interpolation(int16_t X, int16_t X1, int16_t X2, int16_t Y1, int16_t Y2);
_Q16 successive_interpolation_Q16(_Q16 X, _Q16 X1, _Q16 X2, _Q16 Y1, _Q16 Y2);

// Convert cm/s airspeed to m/s in minifloat
minifloat afrm_aspdcm_to_m(int16_t airspeedCm);

// Get the required operating point data for the required Cl at aspd and flap index.
// Warning, linear search does not cope with stalled polar in negative Cl
void afrm_get_polar_op_point(op_point* popp, minifloat Clmf, uint16_t aspd_index, uint16_t flap_index);


// Interpolate between two operating points using ratio to weight between them
// Ratio = 0 = 100% popp1, 0% popp2
// Ratio = 1 = 0% popp1, 100% popp2
void interpolate_op_point(_Q16 ratio, op_point* result, const op_point* popp1, const op_point* popp2);


// FUNCTION BODIES


// Update the status of the airframe including wing polars and aoa estimates
void airframeStateUpdate( void )
{
	afrm_find_working_polar( get_filtered_airspeed(), out_cntrls[IN_CNTRL_CAMBER] );
}



// Find the closest polars to the operating conditions and
// calculate the interpolation weightings for them
// Weightings used to calculate parameters Cl, Cd, Cm etc..
// Camber input is RMAX scaled control value which is turned into flap angle
void afrm_find_working_polar(int16_t airspeed, fractional camber)
{
	int16_t index;
	int16_t aspd_index  = 0;
	int16_t flap_index = 0;
	_Q16 flap_interp_gain = 65536;
	_Q16 aspd_interp_gain = 65536;

	_Q16 flap_angle = afrm_calc_flap_angle(camber);

	index =  AFRM_ASPD_POINTS;
	while(index >= 0)
	{
		if(airspeed < afrm_polar_aspd_settings[index])
			aspd_index = index - 1;
		index --;
	}

	index = AFRM_FLAP_POINTS;
	while(index >= 0)
	{
		if(flap_angle < afrm_polar_flap_settings[index])
			flap_index = index - 1;
		index --;
	}

	if(flap_index == -1)
	{
		flap_index = 0;
		flap_interp_gain = 0;
	}
	else if(flap_index == AFRM_FLAP_POINTS)
	{
		flap_interp_gain = 0;
	}
	else
	{
		flap_interp_gain = successive_interpolation_Q16(flap_angle,
			afrm_polar_flap_settings[flap_index],
			afrm_polar_flap_settings[flap_index + 1],
			0,
			65536);
	}


	if(aspd_index == -1)
	{
		aspd_index = 0;
		aspd_interp_gain = 0;
	}
	else if(aspd_index == AFRM_ASPD_POINTS)
	{
		aspd_interp_gain = 0;
	}
	else
	{
		aspd_interp_gain = successive_interpolation_Q16(airspeed,
			afrm_polar_aspd_settings[aspd_index],
			afrm_polar_aspd_settings[aspd_index + 1],
			0,
			65536);
	}

	afrm_aspd_index = aspd_index;
	afrm_flap_index = flap_index;
	afrm_flap_interp_gain = flap_interp_gain;
	afrm_aspd_interp_gain = aspd_interp_gain;
}


// Return load that is limited to airfame constraints
extern minifloat afrm_load_limit(minifloat requested_load)
{
	minifloat mftemp;

	if(requested_load.mant >= 0)
	{
		mftemp = Q16tomf(maxG_positive);
		if( mf_larger( mftemp , requested_load) == 1 )
			return mftemp;
		else
			return requested_load;
		
	}
	else
	{
		mftemp = Q16tomf(maxG_negative);
		if( mf_larger( mftemp , requested_load) == -1 )
			return mftemp;
		else
			return requested_load;
	}
}


// Get the required lift coefficient for the airspeed and load
// airspeed in cm/s
// load in GRAVITY scale
// Calculates Cl=2*m*g*load / (p * A * V^2)
// p=air density
// A = wing area
// m = mass, g = gravity constant
//  
minifloat afrm_get_required_Cl_mf(int16_t airspeed, minifloat load)
{
	minifloat Clmf = {0,0};
	union longww temp;

	// If airspeed is lower than 1m/s then don't try this calculation
	if(airspeed < 100) return Clmf;

	minifloat aspdmf = afrm_aspdcm_to_m(airspeed);
	minifloat aspd2mf = mf_mult(aspdmf, aspdmf);		// Airspeed^2

	temp.WW = (int32_t) (AFRM_CL_CALC_CONST_G * AFRM_Q16_SCALE);
	minifloat constmf = Q16tomf(temp.WW);

	Clmf = mf_mult(load, constmf);		// load * Cl calc constant
		
	Clmf = mf_div(Clmf, aspd2mf);			// Cl = load * Cl calc constant / aispeed^2

	return Clmf;
}
// Get the required operating point data for the required Cl at aspd and flap index.
// Warning, linear search does not cope with stalled polar in negative Cl
void afrm_get_polar_op_point(op_point* popp, minifloat Clmf, uint16_t aspd_index, uint16_t flap_index)
{
	const polar_point*  ppoint = NULL;
	const polar_point*  ppoint2 = NULL;
	uint16_t index = (aspd_index * AFRM_FLAP_POINTS) + flap_index;
	const polar2* const ppolar = &afrm_ppolars[index];

	uint16_t pnt_index;

	_Q16 Cl = mftoQ16(Clmf);

	int32_t temp = ppolar->point_count;
	temp++;
	temp = ppolar->maxCl_index;
	temp++;

	// Check if demand Cl is beyond endpoints of the polar.
	ppoint = &(ppolar->ppoints[0]);
	if(Cl <= ppoint->Cl) 
		ppoint2 = ppoint;
	else
	{
		ppoint = &(ppolar->ppoints[ppolar->maxCl_index]);
		if(Cl >= ppoint->Cl)
			ppoint2 = ppoint;
	}


	if(ppoint2 == NULL)
	{
		index = ppolar->maxCl_index;
		while(index > 0)		// No need to check zero index.  Done above.
		{
			if(Cl < ppolar->ppoints[index].Cl) 
				pnt_index = index;
			index--;
		}
		
		ppoint 	= &(ppolar->ppoints[pnt_index - 1]);
		ppoint2 = &(ppolar->ppoints[pnt_index]);
	
		popp->alpha =	successive_interpolation_Q16(Cl, 
				ppoint->Cl,
				ppoint2->Cl,
				ppoint->alpha, 
				ppoint2->alpha);
	
		popp->Cm =	successive_interpolation_Q16(Cl, 
				ppoint->Cl,
				ppoint2->Cl,
				ppoint->Cm,
				ppoint2->Cm);
		
		popp->Cd =	successive_interpolation_Q16(Cl, 
				ppoint->Cl,
				ppoint2->Cl,
				ppoint->Cd, 
				ppoint2->Cd);
	}
	else
	{
		popp->alpha = ppoint->alpha;
		popp->Cd = ppoint->Cd;
		popp->Cm = ppoint->Cm;
	}

	popp->Cl = Cl;
	popp->Clmax = ppolar->ppoints[ppolar->maxCl_index].Cl;

	return;
}


minifloat afrm_get_required_alpha_mf(int16_t airspeed, minifloat Clmf)
{
	minifloat mf;

	op_point opp_flaplow_aspdlow;
	op_point opp_flaplow_aspdhigh;
	op_point opp_flaphigh_aspdlow;
	op_point opp_flaphigh_aspdhigh;

	op_point opp_flaplow;
	op_point opp_flaphigh;

	op_point opp_aspdlow;
	op_point opp_aspdhigh;

	op_point opp;

	// Interpolate between operating points taking care of boundary conditions
	// where required flap or aspd is on the end of a polar.
	if( afrm_aspd_interp_gain == 0 )
	{
		if(afrm_flap_interp_gain == 0)
		{
			afrm_get_polar_op_point(&opp, Clmf, afrm_aspd_index, afrm_flap_index );	
		}
		else
		{
			afrm_get_polar_op_point(&opp_flaplow, Clmf, afrm_aspd_index, afrm_flap_index );
			afrm_get_polar_op_point(&opp_flaphigh, Clmf, afrm_aspd_index, afrm_flap_index +1 );
			interpolate_op_point(afrm_flap_interp_gain, &opp, &opp_flaplow, &opp_flaphigh);
		}
	}
	else
	{
		if( afrm_flap_interp_gain != 0 )
		{	
			afrm_get_polar_op_point(&opp_aspdlow, Clmf, afrm_aspd_index, afrm_flap_index );
			afrm_get_polar_op_point(&opp_aspdhigh, Clmf, afrm_aspd_index + 1, afrm_flap_index );
			interpolate_op_point(afrm_aspd_interp_gain, &opp, &opp_aspdlow, &opp_aspdhigh);
		}
		else
		{
			afrm_get_polar_op_point(&opp_flaplow_aspdlow, Clmf, afrm_aspd_index, afrm_flap_index );
			afrm_get_polar_op_point(&opp_flaplow_aspdhigh, Clmf, afrm_aspd_index + 1, afrm_flap_index );
			afrm_get_polar_op_point(&opp_flaphigh_aspdlow, Clmf, afrm_aspd_index, afrm_flap_index + 1);
			afrm_get_polar_op_point(&opp_flaphigh_aspdhigh, Clmf, afrm_aspd_index + 1, afrm_flap_index + 1);

			interpolate_op_point(afrm_aspd_interp_gain, &opp_flaplow, &opp_flaplow_aspdlow, &opp_flaplow_aspdhigh);
			interpolate_op_point(afrm_aspd_interp_gain, &opp_flaphigh, &opp_flaphigh_aspdlow, &opp_flaphigh_aspdhigh);

			interpolate_op_point(afrm_flap_interp_gain, &opp, &opp_flaplow, &opp_flaphigh);
		}		
	}

	afrm_opp = opp;

	mf = Q16tomf(afrm_opp.alpha);

	return mf;
}

// Return the expected glide ratio from the operating point
minifloat afrm_get_opp_glide_ratio(minifloat wing_aoa)
{
	minifloat ClCd;

	minifloat Cl = Q16tomf(afrm_opp.Cl);
	minifloat Cd = Q16tomf(afrm_opp.Cd);

	ClCd = mf_div(Cl, Cd);
	return ClCd;
}

// Return the expected glide ratio from the operating point
minifloat afrm_get_opp_sink_rate(uint16_t airspeed)
{
	minifloat Cd = Q16tomf(afrm_opp.Cd);
	minifloat aspd = afrm_aspdcm_to_m(airspeed);
	minifloat aspd3 = mf_mult(aspd, aspd);	// aspd**2
	aspd3 = mf_mult(aspd3, aspd);			// aspd**3

	// Calc Cd*p*A*V^3/2mg
	minifloat temp = ftomf(AFRM_SINKRATE_CONST);
	temp = mf_mult(temp, Cd);
	temp = mf_mult(temp, aspd3);
	return temp;
}

// Tail coefficient of lift = Wing Cm / effective tail volume
// wing_aoa in degrees
minifloat afrm_get_tail_required_Cl_mf(minifloat wing_aoa)
{
	minifloat Cm_mf;
	minifloat mf_temp;

	// Find wing Cm
	Cm_mf = Q16tomf(afrm_opp.Cm);
	
	mf_temp = Q16tomf(AFRM_Q16_SCALE * AFRM_INV_TAIL_VOLUME);

	// Multiply wing Cm by 1 / tail volume
	mf_temp = mf_mult(mf_temp, Cm_mf);

	return mf_temp;
}

minifloat afrm_get_tail_required_alpha(minifloat Clmf_tail)
{
	// Scale Cl to alpha
	minifloat mf = {160, 4}; // 10.0 degrees at Cl = 1;
	mf = mf_mult(mf, Clmf_tail);

	return mf;
}


// return the RMAX scale control requried for an required elevator pitch
fractional lookup_elevator_control( minifloat pitch )
{
	_Q16 elev_pitch = mftoQ16(pitch);

	int16_t index;
	// Make sure that if the angle is out of bounds then the limits are returned
	if(elev_pitch < elevator_angles[0].surface_deflection)
		return elevator_angles[0].ap_control;

	if(elev_pitch > elevator_angles[elevator_angle_points - 1].surface_deflection)
		return elevator_angles[elevator_angle_points - 1].ap_control;

	index = elevator_angle_points - 1;
	while(elev_pitch < elevator_angles[index - 1].surface_deflection)
	{
		index--;
	}

	return successive_interpolation_Q16(elev_pitch, 
			elevator_angles[index-1].surface_deflection,
			elevator_angles[index].surface_deflection, 
			elevator_angles[index-1].ap_control,
			elevator_angles[index].ap_control);
}


// Find the Cl required from the rudder for the yaw moment
// Yaw moment in Nm
// airspeed in cm/s
// Moment = force * length of moment arm
// Force = Cl * p * A / 
minifloat afrm_get_rudd_required_Cl(int16_t airspeed, minifloat yaw_moment)
{
	minifloat Clmf = {0,0};
	union longww temp;

	// If airspeed is lower than 1m/s then don't try this calculation
	if(airspeed < 100) return Clmf;

	minifloat aspdmf = afrm_aspdcm_to_m(airspeed);
	minifloat aspd2mf = mf_mult(aspdmf, aspdmf);		// Airspeed^2

	temp.WW = (int32_t) (AFRM_RUDD_CL_CALC_CONST * AFRM_Q16_SCALE);
	minifloat constmf = Q16tomf(temp.WW);

	Clmf = mf_mult(yaw_moment, constmf);		// load * Cl calc constant
		
	Clmf = mf_div(Clmf, aspd2mf);			// Cl = load * Cl calc constant / aispeed^2

	return Clmf;
}

// Convert rudder aoa into rudder command
fractional lookup_rudder_control( minifloat aoa )
{
	_Q16 rudd_angle = mftoQ16(aoa);

	int16_t index;
	// Make sure that if the angle is out of bounds then the limits are returned
	if(rudd_angle < rudder_angles[0].surface_deflection)
		return rudder_angles[0].ap_control;

	if(rudd_angle > rudder_angles[rudder_angle_points - 1].surface_deflection)
		return rudder_angles[rudder_angle_points - 1].ap_control;

	index = rudder_angle_points - 1;
	while(rudd_angle < rudder_angles[index - 1].surface_deflection)
	{
		index--;
	}

	return successive_interpolation_Q16(rudd_angle, 
			rudder_angles[index-1].surface_deflection,
			rudder_angles[index].surface_deflection, 
			rudder_angles[index-1].ap_control,
			rudder_angles[index].ap_control);
}



// Find the aoa delta required to give a required roll rate
// This is based on the helix angle
minifloat afrm_get_roll_rate_required_aoa_delta(int16_t airspeed, minifloat roll_rate)
{
	minifloat aoa_delta = {0,0};
	minifloat aspdmf = afrm_aspdcm_to_m(airspeed);

	// Adjust for aileron authority
	aspdmf = mf_mult(aspdmf, ftomf(AFRM_AILERON_AUTHORITY) );

	// Calculate required delta aoa at 1m wingspan
	aoa_delta = mf_div(roll_rate, aspdmf);
	
	// Multiply the roll rate by the span to get change in aoa
	aoa_delta = mf_mult(aoa_delta, ftomf(AFRM_AILERON_AERODYNAMIC_SPAN) );

	return aoa_delta;
}

// 
minifloat afrm_get_aileron_deflection(minifloat aoa_delta)
{
	minifloat ail_def = mf_mult(aoa_delta, ftomf( 1 / AFRM_AILERON_CHORD_RATIO ));
	return ail_def;
}


// Convert aileron aoa into rudder command
fractional afrm_lookup_aileron_control( minifloat angle )
{
	_Q16 ail_angle = mftoQ16(angle);

	int16_t index;
	// Make sure that if the angle is out of bounds then the limits are returned
	if(ail_angle < aileron_angles[0].surface_deflection)
		return aileron_angles[0].ap_control;

	if(ail_angle > aileron_angles[aileron_angle_points - 1].surface_deflection)
		return aileron_angles[aileron_angle_points - 1].ap_control;

	index = aileron_angle_points - 1;
	while(ail_angle < aileron_angles[index - 1].surface_deflection)
	{
		index--;
	}

	return successive_interpolation_Q16(ail_angle, 
			aileron_angles[index-1].surface_deflection,
			aileron_angles[index].surface_deflection, 
			aileron_angles[index-1].ap_control,
			aileron_angles[index].ap_control);
}


_Q16 afrm_calc_flap_angle(fractional flap_setting)
{
	int16_t index;
	// Make sure that if the angle is out of bounds then the limits are returned
	if(flap_setting < flap_angles[0].ap_control)
		return flap_angles[0].surface_deflection;

	if(flap_setting > flap_angles[flap_angle_points - 1].ap_control)
		return flap_angles[flap_angle_points - 1].surface_deflection;

	index = aileron_angle_points - 1;
	while(flap_setting < flap_angles[index - 1].ap_control)
	{
		index--;
	}

	return successive_interpolation_Q16(flap_setting,
			flap_angles[index-1].ap_control,
			flap_angles[index].ap_control,
			flap_angles[index-1].surface_deflection,
			flap_angles[index].surface_deflection);
}




// Calculate the expected descent rate in cm/s at the given airspeed in cm/s
// if the aircraft were gliding.  This is a measure of expected energy loss.
// It is also known as Cl/Cd or the Lift/Drag coefficent.
// Output is positive for descending.
// inputs are airspeed in cm/s and degrees angle of attack
int16_t expected_glide_descent_rate(int16_t airspeed, minifloat aoa)
{
	// Get the expected descent rate
	union longww temp;
	temp.WW = __builtin_muluu( air_speed_3DIMU, INVERSE_GLIDE_RATIO);
	temp.WW <<= 2;
	return temp._.W1;
}

// Calculate the expected climb rate depending on a throttle setting and airspeed
// glide_ratio.  rate and airspeed in cm/s.  Descent rate is positive falling.
// Returned value is positive rising in cm/s
int16_t feedforward_climb_rate(fractional throttle, int16_t glide_descent_rate, int16_t airspeed)
{
	// TODO - Correct the assumption that scale changes with descent rate.
	int16_t rateScale = (MAX_THROTTLE_CLIMB_RATE * 100) + glide_descent_rate;

	union longww temp;
	temp.WW = __builtin_mulss( throttle, rateScale);
	temp.WW <<= 2;
	temp._.W1 -= glide_descent_rate;
	return temp._.W1;
}


// Convert cm/s to m/s minifloat
minifloat afrm_aspdcm_to_m(int16_t airspeedCm)
{
	minifloat aspdmf = ltomf(airspeedCm);
	minifloat tempmf = {164, -6};		// 0.01
	aspdmf = mf_mult(aspdmf, tempmf);
	return aspdmf;
}


// Airframe data interpolation funcitons

// Interpolate between two operating points using ratio to weight between them
// Ratio = 0 = 100% popp1, 0% popp2
// Ratio = 1 = 0% popp1, 100% popp2
void interpolate_op_point(_Q16 ratio, op_point* result, const op_point* popp1, const op_point* popp2)
{
	if(popp1->Cl != popp2->Cl)
		result->Cl = successive_interpolation_Q16(ratio,
			0,
			65536,
			popp1->Cl,
			popp2->Cl);
	else
		result->Cl = popp1->Cl;	
	
	if(popp1->Cd != popp2->Cd)
		result->Cd = successive_interpolation_Q16(ratio,
			0,
			65536,
			popp1->Cd,
			popp2->Cd);
	else
		result->Cd = popp1->Cd;	
	
	if(popp1->Cm != popp2->Cm)
		result->Cm = successive_interpolation_Q16(ratio,
			0,
			65536,
			popp1->Cm,
			popp2->Cm);
	else
		result->Cm = popp1->Cm;	

	if(popp1->alpha != popp2->alpha)
		result->alpha = successive_interpolation_Q16(ratio,
			0,
			65536,
			popp1->alpha,
			popp2->alpha);
	else
		result->alpha = popp1->alpha;	
	
}



int16_t successive_interpolation(int16_t X, int16_t X1, int16_t X2, int16_t Y1, int16_t Y2)
{
	int16_t X1temp = X1;
	int16_t X2temp = X2;
	int16_t Y1temp = Y1;
	int16_t Y2temp = Y2;
	int16_t Xtemp;

	// Test for out of limit.  Return limit if so.
	if( (X2-X1) > 0)
	{
		if(X > X2) return Y2;
		if(X < X1) return Y1;
	}
	else
	{
		if(X < X2) return Y2;
		if(X > X1) return Y1;
	}

	// Repeat approximation until magnitude difference between X estiamtes is <= 1
	while( ((X2temp - X1temp) >> 1) != 0)
	{ 
		int16_t deltaX = (X2temp - X1temp) >> 1;
		int16_t deltaY = (Y2temp - Y1temp) >> 1;
		Xtemp  = X - X1temp;

		if(deltaX > 0)
		{
			if(Xtemp > deltaX)
			{
				X1temp += deltaX;
				Y1temp += deltaY;
			}
			else
			{
				X2temp -= deltaX;
				Y2temp -= deltaY;
			}
		}
		else
		{
			if(Xtemp < deltaX)
			{
				X1temp -= deltaX;
				Y1temp -= deltaY;
			}
			else
			{
				X2temp += deltaX;
				Y2temp += deltaY;
			}
		}

	}

	// Last selection is when |X1-X2| <= 1
	if(X == X1temp)
		return Y1temp;
	else
		return Y2temp;
}


// Succesive interpolation between X inputs of Y outputs
_Q16 successive_interpolation_Q16(_Q16 X, _Q16 X1, _Q16 X2, _Q16 Y1, _Q16 Y2)
{
	_Q16 X1temp = X1;
	_Q16 X2temp = X2;
	_Q16 Y1temp = Y1;
	_Q16 Y2temp = Y2;
	_Q16 Xtemp;

	// Test for out of limit.  Return limit if so.
	if( (X2-X1) > 0)
	{
		if(X > X2) return Y2;
		if(X < X1) return Y1;
	}
	else
	{
		if(X < X2) return Y2;
		if(X > X1) return Y1;
	}

	// Repeat approximation until magnitude difference between X estiamtes is <= 1
	while( ((X2temp - X1temp) >> 1) != 0)
	{ 
		_Q16 deltaX = (X2temp - X1temp) >> 1;
		_Q16 deltaY = (Y2temp - Y1temp) >> 1;
		Xtemp  = X - X1temp;

		if(deltaX > 0)
		{
			if(Xtemp > deltaX)
			{
				X1temp += deltaX;
				Y1temp += deltaY;
			}
			else
			{
				X2temp -= deltaX;
				Y2temp -= deltaY;
			}
		}
		else
		{
			if(Xtemp < deltaX)
			{
				X1temp -= deltaX;
				Y1temp -= deltaY;
			}
			else
			{
				X2temp += deltaX;
				Y2temp += deltaY;
			}
		}

	}

	// Last selection is when |X1-X2| <= 1
	if(X == X1temp)
		return Y1temp;
	else
		return Y2temp;
}
