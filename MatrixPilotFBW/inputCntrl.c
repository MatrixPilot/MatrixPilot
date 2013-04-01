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

// Responsible for turning tranmitter input values into RMAX scaled values
// with corrected direction.  It is done here so that nowhere else has to do it.

#include "../MatrixPilot/defines.h"
#include "inputCntrl.h"
#include "fbw_options.h"

#if(USE_INPUT_CONTROL == 1)

// RMAX scaled inputs
fractional in_cntrls[IN_CNTRL_MAX];


//limit a int32_t variable to +-RMAX
int32_t limitRMAX(int32_t input)
{
	if(input > RMAX) return RMAX;
	if(input < -RMAX) return -RMAX;
	return input;
}

// Turn PWM into fraction subtracting the offset and reversing
fractional PWM_to_frac(int16_t PWM, int16_t offset, boolean reversed)
{
	union longww temp;
	temp.WW = ( (RMAX * 256.0) / ( MIX_PWM_RANGE ) );
	temp.WW = __builtin_mulss( PWM - offset, temp._.W0);  //
	temp.WW >>= 8;
	if(reversed)
		return -(fractional) temp._.W0;
	else
		return (fractional) temp._.W0;
};

// Turn fractional RMAX scaled into PWM adding the offset and reversing
// doubleRange is used for throttle which goes +-100% of servo travel
int16_t frac_to_PWM(fractional frac, int16_t offset, boolean reversed, boolean doubleRange)
{
	union longww temp;

	if(doubleRange == false)
		temp.WW = MIX_PWM_RANGE;
	else
		temp.WW = (MIX_PWM_RANGE * 2);

	if(reversed)
		temp.WW = -temp.WW;

	temp.WW = __builtin_mulss( frac , temp._.W0);
	temp.WW <<= 2;
	temp._.W1 += offset;
	return temp._.W1;
};


// turn PWM inputs into RMAX scaled values with corrected reversing
// Check if radio is on, set to zero if not.
void input_controls( void )
{
	// Scale throttle to 0 to MIX_PWM_RANGE instead of 0 to 2 * MIX_PWM_RANGE
	// This stops the fractional overflowing 2*RMAX
	int16_t tempThrottle = udb_pwIn[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL];
	tempThrottle = tempThrottle >> 1;

	if (get_flightmode() != FLIGHT_MODE_NO_RADIO)
	{
		in_cntrls[IN_CNTRL_YAW] 		= PWM_to_frac(udb_pwIn[YAW_INPUT_CHANNEL], udb_pwTrim[YAW_INPUT_CHANNEL], YAW_CHANNEL_REVERSED);
		in_cntrls[IN_CNTRL_ROLL] 		= PWM_to_frac(udb_pwIn[ROLL_INPUT_CHANNEL], udb_pwTrim[ROLL_INPUT_CHANNEL], ROLL_CHANNEL_REVERSED);
		in_cntrls[IN_CNTRL_PITCH] 		= PWM_to_frac(udb_pwIn[PITCH_INPUT_CHANNEL], udb_pwTrim[PITCH_INPUT_CHANNEL], PITCH_CHANNEL_REVERSED);
		in_cntrls[IN_CNTRL_CAMBER] 		= PWM_to_frac(udb_pwIn[CAMBER_INPUT_CHANNEL], udb_pwTrim[CAMBER_INPUT_CHANNEL], CAMBER_CHANNEL_REVERSED);
		in_cntrls[IN_CNTRL_FLAP] 		= PWM_to_frac(udb_pwIn[FLAP_INPUT_CHANNEL], udb_pwTrim[CAMBER_INPUT_CHANNEL], FLAP_CHANNEL_REVERSED);
		in_cntrls[IN_CNTRL_BRAKE] 		= PWM_to_frac(udb_pwIn[BRAKE_INPUT_CHANNEL], udb_pwTrim[CAMBER_INPUT_CHANNEL], BRAKE_CHANNEL_REVERSED);

		// Throttle range /2 to fit normal RMAX scaling.
		if(udb_pwIn[THROTTLE_INPUT_CHANNEL] < udb_pwTrim[THROTTLE_INPUT_CHANNEL])
			tempThrottle = 0;
		else
			tempThrottle = udb_pwIn[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL];

		tempThrottle = tempThrottle >> 1;

		in_cntrls[IN_CNTRL_THROTTLE] 	= PWM_to_frac(tempThrottle, 0, THROTTLE_CHANNEL_REVERSED);
	}
	else
	{
		in_cntrls[IN_CNTRL_YAW] 		= 0;
		in_cntrls[IN_CNTRL_ROLL] 		= 0;
		in_cntrls[IN_CNTRL_PITCH] 		= 0;
		in_cntrls[IN_CNTRL_THROTTLE] 	= 0;
		in_cntrls[IN_CNTRL_FLAP]	 	= 0;
		in_cntrls[IN_CNTRL_CAMBER]	 	= 0;
		in_cntrls[IN_CNTRL_BRAKE]	 	= 0;
	}
}

#endif	//#if(USE_INPUT_CONTROL == 1)
