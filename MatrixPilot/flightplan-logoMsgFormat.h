// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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
#ifndef _FLIGHT_PLAN_LOGOMSGFORMAT_H_
#define _FLIGHT_PLAN_LOGOMSGFORMAT_H_

#if (FLIGHT_PLAN_TYPE == FP_LOGO)
#include <stdint.h>
#include "defines.h"

struct logoInstructionDef {
	uint16_t cmd		:  6 ;
	uint16_t do_fly		:  1 ;
	uint16_t use_param	:  1 ;
	uint16_t subcmd		:  8 ;
	int16_t arg					: 16 ;
} ;


// Note that any instruction with an odd subcmd is a FLY command.
// Interpretation stops on a FLY command until the plane arrives at that
// location, similar to a waypoint.  This includes PEN_DOWN.
// When the pen is up, FLY commands do not stop the interpreter.  So when
// the pen goes back down, we FLY to wherever the turtle has moved to
// while the pen was up.  We also skip flying when the CAMERA turtle is
// the active turtle.

// Define the conditional VAL values for IF commands
enum {
	LOGO_VAL_ZERO = 0,
	// XX_INPUT_CHANNEL // leave room for input channels: 1 - NUM_INPUTS (up to 15)
	DIST_TO_HOME = 16,
	DIST_TO_GOAL,
	ALT,
	CURRENT_ANGLE,
	ANGLE_TO_HOME,
	ANGLE_TO_GOAL,
	REL_ANGLE_TO_HOME,
	REL_ANGLE_TO_GOAL,
	GROUND_SPEED,
	AIR_SPEED,
	AIR_SPEED_Z,
	WIND_SPEED,
	WIND_SPEED_X,
	WIND_SPEED_Y,
	WIND_SPEED_Z,
	PARAM
};


// Define the Low-level Commands
//					cmd,    fly,    param,  sub,    x
#define _REPEAT(n, pr)			{1,	0,	pr,	0,	n},
#define _END				{1,	0,	0,	1,	0},
#define _ELSE				{1,	0,	0,	3,	0},
#define _TO(fn)				{1,	0,	0,	2,	fn},

#define _DO(fn, x, pr)			{2,	0,	pr,	fn,     x},
#define _EXEC(fn, x, pr)		{10,    0,	pr,	fn,     x},

#define _FD(x, fl, pr)			{3,	fl,	pr,	0,	x},

#define _RT(x, pr)			{4,	0,	pr,     0,	x},
#define _SET_ANGLE(x, pr)		{4,	0,	pr,     1,	x},
#define _USE_CURRENT_ANGLE		{4,	0,	0,	2,	0},
#define _USE_ANGLE_TO_GOAL		{4,	0,	0,	3,	0},

#define _MV_X(x, fl, pr)		{5,	fl,	pr,	0,	x},
#define _SET_X(x, fl, pr)		{5,	fl,	pr,	1,	x},
#define _MV_Y(y, fl, pr)		{5,	fl,	pr,	2,	y},
#define _SET_Y(y, fl, pr)		{5,	fl,	pr,	3,	y},
#define _MV_Z(z, fl, pr)		{5,	fl,	pr,	4,	z},
#define _SET_Z(z, fl, pr)		{5,	fl,	pr,	5,	z},
#define _USE_CURRENT_POS(fl)            {5,     fl,	0,	6,	0},
#define _HOME(fl)			{5,	fl,	0,	7,	0},

#define _SET_ABS_VAL_HIGH(x)            {5,	0,	0,	8,	x}, // Set the high and then low words for X and
#define _SET_ABS_X_LOW(x)		{5,	0,	0,	9,	x}, // then Y, as 4 consecutive instructions.
#define _SET_ABS_Y_LOW(y, fl)           {5,	fl,	0,	10,	y}, // (as VAL_HIGH, X_LOW, VAL_HIGH, Y_LOW)

