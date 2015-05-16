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
#include "config.h"
#include "states.h"
#include "helicalTurnCntrl.h"
#include "../libDCM/rmat.h"

uint16_t yawkdail;
uint16_t rollkp;
uint16_t rollkd;
uint16_t hoverrollkp;
uint16_t hoverrollkd;

void normalRollCntrl(void);
void hoverRollCntrl(void);

void init_rollCntrl(void)
{
	yawkdail    = (uint16_t)(gains.YawKDAileron*SCALEGYRO*RMAX);
	rollkp      = (uint16_t)(gains.RollKP*RMAX);
	rollkd      = (uint16_t)(gains.RollKD*SCALEGYRO*RMAX);
	hoverrollkp = (uint16_t)(hover.HoverRollKP*SCALEGYRO*RMAX);
	hoverrollkd = (uint16_t)(hover.HoverRollKD*SCALEGYRO*RMAX);
}

void save_rollCntrl(void)
{
	gains.YawKDAileron = (float)yawkdail    / (SCALEGYRO*RMAX);
	gains.RollKP       = (float)rollkp      / (RMAX);
	gains.RollKD       = (float)rollkd      / (SCALEGYRO*RMAX);
	hover.HoverRollKP  = (float)hoverrollkp / (SCALEGYRO*RMAX);
	hover.HoverRollKD  = (float)hoverrollkd / (SCALEGYRO*RMAX);
}

void rollCntrl(void)
{
	if (canStabilizeHover() && current_orientation == F_HOVER)
	{
		hoverRollCntrl();
	}
	else
	{
		normalRollCntrl();
	}
}

void normalRollCntrl(void)
{
	union longww rollAccum = { 0 };
	union longww gyroRollFeedback;
	union longww gyroYawFeedback;
	fractional rmat6;
	fractional omegaAccum2;

	if (!canStabilizeInverted() || !desired_behavior._.inverted)
	{
		rmat6 = rmat[6];
		omegaAccum2 = omegaAccum[2];
	}
	else
	{
		rmat6 = -rmat[6];
		omegaAccum2 = -omegaAccum[2];
	}
#ifdef TestGains
	state_flags._.GPS_steering = 0; // turn off navigation
#endif
	if (AILERON_NAVIGATION && state_flags._.GPS_steering)
	{
		rollAccum._.W1 = navigate_determine_deflection('a');
	}
#ifdef TestGains
	state_flags._.pitch_feedback = 1;
#endif
	if (settings._.RollStabilizaionAilerons && state_flags._.pitch_feedback)
	{
		gyroRollFeedback.WW = __builtin_mulus(rollkd , omegaAccum[1]);
		rollAccum.WW += __builtin_mulsu(rmat6 , rollkp);
	}
	else
	{
		gyroRollFeedback.WW = 0;
	}
	if (settings._.YawStabilizationAileron && state_flags._.pitch_feedback)
	{
		gyroYawFeedback.WW = __builtin_mulus(yawkdail, omegaAccum2);
	}
	else
	{
		gyroYawFeedback.WW = 0;
	}
	roll_control = (int32_t)rollAccum._.W1 - (int32_t)gyroRollFeedback._.W1 - (int32_t)gyroYawFeedback._.W1;
	// Servo reversing is handled in servoMix.c
}

void hoverRollCntrl(void)
{
	int16_t rollNavDeflection;
	union longww gyroRollFeedback;

	if (state_flags._.pitch_feedback)
	{
		if (settings._.AileronNavigation && state_flags._.GPS_steering)
		{
			rollNavDeflection = (tofinish_line > hover.HoverNavMaxPitchRadius/2) ? navigate_determine_deflection('h') : 0;
		}
		else
		{
			rollNavDeflection = 0;
		}
		gyroRollFeedback.WW = __builtin_mulus(hoverrollkd , omegaAccum[1]);
	}
	else
	{
		rollNavDeflection = 0;
		gyroRollFeedback.WW = 0;
	}
	roll_control = rollNavDeflection -(int32_t)gyroRollFeedback._.W1;
}
