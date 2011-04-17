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

#if (FLIGHT_PLAN_TYPE == FP_LOGO)


struct logoInstructionDef {
	unsigned int cmd		:  6 ;
	unsigned int do_fly		:  1 ;
	unsigned int use_param	:  1 ;
	unsigned int subcmd		:  8 ;
	int arg					: 16 ;
} ;

#define PLANE				0
#define CAMERA				1


// If we've processed this many instructions without commanding the plane to fly,
// then stop and continue on the next run through
#define MAX_INSTRUCTIONS_PER_CYCLE	32


// Note that any instruction with an odd subcmd is a FLY command.
// Interpretation stops on a FLY command until the plane arrives at that
// location, similar to a waypoint.  This includes PEN_DOWN.
// When the pen is up, FLY commands do not stop the interpreter.  So when
// the pen goes back down, we FLY to wherever the turtle has moved to
// while the pen was up.  We also skip flying when the CAMERA turtle is
// the active turtle.


// Define the Low-level Commands
//							   cmd,fly,param,sub,x
#define _REPEAT(n, pr)			{1,	0,	pr,	0,	n},
#define _END					{1,	0,	0,	1,	0},
#define _TO(fn)					{1,	0,	0,	2,	fn},

#define _DO(fn, x, pr)			{2,	0,	pr,	fn, x},
#define _EXEC(fn, x, pr)		{10,0,	pr,	fn, x},

#define _FD(x, fl, pr)			{3,	fl,	pr,	0,	x},

#define _RT(x, pr)				{4,	0,	pr, 0,	x},
#define _SET_ANGLE(x, pr)		{4,	0,	pr, 1,	x},
#define _USE_CURRENT_ANGLE		{4,	0,	0,	2,	0},
#define _USE_ANGLE_TO_GOAL		{4,	0,	0,	3,	0},

#define _MV_X(x, fl, pr)		{5,	fl,	pr,	0,	x},
#define _SET_X(x, fl, pr)		{5,	fl,	pr,	1,	x},
#define _MV_Y(y, fl, pr)		{5,	fl,	pr,	2,	y},
#define _SET_Y(y, fl, pr)		{5,	fl,	pr,	3,	y},
#define _MV_Z(z, fl, pr)		{5,	fl,	pr,	4,	z},
#define _SET_Z(z, fl, ar)		{5,	fl,	ar,	5,	z},
#define _USE_CURRENT_POS(fl)	{5, fl,	0,	6,	0},
#define _HOME(fl)				{5,	fl,	0,	7,	0},

#define _SET_ABS_VAL_HIGH(x)	{5,	0,	0,	8,	x}, // Set the high and then low words for X and
#define _SET_ABS_X_LOW(x)		{5,	0,	0,	9,	x}, // then Y, as 4 consecutive instructions.
#define _SET_ABS_Y_LOW(y, fl)	{5,	fl,	0,	10,	y}, // (as VAL_HIGH, X_LOW, VAL_HIGH, Y_LOW)

#define _FLAG_ON(f)				{6,	0,	0,	0,	f},
#define _FLAG_OFF(f)			{6,	0,	0,	1,	f},
#define _FLAG_TOGGLE(f)			{6,	0,	0,	2,	f},

#define _PEN_UP					{7,	0,	0,	0,	0},
#define _PEN_DOWN				{7,	1,	0,	1,	0},
#define _PEN_TOGGLE				{7,	0,	0,	2,	0},

#define _SET_TURTLE(x)			{8,	0,	0,	0,	x},

#define _PARAM_SET(x)			{9,	0,	0,	0,	x},
#define _PARAM_ADD(x)			{9,	0,	0,	1,	x},
#define _PARAM_MUL(x)			{9,	0,	0,	2,	x},
#define _PARAM_DIV(x)			{9,	0,	0,	3,	x},


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

#define SET_POS(x, y)		_SET_X(x, 0, 0) _SET_Y(y, 1, 0)
#define SET_ABS_POS(x, y)	_SET_ABS_VAL_HIGH((((unsigned long)(x))>>16)&0xFFFF) _SET_ABS_X_LOW(((unsigned long)(x))&0xFFFF) \
							_SET_ABS_VAL_HIGH((((unsigned long)(y))>>16)&0xFFFF) _SET_ABS_Y_LOW(((unsigned long)(y))&0xFFFF, 1)
#define HOME				_HOME(1)


#include "flightplan-logo.h"


