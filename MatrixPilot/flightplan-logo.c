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
#if (FLIGHT_PLAN_TYPE == FP_LOGO)

#include "flightplan-logo.h"

#define PLANE				0
#define CAMERA				1


#define NUM_INSTRUCTIONS (( sizeof instructions ) / sizeof ( struct logoInstructionDef ))
#define NUM_RTL_INSTRUCTIONS (( sizeof rtlInstructions ) / sizeof ( struct logoInstructionDef ))
int16_t instructionIndex = 0 ;
int16_t waypointIndex = 0 ; // used for telemetry
int16_t absoluteHighWord = 0 ;
union longww absoluteXLong ;

struct logoInstructionDef *currentInstructionSet = (struct logoInstructionDef*)instructions ;
int16_t numInstructionsInCurrentSet = NUM_INSTRUCTIONS ;

// If we've processed this many instructions without commanding the plane to fly,
// then stop and continue on the next run through
#define MAX_INSTRUCTIONS_PER_CYCLE	32
int16_t instructionsProcessed = 0 ;

// Storage for command injection
struct logoInstructionDef logo_inject_instr ;
uint8_t logo_inject_pos = 0 ;
#define LOGO_INJECT_READY 255

// Storage for interrupt handling
int16_t interruptIndex = 0 ;		// intruction index of the beginning of the interrupt function
int8_t interruptStackBase = 0 ;	// stack depth when entering interrupt (clear interrupt when dropping below this depth)

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_LOGO == 1)
// instruction list loaded from remote connection
struct logoInstructionDef remoteInstructions[LOGO_REMOTE_MISSION_MAX][LOGO_REMOTE_INSTRUCTIONS_MAX_LENGTH];
#endif

// How many layers deep can Ifs, Repeats and Subroutines be nested
#define LOGO_STACK_DEPTH			12

struct logoStackFrame {
	uint16_t frameType				:  2 ;
	int16_t returnInstructionIndex	: 14 ;	// instructionIndex before the first instruction of the subroutine (a TO or REPEAT line, or -1 for MAIN)
	int16_t arg						: 16 ;
} ;
struct logoStackFrame logoStack[LOGO_STACK_DEPTH] ;
int16_t logoStackIndex = 0 ;

#define LOGO_FRAME_TYPE_IF			1
#define LOGO_FRAME_TYPE_REPEAT		2
#define LOGO_FRAME_TYPE_SUBROUTINE	3

#define LOGO_MAIN	0 	// Allows for DO(LOGO_MAIN) or EXEC(LOGO_MAIN) to start at the top


// These values are relative to the origin, and North
// x and y are in 16.16 fixed point
struct logoLocation { union longww x; union longww y; int16_t z; } ;
struct logoLocation turtleLocations[2] ;
struct relative3D lastGoal = {0, 0, 0} ;

// Angles are stored as 0-359
int16_t turtleAngles[2] = {0, 0} ;

uint8_t currentTurtle ;
int16_t penState ;

boolean process_one_instruction( struct logoInstructionDef instr ) ;
void update_goal_from( struct relative3D old_waypoint ) ;
void process_instructions( void ) ;



// In the future, we could include more than 2 flight plans...
// flightplanNum is 0 for the main lgo instructions, and 1 for RTL instructions
void init_flightplan(int16_t flightplanNum, int16_t mission, uint8_t startIndex)
{
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_LOGO == 1)
  int i;
