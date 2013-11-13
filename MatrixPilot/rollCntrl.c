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


#if (USE_CONFIGFILE == 1)
#include "config.h"
#include "redef.h"
#endif // USE_CONFIGFILE

uint16_t yawkdail;
uint16_t rollkp;
uint16_t rollkd;
uint16_t hoverrollkp;
uint16_t hoverrollkd;

PARAM_SECTION(PID, 0, NULL);
PARAM_DEFINE_Q14(PID_YAWKDAIL,  &yawkdail,  0, 0.5, false);
PARAM_DEFINE_Q14(PID_ROLLKP,    &rollkp,    0, 0.5, false);
PARAM_DEFINE_Q14(PID_ROLLKD,    &rollkd,    0, 0.5, false);

PARAM_DEFINE_Q14(PID_HROLLKP,    &hoverrollkp, 0, 0.5, false);
PARAM_DEFINE_Q14(PID_HROLLKD,    &hoverrollkd, 0, 0.5, false);

void normalRollCntrl(void);
void hoverRollCntrl(void);

void init_rollCntrl(void)
{
	yawkdail    = (uint16_t)(YAWKD_AILERON*SCALEGYRO*RMAX);
	rollkp      = (uint16_t)(ROLLKP*RMAX);
	rollkd      = (uint16_t)(ROLLKD*SCALEGYRO*RMAX);
	hoverrollkp = (uint16_t)(HOVER_ROLLKP*SCALEGYRO*RMAX);
	hoverrollkd = (uint16_t)(HOVER_ROLLKD*SCALEGYRO*RMAX);
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
	flags._.GPS_steering = 0; // turn off navigation
#endif
	if (AILERON_NAVIGATION && flags._.GPS_steering)
	{
		rollAccum._.W1 = determine_navigation_deflection('a');
	}
#ifdef TestGains
	flags._.pitch_feedback = 1;
#endif
	if (ROLL_STABILIZATION_AILERONS && flags._.pitch_feedback)
	{
		gyroRollFeedback.WW = __builtin_mulus(rollkd , omegaAccum[1]);
		rollAccum.WW += __builtin_mulsu(rmat6 , rollkp);
	}
	else
	{
		gyroRollFeedback.WW = 0;
	}
	if (YAW_STABILIZATION_AILERON && flags._.pitch_feedback)
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

	if (flags._.pitch_feedback)
	{
		if (AILERON_NAVIGATION && flags._.GPS_steering)
		{
			rollNavDeflection = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS/2) ? determine_navigation_deflection('h') : 0;
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
