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

// FLY BY WIRE module
// Responsible for turning user control demands into actions
// roll control -> roll angle demand + turn rate demand
// pitch control + camber control -> airspeed demand


#include "../MatrixPilot/defines.h"
#include "fbwCntrl.h"
#include "airframe.h"
#include "airspeedCntrlFBW.h"

// External variables
int16_t height_target_min		= HEIGHT_TARGET_MIN;
int16_t height_target_max		= HEIGHT_TARGET_MAX;
int16_t fbw_rollPositionMax 		= FBW_ROLL_POSITION_MAX;

_Q16 desiredRollPosition  	= 0;
_Q16 desiredPitchPosition  	= 0;
//fractional desiredTurnRate  = 0;

FBW_ASPD_MODE 		fbw_airspeed_mode 	= DEFAULT_FBW_AIRSPEED_MODE;
FBW_ROLL_MODE 		fbw_roll_mode 		= DEFAULT_FBW_ROLL_MODE;
FBW_ALTITUDE_MODE 	fbw_altitude_mode 	= DEFAULT_FBW_ALTITUDE_MODE;
FBW_PITCH_MODE 		fbw_pitch_mode 		= DEFAULT_FBW_PITCH_MODE;

AUTOPILOT_MODE old_flightmode = FLIGHT_MODE_MANUAL;

// Functions for handling state exit and entry
inline void fbwExitAPState(AUTOPILOT_MODE exitState);
inline void fbwEnterAPState(AUTOPILOT_MODE enterState);

// Get demand airspeed based on the mode.
int16_t fbwAirspeedControl(FBW_ASPD_MODE mode);

inline int16_t fbwAirspeedCamberControl();			// Get demand airspeed based on camber input.
inline int16_t fbwAirspeedCamberPitchControl();		// Get demand airspeed based on camber and pitch input.
inline _Q16 fbwPitchControlPitch(void);			// Get demand pitch based on pitch control

// Get demand roll position based on roll input.
_Q16 fbwRollPositionRollControl();

// Set the deisred airspeed in cm/s where desired airspeed units is dm/s
void setDesiredAirspeed(int16_t aspd);


inline _Q16 fbw_desiredRollPosition(void) { return desiredRollPosition;};
inline _Q16 fbw_desiredPitchPosition(void) { return desiredPitchPosition;};

extern FBW_ROLL_MODE fbw_get_roll_mode(void) 			{ return fbw_roll_mode;};
extern FBW_ALTITUDE_MODE fbw_get_altitude_mode(void) 	{ return fbw_altitude_mode;};
extern FBW_ASPD_MODE fbw_get_airspeed_mode(void) 		{ return fbw_airspeed_mode;};
extern FBW_PITCH_MODE fbw_get_pitch_mode(void) 			{ return fbw_pitch_mode;};

