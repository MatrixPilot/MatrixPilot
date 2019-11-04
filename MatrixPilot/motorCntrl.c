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

// Modif gfm Quadcopter
//This file is added to the current (R3064?) 4.1 version of MatrixPilot
//Coming from branch MPMTricopter.
//When this file is upgraded, the modification is noted "Modif gfm Quadcopter"

//#include <libq.h>
#include "../libDCM/libDCM.h"
#include "../Config/options_multicopter.h"
#include "../libUDB/libUDB.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/servoOut.h"
#include "states.h"
#include "motorCntrl.h"
#include "config.h"

#define MAXIMUM_ERROR_INTEGRAL ((long int) 32768000 )
#define RP_DEADBAND     4       // prevent Tx pulse variation from causing yaw drift
#define YAW_DEADBAND    70
//gfm #define MAX_SLEW_RATE   1092    // 1092 * 50Hz * pi/32K * 180/pi = 300 deg/sec
#define MAX_SLEW_RATE   256    // 256 * 200Hz * pi/32K * 180/pi = 300 deg/sec

//extern boolean didCalibrate;
// decoded control mode switch
int control_mode = 0;
int current_control_mode = 0;
int motorsArmed = 0;
boolean didCalibrate;

//int16_t roll_control;
//int16_t pitch_control;
//int16_t yaw_control;
//int16_t throttle_control;
int rolladvanced, pitchadvanced;
signed char lagBC, precessBC;

int pwManual[5]; // channels 1-4 are control inputs from RX


struct int_RPY cmd_RPY, prev_RPY, adv_RPY;
// Modif gfm Quadcopter
extern void rotate_2D( struct relative2D *xy , signed char angle ) ;
extern int16_t rect_to_polar16(struct relative2D *xy);
// fin modif gfm quadcopter


#if (NUM_ROTORS == 3)
// these are the rotation angles for "Y" configuration, with motors:
// A right, angle 60 degrees,
// B rear, angle 180 degrees
// C left, angle 300 degrees
const fractional motor_cos[NUM_ROTORS] = {0.866 * 32768,    0,      -0.866 * 32768};
const fractional motor_sin[NUM_ROTORS] = {0.5 * 32768,      -32768, 0.5 * 32768};

int motorMap[NUM_ROTORS] = {MOTOR_A_OUTPUT_CHANNEL, MOTOR_B_OUTPUT_CHANNEL,
    MOTOR_C_OUTPUT_CHANNEL};

#elif (NUM_ROTORS == 4)

// rotate copter CCW 45 degrees
#define X_SIN ((int)(-0.707 * 32768))
#define X_COS ((int)(0.707 * 32768))

// these are the rotation angles for "plus" configuration, with motors:
// A front, angle 0 degrees,
// B right, angle 90 degrees
// C rear, angle 180 degrees
// D left, angle 270 degrees
const fractional motor_cos[NUM_ROTORS] = {0, 32767, 0, -32768};
const fractional motor_sin[NUM_ROTORS] = {32767, 0, -32768, 0};

int motorMap[NUM_ROTORS] = {MOTOR_A_OUTPUT_CHANNEL, MOTOR_B_OUTPUT_CHANNEL,
    MOTOR_C_OUTPUT_CHANNEL, MOTOR_D_OUTPUT_CHANNEL};

#elif (NUM_ROTORS == 6)

// rotate copter clockwise 30 degrees
#define X_SIN ((const fractional)(0.5   * 32768))
#define X_COS ((const fractional)(0.866 * 32768))

// these are the rotation angles for "plus" configuration, with motors:
// A front,         angle 0 degrees,
// B front right,   angle 60 degrees
// C rear right,    angle 120 degrees
// D rear,          angle 180 degrees
// E rear left,     angle 240 degrees
// F front left,    angle 300 degrees
const fractional motor_cos[NUM_ROTORS] = {0, 0.866 * 32768, 0.866 * 32768, 0, -0.866 * 32768, -0.866 * 32768};
const fractional motor_sin[NUM_ROTORS] = {32767, 0.5 * 32768, -0.5 * 32768, -32768, -0.5 * 32768, 0.5 * 32768};

