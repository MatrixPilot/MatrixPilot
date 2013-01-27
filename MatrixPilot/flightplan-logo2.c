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


#include "defines.h"

#if (FLIGHT_PLAN_TYPE == FP_LOGO2)

#include "logo2.h"
#include "flightplan-logo2.h"

#define NUM_INSTRUCTIONS (( sizeof instructions ) / sizeof ( struct logoInstructionDef ))
#define NUM_RTL_INSTRUCTIONS (( sizeof rtlInstructions ) / sizeof ( struct logoInstructionDef ))
int instructionIndex = 0 ;
int waypointIndex = 0 ; // used for telemetry
int absoluteHighWord = 0 ;
union longww absoluteXLong ;

struct logoInstructionDef *currentInstructionSet = (struct logoInstructionDef*)instructions ;
int numInstructionsInCurrentSet = NUM_INSTRUCTIONS ;

// If we've processed this many instructions without commanding the plane to fly,
// then stop and continue on the next run through
#define MAX_INSTRUCTIONS_PER_CYCLE	32
int instructionsProcessed = 0 ;

// Storage for command injection
struct logoInstructionDef logoInstructionBuffer[LOGO_INSTRUCTION_BUFFER_SIZE];
unsigned int instrBufferFillCount = 0;

enum
{
	LOGO_INJECT_BUFFER_EMPTY,
	LOGO_INJECT_READY,
};

unsigned char logo_inject_instructions = LOGO_INJECT_BUFFER_EMPTY ;

// Storage for interrupt handling
int interruptIndex = 0 ;		// intruction index of the beginning of the interrupt function
char interruptStackBase = 0 ;	// stack depth when entering interrupt (clear interrupt when dropping below this depth)


// How many layers deep can Ifs, Repeats and Subroutines be nested
#define LOGO_STACK_DEPTH			12

struct logoStackFrame {
	unsigned int frameType				:  2 ;
	unsigned int returnInstructionIndex	: 14 ;	// instructionIndex before the first instruction of the subroutine (a TO or REPEAT line, or -1 for MAIN)
	int arg								: 16 ;
} ;

struct logoStackFrame logoStack[LOGO_STACK_DEPTH] ;
int logoStackIndex = 0 ;

#define LOGO_FRAME_TYPE_IF			1
#define LOGO_FRAME_TYPE_REPEAT		2
#define LOGO_FRAME_TYPE_SUBROUTINE	3