// Interpolate between two input points X1,Y1 and X2,Y2 where the input value is
// between X1 and X2.
// NOTE: X2 MUST BE LARGER THAN X1.
fractional interpolate(fractional input, fractional X1, fractional Y1, fractional X2, fractional Y2)
{
	fractional 		output;
	fractional 		delta;
	union longww 	ltemp;

	int16_t 			gain = 0;

	input -= X1;
	if(X2 <= X1) return Y1;
	if(Y1 == Y2) return Y1;

	delta = X2 - X1;

	// Find the gain required to increase delta to be in range RMAX to RMAX/2
	while(delta < RMAX/2)
	{
		gain++;
		delta <<= 1;
	}

	if(delta > RMAX)
	{
		gain--;
		delta >>= 1;
	}

	ltemp.WW = 0;
	ltemp._.W1 = (Y2 - Y1);		//  does this need to be inverted???


	// Limit numerator to +-RMAX/4 and adjust gain
	if(ltemp.WW > 0)
	{
		while(ltemp._.W1 > RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}
	else
	{
		while(ltemp._.W1 < -RMAX/4)
		{
			gain++;
			ltemp.WW >>= 1;
		}
	}

	output = __builtin_divsd( ltemp.WW,  delta ); //(int16_t) (fractional)

	
	ltemp.WW = __builtin_mulss(output, input);
	ltemp.WW <<= gain;
	output = (fractional) (ltemp._.W1 + Y1);

	return output;
}


int16_t find_aero_data_index_for_ref_input(aero_condition_point* pCondList, int16_t maxConds, fractional input)
{
	if(input < pCondList[0].condition_point) return -1;
	int16_t index;
	int16_t condLo, condHi;

	for(index=0; index < (maxConds-1); index++)
	{
		condLo = pCondList[index].condition_point;
		condHi = pCondList[index+1].condition_point;
		if( (input >= condLo) && 
			(input <  condHi) ) 
				return index;
	}
	return maxConds;
}

// return altitude in int32_t format where upper word is in meters, lower word is fractional meters.
inline int32_t get_fbw_demand_altitude(void)
{
	switch(fbw_altitude_mode)
	{
	case FBW_ALTITUDE_MODE_STANDARD:
	{
	#if (ALTITUDEHOLD_STABILIZED == AH_PITCH_ONLY)
		return goal.height ;
	//			// In stabilized mode using pitch-only altitude hold, use desiredHeight as
	//			// set from the state machine upon entering stabilized mode in ent_stabilizedS().
	#elif ( (ALTITUDEHOLD_STABILIZED == AH_FULL) || (ALTITUDEHOLD_STABILIZED == AH_THROTTLE_ONLY) )
	//			// In stabilized mode using full altitude hold, use the throttle stick value to determine desiredHeight,
		
		union longww temp ;
	
		// Scale the throttle to full scale less the throttle deadband
		temp._.W1 = in_cntrls[IN_CNTRL_THROTTLE] - THROTTLE_DEADBAND_RMAX;
		temp.WW = __builtin_mulss( temp._.W1, THROTTLE_DEADBAND_GAIN);
		temp.WW <<= 2;
	
		// Multiply scaled throttle by height target range.
		temp.WW = __builtin_mulss( temp._.W1 , (height_target_max - height_target_min) );
		temp.WW <<= 2;
	
		// Add the minimum height target offset
		temp._.W1 += (int32_t) height_target_min;
		int32_t desiredHeight = temp.WW;
	
		// Sanity check the result is in range.
//		if (desiredHeight < (int16_t)( height_target_min )) desiredHeight = (int16_t)( height_target_min ) ;
//		if (desiredHeight > (int16_t)( height_target_max )) desiredHeight = (int16_t)( height_target_max ) ;
	
		return desiredHeight;
	} break;
	default:
		return goal.height ;
		break;
	}	

#endif
}

// Fly by wire demand control.  Turns user input into demand.
void fbwDemandCntrl( void )
{
	
	if(get_flightmode() != old_flightmode)
	{
		fbwExitAPState(get_flightmode());
		fbwEnterAPState(get_flightmode());
	}

	if(get_flightmode() != FLIGHT_MODE_ASSISTED)
		return;

	switch(fbw_roll_mode)
	{
	case FBW_ROLL_MODE_POSITION:
		desiredRollPosition = fbwRollPositionRollControl();
		break;
	default:
		desiredRollPosition = 0;
		break;
	}

	switch(fbw_pitch_mode)
	{
	case FBW_PITCH_MODE_NONE:
		desiredPitchPosition = 0;
		break;
	case FBW_PITCH_MODE_PITCH:
		desiredPitchPosition = fbwPitchControlPitch();
		break;
	case FBW_PITCH_MODE_ASPD:
		fbwAirspeedControl(fbw_airspeed_mode);
		break;
	}
}


inline void fbwExitAPState(AUTOPILOT_MODE exitState)
{
	switch(exitState)
	{
	case FLIGHT_MODE_ASSISTED:
		setDesiredAirspeed(cruise_airspeed);
		break;
	default:
		break;
	}
}

inline void fbwEnterAPState(AUTOPILOT_MODE enterState)
{
	switch(enterState)
	{
	case FLIGHT_MODE_AUTONOMOUS:
		setDesiredAirspeed(cruise_airspeed);
		break;
	default:
		break;	
	}

	old_flightmode = enterState;
}

// Get demand airspeed in dm/s based on camber input.
int16_t fbwAirspeedControl(FBW_ASPD_MODE mode)
{
	int16_t aspd = 0;

	switch(mode)
	{
	case FBW_ASPD_MODE_CAMBER:
		aspd = fbwAirspeedCamberControl();
		break;
	case FBW_ASPD_MODE_CAMBER_AND_PITCH:
		aspd = fbwAirspeedCamberPitchControl();
		break;
	default:
		aspd = CRUISE_AIRSPEED;
	}
	
	setDesiredAirspeed(aspd);
	return aspd;
}

// sets desired airspeed in cm/s.  desired airspeed is stored in dm/s.
void setDesiredAirspeed(int16_t aspd)
{
	// Adjust cm/s airspeed to desiredSpeed dm/s units.
	union longww temp ;
	temp.WW = __builtin_mulss(aspd , (RMAX * 0.1) );
	temp.WW <<= 2;
	desiredSpeed = temp._.W1;
}


inline _Q16 fbwPitchControlPitch(void)
{
	union longww temp ;

	if(in_cntrls[IN_CNTRL_PITCH] >= 0)
	{
		temp.WW = __builtin_mulss( in_cntrls[IN_CNTRL_PITCH] , ( 3.14159265 / 2.0) * FBW_PITCH_MAX * (RMAX / 90.0) ) << 2;
		temp.WW >>= 14;
		return temp.WW;
	}
	else
	{
		temp.WW = __builtin_mulss( in_cntrls[IN_CNTRL_PITCH] , -1.0 * ( 3.14159265 / 2.0) * FBW_PITCH_MIN * (RMAX / 90.0) ) << 2;
		temp.WW >>= 14;
		return temp.WW;
	}
}


inline int16_t fbwAirspeedCamberControl()
{
	int16_t index = find_aero_data_index_for_ref_input(camber_aero_data, camber_aero_datapoints, in_cntrls[IN_CNTRL_CAMBER]);
	int16_t aspd = 0;

	if(index == -1)
	{
		aspd = camber_aero_data[0].data_point.airspeed_cruise;		
	}
	else if(index == camber_aero_datapoints)
	{
		aspd = camber_aero_data[camber_aero_datapoints].data_point.airspeed_cruise;
	}
	else
	{
		aspd = interpolate(in_cntrls[IN_CNTRL_CAMBER],
						camber_aero_data[index].condition_point,
						camber_aero_data[index].data_point.airspeed_cruise,
						camber_aero_data[index+1].condition_point,
						camber_aero_data[index+1].data_point.airspeed_cruise);
	};

	return aspd;
}



inline int16_t fbwAirspeedCamberPitchControl()
{
	int16_t index = find_aero_data_index_for_ref_input(camber_aero_data, camber_aero_datapoints, in_cntrls[IN_CNTRL_CAMBER]);
	int16_t aspdCruise = 0;
	int16_t aspdPoint = 0;
	fractional pitch = in_cntrls[IN_CNTRL_PITCH];

	if(index == -1)
	{
		aspdCruise = camber_aero_data[0].data_point.airspeed_cruise;
		if(pitch >= 0)
			aspdPoint = camber_aero_data[0].data_point.airspeed_stall;
		else
			aspdPoint = camber_aero_data[0].data_point.airspeed_vne;

	}
	else if(index == camber_aero_datapoints)
	{
		aspdCruise = camber_aero_data[camber_aero_datapoints].data_point.airspeed_cruise;
		if(pitch >= 0)
			aspdPoint = camber_aero_data[camber_aero_datapoints].data_point.airspeed_stall;
		else
			aspdPoint = camber_aero_data[camber_aero_datapoints].data_point.airspeed_vne;
	}
	else
	{
		aspdCruise = interpolate(in_cntrls[IN_CNTRL_CAMBER],
						camber_aero_data[index].condition_point,
						camber_aero_data[index].data_point.airspeed_cruise,
						camber_aero_data[index+1].condition_point,
						camber_aero_data[index+1].data_point.airspeed_cruise);

		if(pitch >= 0)
			aspdPoint = interpolate(in_cntrls[IN_CNTRL_CAMBER],
							camber_aero_data[index].condition_point,
							camber_aero_data[index].data_point.airspeed_stall,
							camber_aero_data[index+1].condition_point,
							camber_aero_data[index+1].data_point.airspeed_stall);
		else
			aspdPoint = interpolate(in_cntrls[IN_CNTRL_CAMBER],
							camber_aero_data[index].condition_point,
							camber_aero_data[index].data_point.airspeed_vne,
							camber_aero_data[index+1].condition_point,
							camber_aero_data[index+1].data_point.airspeed_vne);
	};

	if(pitch >= 0)
		return interpolate(	pitch, 0, aspdCruise, RMAX,	aspdPoint);
	else
		return interpolate(	pitch, -RMAX, aspdPoint, 0, aspdCruise );

}


// Return roll position in Q16 radians
_Q16 fbwRollPositionRollControl()
{
	union longww temp ;
	temp.WW = __builtin_mulss( fbw_rollPositionMax , ( ( 3.14159265 / 2.0) * RMAX / 90.0) );
	temp.WW = __builtin_mulss(in_cntrls[IN_CNTRL_ROLL] , temp._.W0 );
	temp.WW >>= 12;
	return temp.WW;
}


extern boolean fbwManualControlLockout(IN_CNTRL channel)
{
	// if in non fly-by-wire mode, never do lockout
	if(get_flightmode() != FLIGHT_MODE_ASSISTED)
		return false;

	switch(channel)
		{
		case IN_CNTRL_PITCH:
		{
			switch(fbw_pitch_mode)
			{
				case FBW_PITCH_MODE_NONE:
					return false;
					break;
				case FBW_PITCH_MODE_ASPD:
				{			
					switch(fbw_airspeed_mode)
						{
						case FBW_ASPD_MODE_CAMBER_AND_PITCH:
						case FBW_ASPD_MODE_PITCH:
							return true;
						default:
							return false;
							break;
						}
				} break;
				case FBW_PITCH_MODE_PITCH:
					return true;
					break;
			} break;
		}

		case IN_CNTRL_ROLL:
		{
			switch(fbw_roll_mode)
				{
				case FBW_ROLL_MODE_POSITION:
					return true;
				default:
					break;
				}
			} break;

		case IN_CNTRL_THROTTLE:
		case IN_CNTRL_YAW:
		case IN_CNTRL_BRAKE:
		case IN_CNTRL_FLAP:
		case IN_CNTRL_CAMBER:
		default:
			return false;
	}
	return false;
}