int motorMap[NUM_ROTORS] = {MOTOR_A_OUTPUT_CHANNEL, MOTOR_B_OUTPUT_CHANNEL,
    MOTOR_C_OUTPUT_CHANNEL, MOTOR_D_OUTPUT_CHANNEL,
    MOTOR_E_OUTPUT_CHANNEL, MOTOR_F_OUTPUT_CHANNEL};

#else
#error ("unsupported value for NUM_ROTORS")
#endif

void rotateRP(struct int_RPY* command, signed char angle) {
    struct relative2D xy;
    xy.x = command->roll;
    xy.y = command->pitch;
    rotate_2D(&xy, angle);
    command->roll = xy.x;
    command->pitch = xy.y;
}

void rotate2D(int *x, int *y, signed char angle) {
    struct relative2D xy;
    xy.x = *x;
    xy.y = *y;
    rotate_2D(&xy, angle);
    *x = xy.x;
    *y = xy.y;
}

void deadBandRP(struct int_RPY* command, int bandRP) {
    if (command->roll >= bandRP) {
        command->roll -= bandRP;
    } else if (command->roll <= -bandRP) {
        command->roll += bandRP;
    } else {
        command->roll = 0;
    }

    if (command->pitch >= bandRP) {
        command->pitch -= bandRP;
    } else if (command->pitch <= -bandRP) {
        command->pitch += bandRP;
    } else {
        command->pitch = 0;
    }
}

void deadBand(int *input, int band) {
    if (*input >= band)
        *input -= band;
    else if (*input <= -band)
        *input += band;
    else
        *input = 0;
}

void magClamp(int *in, int mag) {
    if (*in < -mag)
        *in = -mag;
    else if (*in > mag)
        *in = mag;
}

void magClampRP(struct int_RPY* command, int mag) {
    if (command->roll < -mag) {
        command->roll = -mag;
    } else if (command->roll > mag) {
        command->roll = mag;
    }

    if (command->pitch < -mag) {
        command->pitch = -mag;
    } else if (command->pitch > mag) {
        command->pitch = mag;
    }
}

void magClampYAW(struct int_RPY* command, int mag) {
    if (command->yaw < -mag) {
        command->yaw = -mag;
    } else if (command->yaw > mag) {
        command->yaw = mag;
    }
}

void magClampRPY(struct int_RPY* command, int mag) {
    if (command->roll < -mag) {
        command->roll = -mag;
    } else if (command->roll > mag) {
        command->roll = mag;
    }

    if (command->pitch < -mag) {
        command->pitch = -mag;
    } else if (command->pitch > mag) {
        command->pitch = mag;
    }

    if (command->yaw < -mag) {
        command->yaw = -mag;
    } else if (command->yaw > mag) {
        command->yaw = mag;
    }
}

void magClamp32(long *in, long mag) {
    if (*in < -mag)
        *in = -mag;
    else if (*in > mag)
        *in = mag;
}

//Motor command udb_pwOut is sent every 2.5 ms by udb_set_dc in servoOut.c called by
void motorOut(int throttle, struct int_RPY *command) {
    union longww long_accum;
    int index;

    for (index = 0; index < NUM_ROTORS; index++) {
        long_accum.WW = __builtin_mulss(motor_cos[index], command->roll);
        long_accum.WW += __builtin_mulss(motor_sin[index], command->pitch);
        long_accum.WW <<= 1; // drop extra sign bit
        int mval = throttle - long_accum._.W1;// Takes only the last two bytes

        #if (NUM_ROTORS != 3)
        if (index & 0b1) {
            // CW rotation
            mval -= command->yaw;
        } else {
            // CCW rotation
            mval += command->yaw;
        }
        #endif
        udb_pwOut[motorMap[index]] = udb_servo_pulsesat(mval);
    }
    #if (NUM_ROTORS == 3)
        long_accum.WW = __builtin_mulss(32768, command->yaw);
        long_accum.WW <<= 1; // drop extra sign bit

        udb_pwOut[SERVO_TAIL_CHANNEL] = udb_servo_pulsesat(udb_pwTrim[RUDDER_INPUT_CHANNEL] + long_accum._.W1);
    #endif
}