#endif
  
	switch (flightplanNum)
  {
  case FP_HARDCODED: // Main instructions set
		currentInstructionSet = (struct logoInstructionDef*)instructions ;
		numInstructionsInCurrentSet = NUM_INSTRUCTIONS ;
  	instructionIndex = 0;
    break;

  case FP_RTL: // RTL instructions set
		currentInstructionSet = (struct logoInstructionDef*)rtlInstructions ;
		numInstructionsInCurrentSet = NUM_RTL_INSTRUCTIONS ;
  	instructionIndex = 0;
    break;

#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_LOGO == 1)
  case FP_REMOTE: // User-setable from remote connection. supports multiple flightplans
    if (mission >= LOGO_REMOTE_MISSION_MAX) // check for out-of-range
      return;
    for (i=0;i<LOGO_REMOTE_INSTRUCTIONS_MAX_LENGTH;i++)
    {
      // find the mission length (cmd count)
      if (remoteInstructions[mission][i].cmd == 0)
        break;
    }
    currentInstructionSet = (struct logoInstructionDef*)remoteInstructions[mission] ;
    numInstructionsInCurrentSet = i;
  	instructionIndex = startIndex;
    break;
#endif	
  default:
    return;
	}
	
	logoStackIndex = 0 ;
	logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_SUBROUTINE ;
	logoStack[logoStackIndex].arg = 0 ;
	logoStack[logoStackIndex].returnInstructionIndex = -1 ;  // When starting over, begin on instruction 0
	
	currentTurtle = PLANE ;
	penState = 0 ; // 0 means down.  more than 0 means up
	
	turtleLocations[PLANE].x._.W1 = IMUlocationx._.W1 ;
	turtleLocations[PLANE].y._.W1 = IMUlocationy._.W1 ;
	turtleLocations[PLANE].z = IMUlocationz._.W1 ;
	
	turtleLocations[CAMERA].x._.W1 = IMUlocationx._.W1 ;
	turtleLocations[CAMERA].y._.W1 = IMUlocationy._.W1 ;
	turtleLocations[CAMERA].z = IMUlocationz._.W1 ;
	
	// Calculate heading from Direction Cosine Matrix (rather than GPS), 
	// So that this code works when the plane is static. e.g. at takeoff
	struct relative2D curHeading ;
	curHeading.x = -rmat[1] ;
	curHeading.y = rmat[4] ;
	int8_t earth_yaw = rect_to_polar(&curHeading) ;//  (0=East,  ccw)
	int16_t angle = (earth_yaw * 180 + 64) >> 7 ;			//  (ccw, 0=East)
	angle = -angle + 90;								//  (clockwise, 0=North)
	turtleAngles[PLANE] = turtleAngles[CAMERA] = angle ;
	
	setBehavior( 0 ) ;
	
	struct relative3D IMUloc ;
	IMUloc.x = IMUlocationx._.W1 ;
	IMUloc.y = IMUlocationy._.W1 ;
	IMUloc.z = IMUlocationz._.W1 ;
	update_goal_from(IMUloc) ;
	
	interruptIndex = 0 ;
	interruptStackBase = 0 ;
	
	process_instructions() ;
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
	standardizedOrigin.z = (int32_t)(origin.z * 100) ;
	
	return standardizedOrigin ;
}


boolean logo_goal_has_moved( void )
{
	return (lastGoal.x != turtleLocations[PLANE].x._.W1 ||
			lastGoal.y != turtleLocations[PLANE].y._.W1 ||
			lastGoal.z != turtleLocations[PLANE].z);
}


void update_goal_from( struct relative3D old_goal )
{
	struct relative3D new_goal ;
	
	lastGoal.x = new_goal.x = (turtleLocations[PLANE].x._.W1) ;
	lastGoal.y = new_goal.y = (turtleLocations[PLANE].y._.W1) ;
	lastGoal.z = new_goal.z = turtleLocations[PLANE].z ;
	
	if (old_goal.x == new_goal.x && old_goal.y == new_goal.y)
	{
		old_goal.x = IMUlocationx._.W1 ;
		old_goal.y = IMUlocationy._.W1 ;
		old_goal.z = IMUlocationz._.W1 ;
	}

	set_goal( old_goal, new_goal ) ;
	
	new_goal.x = (turtleLocations[CAMERA].x._.W1) ;
	new_goal.y = (turtleLocations[CAMERA].y._.W1) ;
	new_goal.z = turtleLocations[CAMERA].z ;
	set_camera_view( new_goal ) ;
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
			if (logo_goal_has_moved()) {
				update_goal_from(lastGoal) ;
				compute_bearing_to_goal() ;
			}
		}
		logo_inject_pos = 0 ;
		
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
			update_goal_alt(turtleLocations[PLANE].z) ;
			lastGoal.z = turtleLocations[PLANE].z ;
		}
	}
	
 	// waypoint arrival is detected computing distance to the "finish line".
	// note: locations are measured in meters
	// locations have a range of +-32000 meters (20 miles) from origin
	
	if ( desired_behavior._.altitude )
	{
		if ( abs(IMUheight - goal.height) < ((int16_t) HEIGHT_MARGIN )) // reached altitude goal
		{
			process_instructions() ;
		}
	}
	else
	{
		if ( tofinish_line < WAYPOINT_RADIUS ) // crossed the finish line
		{
			process_instructions() ;
		}
	}
}


// For DO and EXEC, find the location of the given subroutine
int16_t find_start_of_subroutine(uint8_t subcmd)
{
	if (subcmd == 0) return -1; // subcmd 0 is reserved to always mean the start of the logo program

	int16_t i ;
	for (i = 0; i < numInstructionsInCurrentSet; i++)
	{
		if (currentInstructionSet[i].cmd == 1 && currentInstructionSet[i].subcmd == 2 && currentInstructionSet[i].arg == subcmd)
		{
			return i ;
		}
	}
	return -1 ;
}


