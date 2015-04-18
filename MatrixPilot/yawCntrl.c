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
#include "servoPrepare.h"
#include "states.h"
#include "helicalTurnCntrl.h"
#include "../libDCM/rmat.h"

#define HOVERYOFFSET ((int32_t)(HOVER_YAW_OFFSET*(RMAX/57.3)))

#if (USE_CONFIGFILE == 1)
#include "config.h"
#include "redef.h"
#endif // USE_CONFIGFILE

uint16_t yawkdrud;
uint16_t yawkpfdfwd;
uint16_t rollkprud;
uint16_t rollkdrud;
uint16_t hoveryawkp;
uint16_t hoveryawkd;

void normalYawCntrl(void);
void hoverYawCntrl(void);

void init_yawCntrl(void)
{
	yawkdrud   = (uint16_t)(YAWKD_RUDDER*SCALEGYRO*RMAX);
	yawkpfdfwd = (uint16_t)(FEED_FORWARD*YAWKP_RUDDER*RMAX);
	rollkprud  = (uint16_t)(ROLLKP_RUDDER*RMAX);
	rollkdrud  = (uint16_t)(ROLLKD_RUDDER*SCALEGYRO*RMAX);
	hoveryawkp = (uint16_t)(HOVER_YAWKP*RMAX);
	hoveryawkd = (uint16_t)(HOVER_YAWKD*SCALEGYRO*RMAX);
}

void save_yawCntrl(void)
{
#if (USE_CONFIGFILE == 1)
	gains.YawKDRudder  = (float)yawkdrud   / (SCALEGYRO*RMAX);
	gains.RollKPRudder = (float)rollkprud  / (RMAX);
	gains.RollKDRudder = (float)rollkdrud  / (SCALEGYRO*RMAX);
	gains.HoverYawKP   = (float)hoveryawkp / (RMAX);
	gains.HoverYawKD   = (float)hoveryawkd / (SCALEGYRO*RMAX);
#endif // USE_CONFIGFILE
}

void yawCntrl(void)
{
	if (canStabilizeHover() && current_orientation == F_HOVER)
	{
		hoverYawCntrl();
	}
	else
	{
		normalYawCntrl();
	}
}

void normalYawCntrl(void)
{
	union longww rollStabilization;
	union longww gyroYawFeedback;
	union longww yawStabilization ;
	int16_t ail_rud_mix;

#ifdef TestGains
	state_flags._.GPS_steering = 0; // turn off navigation
	state_flags._.pitch_feedback = 1; // turn on stabilization
#endif 

	if (YAW_STABILIZATION_RUDDER && state_flags._.pitch_feedback)
	{
		gyroYawFeedback.WW =  - __builtin_mulsu(rotationRateError[2], yawkdrud);
		yawStabilization.WW = - __builtin_mulsu(tiltError[2], yawkprud ) ;  // yaw orientation error in body frame
		yawStabilization.WW += __builtin_mulsu(desiredRotationRateRadians[2], yawkpfdfwd ) ; // feed forward term
	}
	else
	{
		gyroYawFeedback.WW = 0;
		yawStabilization.WW = 0;
	}

	rollStabilization.WW = 0; // default case is no roll rudder stabilization
	if (ROLL_STABILIZATION_RUDDER && state_flags._.pitch_feedback)
	{
		rollStabilization.WW = - __builtin_mulsu(tiltError[1], rollkprud); // this works right side up or upside down
	}

	if (state_flags._.pitch_feedback)
	{
		int16_t ail_offset = (udb_flags._.radio_on) ? (udb_pwIn[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]) : 0;
		ail_rud_mix = MANUAL_AILERON_RUDDER_MIX * REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED, ail_offset);
		if (canStabilizeInverted() && current_orientation == F_INVERTED) ail_rud_mix = -ail_rud_mix;
	}
	else
	{
		ail_rud_mix = 0;
	}

	yaw_control = (int32_t)gyroYawFeedback._.W1 
	            + (int32_t)rollStabilization._.W1 
	            + (int32_t)yawStabilization._.W1 
	            + ail_rud_mix;
	// Servo reversing is handled in servoMix.c
}

void hoverYawCntrl(void)
{
	union longww yawAccum;
	union longww gyroYawFeedback;
	int16_t yawInput;
	int16_t manualYawOffset;

	if (state_flags._.pitch_feedback)
	{
		gyroYawFeedback.WW = __builtin_mulus(hoveryawkd, omegaAccum[2]);
		yawInput = (udb_flags._.radio_on == 1) ? REVERSE_IF_NEEDED(RUDDER_CHANNEL_REVERSED, udb_pwIn[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) : 0;
		manualYawOffset = yawInput * (int16_t)(RMAX/2000);
		yawAccum.WW = __builtin_mulsu(rmat[6] + HOVERYOFFSET + manualYawOffset, hoveryawkp);
	}
	else
	{
		gyroYawFeedback.WW = 0;
		yawAccum.WW = 0;
	}
	yaw_control = (int32_t)yawAccum._.W1 - (int32_t)gyroYawFeedback._.W1;
}