#define NUM_INSTRUCTIONS (( sizeof instructions ) / sizeof ( struct logoInstructionDef ))
#define NUM_RTL_INSTRUCTIONS (( sizeof rtlInstructions ) / sizeof ( struct logoInstructionDef ))
int instructionIndex = 0 ;
int waypointIndex = 0 ; // used for telemetry
int absoluteHighWord = 0 ;
union longww absoluteXLong ;

struct logoInstructionDef *currentInstructionSet = (struct logoInstructionDef*)instructions ;
int numInstructionsInCurrentSet = NUM_INSTRUCTIONS ;

struct logoInstructionDef logo_inject_instr ;
unsigned char logo_inject_pos = 0 ;
#define LOGO_INJECT_READY 255


// How many layers deep can Repeats and Subroutines be nested
#define LOGO_STACK_DEPTH			12

struct logoStackFrame {
	unsigned int frameType				:  1 ;
	unsigned int returnInstructionIndex	: 15 ;
	int arg								: 16 ;
} ;
struct logoStackFrame logoStack[LOGO_STACK_DEPTH] ;
int logoStackIndex = 0 ;

#define LOGO_FRAME_TYPE_REPEAT		0
#define LOGO_FRAME_TYPE_SUBROUTINE	1



// These values are relative to the origin, and North
// x and y are in 16.16 fixed point
struct logoLocation { union longww x; union longww y; int z; } ;
struct logoLocation turtleLocations[2] ;
struct relative3D lastGoal = {0, 0, 0} ;

// Angles are stored as 0-359
int turtleAngles[2] = {0, 0} ;

unsigned char currentTurtle ;
int penState ;

boolean process_one_instruction( struct logoInstructionDef instr ) ;
void update_goal_from( struct relative3D old_waypoint ) ;
void process_instructions( void ) ;



// In the future, we could include more than 2 flight plans...
// flightplanNum is 0 for the main lgo instructions, and 1 for RTL instructions
void init_flightplan ( int flightplanNum )
{
	if ( flightplanNum == 1 ) // RTL instructions set
	{
		currentInstructionSet = (struct logoInstructionDef*)rtlInstructions ;
		numInstructionsInCurrentSet = NUM_RTL_INSTRUCTIONS ;
	}
	else if ( flightplanNum == 0 ) // Main instructions set
	{
		currentInstructionSet = (struct logoInstructionDef*)instructions ;
		numInstructionsInCurrentSet = NUM_INSTRUCTIONS ;
	}
	
	instructionIndex = 0 ;
	logoStackIndex = 0 ;
	currentTurtle = PLANE ;
	penState = 0 ; // 0 means down.  more than 0 means up
	
	turtleLocations[PLANE].x._.W1 = GPSlocation.x ;
	turtleLocations[PLANE].y._.W1 = GPSlocation.y ;
	turtleLocations[PLANE].z = GPSlocation.z ;
	
	turtleLocations[CAMERA].x._.W1 = GPSlocation.x ;
	turtleLocations[CAMERA].y._.W1 = GPSlocation.y ;
	turtleLocations[CAMERA].z = GPSlocation.z ;
	
	// calculated_heading								// 0-255 (ccw, 0=East)
	int angle = (calculated_heading * 180 + 64) >> 7 ;	// 0-359 (ccw, 0=East)
	angle = -angle + 90;								// 0-359 (clockwise, 0=North)
	turtleAngles[PLANE] = turtleAngles[CAMERA] = angle ;
	
	setBehavior( 0 ) ;
	
	update_goal_from(GPSlocation) ;
	
	process_instructions() ;
	
	return ;
}


boolean use_fixed_origin( void )
{
#if ( USE_FIXED_ORIGIN == 1 )
	return 1 ;
#else
	return 0 ;
#endif
}


struct absolute3D get_fixed_origin( void )
{
	struct fixedOrigin3D origin = FIXED_ORIGIN_LOCATION ;
	
	struct absolute3D standardizedOrigin ;
	standardizedOrigin.x = origin.x ;
	standardizedOrigin.y = origin.y ;
	standardizedOrigin.z = (long)(origin.z * 100) ;
	
	return standardizedOrigin ;
}


void update_goal_from( struct relative3D old_goal )
{
	struct relative3D new_goal ;
	
	lastGoal.x = new_goal.x = (turtleLocations[PLANE].x._.W1) ;
	lastGoal.y = new_goal.y = (turtleLocations[PLANE].y._.W1) ;
	lastGoal.z = new_goal.z = turtleLocations[PLANE].z ;
	
	if (old_goal.x == new_goal.x && old_goal.y == new_goal.y)
	{
		set_goal( GPSlocation, new_goal ) ;
	}
	else
	{
		set_goal( old_goal, new_goal ) ;
	}
	
	new_goal.x = (turtleLocations[CAMERA].x._.W1) ;
	new_goal.y = (turtleLocations[CAMERA].y._.W1) ;
	new_goal.z = turtleLocations[CAMERA].z ;
	set_camera_view( new_goal ) ;
	
	return ;
}