#define _FLAG_ON(f)			{6,	0,	0,	0,	f},
#define _FLAG_OFF(f)			{6,	0,	0,	1,	f},
#define _FLAG_TOGGLE(f)			{6,	0,	0,	2,	f},

#define _PEN_UP				{7,	0,	0,	0,	0},
#define _PEN_DOWN			{7,	1,	0,	1,	0},
#define _PEN_TOGGLE			{7,	0,	0,	2,	0},

#define _SET_TURTLE(x)			{8,	0,	0,	0,	x},

#define _PARAM_SET(x)			{9,	0,	0,	0,	x},
#define _PARAM_ADD(x)			{9,	0,	0,	1,	x},
#define _PARAM_MUL(x)			{9,	0,	0,	2,	x},
#define _PARAM_DIV(x)			{9,	0,	0,	3,	x},

#define _SPEED_INCREASE(s, pr)          {11,    0,	pr,	0,	s},
#define _SET_SPEED(s, pr)		{11,    0,	pr,	1,	s},

#define _SET_INTERRUPT(fn)		{12,    0,	0,	1,	fn},
#define _CLEAR_INTERRUPT		{12,    0,	0,	0,	0},

#define _LOAD_TO_PARAM(val)		{13,    0,	0,	val,    0},

#define _IF_EQ(val, x, pr)		{14,    0,	pr,	val,    x},
#define _IF_NE(val, x, pr)		{15,    0,	pr,	val,    x},
#define _IF_GT(val, x, pr)		{16,    0,	pr,	val,    x},
#define _IF_LT(val, x, pr)		{17,    0,	pr,	val,    x},
#define _IF_GE(val, x, pr)		{18,    0,	pr,	val,    x},
#define _IF_LE(val, x, pr)		{19,    0,	pr,	val,    x}, // if this list grows, update LOGO_HIGHEST_VALID_CMD_NUMBER

#define LOGO_HIGHEST_VALID_CMD_NUMBER (19)

// Define the High-level Commands
#define FD(x)				_FD(x, 1, 0)
#define BK(x)				_FD(-x, 1, 0)
#define FD_PARAM			_FD(1, 1, 1)
#define BK_PARAM			_FD(-1, 1, 1)

#define RT(x)				_RT(x, 0)
#define LT(x)				_RT(-x, 0)
#define SET_ANGLE(x)		_SET_ANGLE(x, 0)
#define RT_PARAM			_RT(1, 1)
#define LT_PARAM			_RT(-1, 1)
#define SET_ANGLE_PARAM		_SET_ANGLE(0, 1)
#define USE_CURRENT_ANGLE	_USE_CURRENT_ANGLE
#define USE_ANGLE_TO_GOAL	_USE_ANGLE_TO_GOAL

#define EAST(x)				_MV_X(x, 1, 0)
#define WEST(x)				_MV_X(-x, 1, 0)
#define SET_X_POS(x)		_SET_X(x, 1, 0)
#define EAST_PARAM			_MV_X(1, 1, 1)
#define WEST_PARAM			_MV_X(-1, 1, 1)
#define SET_X_POS_PARAM		_SET_X(1, 1, 1)
#define USE_CURRENT_POS		_USE_CURRENT_POS(1)

#define NORTH(y)			_MV_Y(y, 1, 0)
#define SOUTH(y)			_MV_Y(-y, 1, 0)
#define SET_Y_POS(y)		_SET_Y(y, 1, 0)
#define NORTH_PARAM			_MV_Y(1, 1, 1)
#define SOUTH_PARAM			_MV_Y(-1, 1, 1)
#define SET_Y_POS_PARAM		_SET_Y(1, 1, 1)

#define ALT_UP(z)			_MV_Z(z, 0, 0)
#define ALT_DOWN(z)			_MV_Z(-z, 0, 0)
#define SET_ALT(z)			_SET_Z(z, 0, 0)
#define ALT_UP_PARAM		_MV_Z(1, 0, 1)
#define ALT_DOWN_PARAM		_MV_Z(-1, 0, 1)
#define SET_ALT_PARAM		_SET_Z(1, 0, 1)

