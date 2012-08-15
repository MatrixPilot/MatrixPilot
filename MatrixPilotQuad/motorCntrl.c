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
#include "../libDCM/libDCM.h"

#define MAXIMUM_ERROR_INTEGRAL ((long int) 32768000 )
#define YAW_DEADBAND 50 // prevent Tx pulse variation from causing yaw drift

extern int theta[3];
extern boolean didCalibrate;
extern int flight_mode;
int current_flight_mode = 0;
int motorsArmed = 0;

extern union longww IMUcmx, IMUvx;
extern union longww IMUcmy, IMUvy;
extern union longww IMUcmz, IMUvz;

// these are the current KP, KD and KDD loop gains in 2.14 fractional format
// valid range [0,3.99]
unsigned int pid_gains[PID_GAINS_N];

int roll_control;
int pitch_control;
int rolladvanced, pitchadvanced;
signed char lagBC, precessBC;
int yaw_control;
int pitch_step;
struct relative2D matrix_accum;
extern boolean udb_throttle_enable;
unsigned int earth_yaw; // yaw with respect to earth frame
unsigned int desired_heading = 0;
int accel_feedback;
int rate_error_prev[2] = {0, 0};
int rate_error_dot[2] = {0, 0};

int pwManual[5]; // channels 1-4 are control inputs from RX
int commanded_roll;
int commanded_pitch;
int commanded_yaw;

int roll_error, pitch_error, yaw_error;
int rate_error[3];

int yaw_error_previous = 0;

union longww roll_error_integral = {0};
union longww pitch_error_integral = {0};
union longww rrate_error_integral = {0};
union longww prate_error_integral = {0};
union longww yaw_error_integral = {0};

int poscmd_north, poscmd_east;
int pos_error[3], pos_setpoint[3];
union longww pos_prev[3], pos_delta[3];
int pos_perr[3], pos_derr[3];


//int target_orientation[9] = {RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX};

//const int yaw_command_gain = ((long) MAX_YAW_RATE)*(0.03);

