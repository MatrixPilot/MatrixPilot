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

typedef enum
{
	AP_CNTRL_PITCH,
	AP_CNTRL_ROLL,
	AP_CNTRL_THROTTLE,
	AP_CNTRL_YAW,
	AP_CNTRL_CAMBER,
	AP_CNTRL_BRAKE,
	AP_CNTRL_FLAP,
	AP_CNTRL_WAGGLE,
	AP_CNTRL_MAX,
} AP_CNTRL;


// RMAX scaled inputs
extern fractional in_cntrls[IN_CNTRL_MAX];

// rmax scaled autopilot controls
extern fractional ap_cntrls[AP_CNTRL_MAX];

// RMAX scaled inputs
extern fractional out_cntrls[IN_CNTRL_MAX];

// mixer outputs before safety checks
extern int16_t mixer_outputs[];

extern void	in_controls(void);

//limit a int32_t variable to +-RMAX
extern int32_t limitRMAX(int32_t input);

// Turn PWM into fraction subtracting the offset
extern fractional PWM_to_frac(int16_t PWM, int16_t offset, boolean reversed);

// Turn PWM into fraction subtracting the offset
extern int16_t frac_to_PWM(fractional frac, int16_t offset, boolean reversed, boolean doubleRange);

// Safe copy of inputs to controls for use by control.
// Needs to be done first so that manual control lockouts can be done.
extern void	pre_control(void);

// Call to control responsible for throttle manual control lockout.
extern inline boolean get_throttle_manual_lockout();

// turn control outputs / PWM inputs into RMAX scaled values with corrected reversing
// Do controlled mixing of manual and autopilot
extern void pre_mix(void);

// Change autopilot output into safe formatted mixer input.
// Also optionally do pre-mixing of manual and autopilot commands.
extern void output_controls(void);

// Apply final safety rules to the output
// Optionally do safe throttle mixing first
extern void post_mix(void);


#endif