#define SPEED_INCREASE(x)	_SPEED_INCREASE(x, 0)
#define SPEED_DECREASE(x)	_SPEED_INCREASE(-x, 0)
#define SET_SPEED(x)		_SET_SPEED(x, 0)
#define SPEED_INCREASE_PARAM _SPEED_INCREASE(1, 1)
#define SPEED_DECREASE_PARAM _SPEED_INCREASE(-1, 1)
#define SET_SPEED_PARAM		_SET_SPEED(0, 1)

#define FLAG_ON(f)			_FLAG_ON(f)
#define FLAG_OFF(f)			_FLAG_OFF(f)
#define FLAG_TOGGLE(f)		_FLAG_TOGGLE(f)

#define PEN_UP				_PEN_UP
#define PEN_DOWN			_PEN_DOWN
#define PEN_TOGGLE			_PEN_TOGGLE

#define SET_TURTLE(x)		_SET_TURTLE(x)

#define REPEAT(n)			_REPEAT(n, 0)
#define REPEAT_PARAM		_REPEAT(1, 1)
#define REPEAT_FOREVER		_REPEAT(-1, 0)
#define END					_END
#define ELSE				_ELSE

#define TO(func)			_TO(func)

#define DO(func)			_DO(func, 0, 0)
#define DO_ARG(func, arg)	_DO(func, arg, 0)
#define DO_PARAM(func)		_DO(func, 1, 1)

#define EXEC(func)			_EXEC(func, 0, 0)
#define EXEC_ARG(func, arg)	_EXEC(func, arg, 0)
#define EXEC_PARAM(func)	_EXEC(func, 1, 1)

#define PARAM_SET(x)		_PARAM_SET(x)
#define PARAM_ADD(x)		_PARAM_ADD(x)
#define PARAM_SUB(x)		_PARAM_ADD(-x)
#define PARAM_MUL(x)		_PARAM_MUL(x)
#define PARAM_DIV(x)		_PARAM_DIV(x)

#define SET_INTERRUPT(fn)	_SET_INTERRUPT(fn)
#define CLEAR_INTERRUPT		_CLEAR_INTERRUPT

#define LOAD_TO_PARAM(val)	_LOAD_TO_PARAM(val)

#define IF_EQ(val, x)		_IF_EQ(val, x, 0)
#define IF_NE(val, x)		_IF_NE(val, x, 0)
#define IF_GT(val, x)		_IF_GT(val, x, 0)
#define IF_LT(val, x)		_IF_LT(val, x, 0)
#define IF_GE(val, x)		_IF_GE(val, x, 0)
#define IF_LE(val, x)		_IF_LE(val, x, 0)
#define IF_EQ_PARAM(val)	_IF_EQ(val, 1, 1)
#define IF_NE_PARAM(val)	_IF_NE(val, 1, 1)
#define IF_GT_PARAM(val)	_IF_GT(val, 1, 1)
#define IF_LT_PARAM(val)	_IF_LT(val, 1, 1)
#define IF_GE_PARAM(val)	_IF_GE(val, 1, 1)
#define IF_LE_PARAM(val)	_IF_LE(val, 1, 1)

#define SET_POS(x, y)		_SET_X(x, 0, 0) _SET_Y(y, 1, 0)
#define SET_ABS_POS(x, y)	_SET_ABS_VAL_HIGH((((uint32_t)(x))>>16)&0xFFFF) _SET_ABS_X_LOW(((uint32_t)(x))&0xFFFF) \
							_SET_ABS_VAL_HIGH((((uint32_t)(y))>>16)&0xFFFF) _SET_ABS_Y_LOW(((uint32_t)(y))&0xFFFF, 1)
#define HOME				_HOME(1)


#endif // #if (FLIGHT_PLAN_TYPE == FP_LOGO)
#endif //_FLIGHT_PLAN_LOGOMSGFORMAT_H_