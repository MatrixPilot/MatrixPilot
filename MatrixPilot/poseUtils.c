/*
 * File:   poseUtils.c
 * Author: markw
 *
 * Created on February 10, 2013, 1:12 PM
 */

// This file is part of the MatrixPilot firmware.
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

#include <libq.h>
#include "libDCM.h"
#include "poseUtils.h"

#define MAXIMUM_ERROR_INTEGRAL ((long int) 32768000 )
#define YAW_DEADBAND 50 // prevent Tx pulse variation from causing yaw drift

extern int flight_mode;

struct relative2D matrix_accum;

int pwManual[5]; // channels 1-4 are control inputs from RX

struct int_RPY cmd_RPY, prev_RPY, err_RPY;

// init constant portions of 1D rotation matrices
fractional Rx[9] = {RMAX, 0, 0, 0, 0, 0, 0, 0, 0};
fractional Ry[9] = {0, 0, 0, 0, RMAX, 0, 0, 0, 0};
fractional Rz[9] = {0, 0, 0, 0, 0, 0, 0, 0, RMAX};

// init Rc and Rd to identity matrix
fractional Rc[9] = {RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX};
fractional Rd[9] = {RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX};
fractional Rtmp[9], Rtrans[9];

void rotateRP(struct int_RPY* command, signed char angle)
{
	struct relative2D xy;
	xy.x = command->roll;
	xy.y = command->pitch;
	rotate(&xy, angle);
	command->roll = xy.x;
	command->pitch = xy.y;
}

void rotate2D(int *x, int *y, signed char angle)
{
	struct relative2D xy;
	xy.x = *x;
	xy.y = *y;
	rotate(&xy, angle);
	*x = xy.x;
	*y = xy.y;
}

void deadBandRP(struct int_RPY* command, int bandRP)
{
	if (command->roll >= bandRP)
	{
		command->roll -= bandRP;
	}
	else if (command->roll <= -bandRP)
	{
		command->roll += bandRP;
	}
	else
	{
		command->roll = 0;
	}

	if (command->pitch >= bandRP)
	{
		command->pitch -= bandRP;
	}
	else if (command->pitch <= -bandRP)
	{
		command->pitch += bandRP;
	}
	else
	{
		command->pitch = 0;
	}
}

void deadBand(int *input, int band)
{
	if (*input >= band)
		*input -= band;
	else if (*input <= -band)
		*input += band;
	else
		*input = 0;
}

void magClamp(int *in, int mag)
{
	if (*in < -mag)
		*in = -mag;
	else if (*in > mag)
		*in = mag;
}

void magClampRP(struct int_RPY* command, int mag)
{
	if (command->roll < -mag)
	{
		command->roll = -mag;
	}
	else if (command->roll > mag)
	{
		command->roll = mag;
	}

	if (command->pitch < -mag)
	{
		command->pitch = -mag;
	}
	else if (command->pitch > mag)
	{
		command->pitch = mag;
	}
}

void magClampRPY(struct int_RPY* command, int mag)
{
	if (command->roll < -mag)
	{
		command->roll = -mag;
	}
	else if (command->roll > mag)
	{
		command->roll = mag;
	}

	if (command->pitch < -mag)
	{
		command->pitch = -mag;
	}
	else if (command->pitch > mag)
	{
		command->pitch = mag;
	}

	if (command->yaw < -mag)
	{
		command->yaw = -mag;
	}
	else if (command->yaw > mag)
	{
		command->yaw = mag;
	}
}

void magClamp32(long *in, long mag)
{
	if (*in < -mag)
		*in = -mag;
	else if (*in > mag)
		*in = mag;
}

void transposeR(fractional *t, fractional *r)
{
	t[0] = r[0];
	t[1] = r[3];
	t[2] = r[6];

	t[3] = r[1];
	t[4] = r[4];
	t[5] = r[7];

	t[6] = r[2];
	t[7] = r[5];
	t[8] = r[8];
}

// in X configuration, rotate roll/pitch commands the amount specified by X_SIN,COS

void x_rotate(struct int_RPY* command)
{
#if (CONFIG_X != 0)
	union longww long_accum;

	// for quad X config: theta = -pi/4, sin(theta) = -.707
	// x' = x cos(theta) - y sin(theta)
	long_accum.WW = __builtin_mulss(command->roll, X_COS);
	long_accum.WW -= __builtin_mulss(command->pitch, X_SIN);
	int roll_body_frame = 2 * long_accum._.W1;

	// y' = x sin(theta) + y cos(theta)
	long_accum.WW = __builtin_mulss(command->roll, X_SIN);
	long_accum.WW += __builtin_mulss(command->pitch, X_COS);
	command->pitch = 2 * long_accum._.W1;
	command->roll = roll_body_frame;
#else
	return;
#endif
}

// 1092 * 50Hz * pi/32K * 180/pi = 300 deg/sec
#define MAX_SLEW_RATE 1092

// yaw is always a rate command

void updateYaw(struct int_RPY* command)
{
	int yawRate = (pwManual[RUDDER_INPUT_CHANNEL]
					- udb_pwTrim[RUDDER_INPUT_CHANNEL]);
	// apply deadband and slew rate limiter
	deadBand(&yawRate, 10);
	magClamp(&yawRate, MAX_SLEW_RATE);
	command->yaw += yawRate;
}

// perform rpy command updates at PID_HZ/8
static int rateCounter = 0;

