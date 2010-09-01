// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


struct logoInstructionDef	{ char cmd; char subcmd; int arg; } ;

#define PLANE				0
#define CAMERA				1

#define PEN_UP_STATE		0
#define PEN_DOWN_STATE		1


// How many layers deep can Repeats commands be nested
#define REPEAT_STACK_DEPTH 8

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
#define _FD(x)				{1,0, x},
#define _FD_FLY(x)			{1,1, x},

#define _RT(x)				{2,0, x},
#define _SET_ANGLE(x)		{2,2, x},
#define _USE_CURRENT_ANGLE	{2,4, 0},

#define _MV_X(x)			{3,0, x},
#define _MV_X_FLY(x)		{3,1, x},
#define _SET_X(x)			{3,2, x},
#define _SET_X_FLY(x)		{3,3, x},

#define _MV_Y(y)			{4,0, y},
#define _MV_Y_FLY(y)		{4,1, y},
#define _SET_Y(y)			{4,2, y},
#define _SET_Y_FLY(y)		{4,3, y},

#define _MV_Z(z)			{5,0, z},
#define _MV_Z_FLY(z)		{5,1, z},
#define _SET_Z(z)			{5,2, z},
#define _SET_Z_FLY(z)		{5,3, z},

#define _FLAG_ON(f)			{6,0, f},
#define _FLAG_OFF(f)		{6,2, f},
#define _FLAG_TOGGLE(f)		{6,4, f},

#define _PEN_UP				{7,0, 0},
#define _PEN_DOWN			{7,1, 0},
#define _PEN_TOGGLE			{7,2, 0},

#define _SET_TURTLE(x)		{8,0, x},

#define _REPEAT(n)			{10,0, n},
#define _END				{10,2, 0},


// Define the High-level Commands
#define FD(x)				_FD_FLY(x)
#define BK(x)				_FD_FLY(-x)

#define RT(x)				_RT(x)
#define LT(x)				_RT(-x)
#define SET_ANGLE(x)		_SET_ANGLE(x)
#define USE_CURRENT_ANGLE	_USE_CURRENT_ANGLE

#define EAST(x)				_MV_X_FLY(x)
#define WEST(x)				_MV_X_FLY(-x)
#define SET_X_POS(x)		_SET_X_FLY(x)

#define NORTH(y)			_MV_Y_FLY(y)
#define SOUTH(y)			_MV_Y_FLY(-y)
#define SET_Y_POS(y)		_SET_Y_FLY(y)

#define ALT_UP(z)			_MV_Z(z)
#define ALT_DOWN(z)			_MV_Z(-z)
#define SET_ALT(z)			_SET_Z(z)

#define FLAG_ON(f)			_FLAG_ON(f)
#define FLAG_OFF(f)			_FLAG_OFF(f)
#define FLAG_TOGGLE(f)		_FLAG_TOGGLE(f)

#define PEN_UP				_PEN_UP
#define PEN_DOWN			_PEN_DOWN
#define PEN_TOGGLE			_PEN_TOGGLE

#define SET_TURTLE(x)		_SET_TURTLE(x)

#define REPEAT(n)			_REPEAT(n)
#define REPEAT_FOREVER		_REPEAT(-1)
#define END					_END

#define SET_POS(x, y)		_SET_X(x) _SET_Y_FLY(y)
#define HOME				SET_ANGLE(0) SET_POS(0, 0)


#include "flightplan-logo.h"


#define NUM_INSTRUCTIONS (( sizeof instructions ) / sizeof ( struct logoInstructionDef ))
#define NUM_RTL_INSTRUCTIONS (( sizeof rtlInstructions ) / sizeof ( struct logoInstructionDef ))
int instructionIndex = 0 ;
int waypointIndex = 0 ; // used for telemetry

struct logoInstructionDef *currentInstructionSet = (struct logoInstructionDef*)instructions ;
int numInstructionsInCurrentSet = NUM_INSTRUCTIONS ;

struct repeatFrame { int countdown; int repeatInstructionIndex; } ;
struct repeatFrame repeatStack[REPEAT_STACK_DEPTH] ;
int repeatStackIndex = 0 ;

// These values are relative to the origin, and North
// x and y are in 16.16 fixed point
struct logoLocation { union longww x; union longww y; int z; } ;
struct logoLocation turtleLocations[2] ;
struct relative3D lastGoal = {0, 0, 0} ;

// Angles are stored as 0-359
int turtleAngles[2] = {0, 0} ;

unsigned char currentTurtle ;
unsigned char penState ;

void update_goal_from( struct relative3D old_waypoint ) ;
void process_instructions( void ) ;



// In the future, we could include more than 2 flight plans...
// flightplanNum is 0 for the main lgo instructions, and 1 for RTL instructions
void init_flight_plan ( int flightplanNum )
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
	repeatStackIndex = 0 ;
	currentTurtle = PLANE ;
	penState = 1 ;
	
	turtleLocations[PLANE].x._.W1 = GPSlocation.x ;
	turtleLocations[PLANE].y._.W1 = GPSlocation.y ;
	turtleLocations[PLANE].z = GPSlocation.z ;
	
	turtleLocations[CAMERA].x._.W1 = GPSlocation.x ;
	turtleLocations[CAMERA].y._.W1 = GPSlocation.y ;
	turtleLocations[CAMERA].z = GPSlocation.z ;
	
	// calculated_heading								// 0-255 (ccw, 0=East)
	int angle = (calculated_heading * 180 + 64) >> 7 ;	// 0-359 (ccw, 0=East)
	angle = -angle - 90;								// 0-359 (clockwise, 0=North)
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


