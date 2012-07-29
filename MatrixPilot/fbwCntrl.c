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


#include "defines.h"
#include "fbwCntrl.h"
#include "airframe.h"
#include "airspeedCntrl.h"

#if(USE_FBW == 1)

fractional desiredRollPosition  = 0;
fractional desiredTurnRate 		= 0;

FBW_ASPD_MODE fbw_airspeed_mode = DEFAULT_FBW_AIRSPEED_MODE;
FBW_ROLL_MODE fbw_roll_mode 	= DEFAULT_FBW_ROLL_MODE;

// Remember the autopilot state so that FBW can enter and exit those states in a tidy way.
AP_STATE fbw_ap_state = AP_STATE_MANUAL;

// Functions for handling state exit and entry
inline void fbwExitAPState(AP_STATE exitState);
inline void fbwEnterAPState(AP_STATE enterState);


// Get demand airspeed based on the mode.
int fbwAirspeedControl(FBW_ASPD_MODE mode);

inline int fbwAirspeedCamberControl();			// Get demand airspeed based on camber input.
inline int fbwAirspeedCamberPitchControl();	// Get demand airspeed based on camber and pitch input.

// Get demand roll position based on roll input.
fractional fbwRollPositionRollControl();

// Set the deisred airspeed in cm/s where desired airspeed units is dm/s
void setDesiredAirspeedCMS(int aspd);


// Interpolate between two input points X1,Y1 and X2,Y2 where the input value is
// between X1 and X2.
// NOTE: X2 MUST BE LARGER THAN X1.
fractional interpolate(fractional input, fractional X1, fractional Y1, fractional X2, fractional Y2)
{
	fractional 		output;
	fractional 		delta;
	union longww 	ltemp;

	int 			gain = 0;

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

	output = __builtin_divsd( ltemp.WW,  delta ); //(int) (fractional)

	
	ltemp.WW = __builtin_mulss(output, input);
	ltemp.WW <<= gain;
	output = (fractional) (ltemp._.W1 + Y1);

	return output;
}


int find_aero_data_index_for_ref_input(aero_condition_point* pCondList, int maxConds, fractional input)
{
	if(input < pCondList[0].condition_point) return -1;
	int index;
	int condLo, condHi;

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


// Fly by wire demand control.  Turns user input into demand.
void fbwDemandCntrl( void )
{
	
	if(ap_state() != fbw_ap_state)
	{
		fbwExitAPState(fbw_ap_state);
		fbwEnterAPState(ap_state());
	}

	if(fbw_ap_state != AP_STATE_STABILIZED)
		return;

	switch(fbw_airspeed_mode)
	{
	case FBW_ASPD_MODE_CAMBER:
	case FBW_ASPD_MODE_CAMBER_AND_PITCH:
		desiredSpeed = fbwAirspeedControl(fbw_airspeed_mode);
		break;
	default:
		setDesiredAirspeed(cruise_airspeed);
		break;
	}


	switch(fbw_roll_mode)
	{
	case FBW_ROLL_MODE_POSITION:
		desiredRollPosition = fbwRollPositionRollControl();
		break;
	default:
		setDesiredAirspeed(cruise_airspeed);
		break;
	}
}


inline void fbwExitAPState(AP_STATE exitState)
{
	switch(exitState)
	{
	case AP_STATE_STABILIZED:
		setDesiredAirspeed(cruise_airspeed);
		break;
	default:
		break;
	}
}

inline void fbwEnterAPState(AP_STATE enterState)
{
	switch(enterState)
	{
	case AP_STATE_GUIDED:
		setDesiredAirspeed(cruise_airspeed);
	}

	fbw_ap_state = enterState;
}

// Get demand airspeed in dm/s based on camber input.
int fbwAirspeedControl(FBW_ASPD_MODE mode)
{
	int aspd = 0;

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
}

// sets desired airspeed in cm/s.  desired airspeed is stored in dm/s.
void setDesiredAirspeed(int aspd)
{
	// Adjust cm/s airspeed to desiredSpeed dm/s units.
	union longww temp ;
	temp.WW = __builtin_mulss(aspd , (RMAX * 0.1) );
	temp.WW <<= 2;
	desiredSpeed = temp._.W1;
}

inline int fbwAirspeedCamberControl()
{
	int index = find_aero_data_index_for_ref_input(camber_aero_data, camber_aero_datapoints, in_cntrls[IN_CNTRL_CAMBER]);
	int aspd = 0;

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

inline int fbwAirspeedCamberPitchControl()
{
	int index = find_aero_data_index_for_ref_input(camber_aero_data, camber_aero_datapoints, in_cntrls[IN_CNTRL_CAMBER]);
	int aspdCruise = 0;
	int aspdPoint = 0;
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



fractional fbwRollPositionRollControl()
{
	union longww temp ;
	temp.WW = __builtin_mulss(in_cntrls[IN_CNTRL_ROLL] , (RMAX * FBW_ROLL_POSITION_MAX / 90.0) );
//	temp.WW <<= 2;	Don't do this to do the right scaling.
	temp.WW >>= 2;
	return temp._.W1;
}


extern boolean fbwManualControlLockout(IN_CNTRL channel)
{
	// if in manual mode, never do lockout
	if(ap_state == AP_STATE_MANUAL)
		return false;

	switch(channel)
		{
		case IN_CNTRL_PITCH:
			{
			switch(fbw_airspeed_mode)
				{
				case FBW_ASPD_MODE_CAMBER_AND_PITCH:
				case FBW_ASPD_MODE_ELEVATOR:
					return true;
				default:
					break;
				}
			} break;
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

#endif	//#if(USE_FBW == 1)