void get_rateMode_commands(struct int_RPY* command)
{
	if (rateCounter++ > 7)
	{
		rateCounter = 0;
		// manual rate flight mode
		// range +/- 1250 corresponds to max of 50Hz * 1250 * pi/32K * (180/pi) = 343 deg/sec
		struct int_RPY cmdRate;
		cmdRate.roll = (pwManual[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]);
		cmdRate.pitch = (pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]);

		// apply deadband and slew rate limit to roll and pitch
		deadBandRP(&cmdRate, 4);
		magClampRP(&cmdRate, MAX_SLEW_RATE);

		command->roll += cmdRate.roll;
		command->pitch += cmdRate.pitch;

		// rotate commanded roll/pitch into X body frame, if required
		x_rotate(command);

		// yaw is always rate mode
		updateYaw(command);
	}
}

void get_angleMode_commands(struct int_RPY* command, int tiltGain)
{
	if (rateCounter++ > 7)
	{
		rateCounter = 0;
		// manual angle flight mode: +/-XX degrees of roll/pitch
		command->roll = (pwManual[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]) * tiltGain;
		command->pitch = (pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]) * tiltGain;

		// apply slew rate limit to roll and pitch
		struct int_RPY delta;
		delta.roll = command->roll - prev_RPY.roll;
		delta.pitch = command->pitch - prev_RPY.pitch;
		magClampRP(&delta, MAX_SLEW_RATE);

		command->roll = prev_RPY.roll + delta.roll;
		command->pitch = prev_RPY.pitch + delta.pitch;

		prev_RPY.roll = command->roll;
		prev_RPY.pitch = command->pitch;

		// rotate commanded roll/pitch into X body frame, if required
		x_rotate(command);

		// yaw is always rate mode
		updateYaw(command);
	}
}

void set_Rd(struct int_RPY* command)
{
	// use commanded roll, pitch and yaw to generate desired orientation matrix Rd
	// constrain pitch to range [-90, 90] -> [-16384, 16384] -> [-64, 64] byte circular
	// R_d = R_z R_y R_x

	// R_x =
	// 1        0           0
	// 0    cos(pitch)  -sin(pitch)
	// 0    sin(pitch)   cos(pitch)

	// commanded_pitch is angle / pi as Q15 fractional
	fractional q15_angle = command->pitch;
	// clamp to +/-90 degrees
	magClamp(&q15_angle, 16384);
	fractional q14sin = _Q15sinPI(q15_angle) / 2;
	fractional q14cos = _Q15cosPI(q15_angle) / 2;
	Rx[4] = q14cos;
	Rx[5] = -q14sin;
	Rx[7] = q14sin;
	Rx[8] = q14cos;

	// R_y =
	// cos(roll)   0    sin(roll)
	//     0       1       0
	//-sin(roll)   0    cos(roll)

	// commanded_roll is angle / pi as Q15 fractional
	q15_angle = command->roll;
	q14sin = _Q15sinPI(q15_angle) / 2;
	q14cos = _Q15cosPI(q15_angle) / 2;
	Ry[0] = q14cos;
	Ry[2] = q14sin;
	Ry[6] = -q14sin;
	Ry[8] = q14cos;

	// R_z =
	// cos(yaw)   -sin(yaw)    0
	// sin(yaw)    cos(yaw)    0
	//    0           0        1

	// angle / pi as Q15 fractional
	q15_angle = command->yaw;
	q14sin = _Q15sinPI(q15_angle) / 2;
	q14cos = _Q15cosPI(q15_angle) / 2;
	Rz[0] = q14cos;
	Rz[1] = -q14sin;
	Rz[3] = q14sin;
	Rz[4] = q14cos;

	// construct Rd = Rz Ry Rx
	// Rd = Rz * Ry
	MatrixMultiply(3, 3, 3, Rtmp, Rz, Ry);
	//	multiply by 2 and copy back from Rtmp to Rd:
	MatrixAdd(3, 3, Rd, Rtmp, Rtmp);
	// Rd = Rd * Rx
	MatrixMultiply(3, 3, 3, Rtmp, Rd, Rx);
	//	multiply by 2 and copy back
	MatrixAdd(3, 3, Rd, Rtmp, Rtmp);
}

// manual (rate) flight mode
//	get_rateMode_commands(&cmd_RPY);

//	 manual (tilt) flight mode
//	get_angleMode_commands(&cmd_RPY, CMD_TILT_GAIN);

//	getRPYerror(&perr_RPY, &cmd_RPY);

void getRPYerror(struct int_RPY *perr_RPY, struct int_RPY *pcmd_RPY)
{
	fractional* prmat = &rmat[0];
	//	fractional* pomega = &omegagyro[0];
	//	fractional* pomega = &omega[0];

	set_Rd(pcmd_RPY);

	// construct Rc = Rimu^T * Rd
	transposeR(Rtrans, prmat);
	MatrixMultiply(3, 3, 3, Rtmp, Rtrans, Rd);

	// multiply by 2 and copy back
	MatrixAdd(3, 3, Rc, Rtmp, Rtmp);

	// error Rc = Rimu^T * Rd and the error angles are Rc as xyz fixed angles

	// rotation about x axis is pitch
	// gamma = atan2(r21, r22)
	matrix_accum.y = Rc[7];
	matrix_accum.x = Rc[8];
	perr_RPY->pitch = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

	// rotation about y is roll
	// beta = atan2(-r20, sqrt(r00^2 + r10^2))
	matrix_accum.y = -Rc[6];
	matrix_accum.x = sqrt(pow(Rc[0], 2) + pow(Rc[3], 2));
	perr_RPY->roll = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

	// rotation about z is yaw
	// alpha = atan2(r10, r00)
	matrix_accum.y = Rc[3];
	matrix_accum.x = Rc[0];
	perr_RPY->yaw = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

}
