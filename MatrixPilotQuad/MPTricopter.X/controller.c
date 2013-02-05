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

int pwManual[5]; // channels 1-4 are control inputs from RX
unsigned int pid_gains[PID_GAINS_N];
unsigned int throttle_limit = (unsigned int)(65536 * THROTTLE_LIMIT);
extern int Fzb_commanded;      // Force along axis z in body frame

void read_inputs(void);
unsigned int check_state(void);
void IdleMotorCtrl(unsigned int mode);
void SpeedLoop(int rrate_setp, int prate_setp, int yrate_setp);
void AngleLoop(int roll_setp, int pitch_setp, int yaw_setp);
void PositionLoop(void);

void rotate2D(int *x, int *y, signed char angle);
void deadBand(int *input, int band);
void magClamp(int *in, int mag);
void magClamp32(long *in, long mag);

#define YAW_DEADBAND 50 // prevent Tx pulse variation from causing yaw drift

void controller(void)
{
    int rate_setp[3];
    int angle_setp[3];
    extern int flight_mode;
    
    read_inputs();  // Fills pwManual
//    check_state();
    switch (check_state())
    {
        case 1:
            IdleMotorCtrl(1);
            break;
        case 2:
            IdleMotorCtrl(2);
            break;
        case 3:
            IdleMotorCtrl(3);
            break;
        case 0:
            switch (flight_mode)
            {
                case POS_MODE:
                    PositionLoop();
                    break;
                case COMPASS_MODE:
                    break;
                case TILT_MODE:
                    angle_setp[0] = (pwManual[ROLL_INPUT_CHANNEL] - udb_pwTrim[ROLL_INPUT_CHANNEL]) * CMD_TILT_GAIN;
                    angle_setp[1] = (pwManual[PITCH_INPUT_CHANNEL] - udb_pwTrim[PITCH_INPUT_CHANNEL]) * CMD_TILT_GAIN;
                    angle_setp[2] = (pwManual[YAW_INPUT_CHANNEL] - udb_pwTrim[YAW_INPUT_CHANNEL]);
                    deadBand(&angle_setp[2], YAW_DEADBAND);
                    Fzb_commanded = pwManual[THROTTLE_INPUT_CHANNEL];

                    AngleLoop(angle_setp[0],angle_setp[1],angle_setp[2]);
                    break;
                case RATE_MODE:
                    rate_setp[0] = (pwManual[ROLL_INPUT_CHANNEL] - udb_pwTrim[ROLL_INPUT_CHANNEL]) * CMD_TILT_GAIN;
                    rate_setp[1] = (pwManual[PITCH_INPUT_CHANNEL] - udb_pwTrim[PITCH_INPUT_CHANNEL]) * CMD_TILT_GAIN;
                    rate_setp[2] = (pwManual[YAW_INPUT_CHANNEL] - udb_pwTrim[YAW_INPUT_CHANNEL]);
                    deadBand(&rate_setp[2], YAW_DEADBAND);
                    Fzb_commanded = pwManual[THROTTLE_INPUT_CHANNEL];
                    
                    SpeedLoop(rate_setp[0],rate_setp[1],(rate_setp[2] << 4));
                    break;
            }
            break;
    }
}

void read_inputs(void)
{
    int temp;
    union longww long_accum;

    extern union longww primary_voltage;
    extern unsigned int lowVoltageWarning;

    // limit throttle to 70% if battery is low
    if (primary_voltage._.W1 < lowVoltageWarning)
    {
        throttle_limit = (unsigned int)(0.7 * 65536);
    }
    else if (primary_voltage._.W1 > (lowVoltageWarning + 500))
    {
        // hysteresis of 500mV
        throttle_limit = (unsigned int)(THROTTLE_LIMIT * 65536);
    }

    for (temp = 0; temp <= 4; temp++)
    {
        if (udb_flags._.radio_on)
        {
            if (temp == THROTTLE_INPUT_CHANNEL)
            {
                // limit throttle to leave some control headroom
                long_accum.WW = __builtin_mulus(throttle_limit, (udb_pwIn[temp] - udb_pwTrim[temp]));
                pwManual[temp] = long_accum._.W1 + udb_pwTrim[temp];
            } else
                pwManual[temp] = udb_pwIn[temp];
        }
        else
        {
            // If radio is off, use udb_pwTrim values instead of the udb_pwIn values
            pwManual[temp] = udb_pwTrim[temp];
        }
    }
}

unsigned int check_state(void)
{
    extern boolean didCalibrate;
    static int motorsArmed = 0;
    
    if (!didCalibrate)
    {
        return 1;    // pass throttle channel through to all ESCs to allow ESC calibration
    }
    else if (motorsArmed < 2)
    {
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
        return 2;   // not armed yet; set ESCs to idle
    }
    else if ((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) < THROTTLE_DEADBAND)
    {
        return 3;   // Commands ESCs
    }
    else
    {
        return 0;   // allow the stabilization to run
    }

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
