// This file is part of the MatrixPilotQuad firmware.
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
#include "../../libDCM/libDCM.h"

#define MAXIMUM_ERROR_INTEGRAL ((long int) 32768000 )

union longww roll_error_integral = {0};
union longww pitch_error_integral = {0};
union longww rrate_error_integral = {0};
union longww prate_error_integral = {0};
union longww yrate_error_integral = {0};
int rate_error_prev[3] = {0, 0, 0};
int rate_error_dot[3] = {0, 0, 0};

void magClamp(int *in, int mag);
void magClamp32(long *in, long mag);
void motorCtrl(void);
void SpeedLoop(int rrate_setp, int prate_setp, int yrate_setp);


void SpeedLoop(int rrate_setp, int prate_setp, int yrate_setp)
{
    union longww long_accum;
    fractional* pomegagyro = &omegagyro[0];
    int rate_error[3];
    extern int Tau_b_commanded[3]; // Moments in body frame

    extern unsigned int pid_gains[PID_GAINS_N];

    // Use commanded roll/pitch as desired rates, with gain ACRO_KP
    // Command is positive for forward pitch and right roll
    // gyro output is positive for forward pitch and right roll
//    long_accum.WW = __builtin_mulus(pid_gains[ACRO_KP_INDEX], (rrate_setp >> 2) - pomegagyro[1]);
//    rate_error[0] = long_accum._.W1;    // Proportional error

//    long_accum.WW = __builtin_mulus(pid_gains[ACRO_KP_INDEX], (prate_setp >> 2) - pomegagyro[0]);
//    rate_error[1] = long_accum._.W1;
    rate_error[0] = (rrate_setp >> 2) - pomegagyro[1];
    rate_error[1] = (prate_setp >> 2) - pomegagyro[0];
    rate_error[2] = yrate_setp - pomegagyro[2];


    // Compute the rate error integrals
    rrate_error_integral.WW += ((__builtin_mulus(pid_gains[RATE_KI_INDEX], rate_error[0])) >> 8);
    magClamp32(&rrate_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);   // Integral error

    prate_error_integral.WW += ((__builtin_mulus(pid_gains[RATE_KI_INDEX], rate_error[1])) >> 8);
    magClamp32(&prate_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);

    yrate_error_integral.WW += ((__builtin_mulus(pid_gains[YAW_KI_INDEX], rate_error[2])) >> 8);
    magClamp32(&yrate_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);

    // compute backward first difference of rate_error for D term in rate
    // control loop. For PID_HZ=400, feedback gain is RATE_KD
    rate_error_dot[0] = PID_HZ / 400 * (rate_error[0] - rate_error_prev[0]);    // Derivative error
    rate_error_dot[1] = PID_HZ / 400 * (rate_error[1] - rate_error_prev[1]);
    rate_error_dot[2] = PID_HZ / 400 * (rate_error[2] - rate_error_prev[2]);
    rate_error_prev[0] = rate_error[0];
    rate_error_prev[1] = rate_error[1];
    rate_error_prev[2] = rate_error[2];

    long_accum.WW = __builtin_mulus(pid_gains[RATE_KP_INDEX], rate_error[0]);
    Tau_b_commanded[0] = long_accum._.W1;             // P action
    Tau_b_commanded[0] += rrate_error_integral._.W1;  // I action
    long_accum.WW = __builtin_mulus(pid_gains[RATE_KD_INDEX], rate_error_dot[1]);
    Tau_b_commanded[0] += long_accum._.W1;            // D action

    long_accum.WW = __builtin_mulus(pid_gains[RATE_KP_INDEX], rate_error[1]);
    Tau_b_commanded[1] = long_accum._.W1;            // P action
    Tau_b_commanded[1] += prate_error_integral._.W1; // I action
    long_accum.WW = __builtin_mulus(pid_gains[RATE_KD_INDEX], rate_error_dot[0]);
    Tau_b_commanded[1] += long_accum._.W1;           // D action

    long_accum.WW = __builtin_mulus(pid_gains[YAW_KP_INDEX], rate_error[2]);
    Tau_b_commanded[2] = long_accum._.W1;             // P action
    Tau_b_commanded[2] += yrate_error_integral._.W1;  // I action
    long_accum.WW = __builtin_mulus(pid_gains[YAW_KD_INDEX], rate_error_dot[2]);
    Tau_b_commanded[2] += long_accum._.W1;            // D action
    Tau_b_commanded[2] *= YAW_SIGN;

    magClamp(&Tau_b_commanded[0], ROLLPITCH_CLAMP);
    magClamp(&Tau_b_commanded[1], ROLLPITCH_CLAMP);
    magClamp(&Tau_b_commanded[2], YAW_CLAMP);

    //TODO: add back lag compensation
    //TODO: change Tau_b_commanded only once.
    motorCtrl();
}

void AngleLoop(int roll_setp, int pitch_setp, int yaw_setp)
{
    int rate_setp[3];
    int angle_error[3];
    struct relative2D matrix_accum;
    unsigned int earth_yaw; // yaw with respect to earth frame
    static unsigned int desired_heading = 0;
    union longww long_accum;
    fractional* prmat = &rmat[0];
    extern unsigned int pid_gains[PID_GAINS_N];

    matrix_accum.x = prmat[4];
    matrix_accum.y = prmat[1];
    earth_yaw = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

    // Compute orientation errors: rmat[6,7] is sin(roll,pitch) in 2.14 format
    angle_error[0] = roll_setp + prmat[6];
    angle_error[1] = pitch_setp - prmat[7];

    if (abs(yaw_setp) > 0)
    {
        // 180 degrees is 2^15; 1 degree is 182 counts; commanded_yaw is +/-1000
        desired_heading = earth_yaw;
        angle_error[2] = 6 * yaw_setp;
    }
    else
    {
        // (otherwise, hold last commanded heading)
        angle_error[2] = (int) (earth_yaw - desired_heading);
    }

    // Compute the tilt error integrals
    roll_error_integral.WW += ((__builtin_mulus(pid_gains[TILT_KI_INDEX], angle_error[0])) >> 8);
    magClamp32(&roll_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);

    pitch_error_integral.WW += ((__builtin_mulus(pid_gains[TILT_KI_INDEX], angle_error[1])) >> 8);
    magClamp32(&pitch_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);

    // Use tilt PI output as desired rate
    long_accum.WW = __builtin_mulus(pid_gains[TILT_KP_INDEX], angle_error[0]);
    rate_setp[0] = angle_error[0] + roll_error_integral._.W1;

    long_accum.WW = __builtin_mulus(pid_gains[TILT_KP_INDEX], angle_error[1]);
    rate_setp[1] = angle_error[1] + pitch_error_integral._.W1;

    rate_setp[2] = angle_error[2];

    SpeedLoop(rate_setp[0],rate_setp[1],rate_setp[2]);
}
