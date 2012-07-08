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

#ifndef INPUTCNTRL_H
#define INPUTCNTRL_H

// Fix this to your particular radio at 100% travel
#define MIX_PWM_RANGE 800

typedef enum
{
	IN_CNTRL_PITCH,
	IN_CNTRL_ROLL,
	IN_CNTRL_THROTTLE,
	IN_CNTRL_YAW,
	IN_CNTRL_CAMBER,
	IN_CNTRL_BRAKE,
	IN_CNTRL_FLAP,
	IN_CNTRL_MAX,
} IN_CNTRL;

// RMAX scaled inputs
extern fractional in_cntrls[IN_CNTRL_MAX];

// Turn PWM into fraction subtracting the offset
extern fractional PWM_to_frac(int PWM, int offset, boolean reversed);

// turn PWM inputs into RMAX scaled values with corrected reversing
extern void input_controls(void);

#endif