// in X configuration, rotate roll/pitch commands the amount specified by X_SIN,COS

void x_rotate(struct int_RPY* command) {
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


// yaw is always a rate command

void updateYaw(struct int_RPY* command) {
    int yawRate = (pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) << 1;
    // apply deadband and slew rate limiter
    deadBand(&yawRate, YAW_DEADBAND);
    magClamp(&yawRate, MAX_SLEW_RATE);
    command->yaw += yawRate >>2;
    magClampYAW(command, YAW_CLAMP);// ajout gfm
}

// perform rpy command updates at PID_HZ/8
static int rateCounter = 0;

void get_rateMode_commands(struct int_RPY* command) {
    if (rateCounter++ > 7) {
        rateCounter = 0;
        // manual rate flight mode
        // range +/- 1250 corresponds to max of 50Hz * 1250 * pi/32K * (180/pi) = 343 deg/sec
        struct int_RPY cmdRate;
        cmdRate.roll = (pwManual[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]);
        cmdRate.pitch = (pwManual[ELEVATOR_INPUT_CHANNEL] - udb_pwTrim[ELEVATOR_INPUT_CHANNEL]);

        // apply deadband and slew rate limit to roll and pitch
        deadBandRP(&cmdRate, RP_DEADBAND);
        magClampRP(&cmdRate, MAX_SLEW_RATE);

        command->roll += cmdRate.roll >> 2;
        command->pitch += cmdRate.pitch >> 2;

        // rotate commanded roll/pitch into X body frame, if required
//        x_rotate(command);

        // yaw is always rate mode
        updateYaw(command);
    }
}

void get_angleMode_commands(struct int_RPY* command, int tiltGain) {
    if (rateCounter++ > 7) {
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
//        x_rotate(command);

        // yaw is always rate mode
//        updateYaw(command);
        command->yaw = YAW_SIGN * (pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]);
        magClampYAW(command, YAW_CLAMP);// ajout gfm
    }
}

unsigned int throttle_limit = (unsigned int) (65536 * THROTTLE_LIMIT);