struct absolute2D get_fixed_origin( void )
{
	struct absolute2D origin = FIXED_ORIGIN_LOCATION ;
	return origin ;
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


void process_instructions( void )
{
	int instructionsProcessed = 0 ;
	
	while (1)
	{
		char cmd = currentInstructionSet[instructionIndex].cmd ;
		char subcmd = currentInstructionSet[instructionIndex].subcmd ;
		int arg = currentInstructionSet[instructionIndex].arg ;
		
		switch (cmd)
		{
			case 1: // Forward/Back
				switch (subcmd)
				{
					case 0: // Forward
					case 1: // Forward and Fly
					{
						int cangle = turtleAngles[currentTurtle] ;			// 0-359 (clockwise, 0=North)
						signed char b_angle = (cangle * 182 + 128) >> 8 ;	// 0-255 (clockwise, 0=North)
						b_angle = -b_angle + 64 ;							// 0-255 (ccw, 0=East)
						
						turtleLocations[currentTurtle].x.WW += (__builtin_mulss(-cosine(b_angle), arg) << 2) ;
						turtleLocations[currentTurtle].y.WW += (__builtin_mulss(-sine(b_angle), arg) << 2) ;
						break ;
					}
				}
				break ;
			
			case 2: // Rotate
				switch (subcmd)
				{
					case 0: // Right
					{
						int angle = turtleAngles[currentTurtle] + arg ;
						while (angle < 0) angle += 360 ;
						angle = angle % 360 ;
						turtleAngles[currentTurtle] = angle ;
						break ;
					}
					case 2:
						turtleAngles[currentTurtle] = arg ;
						break ;
					case 4: // Use current angle
					{
						// calculated_heading								// 0-255 (ccw, 0=East)
						int angle = (calculated_heading * 180 + 64) >> 7 ;	// 0-359 (ccw, 0=East)
						angle = -angle - 90;								// 0-359 (clockwise, 0=North)
						turtleAngles[currentTurtle] = angle ;
						break ;
					}
				}
				break ;
			
			case 3: // X location (East/West)
				switch (subcmd)
				{
					case 0: // Move X
					case 1: // Move X and Fly
						turtleLocations[currentTurtle].x._.W1 += arg ;
						break ;
					case 2: // Set X location
					case 3: // Set X location and Fly
						turtleLocations[currentTurtle].x._.W0 = 0 ;
						turtleLocations[currentTurtle].x._.W1 = arg ;
						break ;
				}
				break ;
			
			case 4: // Y location (North/South)
				switch (subcmd)
				{
					case 0: // Move Y
					case 1: // Move Y and Fly
						turtleLocations[currentTurtle].y._.W1 += arg ;
						break ;
					case 2: // Set Y location
					case 3: // Set Y location and Fly
						turtleLocations[currentTurtle].y._.W0 = 0 ;
						turtleLocations[currentTurtle].y._.W1 = arg ;
						break ;
				}
				break ;
			
			case 5: // Z location / Altitude
				switch (subcmd)
				{
					case 0: // Move Z
					case 1: // Move Z and Fly
						turtleLocations[currentTurtle].z += arg ;
						break ;
					case 2: // Set Z location
					case 3: // Set Z location and Fly
						turtleLocations[currentTurtle].z = arg ;
						break ;
				}
				break ;
			
			case 6: // Flags
				switch (subcmd)
				{
					case 0: // Flag On
						setBehavior(desired_behavior.W | arg) ;
						break ;
					case 2: // Flag Off
						setBehavior(desired_behavior.W & ~arg) ;
						break ;
					case 4: // Flag Toggle
						setBehavior(desired_behavior.W ^ arg) ;
						break ;
				}
				break ;
			
			case 7: // Pen Up/Down
				switch (subcmd)
				{
					case 0: // Pen Up
						penState = PEN_UP_STATE ;
						break ;
					case 1: // Pen Down
						penState = PEN_DOWN_STATE ;
						break ;
					case 2: // Pen Toggle
						penState = !penState ;
						if (penState == PEN_DOWN_STATE) subcmd = 3; // Set the Fly Flag
						break ;
				}
				break ;
			
			case 8: // Set Turtle (choose plane or camera target)
				currentTurtle = (arg == CAMERA) ? CAMERA : PLANE ;
				break ;
			
			case 10: // Repeat
				switch (subcmd)
				{
					case 0: // Repeat N times (or forever if N == -1)
						if (repeatStackIndex < REPEAT_STACK_DEPTH)
						{
							repeatStack[repeatStackIndex].countdown = arg ;
							repeatStack[repeatStackIndex].repeatInstructionIndex = instructionIndex ;
							repeatStackIndex++ ;
						}
						break ;
					case 2: // End
						if (repeatStackIndex > 0)
						{
							repeatStackIndex-- ;
							
							if ( repeatStack[repeatStackIndex].countdown > 1 || repeatStack[repeatStackIndex].countdown == -1 )
							{
								if (repeatStack[repeatStackIndex].countdown != -1)
								{
									repeatStack[repeatStackIndex].countdown-- ;
								}
								instructionIndex = repeatStack[repeatStackIndex].repeatInstructionIndex ;
								repeatStackIndex++ ;
							}
						}
						break ;
				}
				break ;
		}
		
		instructionsProcessed++ ;
		instructionIndex++ ;
		if ( instructionIndex >= numInstructionsInCurrentSet ) instructionIndex = 0 ;
		
		if ( ((subcmd % 2) == 1 && penState == PEN_DOWN_STATE && currentTurtle == PLANE) || instructionsProcessed >= MAX_INSTRUCTIONS_PER_CYCLE)
			break ;
	}
	
	waypointIndex = instructionIndex - 1 ;
	
	update_goal_from(lastGoal) ;
	compute_bearing_to_goal() ;
	
	return ;
}

#endif
