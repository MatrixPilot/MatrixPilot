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

uint16_t yawkdail;
uint16_t rollkp;
uint16_t rollkd;
#elif ((SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || (GAINS_VARIABLE == 1))
uint16_t yawkdail = (uint16_t) (YAWKD_AILERON*SCALEGYRO*RMAX);
uint16_t rollkp = (uint16_t) (ROLLKP*RMAX);
uint16_t rollkd = (uint16_t) (ROLLKD*SCALEGYRO*RMAX);
#else 
const uint16_t yawkdail = (uint16_t) (YAWKD_AILERON*SCALEGYRO*RMAX);
const uint16_t rollkp = (uint16_t) (ROLLKP*RMAX);
const uint16_t rollkd = (uint16_t) (ROLLKD*SCALEGYRO*RMAX);
#endif	

#if (USE_CONFIGFILE == 1)
uint16_t hoverrollkp;
uint16_t hoverrollkd;
#elif ((SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK) || (GAINS_VARIABLE == 1))
uint16_t hoverrollkp = (uint16_t) (HOVER_ROLLKP*SCALEGYRO*RMAX);
uint16_t hoverrollkd = (uint16_t) (HOVER_ROLLKD*SCALEGYRO*RMAX);
#else
const uint16_t hoverrollkp = (uint16_t) (HOVER_ROLLKP*SCALEGYRO*RMAX);
const uint16_t hoverrollkd = (uint16_t) (HOVER_ROLLKD*SCALEGYRO*RMAX);
#endif

void normalRollCntrl(void);
void hoverRollCntrl(void);

#if (USE_CONFIGFILE == 1)

void init_rollCntrl(void) {
    yawkdail = (uint16_t) (YAWKD_AILERON * SCALEGYRO * RMAX);
    rollkp = (uint16_t) (ROLLKP * RMAX);
    rollkd = (uint16_t) (ROLLKD * SCALEGYRO * RMAX);

    hoverrollkp = (uint16_t) (HOVER_ROLLKP * SCALEGYRO * RMAX);
    hoverrollkd = (uint16_t) (HOVER_ROLLKD * SCALEGYRO * RMAX);
}
#endif

void rollCntrl(void) {
    if (canStabilizeHover() && current_orientation == F_HOVER) {
        hoverRollCntrl();
    } else {
        normalRollCntrl();
    }
}

void normalRollCntrl(void) {
    union longww rollAccum = {0};
    union longww gyroRollFeedback;
    union longww gyroYawFeedback;

    fractional rmat6;
    fractional omegaAccum2;

    if (!canStabilizeInverted() || !desired_behavior._.inverted) {
        rmat6 = rmat[6];
        omegaAccum2 = omegaAccum[2];
    } else {
        rmat6 = -rmat[6];
        omegaAccum2 = -omegaAccum[2];
    }

#ifdef TestGains
    flags._.GPS_steering = 0; // turn off navigation
#endif
    // manual input is 2 * delta usec (range [-1000, 1000])
    int16_t roll_manual =  REVERSE_IF_NEEDED(AILERON_CHANNEL_REVERSED,
            (udb_pwIn[AILERON_INPUT_CHANNEL] - udb_pwTrim[AILERON_INPUT_CHANNEL]));

    // To convert roll_setpoint to a DCM angle, we need to scale it
    // to range from zero to +/- max. bank angle:
    // note that DX7 TX needs travel adjust +/-150% to achieve full PWM range
    // rmat ranges [-16384, 16383] for +/- 90 degrees
    roll_setpoint = (roll_manual << 4) + (roll_manual << 3);

    if (AILERON_NAVIGATION && flags._.GPS_steering) {
        // subtract from angle setpoint (sign is correct, rmat6 is special)
        roll_setpoint += determine_navigation_deflection('a') << 3;
    }

    // limit combined manual and nav roll setpoint to less than +/-90 degrees
    magClamp(&roll_setpoint, 16000);

#ifdef TestGains
    flags._.pitch_feedback = 1;
#endif

    if (ROLL_STABILIZATION_AILERONS && flags._.pitch_feedback) {
        gyroRollFeedback.WW = __builtin_mulus(rollkd, omegaAccum[1]);
        // Beware: -rmat6 is roll angle, so it must be added here
        // NOT THE SAME as pitch which is (pitch_setpoint - rmat7)
        rollAccum.WW = __builtin_mulsu(roll_setpoint + rmat6, rollkp);
    } else {
        // no stabilization; pass manual input through
        rollAccum._.W1 = roll_manual;
        gyroRollFeedback.WW = 0;
    }

    if (YAW_STABILIZATION_AILERON && flags._.pitch_feedback) {
        gyroYawFeedback.WW = __builtin_mulus(yawkdail, omegaAccum2);
    } else {
        gyroYawFeedback.WW = 0;
    }

    // roll_control is the final PD loop (no I term) output for roll:
    // roll_control = nav_term + roll_stab_term + yaw_stab_term

    // For aileron navigation, the nav_term is proportional to heading error
    // and has gain yaw_kp. The roll_stab term is proportional to roll angle
    // and rate (absolute) with gains rollkp and rollkd, respectively.
    // In order to prevent navigation inputs from affecting stability of the
    // PID stabilization loop, it seems preferable to use the nav_term to alter
    // the angle setpoint of the roll stabilization loop.

    // To change the roll_control output to drive us to the nav setpoint:
    // roll_control = (measured angle - angle setpoint) * Pgain + (measured rate - rate setpoint) * Dgain

    // rollAccum._.W1 needs to become the roll angle setpoint and feed into line 114 instead of line 148 below

    // the gyroRollFeedback would become:
    // rollkp * (setpoint - rmat6) + rollkd * omegaAccum[1]

    // The roll setpoint might be +/-30 degrees, dependent
    // only on whether a left or right turn is desired.

    // For YAW_STABILIZATION_AILERON, the gyroYawFeedback term needs a
    // setpoint specifying the desired yaw rate (this could be a constant e.g. all turns
    // are standard rate turns), but YAW_STABILIZATION is not enabled in current testing.

    roll_control = rollAccum._.W1 - (int32_t) gyroRollFeedback._.W1 - (int32_t) gyroYawFeedback._.W1;
    // Servo reversing is handled in servoMix.c
}

void hoverRollCntrl(void) {
    int16_t rollNavDeflection;
    union longww gyroRollFeedback;

    if (flags._.pitch_feedback) {
        if (AILERON_NAVIGATION && flags._.GPS_steering) {
            rollNavDeflection = (tofinish_line > HOVER_NAV_MAX_PITCH_RADIUS / 2) ? determine_navigation_deflection('h') : 0;
        } else {
            rollNavDeflection = 0;
        }

        gyroRollFeedback.WW = __builtin_mulus(hoverrollkd, omegaAccum[1]);
    } else {
        rollNavDeflection = 0;
        gyroRollFeedback.WW = 0;
    }

    roll_control = rollNavDeflection - (int32_t) gyroRollFeedback._.W1;
}