// motorCntrl is called every 5ms by dcm_servo_callback_prepare_outputs in servoPrepare.c
void motorCntrl(void) {

    int temp;
    union longww long_accum;

    // Read radio inputs
    for (temp = 0; temp <= 4; temp++) {
        if (udb_flags._.radio_on) {
            if (temp == THROTTLE_INPUT_CHANNEL) {
                // limit throttle to leave some control headroom
                long_accum.WW = __builtin_mulus(throttle_limit, (udb_pwIn[temp] - udb_pwTrim[temp]));
                pwManual[temp] = long_accum._.W1 + udb_pwTrim[temp];
            } else
                pwManual[temp] = udb_pwIn[temp];
        } else
            // If radio is off, use udb_pwTrim values instead of the udb_pwIn values
            pwManual[temp] = udb_pwTrim[temp];

    }

    if (!didCalibrate) {
        motorsArmed = 0;
        LED_ORANGE = LED_OFF;
    } else {
        switch (motorsArmed) {
            case 0:
                // wait for high throttle
                if ((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) > (SERVORANGE / 2))
                    motorsArmed = 1;
                    LED_ORANGE = LED_ON;
                    LED_RED = LED_OFF;
                    LED_BLUE = LED_OFF;
                break;
            case 1:
                // wait for low throttle and > half right rudder
                if (((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) < THROTTLE_DEADBAND) &&
                        (udb_pwIn[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) > (SERVORANGE / 2)
                        ) {
                    motorsArmed = 2;
                    LED_ORANGE = LED_OFF;
                    LED_RED = LED_OFF;
                    LED_BLUE = LED_ON;
                }
                break;
            case 2:
                // wait for low throttle and neutral rudder
                if (((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) < THROTTLE_DEADBAND) &&
                        (udb_pwIn[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) < THROTTLE_DEADBAND
                        ) {
                    motorsArmed = 3;
                    LED_ORANGE = LED_OFF;
                    LED_RED = LED_ON;
                    LED_BLUE = LED_OFF;
                }
                break;
            case 3:
                // wait for low throttle and > half left rudder
                if (((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) < THROTTLE_DEADBAND) &&
                        (udb_pwIn[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]) < -(SERVORANGE / 2)
                        ) {
                    motorsArmed = 4;
                    LED_RED = LED_OFF;
                    LED_ORANGE = LED_OFF;
                    LED_BLUE = LED_OFF;
                }
                break;
        }
    }

    if (motorsArmed < 4) {
        // not armed yet; set ESCs to idle
        cmd_RPY.roll = 0;
        cmd_RPY.pitch = 0;
        cmd_RPY.yaw = 0;
        motorOut(udb_pwTrim[THROTTLE_INPUT_CHANNEL], &cmd_RPY);
    } else if ((pwManual[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) < THROTTLE_DEADBAND) {
        // test motor responses
        // command motors to spin at rates proportional to command
        cmd_RPY.roll = (pwManual[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]);
        cmd_RPY.pitch = (udb_pwTrim[ELEVATOR_INPUT_CHANNEL] - pwManual[ELEVATOR_INPUT_CHANNEL] );
        x_rotate(&cmd_RPY);
        cmd_RPY.yaw = YAW_SIGN * (pwManual[RUDDER_INPUT_CHANNEL] - udb_pwTrim[RUDDER_INPUT_CHANNEL]);
        magClampRPY(&cmd_RPY, 30);//gfm to avoid too much RPM after yaw command initialization
        motorOut(udb_pwTrim[THROTTLE_INPUT_CHANNEL], &cmd_RPY);

    } else {
       // fly!
        // check flight mode
        if (control_mode != current_control_mode) {
            // on change of flight mode, record current IMU position
            current_control_mode = control_mode;
        }
        if (control_mode == TILT_MODE) {
            // manual (tilt) flight mode
            get_angleMode_commands(&cmd_RPY, CMD_TILT_GAIN);
        } else if (control_mode == RATE_MODE) {
            // manual (rate) flight mode
            get_rateMode_commands(&cmd_RPY);
            //get_rateMode_commands(&cmd_RPY);
            get_angleMode_commands(&cmd_RPY, CMD_TILT_GAIN);
            updateYaw(&cmd_RPY);
        } else if (control_mode == COMPASS_MODE) {
            // manual mode: forward cyclic is North
            get_angleMode_commands(&cmd_RPY, CMD_TILT_GAIN);
            // rotate forward stick North (angle -heading)
//            rotateRP(&cmd_RPY, (-earth_yaw) >> 8);
            cmd_RPY.yaw = 0;    // Point straight north
        }
        adv_RPY.roll = cmd_RPY.roll-roll_control;
        adv_RPY.pitch = -1*(cmd_RPY.pitch+pitch_control);
        adv_RPY.yaw = cmd_RPY.yaw-yaw_control;
//        rotateRP(&adv_RPY, lagBC);
        x_rotate(&adv_RPY);
        magClampRP(&adv_RPY, RP_CLAMP);
        magClampYAW(&adv_RPY, YAW_CLAMP);

        // Compute the signals that are common to all 4 motors
        // Mix in the yaw, pitch, and roll signals into the motors
        if (state_flags._.man_req)
        if (!state_flags._.altitude_hold_throttle )
            motorOut(pwManual[THROTTLE_INPUT_CHANNEL], &adv_RPY);
        else
            motorOut(udb_pwTrim[THROTTLE_INPUT_CHANNEL]+(int)throttle_control, &adv_RPY);

    }
}