#define LOGO_MAIN	0 	// Allows for DO(LOGO_MAIN) or EXEC(LOGO_MAIN) to start at the top


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
	switch( flightplanNum )
	{
	case LOGO_FLIGHTPLAN_MAIN:
		currentInstructionSet = (struct logoInstructionDef*)instructions ;
		numInstructionsInCurrentSet = NUM_INSTRUCTIONS ;
		break;
	case LOGO_FLIGHTPLAN_RTL:
		currentInstructionSet = (struct logoInstructionDef*)rtlInstructions ;
		numInstructionsInCurrentSet = NUM_RTL_INSTRUCTIONS ;
		break;
//	case LOGO_FLIGHTPLAN_RAM:
//		currentInstructionSet = logoInstructionBuffer;
//		numInstructionsInCurrentSet = instrBufferFillCount ;
//		break;
	};

	
	instructionIndex = 0 ;
	
	logoStackIndex = 0 ;
	logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_SUBROUTINE ;
	logoStack[logoStackIndex].arg = 0 ;
	logoStack[logoStackIndex].returnInstructionIndex = -1 ;  // When starting over, begin on instruction 0
	
	currentTurtle = PLANE ;
	penState = 0 ; // 0 means down.  more than 0 means up
	
	turtleLocations[PLANE].x._.W1 = GPSlocation.x ;
	turtleLocations[PLANE].y._.W1 = GPSlocation.y ;
	turtleLocations[PLANE].z = GPSlocation.z ;
	
	turtleLocations[CAMERA].x._.W1 = GPSlocation.x ;
	turtleLocations[CAMERA].y._.W1 = GPSlocation.y ;
	turtleLocations[CAMERA].z = GPSlocation.z ;
	
	// Calculate heading from Direction Cosine Matrix (rather than GPS), 
	// So that this code works when the plane is static. e.g. at takeoff
	struct relative2D curHeading ;
	curHeading.x = -rmat[1] ;
	curHeading.y = rmat[4] ;
	signed char earth_yaw = rect_to_polar(&curHeading) ;//  (0=East,  ccw)
	int angle = (earth_yaw * 180 + 64) >> 7 ;			//  (ccw, 0=East)
	angle = -angle + 90;								//  (clockwise, 0=North)
	turtleAngles[PLANE] = turtleAngles[CAMERA] = angle ;
	
	setBehavior( 0 ) ;
	
	update_goal_from(GPSlocation) ;
	
	interruptIndex = 0 ;
	interruptStackBase = 0 ;
	
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
		if (logo_inject_instr.cmd == 2 || logo_inject_instr.cmd == 10) // DO / EXEC
		{
			instructionIndex++ ;
			process_instructions() ;
		}
		else
		{
			update_goal_from(lastGoal) ;
			compute_bearing_to_goal() ;
		}
		logo_inject_pos = LOGO_INJECT_BUFFER_EMPTY ;
		
		return ;
	}
	
	// otherwise run the interrupt handler, if configured, and not in-progress
	if (interruptIndex && !interruptStackBase)
	{
		if (logoStackIndex < LOGO_STACK_DEPTH-1)
		{
			logoStackIndex++ ;
			logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_SUBROUTINE ;
			logoStack[logoStackIndex].arg = 0 ;
			logoStack[logoStackIndex].returnInstructionIndex = instructionIndex-1 ;
			instructionIndex = interruptIndex+1 ;
			interruptStackBase = logoStackIndex ;
			process_instructions() ;
		}
	}
	
 	// waypoint arrival is detected computing distance to the "finish line".
	// note: locations are measured in meters
	// locations have a range of +-32000 meters (20 miles) from origin
	
	if ( desired_behavior._.altitude )
	{
		if ( abs(IMUheight - goal.height) < ((int) HEIGHT_MARGIN )) // reached altitude goal
		{
			process_instructions() ;
		}
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


// For DO and EXEC, find the location of the given subroutine
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


// When an IF condition was false, use this to skip to ELSE or END
// When an IF condition was true, and we ran the block, and reach an ELSE, skips to the END
unsigned int find_end_of_current_if_block( void )
{
	int i ;
	int nestedDepth = 0 ;
	for (i = instructionIndex+1; i < numInstructionsInCurrentSet; i++)
	{
		if (currentInstructionSet[i].cmd == 1 && currentInstructionSet[i].subcmd == 0) nestedDepth++ ; // into a REPEAT
		else if (currentInstructionSet[i].cmd >= 14 && currentInstructionSet[i].cmd <= 19 ) nestedDepth++ ; // into an IF
		else if (nestedDepth > 0 && currentInstructionSet[i].cmd == 1 && currentInstructionSet[i].subcmd == 1) nestedDepth-- ; // nested END
		else if ( nestedDepth == 0 && currentInstructionSet[i].cmd == 1 && (currentInstructionSet[i].subcmd == 1 || currentInstructionSet[i].subcmd == 3))
		{
			// This is the target ELSE or END
			return i ;
		}
	}
	return 0 ;
}


// Referencing PARAM in a LOGO program uses the PARAM from the current subroutine frame, even if
// we're also nested deeper inside of IF or REPEAT frames.  This finds the current subroutine's frame.
int get_current_stack_parameter_frame_index( void )
{
	int i ;
	for (i = logoStackIndex; i >= 0; i--)
	{
		if (logoStack[i].frameType == LOGO_FRAME_TYPE_SUBROUTINE)
		{
			return i ;
		}
	}
	return 0 ;
}


int get_current_angle( void )
{
	// Calculate heading from Direction Cosine Matrix (rather than GPS), 
	// So that this code works when the plane is static. e.g. at takeoff
	struct relative2D curHeading ;
	curHeading.x = -rmat[1] ;
	curHeading.y = rmat[4] ;
	signed char earth_yaw = rect_to_polar(&curHeading) ;// (0=East,  ccw)
	int angle = (earth_yaw * 180 + 64) >> 7 ;			// (ccw, 0=East)
	angle = -angle + 90;								// (clockwise, 0=North)
	return angle ;
}


int get_angle_to_point( int x, int y )
{
	struct relative2D vectorToGoal;
	vectorToGoal.x = turtleLocations[currentTurtle].x._.W1 - x ;
	vectorToGoal.y = turtleLocations[currentTurtle].y._.W1 - y ;
	signed char dir_to_goal = rect_to_polar ( &vectorToGoal ) ;
	
	// dir_to_goal										// 0-255 (ccw, 0=East)
	int angle = (dir_to_goal * 180 + 64) >> 7 ;			// 0-359 (ccw, 0=East)
	angle = -angle + 90;								// 0-359 (clockwise, 0=North)
	return angle ;
}


int logo_value_for_identifier(char ident)
{
	if (ident > 0 && ident <= NUM_INPUTS)
	{
		return udb_pwIn[(int)ident] ; // 2000 - 4000
	}
	
	switch (ident) {
		case DIST_TO_HOME: // in m
			return sqrt_long(IMUlocationx._.W1 * (long)IMUlocationx._.W1 + IMUlocationy._.W1 * (long)IMUlocationy._.W1) ;

		case DIST_TO_GOAL: // in m
			return tofinish_line ;

		case ALT: // in m
			return IMUlocationz._.W1 ;

		case CURRENT_ANGLE: // in degrees. 0-359 (clockwise, 0=North)
			return get_current_angle() ;

		case ANGLE_TO_HOME: // in degrees. 0-359 (clockwise, 0=North)
			return get_angle_to_point(0, 0) ;

		case ANGLE_TO_GOAL: // in degrees. 0-359 (clockwise, 0=North)
			return get_angle_to_point(IMUlocationx._.W1, IMUlocationy._.W1) ;

		case REL_ANGLE_TO_HOME: // in degrees. -180-179 (0=heading directly towards home. clockwise offset is positive)
		{
			int angle = get_current_angle() - get_angle_to_point(0, 0) ;
			if (angle < -180) angle += 360 ;
			if (angle >= 180) angle -= 360 ;
			return angle ;
		}
		case REL_ANGLE_TO_GOAL: // in degrees. -180-179 (0=heading directly towards goal. clockwise offset is positive)
		{
			int angle = get_current_angle() - get_angle_to_point(IMUlocationx._.W1, IMUlocationy._.W1) ;
			if (angle < -180) angle += 360 ;
			if (angle >= 180) angle -= 360 ;
			return angle ;
		}
		case GROUND_SPEED: // in cm/s
			return ground_velocity_magnitudeXY ;

		case AIR_SPEED: // in cm/s
			return air_speed_magnitudeXY ;

		case AIR_SPEED_Z: // in cm/s
			return IMUvelocityz._.W1 - estimatedWind[2] ;

		case WIND_SPEED: // in cm/s
			return sqrt_long(estimatedWind[0] * (long)estimatedWind[0] + estimatedWind[1] * (long)estimatedWind[1]) ;

		case WIND_SPEED_X: // in cm/s
			return estimatedWind[0] ;

		case WIND_SPEED_Y: // in cm/s
			return estimatedWind[1] ;

		case WIND_SPEED_Z: // in cm/s
			return estimatedWind[2] ;

		case PARAM:
		{
			int ind = get_current_stack_parameter_frame_index() ;
			return logoStack[ind].arg ;
		}
	}
	
	return 0 ;
}


boolean process_one_instruction( struct logoInstructionDef instr )
{
	if (instr.use_param)
	{
		// Use the subroutine's parameter instead of the instruction's arg value
		int ind = get_current_stack_parameter_frame_index() ;
		instr.arg *= logoStack[ind].arg ;
	}
	
	switch (instr.cmd)
	{
		case LOGO_CMD_REPEAT: // Repeat
					if (logoStackIndex < LOGO_STACK_DEPTH-1)
					{
						logoStackIndex++ ;
						logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_REPEAT ;
						logoStack[logoStackIndex].arg = instr.arg ;
						logoStack[logoStackIndex].returnInstructionIndex = instructionIndex ;
					}
			break ;
		case LOGO_CMD_END: // End
					if (logoStackIndex > 0)
					{
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
							}
							else
							{
								logoStackIndex-- ;								
							}
						}
						else if ( logoStack[logoStackIndex].frameType == LOGO_FRAME_TYPE_SUBROUTINE )
						{
							// END SUBROUTINE
							instructionIndex = logoStack[logoStackIndex].returnInstructionIndex ;
							logoStackIndex-- ;
							if (logoStackIndex < interruptStackBase)
							{
								interruptStackBase = 0 ;
								instructionsProcessed = MAX_INSTRUCTIONS_PER_CYCLE ; // stop processing instructions after finishing interrupt
							}
						}
						else if ( logoStack[logoStackIndex].frameType == LOGO_FRAME_TYPE_IF )
						{
							// Do nothing at the end of an IF block
							logoStackIndex-- ;
						}
					}
					else
					{
						// Extra, unmatched END goes back to the start of the program
						instructionIndex = logoStack[0].returnInstructionIndex ;
						logoStackIndex = 0 ;
						interruptStackBase = 0;
					}
					break ;
				
		case LOGO_CMD_ELSE: // Else
					if ( logoStack[logoStackIndex].frameType == LOGO_FRAME_TYPE_IF )
					{
						instructionIndex = find_end_of_current_if_block() ;
						logoStackIndex-- ;
					}
			break ;
				
		case LOGO_CMD_TO: // To (define a function)
				{
					// Shouldn't ever run these lines.
					// If we do get here, restart from the top of the logo program.
					instructionIndex = logoStack[0].returnInstructionIndex ;
					logoStackIndex = 0 ;
					interruptStackBase = 0;
				}
			break ;

		
		
		case LOGO_CMD_EXEC: // Exec (reset the stack and then call a subroutine)
			if (instr.subcmd == 0)		// subcmd 0 is reserved to always mean the start of the logo program
			{
				instructionIndex = -1 ;
			}
			else
			{
				instructionIndex = find_start_of_subroutine(instr.subcmd) ;
			}
			logoStack[0].returnInstructionIndex = instructionIndex ;
			logoStackIndex = 0 ;
			interruptStackBase = 0;
			break ;
		
		case LOGO_CMD_DO: // Do (call a subroutine)
			if (logoStackIndex < LOGO_STACK_DEPTH-1)
			{
				logoStackIndex++ ;
				logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_SUBROUTINE ;
				logoStack[logoStackIndex].arg = instr.arg ;
				logoStack[logoStackIndex].returnInstructionIndex = instructionIndex ;
			}
			if (instr.subcmd == 0)		// subcmd 0 is reserved to always mean the start of the logo program
			{
				instructionIndex = -1 ;
			}
			else
			{
				instructionIndex = find_start_of_subroutine(instr.subcmd) ;
			}
			break ;
		
		
		case LOGO_CMD_FD: // Forward/Back
				{
					int cangle = turtleAngles[currentTurtle] ;			// 0-359 (clockwise, 0=North)
					signed char b_angle = (cangle * 182 + 128) >> 8 ;	// 0-255 (clockwise, 0=North)
					b_angle = -b_angle - 64 ;							// 0-255 (ccw, 0=East)
					
					turtleLocations[currentTurtle].x.WW += (__builtin_mulss(-cosine(b_angle), instr.arg) << 2) ;
					turtleLocations[currentTurtle].y.WW += (__builtin_mulss(-sine(b_angle), instr.arg) << 2) ;
				}
				break ;
		
		case LOGO_CMD_RT: // Right
				{
					int angle = turtleAngles[currentTurtle] + instr.arg ;
					while (angle < 0) angle += 360 ;
					angle = angle % 360 ;
					turtleAngles[currentTurtle] = angle ;
					break ;
				}
		case LOGO_CMD_SET_ANGLE: // Set Angle
					turtleAngles[currentTurtle] = instr.arg ;
					break ;
		case LOGO_CMD_USE_CURRENT_ANGLE: // Use current angle
				{
					turtleAngles[currentTurtle] = get_current_angle() ;
					break ;
				}
		case LOGO_CMD_USE_ANGLE_TO_GOAL: // Use angle to goal
				{
					turtleAngles[currentTurtle] = get_angle_to_point(IMUlocationx._.W1, IMUlocationy._.W1) ;
					break ;
				}

		
		case LOGO_CMD_MV_X: // Move X
					turtleLocations[currentTurtle].x._.W1 += instr.arg ;
					break ;
		case LOGO_CMD_SET_X: // Set X location
					turtleLocations[currentTurtle].x._.W0 = 0 ;
					turtleLocations[currentTurtle].x._.W1 = instr.arg ;
					break ;
		case LOGO_CMD_MV_Y: // Move Y
					turtleLocations[currentTurtle].y._.W1 += instr.arg ;
					break ;
		case LOGO_CMD_SET_Y: // Set Y location
					turtleLocations[currentTurtle].y._.W0 = 0 ;
					turtleLocations[currentTurtle].y._.W1 = instr.arg ;
					break ;
		case LOGO_CMD_MV_Z: // Move Z
					turtleLocations[currentTurtle].z += instr.arg ;
					break ;
		case LOGO_CMD_SET_Z: // Set Z location
					turtleLocations[currentTurtle].z = instr.arg ;
					break ;
		case LOGO_CMD_USE_CURRENT_POS: // Use current position (for x and y)
					turtleLocations[currentTurtle].x._.W0 = 0 ;
					turtleLocations[currentTurtle].x._.W1 = GPSlocation.x ;
					turtleLocations[currentTurtle].y._.W0 = 0 ;
					turtleLocations[currentTurtle].y._.W1 = GPSlocation.y ;
					break ;
		case LOGO_CMD_HOME: // HOME
					turtleAngles[currentTurtle] = 0 ;
					turtleLocations[currentTurtle].x.WW = 0 ;
					turtleLocations[currentTurtle].y.WW = 0 ;
					break ;
		case LOGO_CMD_SET_ABS_VAL_HIGH: // Absolute set high value
					absoluteHighWord = instr.arg ;
					break ;
		case LOGO_CMD_SET_ABS_X_LOW: // Absolute set low X value
				{
					absoluteXLong._.W1 = absoluteHighWord ;
					absoluteXLong._.W0 = instr.arg ;
					break ;
				}
		case LOGO_CMD_SET_ABS_Y_LOW: // Absolute set low Y value
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

		case LOGO_CMD_SET_ABS_X_Y:
			{
					struct waypoint3D wp ;

					absoluteXLong._.W1 = instr.arg ;
					absoluteXLong._.W0 = instr.arg2 ;

					union longww absoluteYLong ;
					absoluteYLong._.W1 = instr.arg3;
					absoluteYLong._.W0 = instr.arg4 ;
					
					wp.x = absoluteXLong.WW ;
					wp.y = absoluteYLong.WW ;
					wp.z = 0 ;
					struct relative3D rel = dcm_absolute_to_relative(wp) ;
					turtleLocations[currentTurtle].x._.W0 = 0 ;
					turtleLocations[currentTurtle].x._.W1 = rel.x ;
					turtleLocations[currentTurtle].y._.W0 = 0 ;
					turtleLocations[currentTurtle].y._.W1 = rel.y ;
				break;
			}

		case LOGO_CMD_FLAG_ON: // Flag On
					setBehavior(desired_behavior.W | instr.arg) ;
					break ;
		case LOGO_CMD_FLAG_OFF: // Flag Off
					setBehavior(desired_behavior.W & ~instr.arg) ;
					break ;
		case LOGO_CMD_FLAG_TOGGLE: // Flag Toggle
					setBehavior(desired_behavior.W ^ instr.arg) ;
					break ;

		

		case LOGO_CMD_PEN_UP: // Pen Up
					penState++ ;
					break ;
		case LOGO_CMD_PEN_DOWN: // Pen Down
					if (penState > 0)
						penState-- ;
					break ;
		case LOGO_CMD_PEN_TOGGLE: // Pen Toggle
					penState = (penState == 0) ;
					if (penState == 0) instr.do_fly = 1; // Set the Fly Flag
					break ;

		
		case LOGO_CMD_SET_TURTLE: // Set Turtle (choose plane or camera target)
			currentTurtle = (instr.arg == CAMERA) ? CAMERA : PLANE ;
			break ;
		

		case LOGO_CMD_PARAM_SET: // Set param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					logoStack[ind].arg = instr.arg ;
					break ;
				}
		case LOGO_CMD_PARAM_ADD: // Add to param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					logoStack[ind].arg += instr.arg ;
					break ;
				}
		case LOGO_CMD_PARAM_MUL: // Multiply param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					logoStack[ind].arg *= instr.arg ;
					break ;
				}
		case LOGO_CMD_PARAM_DIV: // Divide param
				{
					int ind = get_current_stack_parameter_frame_index() ;
					if (instr.arg != 0) // Avoid divide by 0!
					{
						logoStack[ind].arg /= instr.arg ;
					}
					break ;
				}

