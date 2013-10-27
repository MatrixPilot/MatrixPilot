/* 
 * File:   poseUtils.h
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

#ifndef MOTORCNTRL_H
#define	MOTORCNTRL_H

#ifdef	__cplusplus
extern "C" {
#endif

    // Specify maximum tilt angle setpoint
    // PWM command input range is +/-1000 counts
    // tilt angle is angle / pi as Q15 signed fractional (+/-pi/2 => +/-16384)
#define CMD_TILT_GAIN (unsigned int) (16384 / 1250)

    struct int_RPY {
        int roll;
        int pitch;
        int yaw;
    };
    extern struct int_RPY cmd_RPY;

    extern int pwManual[];
#ifdef	__cplusplus
}
#endif

#endif	/* MOTORCNTRL_H */