void rotate2D(int *x, int *y, signed char angle)
{
    struct relative2D xy;
    xy.x = *x;
    xy.y = *y;
    rotate(&xy, angle);
    *x = xy.x;
    *y = xy.y;

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

void magClamp32(long *in, long mag)
{
    if (*in < -mag)
        *in = -mag;
    else if (*in > mag)
        *in = mag;
}

void motorCntrl(void)
{
    int temp;

    int motor_A;
    int motor_B;
    int motor_C;
    int motor_D;

    int commanded_roll_body_frame;
    int commanded_pitch_body_frame;

    //    int commanded_tilt[3];

    union longww long_accum;
    //	union longww accum ; // debugging temporary

    //    int posKP =0;
    int posKD = 0;

    // If radio is off, use udb_pwTrim values instead of the udb_pwIn values
    for (temp = 0; temp <= 4; temp++)
    {
        if (udb_flags._.radio_on)
        {
            if (temp == THROTTLE_INPUT_CHANNEL)
            {
                // limit throttle to leave some control headroom
                long_accum.WW = __builtin_mulus((unsigned int) (65536 * THROTTLE_LIMIT), (udb_pwIn[temp] - udb_pwTrim[temp]));
                pwManual[temp] = long_accum._.W1 + udb_pwTrim[temp];
            }
            else
                pwManual[temp] = udb_pwIn[temp];
        }
        else
            pwManual[temp] = udb_pwTrim[temp];

    }

    if (!didCalibrate)
    {
        // pass throttle channel through to all ESCs to allow ESC calibration
        udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_pwIn[THROTTLE_INPUT_CHANNEL];
        udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_pwIn[THROTTLE_INPUT_CHANNEL];
        udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_pwIn[THROTTLE_INPUT_CHANNEL];
        udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_pwIn[THROTTLE_INPUT_CHANNEL];
    }
    else if (motorsArmed < 2)
    {
        // not armed yet; set ESCs to idle
        udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
        udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
        udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
        udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
        switch (motorsArmed)
        {
        case 0:
            // wait for high throttle
            if ((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) > (SERVORANGE / 2))
                motorsArmed = 1;
            break;
        case 1:
            // wait for low throttle
            if ((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) < THROTTLE_DEADBAND)
            {
                motorsArmed = 2;
                LED_RED = LED_ON;
            }
            break;
        }
    }
    else if ((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) < THROTTLE_DEADBAND)
    {

        motor_A = pwManual[THROTTLE_INPUT_CHANNEL];
        motor_B = pwManual[THROTTLE_INPUT_CHANNEL];
        motor_C = pwManual[THROTTLE_INPUT_CHANNEL];
        motor_D = pwManual[THROTTLE_INPUT_CHANNEL];

//        VectorCopy(9, target_orientation, rmat);

        commanded_roll = (pwManual[ROLL_INPUT_CHANNEL]
                - udb_pwTrim[ROLL_INPUT_CHANNEL]);
        commanded_pitch = (pwManual[PITCH_INPUT_CHANNEL]
                - udb_pwTrim[PITCH_INPUT_CHANNEL]);
        commanded_yaw = YAW_SIGN * (pwManual[YAW_INPUT_CHANNEL]
                - udb_pwTrim[YAW_INPUT_CHANNEL]);

        // get heading in earth frame from rmat
        matrix_accum.x = rmat[4];
        matrix_accum.y = rmat[1];
        earth_yaw = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)
        desired_heading = earth_yaw;

#ifdef CONFIG_PLUS
        commanded_pitch_body_frame = commanded_pitch;
        commanded_roll_body_frame = commanded_roll;
#endif

#ifdef CONFIG_X
        commanded_pitch_body_frame = 3 * ((commanded_pitch - commanded_roll) / 4); // approximation to .707, not critcal
        commanded_roll_body_frame = 3 * ((commanded_pitch + commanded_roll) / 4);
#endif

        motor_A += +commanded_yaw - commanded_pitch_body_frame;
        motor_B += -commanded_yaw - commanded_roll_body_frame;
        motor_C += +commanded_yaw + commanded_pitch_body_frame;
        motor_D += -commanded_yaw + commanded_roll_body_frame;

        udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_A);
        udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_B);
        udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_C);
        udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_D);

    }
    else
    {
        // get heading in earth frame from rmat
        matrix_accum.x = rmat[4];
        matrix_accum.y = rmat[1];
        earth_yaw = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

        // check flight mode
        if (flight_mode != current_flight_mode)
        {
            // on change of flight mode, record current IMU position
            current_flight_mode = flight_mode;
            pos_setpoint[0] = IMUcmx._.W1;
            pos_setpoint[1] = IMUcmy._.W1;
        }
        // if flight mode is POS_MODE, command roll and pitch to reduce position error
        // Positive commanded_roll rolls right and positive commanded_pitch pitches forward.

        // Now the GPS (GPSloc_cm and IMUcm) x axis points East and the y axis points North.
        // Rotate position error (dx,dy) = (desired_position - IMUcm) from the GPS frame to the body frame:
        // If vehicle heading is North, rotation is zero, if East (-90 degrees) rotation is +90 degrees.
        // For an arbitrary vehicle heading theta, rotation is then (-theta).
        // position hold flight mode: expected error mag [0, 512] cm
        pos_error[0] = pos_setpoint[0] - IMUcmx._.W1;
        pos_error[1] = pos_setpoint[1] - IMUcmy._.W1;

        // calculate 32 bit position delta
        pos_delta[0].WW = IMUcmx.WW - pos_prev[0].WW;
        pos_delta[1].WW = IMUcmy.WW - pos_prev[1].WW;
        pos_prev[0].WW = IMUcmx.WW;
        pos_prev[1].WW = IMUcmy.WW;

        // use PWM input to set KP (range is about 2000-4000)
        //            posKP = (udb_pwIn[GAIN_CHANNEL] - 2000) >> 5;
        //            posKP = posKP < 0 ? 0 : posKP; // result range [0, 62]

        // use PWM input to set KD (range is about 2000-4000)
        posKD = (udb_pwIn[GAIN_CHANNEL] - 2000) >> 5;
        posKD = posKD < 0 ? 0 : posKD; // result range [0, 62]
        //            posKD = 0;

        // limit velocity to 50cm/sec at 5m error 0 = KP*500 - KD*50; KD=10*KP
        pos_perr[0] = POS_HOLD_KP * pos_error[0];
        pos_derr[0] = (posKD * PID_HZ * pos_delta[0].WW) >> 16;
        poscmd_east = pos_perr[0] - pos_derr[0];

        pos_perr[1] = POS_HOLD_KP * pos_error[1];
        pos_derr[1] = (posKD * PID_HZ * pos_delta[1].WW) >> 16;

        poscmd_north = pos_perr[1] - pos_derr[1];

        // clamp position hold control inputs
        magClamp(&poscmd_east, 4000);
        magClamp(&poscmd_north, 4000);

        if (flight_mode == POS_MODE)
        {
            // add in manual control inputs
            commanded_roll = poscmd_east + (pwManual[ROLL_INPUT_CHANNEL]
                    - udb_pwTrim[ROLL_INPUT_CHANNEL]) * CMD_TILT_GAIN;
            commanded_pitch = poscmd_north + (pwManual[PITCH_INPUT_CHANNEL]
                    - udb_pwTrim[PITCH_INPUT_CHANNEL]) * CMD_TILT_GAIN;

            // rotate forward stick North (angle -heading)
            rotate2D(&commanded_roll, &commanded_pitch, (-earth_yaw) >> 8);
        }
        else if (flight_mode == COMPASS_MODE)
        {
            // manual mode: forward cyclic is North
            commanded_roll = (pwManual[ROLL_INPUT_CHANNEL]
                    - udb_pwTrim[ROLL_INPUT_CHANNEL]) * CMD_TILT_GAIN;
            commanded_pitch = (pwManual[PITCH_INPUT_CHANNEL]
                    - udb_pwTrim[PITCH_INPUT_CHANNEL]) * CMD_TILT_GAIN;
            // rotate forward stick North (angle -heading)
            rotate2D(&commanded_roll, &commanded_pitch, (-earth_yaw) >> 8);
        }
        else // TILT_MODE or RATE_MODE
        {
            // manual (tilt) flight mode
            commanded_roll = (pwManual[ROLL_INPUT_CHANNEL]
                    - udb_pwTrim[ROLL_INPUT_CHANNEL]) * CMD_TILT_GAIN;
            commanded_pitch = (pwManual[PITCH_INPUT_CHANNEL]
                    - udb_pwTrim[PITCH_INPUT_CHANNEL]) * CMD_TILT_GAIN;
        }

        commanded_yaw = (pwManual[YAW_INPUT_CHANNEL]
                - udb_pwTrim[YAW_INPUT_CHANNEL]);

        // apply deadband to yaw command
        deadBand(&commanded_yaw, YAW_DEADBAND);

        // adjust roll and pitch commands to prevent combined tilt from exceeding 90 degrees
        //        commanded_tilt[0] = commanded_roll;
        //        commanded_tilt[1] = commanded_pitch;
        //        commanded_tilt[2] = RMAX;
        //        vector3_normalize(commanded_tilt, commanded_tilt);
        //        commanded_roll = commanded_tilt[0];
        //        commanded_pitch = commanded_tilt[1];

#ifdef CONFIG_PLUS

        commanded_pitch_body_frame = commanded_pitch;
        commanded_roll_body_frame = commanded_roll;

#endif

#ifdef CONFIG_X

        commanded_pitch_body_frame = 3 * ((commanded_pitch - commanded_roll) / 4); // approximation to .707, not critcal
        commanded_roll_body_frame = 3 * ((commanded_pitch + commanded_roll) / 4);

#endif

        // if commanded_yaw was recently nonzero, reset desired_heading to current heading
        //WTF: !!! if (commanded_yaw != 0) didn't behave as expected !!!
        if (abs(commanded_yaw) > 0)
        {
            // positive commanded_yaw causes positive yaw_control and decrease in earth_yaw
            // If earth_yaw increases, positive command is required to correct.
            // since desired_heading and earth_yaw are "word circular"
            // their difference is the 2's complement signed integer heading error
            // 180 degrees is 2^15; 1 degree is 182 counts; commanded_yaw is +/-1000
            // yaw rate is proportional to either heading error or yaw command.
            // Full stick is equivalent to a heading error of about 8 degrees
            desired_heading = earth_yaw;
            yaw_error = 32 * commanded_yaw;
        }
        else
        {
            // (otherwise, hold last commanded heading)
            yaw_error = (int) (earth_yaw - desired_heading);
        }
        // light taillight whenever heading is within 5 degrees of North
        if (flight_mode == COMPASS_MODE)
        {
            if (abs((int) earth_yaw) < 910)
                TAIL_LIGHT = LED_ON;
            else
                TAIL_LIGHT = LED_OFF;
        }

        // Compute the signals that are common to all 4 motors
        long_accum.WW = __builtin_mulus((unsigned int) pid_gains[ACCEL_K_INDEX], accelEarth[2]);
        accel_feedback = long_accum._.W1;
        motor_A = motor_B = motor_C = motor_D = pwManual[THROTTLE_INPUT_CHANNEL] - accel_feedback;


        // If in rate mode use roll/pitch command as desired tilt rate
        if (flight_mode == RATE_MODE)
        {
            // Use commanded roll/pitch as desired rates, with gain ACRO_KP
            // Command is positive for forward pitch and right roll
            // gyro output is positive for forward pitch and right roll
            long_accum.WW = __builtin_mulus(pid_gains[ACRO_KP_INDEX], (commanded_roll_body_frame >> 2) - omegagyro[1]);
            rate_error[0] = long_accum._.W1;

            long_accum.WW = __builtin_mulus(pid_gains[ACRO_KP_INDEX], (commanded_pitch_body_frame >> 2) - omegagyro[0]);
            rate_error[1] = long_accum._.W1;
        }
        else // in all other flight modes, control tilt
        {
            // Compute orientation errors: rmat[6,7] is sin(roll,pitch) in 2.14 format
            roll_error = commanded_roll_body_frame + rmat[6];
            pitch_error = commanded_pitch_body_frame - rmat[7];

            // Compute the tilt error integrals
            roll_error_integral.WW += ((__builtin_mulus(pid_gains[TILT_KI_INDEX], roll_error)) >> 8);
            magClamp32(&roll_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);

            pitch_error_integral.WW += ((__builtin_mulus(pid_gains[TILT_KI_INDEX], pitch_error)) >> 8);
            magClamp32(&pitch_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);

            // Use tilt error as desired rate, with gain TILT_KP
            long_accum.WW = __builtin_mulus(pid_gains[TILT_KP_INDEX], roll_error);
            rate_error[0] = long_accum._.W1 - (omegagyro[1] >> 2);
            rate_error[0] += roll_error_integral._.W1;

            long_accum.WW = __builtin_mulus(pid_gains[TILT_KP_INDEX], pitch_error);
            rate_error[1] = long_accum._.W1 - (omegagyro[0] >> 2);
            rate_error[1] += pitch_error_integral._.W1;
        }

        // Compute the rate error integrals
        rrate_error_integral.WW += ((__builtin_mulus(pid_gains[RATE_KI_INDEX], rate_error[0])) >> 8);
        magClamp32(&rrate_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);
        rate_error[0] += rrate_error_integral._.W1;

        prate_error_integral.WW += ((__builtin_mulus(pid_gains[RATE_KI_INDEX], rate_error[1])) >> 8);
        magClamp32(&prate_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);
        rate_error[1] += prate_error_integral._.W1;

        // compute backward first difference of rate_error for D term in rate
        // control loop. For PID_HZ=400, feedback gain is RATE_KD
        rate_error_dot[0] = PID_HZ / 400 * (rate_error[0] - rate_error_prev[0]);
        rate_error_dot[1] = PID_HZ / 400 * (rate_error[1] - rate_error_prev[1]);
        rate_error_prev[0] = rate_error[0];
        rate_error_prev[1] = rate_error[1];

        long_accum.WW = __builtin_mulus(pid_gains[RATE_KP_INDEX], rate_error[0]);
        roll_control = long_accum._.W1;
        long_accum.WW = __builtin_mulus(pid_gains[RATE_KD_INDEX], rate_error_dot[1]);
        roll_control += long_accum._.W1;

        long_accum.WW = __builtin_mulus(pid_gains[RATE_KP_INDEX], rate_error[1]);
        pitch_control = long_accum._.W1;
        long_accum.WW = __builtin_mulus(pid_gains[RATE_KD_INDEX], rate_error_dot[0]);
        pitch_control += long_accum._.W1;

        yaw_error_integral.WW += ((__builtin_mulus(pid_gains[YAW_KI_INDEX], yaw_error)) >> 8);
        magClamp32(&yaw_error_integral.WW, MAXIMUM_ERROR_INTEGRAL);

        // use heading error * KP as desired yaw rate
        long_accum.WW = __builtin_mulus(pid_gains[YAW_KP_INDEX], yaw_error);
        rate_error[2] = long_accum._.W1 - omegagyro[2];
        rate_error[2] += yaw_error_integral._.W1;

        long_accum.WW = __builtin_mulus(pid_gains[YAW_KD_INDEX], rate_error[2]);
        yaw_control = YAW_SIGN * long_accum._.W1;

        //        if (flight_mode == COMPASS_MODE)
        //        {
        //            // inject a yaw command for testing phase lead code
        //            yaw_control += 150;
        //        }

        // limit yaw control input to prevent loss of tilt control
        magClamp(&yaw_control, YAW_CLAMP);

        // compensate for gyroscopic reaction torque proportional to omegagyro[2]
        // Suppose the relative magnitude of gyro. reaction is omega_z in rad/sec:
        // if this is the ratio of gyroscopic torque to applied torque, then the
        // resultant angle shift is atan(omega_z rad/sec)
        //TODO: loop gain will increase directly with magnitude of omega: compensate
        union longww omega_z; // result in rad/sec, decimal point between words
        omega_z.WW = __builtin_mulus((unsigned int) (65536 * (PI / 180) / DEGPERSEC), omegagyro[2]);

        // _Q16atan is a 2-quadrant 32 bit fixed point arctangent
        // domain is [-2^15, 2^15-1]
        // result is Q16 radians, range (-pi/2, pi/2)
        long rotLong;
        rotLong = _Q16atan(omega_z.WW);

        // convert to 8 bit byte circular assuming a range of +/-pi/2 radians
        // rotWord is radians * 2^14
        int rotWord = 0xFFFF & (rotLong >> 2);
        union longww rotAngle;
        // lagAngle high word is radians * 2^14/PI
        // *** note that rotWord is negated here ***
        rotAngle.WW = __builtin_mulus((unsigned int) (65536 / PI), -rotWord);
        // lagBC is in byte circular form; should wrap correctly at 360 degrees
        precessBC = 0xFF & (rotAngle._.W1 >> 6);

        // compensate for rate control latency when spinning;
        // rate latency is 80-1000msec for microQuad
        // result 1.15 fractional format with lsb weight DEGPERSEC count/degree
        union longww lagAngle; // low byte of high word is byte circular angle
        lagAngle.WW = __builtin_mulus((unsigned int) (65536 * (128.0 / 180) * 0.08 / DEGPERSEC), omegagyro[2]);
        // lagBC is in byte circular form; should wrap correctly at 360 degrees
        lagBC = 0xFF & lagAngle._.W1;

        // advance phase of roll_control and pitch_control to compensate for tilt lag and precession
        rolladvanced = roll_control;
        pitchadvanced = pitch_control;
        //        rotate2D(&rolladvanced, &pitchadvanced, lagBC + precessBC);
        rotate2D(&rolladvanced, &pitchadvanced, lagBC);

        magClamp(&rolladvanced, ROLLPITCH_CLAMP);
        magClamp(&pitchadvanced, ROLLPITCH_CLAMP);

        //		Mix in the yaw, pitch, and roll signals into the motors
        motor_A += +yaw_control - pitchadvanced;
        motor_B += -yaw_control - rolladvanced;
        motor_C += +yaw_control + pitchadvanced;
        motor_D += -yaw_control + rolladvanced;

        //		Send the signals out to the motors
        udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_A);
        udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_B);
        udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_C);
        udb_pwOut[MOTOR_D_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor_D);

    }
}


#ifndef CONFIG_PLUS
#ifndef CONFIG_X
#error ("You have not selected a configuration in options.h, select either CONFIG_PLUS or CONFIG_X.")
#endif
#endif

#ifdef CONFIG_PLUS
#ifdef CONFIG_X
#error ("You have selected both CONFIG_PLUS and CONFIG_X in options.h. Select just one of them."
#endif
#endif


//#if  (( ( int ) + MAX_YAW_RATE   < 50 ) || ( ( int ) + MAX_YAW_RATE > 500 ))
//#error ("MAX_YAW_RATE must be between 50.0 and 500.0 degrees/second.")
//#endif

#if (((int) + MAX_TILT) > 45)
#error ("MAX_TILT mus be less than or equal to 45 degrees."
#endif
