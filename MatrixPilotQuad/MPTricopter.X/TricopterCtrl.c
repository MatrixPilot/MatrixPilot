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

int Fzb_commanded;      // Force along axis z in body frame
int Tau_b_commanded[3]; // Moments in body frame


//Conversion from body commands to frame commands
void motorCtrl(void)
{

    int motor[3];           // speed value of motors
    int servo_A;            // value for servo


// approximation of sqrt(3)/2=0.866 to 0.9, computation of 1/2 using >> 1
    motor[0] = Fzb_commanded   +(Tau_b_commanded[0]*9)/10   -(Tau_b_commanded[1] >> 1);
    motor[1] = Fzb_commanded   -(Tau_b_commanded[0]*9)/10   -(Tau_b_commanded[1] >> 1);
    motor[2] = Fzb_commanded                                +Tau_b_commanded[1];
    servo_A  = udb_pwTrim[YAW_INPUT_CHANNEL] - Tau_b_commanded[2] ;

    udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor[0]);
    udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor[1]);
    udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor[2]);
    udb_pwOut[SERVO_A_OUTPUT_CHANNEL] = udb_servo_pulsesat(servo_A);

}

void IdleMotorCtrl(unsigned int mode)
{
    int motor[3];           // speed value of motors
    int servo_A;            // value for servo
    fractional* prmat = &rmat[0];
    extern int pwManual[5];
    int commanded_roll;
    int commanded_pitch;
    int commanded_yaw;
    struct relative2D matrix_accum;

    switch(mode)
    {
        case 1:
            // pass throttle channel through to all ESCs to allow ESC calibration
            udb_pwOut[MOTOR_A_OUTPUT_CHANNEL]   = udb_pwIn[THROTTLE_INPUT_CHANNEL];
            udb_pwOut[MOTOR_B_OUTPUT_CHANNEL]   = udb_pwIn[THROTTLE_INPUT_CHANNEL];
            udb_pwOut[MOTOR_C_OUTPUT_CHANNEL]   = udb_pwIn[THROTTLE_INPUT_CHANNEL];
            udb_pwOut[SERVO_A_OUTPUT_CHANNEL]   = udb_pwIn[YAW_INPUT_CHANNEL];
            break;

        case 2:
            // not armed yet; set ESCs to idle
            udb_pwOut[MOTOR_A_OUTPUT_CHANNEL]   = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
            udb_pwOut[MOTOR_B_OUTPUT_CHANNEL]   = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
            udb_pwOut[MOTOR_C_OUTPUT_CHANNEL]   = udb_pwTrim[THROTTLE_INPUT_CHANNEL];
            udb_pwOut[SERVO_A_OUTPUT_CHANNEL]   = udb_pwTrim[YAW_INPUT_CHANNEL];
            break;
            
        case 3:
            motor[0] = pwManual[THROTTLE_INPUT_CHANNEL];
            motor[1] = pwManual[THROTTLE_INPUT_CHANNEL];
            motor[2] = pwManual[THROTTLE_INPUT_CHANNEL];

            //        VectorCopy(9, target_orientation, prmat);

            commanded_roll = (pwManual[ROLL_INPUT_CHANNEL]          - udb_pwTrim[ROLL_INPUT_CHANNEL]);
            commanded_pitch = (pwManual[PITCH_INPUT_CHANNEL]        - udb_pwTrim[PITCH_INPUT_CHANNEL]);
            commanded_yaw = YAW_SIGN * (pwManual[YAW_INPUT_CHANNEL] - udb_pwTrim[YAW_INPUT_CHANNEL]);

            // get heading in earth frame from rmat
            matrix_accum.x = prmat[4];
            matrix_accum.y = prmat[1];

            motor[0] +=  +(commanded_roll*9)/10 -(commanded_pitch >> 1);
            motor[1] +=  -(commanded_roll*9)/10 -(commanded_pitch >> 1);
            motor[2] +=                         + commanded_pitch;
            servo_A = udb_pwTrim[YAW_INPUT_CHANNEL] - commanded_yaw ;

            udb_pwOut[MOTOR_A_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor[0]);
            udb_pwOut[MOTOR_B_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor[1]);
            udb_pwOut[MOTOR_C_OUTPUT_CHANNEL] = udb_servo_pulsesat(motor[2]);
            udb_pwOut[SERVO_A_OUTPUT_CHANNEL] = udb_servo_pulsesat(servo_A);

            break;
    }
}