void run_flightplan( void )
{
	// first run any injected instruction from the serial port
	if (logo_inject_pos == LOGO_INJECT_READY)
	{
		process_one_instruction(logo_inject_instr) ;
		if (logo_inject_instr.cmd == 2)
		{
			instructionIndex++ ;
			process_instructions() ;
		}
		else
		{
			update_goal_from(lastGoal) ;
			compute_bearing_to_goal() ;
		}
		logo_inject_pos = 0 ;
		
		return ;
	}
	
 	// waypoint arrival is detected computing distance to the "finish line".
	// note: locations are measured in meters
	// locations have a range of +-32000 meters (20 miles) from origin
	
	if ( desired_behavior._.altitude )
	{
		if ( abs(IMUheight - goal.height) < ((int) HEIGHT_MARGIN ))
			process_instructions() ;
	}
	else
	{
		if ( desired_behavior._.cross_track )
		{
			if ( tofinish_line < WAYPOINT_RADIUS ) // crossed the finish line
			{
				process_instructions() ;
			}
		}
		else
		{
			if ( (tofinish_line < WAYPOINT_RADIUS) || (togoal.x < WAYPOINT_RADIUS) ) // crossed the finish line
			{
				process_instructions() ;
			}
		}
	}
	return ;
}


unsigned int find_start_of_subroutine(unsigned char subcmd)
{
	int i ;
	for (i = 0; i < numInstructionsInCurrentSet; i++)
	{
		if (currentInstructionSet[i].cmd == 1 && currentInstructionSet[i].subcmd == 2 && currentInstructionSet[i].arg == subcmd)
		{
			return i ;
		}
	}
	return 0 ;
}


int get_current_stack_parameter_frame_index( void )
{
	int i ;
	for (i = logoStackIndex - 1; i >= 0; i--)
	{
		if (logoStack[i].frameType == 1)
		{
			return i ;
		}
	}
	return -1 ;
}