// When an IF condition was false, use this to skip to ELSE or END
// When an IF condition was true, and we ran the block, and reach an ELSE, skips to the END
uint16_t find_end_of_current_if_block( void )
{
	int16_t i ;
	int16_t nestedDepth = 0 ;
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
int16_t get_current_stack_parameter_frame_index( void )
{
	int16_t i ;
	for (i = logoStackIndex; i >= 0; i--)
	{
		if (logoStack[i].frameType == LOGO_FRAME_TYPE_SUBROUTINE)
		{
			return i ;
		}
	}
	return 0 ;
}


int16_t get_current_angle( void )
{
	// Calculate heading from Direction Cosine Matrix (rather than GPS), 
	// So that this code works when the plane is static. e.g. at takeoff
	struct relative2D curHeading ;
	curHeading.x = -rmat[1] ;
	curHeading.y = rmat[4] ;
	int8_t earth_yaw = rect_to_polar(&curHeading) ;// (0=East,  ccw)
	int16_t angle = (earth_yaw * 180 + 64) >> 7 ;			// (ccw, 0=East)
	angle = -angle + 90;								// (clockwise, 0=North)
	return angle ;
}


int16_t get_angle_to_point( int16_t x, int16_t y )
{
	struct relative2D vectorToGoal;
	vectorToGoal.x = turtleLocations[currentTurtle].x._.W1 - x ;
	vectorToGoal.y = turtleLocations[currentTurtle].y._.W1 - y ;
	int8_t dir_to_goal = rect_to_polar ( &vectorToGoal ) ;
	
	// dir_to_goal										// 0-255 (ccw, 0=East)
	int16_t angle = (dir_to_goal * 180 + 64) >> 7 ;			// 0-359 (ccw, 0=East)
	angle = -angle + 90;								// 0-359 (clockwise, 0=North)
	return angle ;
}


int16_t logo_value_for_identifier(uint8_t ident)
{
	if (ident > 0 && ident <= NUM_INPUTS)
	{
		return udb_pwIn[(int16_t)ident] ; // 2000 - 4000
	}
	
	switch (ident) {
		case DIST_TO_HOME: // in m
			return sqrt_long(IMUlocationx._.W1 * (int32_t)IMUlocationx._.W1 + IMUlocationy._.W1 * (int32_t)IMUlocationy._.W1) ;

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
			int16_t angle = get_current_angle() - get_angle_to_point(0, 0) ;
			if (angle < -180) angle += 360 ;
			if (angle >= 180) angle -= 360 ;
			return angle ;
		}
		case REL_ANGLE_TO_GOAL: // in degrees. -180-179 (0=heading directly towards goal. clockwise offset is positive)
		{
			int16_t angle = get_current_angle() - get_angle_to_point(IMUlocationx._.W1, IMUlocationy._.W1) ;
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
			return sqrt_long(estimatedWind[0] * (int32_t)estimatedWind[0] + estimatedWind[1] * (int32_t)estimatedWind[1]) ;

		case WIND_SPEED_X: // in cm/s
			return estimatedWind[0] ;

		case WIND_SPEED_Y: // in cm/s
			return estimatedWind[1] ;

		case WIND_SPEED_Z: // in cm/s
			return estimatedWind[2] ;

		case PARAM:
		{
			int16_t ind = get_current_stack_parameter_frame_index() ;
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
		int16_t ind = get_current_stack_parameter_frame_index() ;
		instr.arg *= logoStack[ind].arg ;
	}
	
	switch (instr.cmd)
	{
		case 1: // Repeat
			switch (instr.subcmd)
			{
				case 0: // Repeat N times (or forever if N == -1)
					if (logoStackIndex < LOGO_STACK_DEPTH-1)
					{
						logoStackIndex++ ;
						logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_REPEAT ;
						logoStack[logoStackIndex].arg = instr.arg ;
						logoStack[logoStackIndex].returnInstructionIndex = instructionIndex ;
					}
					break ;
				case 1: // End
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
				
				case 3: // Else
					if ( logoStack[logoStackIndex].frameType == LOGO_FRAME_TYPE_IF )
					{
						instructionIndex = find_end_of_current_if_block() ;
						logoStackIndex-- ;
					}
					break ;
				
				case 2: // To (define a function)
				{
					// Shouldn't ever run these lines.
					// If we do get here, restart from the top of the logo program.
					instructionIndex = logoStack[0].returnInstructionIndex ;
					logoStackIndex = 0 ;
					interruptStackBase = 0;
				}
				break ;
			}
			break ;
		
		
		case 10: // Exec (reset the stack and then call a subroutine)
			instructionIndex = find_start_of_subroutine(instr.subcmd) ;
			logoStack[0].returnInstructionIndex = instructionIndex ;
			logoStackIndex = 0 ;
			interruptStackBase = 0;
			break ;
		
		case 2: // Do (call a subroutine)
			if (logoStackIndex < LOGO_STACK_DEPTH-1)
			{
				logoStackIndex++ ;
				logoStack[logoStackIndex].frameType = LOGO_FRAME_TYPE_SUBROUTINE ;
				logoStack[logoStackIndex].arg = instr.arg ;
				logoStack[logoStackIndex].returnInstructionIndex = instructionIndex ;
			}
			instructionIndex = find_start_of_subroutine(instr.subcmd) ;
			break ;
		
		
		case 3: // Forward/Back
			switch (instr.subcmd)
			{
				case 0: // Forward
				{
					int16_t cangle = turtleAngles[currentTurtle] ;			// 0-359 (clockwise, 0=North)
					int8_t b_angle = (cangle * 182 + 128) >> 8 ;	// 0-255 (clockwise, 0=North)
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
					int16_t angle = turtleAngles[currentTurtle] + instr.arg ;
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
					turtleAngles[currentTurtle] = get_current_angle() ;
					break ;
				}
				case 3: // Use angle to goal
				{
					turtleAngles[currentTurtle] = get_angle_to_point(IMUlocationx._.W1, IMUlocationy._.W1) ;
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
					turtleLocations[currentTurtle].x._.W1 = IMUlocationx._.W1 ;
					turtleLocations[currentTurtle].y._.W0 = 0 ;
					turtleLocations[currentTurtle].y._.W1 = IMUlocationy._.W1 ;
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
		
		case 9: // Modify PARAM
			switch (instr.subcmd)
			{
				case 0: // Set param
				{
					int16_t ind = get_current_stack_parameter_frame_index() ;
					logoStack[ind].arg = instr.arg ;
					break ;
				}
				case 1: // Add to param
				{
					int16_t ind = get_current_stack_parameter_frame_index() ;
					logoStack[ind].arg += instr.arg ;
					break ;
				}
				case 2: // Multiply param
				{
					int16_t ind = get_current_stack_parameter_frame_index() ;
					logoStack[ind].arg *= instr.arg ;
					break ;
				}
				case 3: // Divide param
				{
					int16_t ind = get_current_stack_parameter_frame_index() ;
					if (instr.arg != 0) // Avoid divide by 0!
					{
						logoStack[ind].arg /= instr.arg ;
					}
					break ;
				}
				break ;
			}
		
		case 11: // Speed
#if ( SPEED_CONTROL == 1)
			switch (instr.subcmd)
			{
				case 0: // Increase Speed
					desiredSpeed += instr.arg * 10 ;
					break ;
				case 1: // Set Speed
					desiredSpeed = instr.arg * 10 ;
					break ;
			}
			if (desiredSpeed < 0) desiredSpeed = 0 ;
#endif
			break ;
		
		case 12: // Interrupts
			switch (instr.subcmd) {
				case 1: // Set
					interruptIndex = find_start_of_subroutine(instr.arg) ;
					break ;
					
				case 0: // Clear
					interruptIndex = 0 ;
					break ;
			}
			break;
		
		case 13: // Load to PARAM
		{
			int16_t ind = get_current_stack_parameter_frame_index() ;
			logoStack[ind].arg = logo_value_for_identifier(instr.subcmd) ;
			break ;
		}
		
		case 14: // IF commands
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		{
			int16_t val = logo_value_for_identifier(instr.subcmd) ;
			boolean condTrue = false ;
			
			if (instr.cmd == 14 && val == instr.arg) condTrue = true ;		// IF_EQ
			else if (instr.cmd == 15 && val != instr.arg) condTrue = true ;	// IF_NE
			else if (instr.cmd == 16 && val > instr.arg) condTrue = true ;	// IF_GT
			else if (instr.cmd == 17 && val < instr.arg) condTrue = true ;	// IF_LT
			else if (instr.cmd == 18 && val >= instr.arg) condTrue = true ;	// IF_GE
			else if (instr.cmd == 19 && val <= instr.arg) condTrue = true ;	// IF_LE
			
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
	
	if (logo_goal_has_moved()) {
		update_goal_from(lastGoal) ;
		compute_bearing_to_goal() ;
	}
}


void flightplan_live_begin( void )
{
	logo_inject_pos = 0 ;
}


void flightplan_live_received_byte( uint8_t inbyte )
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
}


#endif