#if ( SPEED_CONTROL == 1)

		case LOGO_CMD_SPEED_INCREASE: // Increase Speed
					desiredSpeed += instr.arg * 10 ;
					if (desiredSpeed < 0) desiredSpeed = 0 ;
					break ;
		case LOGO_CMD_SET_SPEED: // Set Speed
					desiredSpeed = instr.arg * 10 ;			
					if (desiredSpeed < 0) desiredSpeed = 0 ;
					break ;
#endif

		

		case LOGO_CMD_SET_INTERRUPT: // Set
					interruptIndex = find_start_of_subroutine(instr.arg) ;
					break ;
					
		case LOGO_CMD_CLEAR_INTERRUPT: // Clear
					interruptIndex = 0 ;
					break ;
		
		case LOGO_CMD_LOAD_TO_PARAM: // Load to PARAM
		{
			int ind = get_current_stack_parameter_frame_index() ;
			logoStack[ind].arg = logo_value_for_identifier(instr.subcmd) ;
			break ;
		}
		
		case 	LOGO_CMD_IF_EQ: // IF commands
		case 	LOGO_CMD_IF_NE:
		case 	LOGO_CMD_IF_GT:
		case 	LOGO_CMD_IF_LT:
		case 	LOGO_CMD_IF_GE:
		case 	LOGO_CMD_IF_LE:

		{
			int val = logo_value_for_identifier(instr.subcmd) ;
			boolean condTrue = false ;
			
			switch (instr.cmd)
			{
			case LOGO_CMD_IF_EQ:
				if (val == instr.arg) condTrue = true ;		// IF_EQ
				break;
			case LOGO_CMD_IF_NE:
				if (val != instr.arg) condTrue = true ;		// IF_EQ
				break;
			case LOGO_CMD_IF_GT:
				if (val > instr.arg) condTrue = true ;		// IF_EQ
				break;
			case LOGO_CMD_IF_LT:
				if (val < instr.arg) condTrue = true ;		// IF_EQ
				break;
			case LOGO_CMD_IF_GE:
				if (val >= instr.arg) condTrue = true ;		// IF_EQ
				break;
			case LOGO_CMD_IF_LE:
				if (val <= instr.arg) condTrue = true ;		// IF_EQ
				break;
			default:
				condTrue = false;
			}

			if (condTrue) {
				if (logoStackIndex < LOGO_STACK_DEPTH-1)
				{
					logoStackIndex++ ;
					logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_IF ;
				}
			}
			else {
				// jump to the matching END or ELSE
				instructionIndex = find_end_of_current_if_block() ;
				if (currentInstructionSet[instructionIndex].subcmd == 3) // is entering an ELSE block
				{
					if (logoStackIndex < LOGO_STACK_DEPTH-1)
					{
						logoStackIndex++ ;
						logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_IF ;
					}
				}
			}
			break ;
		}
	}
	return instr.do_fly ;
}


void process_instructions( void )
{
	instructionsProcessed = 0 ;
	
	while (1)
	{
		boolean do_fly = process_one_instruction( currentInstructionSet[instructionIndex] ) ;
		
		instructionsProcessed++ ;
		instructionIndex++ ;
		if ( instructionIndex >= numInstructionsInCurrentSet ) instructionIndex = 0 ;
		
		if ( do_fly && penState == 0 && currentTurtle == PLANE )
			break ;
		
		if ( instructionsProcessed >= MAX_INSTRUCTIONS_PER_CYCLE )
			return ;  // don't update goal if we didn't hit a FLY command
	}
	
	waypointIndex = instructionIndex - 1 ;
	
	update_goal_from(lastGoal) ;
	compute_bearing_to_goal() ;
	
	return ;
}


void flightplan_live_begin( void )
{
	return ;
}


void flightplan_live_received_byte( unsigned char inbyte )
{
	return ;
}


void flightplan_live_commit( void )
{
	return ;
}


#endif