boolean process_one_instruction( struct logoInstructionDef instr )
{
	if (instr.use_param)
	{
		// Use the subroutine's parameter instead of the instruction's arg value
		int ind = get_current_stack_parameter_frame_index() ;
		instr.arg *= (ind >= 0) ? logoStack[ind].arg : 0 ;
	}
	
	switch (instr.cmd)
	{
		case 1: // Repeat
			switch (instr.subcmd)
			{
				case 0: // Repeat N times (or forever if N == -1)
					if (logoStackIndex < LOGO_STACK_DEPTH)
					{
						logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_REPEAT ;
						logoStack[logoStackIndex].arg = instr.arg ;
						logoStack[logoStackIndex].returnInstructionIndex = instructionIndex ;
						logoStackIndex++ ;
					}
					break ;
				case 1: // End
					if (logoStackIndex > 0)
					{
						logoStackIndex-- ;
						
						if ( logoStack[logoStackIndex].frameType == LOGO_FRAME_TYPE_REPEAT )
						{
							// END REPEAT
							if ( logoStack[logoStackIndex].arg > 1 || logoStack[logoStackIndex].arg == -1 )
							{
								if (logoStack[logoStackIndex].arg != -1)
								{
									logoStack[logoStackIndex].arg-- ;
								}
								instructionIndex = logoStack[logoStackIndex].returnInstructionIndex ;
								logoStackIndex++ ;
							}
						}
						else
						{
							// END SUBROUTINE
							instructionIndex = logoStack[logoStackIndex].returnInstructionIndex ;
						}
					}
					else
					{
						// Extra, unmatched END goes back to the start of the program
						instructionIndex = 0 ;
					}
					break ;
				
				case 2: // To (define a function)
				{
					// Shouldn't ever run these lines.
					// If we do get here, restuart from the top of the logo program.
					instructionIndex = 0 ;
				}
				break ;
			}
			break ;
		
		
		case 10: // Exec (reset the stack and then call a subroutine)
			logoStackIndex = 0 ;
			instructionIndex = 0 ;
		case 2: // Do (call a subroutine)
			if (logoStackIndex < LOGO_STACK_DEPTH)
			{
				logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_SUBROUTINE ;
				logoStack[logoStackIndex].arg = instr.arg ;
				logoStack[logoStackIndex].returnInstructionIndex = instructionIndex ;
				logoStackIndex++ ;
			}
			if (instr.subcmd == 0)		// subcmd 0 is reserved to always mean the start of the logo program
			{
				instructionIndex = 0 ;
			}
			else
			{
				instructionIndex = find_start_of_subroutine(instr.subcmd) ;
			}
			break ;
		
		
		case 3: // Forward/Back
			switch (instr.subcmd)
			{
				case 0: // Forward
				{
					int cangle = turtleAngles[currentTurtle] ;			// 0-359 (clockwise, 0=North)
					signed char b_angle = (cangle * 182 + 128) >> 8 ;	// 0-255 (clockwise, 0=North)
					b_angle = -b_angle - 64 ;							// 0-255 (ccw, 0=East)
					
					turtleLocations[currentTurtle].x.WW += (__builtin_mulss(-cosine(b_angle), instr.arg) << 2) ;
					turtleLocations[currentTurtle].y.WW += (__builtin_mulss(-sine(b_angle), instr.arg) << 2) ;
				}
				break ;
			}
			break ;
		
		case 4: // Rotate
			switch (instr.subcmd)
			{
				case 0: // Right
				{
					int angle = turtleAngles[currentTurtle] + instr.arg ;
					while (angle < 0) angle += 360 ;
					angle = angle % 360 ;
					turtleAngles[currentTurtle] = angle ;
					break ;
				}
				case 1: // Set Angle
					turtleAngles[currentTurtle] = instr.arg ;
					break ;
				case 2: // Use current angle
				{
					// calculated_heading								// 0-255 (ccw, 0=East)
					int angle = (calculated_heading * 180 + 64) >> 7 ;	// 0-359 (ccw, 0=East)
					angle = -angle + 90;								// 0-359 (clockwise, 0=North)
					turtleAngles[currentTurtle] = angle ;
					break ;
				}
				case 3: // Use angle to goal
				{
					struct relative2D vectorToGoal;
					vectorToGoal.x = turtleLocations[currentTurtle].x._.W1 - GPSlocation.x ;
					vectorToGoal.y = turtleLocations[currentTurtle].y._.W1 - GPSlocation.y ;
					signed char dir_to_goal = rect_to_polar ( &vectorToGoal ) ;
					
					// dir_to_goal										// 0-255 (ccw, 0=East)
					int angle = (dir_to_goal * 180 + 64) >> 7 ;			// 0-359 (ccw, 0=East)
					angle = -angle + 90;								// 0-359 (clockwise, 0=North)
					turtleAngles[currentTurtle] = angle ;
					break ;
				}
			}
			break ;
		
		case 5: // MV/SET location - X, Y, and Z
			switch (instr.subcmd)
			{
				case 0: // Move X
					turtleLocations[currentTurtle].x._.W1 += instr.arg ;
					break ;
				case 1: // Set X location
					turtleLocations[currentTurtle].x._.W0 = 0 ;
					turtleLocations[currentTurtle].x._.W1 = instr.arg ;
					break ;
				case 2: // Move Y
					turtleLocations[currentTurtle].y._.W1 += instr.arg ;
					break ;
				case 3: // Set Y location
					turtleLocations[currentTurtle].y._.W0 = 0 ;
					turtleLocations[currentTurtle].y._.W1 = instr.arg ;
					break ;
				case 4: // Move Z
					turtleLocations[currentTurtle].z += instr.arg ;
					break ;
				case 5: // Set Z location
					turtleLocations[currentTurtle].z = instr.arg ;
					break ;
				case 6: // Use current position (for x and y)
					turtleLocations[currentTurtle].x._.W0 = 0 ;
					turtleLocations[currentTurtle].x._.W1 = GPSlocation.x ;
					turtleLocations[currentTurtle].y._.W0 = 0 ;
					turtleLocations[currentTurtle].y._.W1 = GPSlocation.y ;
					break ;
				case 7: // HOME
					turtleAngles[currentTurtle] = 0 ;
					turtleLocations[currentTurtle].x.WW = 0 ;
					turtleLocations[currentTurtle].y.WW = 0 ;
					break ;
				case 8: // Absolute set high value
					absoluteHighWord = instr.arg ;
					break ;
				case 9: // Absolute set low X value
				{
					absoluteXLong._.W1 = absoluteHighWord ;
					absoluteXLong._.W0 = instr.arg ;
					break ;
				}
				case 10: // Absolute set low Y value
				{
					union longww absoluteYLong ;
					absoluteYLong._.W1 = absoluteHighWord ;
					absoluteYLong._.W0 = instr.arg ;
					
					struct waypoint3D wp ;
					wp.x = absoluteXLong.WW ;
					wp.y = absoluteYLong.WW ;
					wp.z = 0 ;
					struct relative3D rel = dcm_absolute_to_relative(wp) ;
					turtleLocations[currentTurtle].x._.W0 = 0 ;
					turtleLocations[currentTurtle].x._.W1 = rel.x ;
					turtleLocations[currentTurtle].y._.W0 = 0 ;
					turtleLocations[currentTurtle].y._.W1 = rel.y ;
					break ;
				}
			}
			break ;
		
		case 6: // Flags
			switch (instr.subcmd)
			{
				case 0: // Flag On
					setBehavior(desired_behavior.W | instr.arg) ;
					break ;
				case 1: // Flag Off
					setBehavior(desired_behavior.W & ~instr.arg) ;
					break ;
				case 2: // Flag Toggle
					setBehavior(desired_behavior.W ^ instr.arg) ;
					break ;
			}
			break ;
		
		case 7: // Pen Up/Down
			switch (instr.subcmd)
			{
				case 0: // Pen Up
					penState++ ;
					break ;
				case 1: // Pen Down
					if (penState > 0)
						penState-- ;
					break ;
				case 2: // Pen Toggle
					penState = (penState == 0) ;
					if (penState == 0) instr.do_fly = 1; // Set the Fly Flag
					break ;
			}
			break ;
		
		case 8: // Set Turtle (choose plane or camera target)
			currentTurtle = (instr.arg == CAMERA) ? CAMERA : PLANE ;
			break ;
		case 9:
			switch (instr.subcmd)
			{
				case 0: // Set param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					if (ind >= 0)
					{
						logoStack[ind].arg = instr.arg ;
					}
					break ;
				}
				case 1: // Add to param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					if (ind >= 0)
					{
						logoStack[ind].arg += instr.arg ;
					}
					break ;
				}
				case 2: // Multiple param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					if (ind >= 0)
					{
						logoStack[ind].arg *= instr.arg ;
					}
					break ;
				}
				case 3: // Divide param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					if (ind >= 0)
					{
						logoStack[ind].arg /= instr.arg ;
					}
					break ;
				}
				break ;
			}
	}
	return instr.do_fly ;
}


