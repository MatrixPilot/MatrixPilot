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
#include "airspeedCntrl.h"
#include "altitudeCntrl.h"
#include "helicalTurnCntrl.h"
#include "../libUDB/servoOut.h"
#include "../libDCM/rmat.h"

#define HOVERPOFFSET ((int32_t)(HOVER_PITCH_OFFSET*(RMAX/57.3)))
#define HOVERPTOWP ((int32_t)(HOVER_PITCH_TOWARDS_WP*(RMAX/57.3)))

#if (USE_CONFIGFILE == 1)
#include "config.h"
#include "redef.h"
#endif // USE_CONFIGFILE

uint16_t pitchgain;
uint16_t pitchfdfwd;
uint16_t pitchkd;
uint16_t hoverpitchgain;
uint16_t hoverpitchkd;

int16_t pitchrate;

int16_t elevInput;

int16_t elevatorLoadingTrim = 0 ;

static void normalPitchCntrl(void);
static void hoverPitchCntrl(void);

void init_pitchCntrl(void)
{
	pitchgain = (uint16_t)(PITCHGAIN*RMAX);
	pitchfdfwd = (uint16_t)(FEED_FORWARD*PITCHGAIN*RMAX);
	pitchkd = (uint16_t) (PITCHKD*SCALEGYRO*RMAX);
	hoverpitchgain = (uint16_t)(HOVER_PITCHGAIN*RMAX);
	hoverpitchkd = (uint16_t) (HOVER_PITCHKD*SCALEGYRO*RMAX);
}

void save_pitchCntrl(void)
{
#if (USE_CONFIGFILE == 1)
	gains.Pitchgain      = (float)pitchgain         / (RMAX);
	gains.PitchKD        = (float)pitchkd           / (SCALEGYRO*RMAX);
	gains.HoverPitchGain = (float)hoverpitchgain    / (RMAX);
	gains.HoverPitchKD   = (float)hoverpitchkd      / (SCALEGYRO*RMAX);
//	gains.RudderElevMix  = (float)rudderElevMixGain / (RMAX);
//	gains.RollElevMix    = (float)rollElevMixGain   / (RMAX);
#endif // USE_CONFIGFILE
}

void pitchCntrl(void)
{
	if (canStabilizeHover() && desired_behavior._.hover)
	{
		hoverPitchCntrl();
	}
	else
	{
		normalPitchCntrl();
	}
}

static void normalPitchCntrl(void)
{
	union longww pitchAccum;
//	int16_t aspd_adj;
//	fractional aspd_err, aspd_diff;

#ifdef TestGains
	state_flags._.GPS_steering = 0; // turn navigation off
	state_flags._.pitch_feedback = 1; // turn stabilization on
#endif

	if (PITCH_STABILIZATION && state_flags._.pitch_feedback)
	{
		pitchAccum.WW = __builtin_mulsu(tiltError[0], pitchgain) 
		              - __builtin_mulsu(desiredRotationRateRadians[0], pitchfdfwd)
		              + __builtin_mulsu(rotationRateError[0], pitchkd );
		pitch_control = (int32_t)pitchAccum._.W1 + (int32_t) elevatorLoadingTrim;
	}
	else
	{
		pitch_control = 0;
	}
}

static void hoverPitchCntrl(void)
{
	union longww pitchAccum;
	int16_t elevInput;
	int16_t manualPitchOffset;
	int32_t pitchToWP;

	if (state_flags._.pitch_feedback)
	{
		pitchAccum.WW = (__builtin_mulss(-rmat[7], omegagyro[0])
		               - __builtin_mulss( rmat[6], omegagyro[1])) << 1;
		pitchrate = pitchAccum._.W1;
		elevInput = (udb_flags._.radio_on == 1) ?
		    REVERSE_IF_NEEDED(ELEVATOR_CHANNEL_REVERSED, udb_pwIn[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) : 0;
		manualPitchOffset = elevInput * (int16_t)(RMAX/600);
		if (state_flags._.GPS_steering)
		{
			pitchToWP = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS) ?
			    HOVERPTOWP : (HOVERPTOWP / HOVER_NAV_MAX_PITCH_RADIUS * tofinish_line);
		}
		else
		{
			pitchToWP = 0;
		}
		pitchAccum.WW = __builtin_mulsu(rmat[8] + HOVERPOFFSET - pitchToWP + manualPitchOffset, hoverpitchgain)
		              + __builtin_mulus(hoverpitchkd, pitchrate);
	}
	else
	{
		pitchAccum.WW = 0;
	}
	pitch_control = (int32_t)pitchAccum._.W1;
}