void process_instructions( void )
{
	int instructionsProcessed = 0 ;
	
	while (1)
	{
		boolean do_fly = process_one_instruction( currentInstructionSet[instructionIndex] ) ;
		
		instructionsProcessed++ ;
		instructionIndex++ ;
		if ( instructionIndex >= numInstructionsInCurrentSet ) instructionIndex = 0 ;
		
		if ( (do_fly && penState == 0 && currentTurtle == PLANE) || instructionsProcessed >= MAX_INSTRUCTIONS_PER_CYCLE)
			break ;
	}
	
	waypointIndex = instructionIndex - 1 ;
	
	update_goal_from(lastGoal) ;
	compute_bearing_to_goal() ;
	
	return ;
}


void flightplan_live_begin( void )
{
	logo_inject_pos = 0 ;
	return ;
}


void flightplan_live_received_byte( unsigned char inbyte )
{
	switch (logo_inject_pos) {
		case 0:
			logo_inject_instr.cmd = inbyte ;
			break ;
		
		case 1:
			logo_inject_instr.subcmd = inbyte ;
			break ;
		
		case 2:
			logo_inject_instr.do_fly = ((inbyte >> 8) & 0x0F) ;
			logo_inject_instr.use_param = (inbyte & 0x0F) ;
			break ;
		
		case 3:
			logo_inject_instr.arg = inbyte * 256 ;
			break ;
		
		case 4:
			logo_inject_instr.arg |= inbyte ;
			break ;
		
		case 5:
			// too many bytes for this command!
			// increment logo_instr_pos below, which invalidates this command
			break ;
		
		default:
			// don't increment while waiting for previous command to complete
			return ;
	}
	
	logo_inject_pos++ ;
	
	return ;
}


void flightplan_live_commit( void )
{
	// The cmd=1 commads (REPEAT, END, TO) are not allowed
	// to be injected.
	if (logo_inject_pos == 5 && logo_inject_instr.cmd != 1)
	{
		logo_inject_pos = LOGO_INJECT_READY ;
	}
	else
	{
		logo_inject_pos = 0 ;
	}
	return ;
}


#endif
