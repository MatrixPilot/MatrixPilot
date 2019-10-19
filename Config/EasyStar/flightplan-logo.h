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


////////////////////////////////////////////////////////////////////////////////
// UDB LOGO Flight Planning definitions
// 
// The UDB Logo flight plan language lets you use a language similar to Logo, aka Turtle graphics, to
// control your plane.  You are commanding an imaginary "turtle" to move to specific locations, and the
// plane will head towards the turtle.
// 
// You can also control the camera targeting code by switching from the plane turtle, to the camera turtle
// by using the SET_TURTLE(CAMERA) command.  Then logo commands will move the location that the camera
// is targeting, instead of the location to aim the plane.
// 
// Each time you enter waypoint mode, the state is reset and your logo program starts from the top.  If
// you enter RTL mode, the state is reset and your RTL logo program is run instead.
// The following state is cleared when entering waypoint mode or RTL mode: (but not when your program
// ends and starts over)
//   - The plane turtle and camera turtle begin at the plane's current position and altitude.
//   - Both turtles begin pointing in the plane's current heading.
//   - The flags are all turned off.
//   - The pen is down, and the PLANE turtle is active.
// 
// To use UDB Logo, set FLIGHT_PLAN_TYPE to FP_LOGO in options.h.

//NOTE: WAYPOINT_PROXIMITY_RADIUS, USE_FIXED_ORIGIN, FIXED_ORIGIN_LOCATION are now defined in options.h
////////////////////////////////////////////////////////////////////////////////
// Waypoint handling

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
// e.g. in options.h you may find the following #define WAYPOINT_PROXIMITY_RADIUS	25

// Origin Location
// When using relative waypoints, the default is to interpret those waypoints as relative to the
// plane's power-up location.  Here you can choose to use any specific, fixed 3D location as the
// origin point for your relative waypoints.
//
// USE_FIXED_ORIGIN should be 0 to use the power-up location as the origin for relative waypoints.
// Set it to 1 to use a fixed location as the origin, no matter where you power up.
// FIXED_ORIGIN_LOCATION is the location to use as the origin for relative waypoints.  It uses the
// format { X, Y, Z } where:
// X is Longitude in degrees * 10^7
// Y is Latitude in degrees * 10^7
// Z is altitude above sea level, in meters, as a floating point value.
// 
// If you are using waypoints for an autonomous landing, it is a good idea to set the altitude value
// to be the altitude of the landing point, and then express the heights of all of the waypoints with
// respect to the landing point.
// If you are using OpenLog, an easy way to determine the altitude of your landing point is to
// examine the telemetry after a flight, take a look in the .csv file, it will be easy to spot the
// altitude, expressed in meters.

// For example in options.h you may find:-
//      #define USE_FIXED_ORIGIN        0
//      #define FIXED_ORIGIN_LOCATION   { 113480854, 472580108, 578 }    // Innsbruck

////////////////////////////////////////////////////////////////////////////////
// Commands
// 
// Use the following commands to create your logo paths:
// 
// HOME                 - Return the turtle to the origin, aiming North.
// 
// FD(x)                - Move the turtle forward x meters, in the turtle's current direction.
// BK(x)                - Move the turtle backwards x meters, in the turtle's current direction.
// USE_CURRENT_POS      - Move the turtle to the plane's current {X,Y} position.  Mostly useful
//                        while being sneaky using PEN_UP.

// RT(x)                - Rotate the turtle to the right by x degrees.
// LT(x)                - Rotate the turtle to the left by x degrees.
// SET_ANGLE(x)         - Set the turtle to point x degrees clockwise from N.
// USE_CURRENT_ANGLE    - Aim the turtle in the direction that the plane is currently headed.
// USE_ANGLE_TO_GOAL    - Aim the turtle in the direction of the goal from the location of the plane.

// EAST(x)              - Move the turtle x meters East.
// WEST(x)              - Move the turtle x meters West.
// SET_X_POS(x)         - Set the X value of the turtle (meters East of the origin) to x.
// 
// NORTH(y)             - Move the turtle y meters North.
// SOUTH(y)             - Move the turtle y meters South.
// SET_Y_POS(y)         - Set the Y value of the turtle (meters North of the origin) to y.
// 
// SET_POS(x, y)        - Set both x and y at the same time.
// SET_ABS_POS(x, y)    - Set absolute X,Y location (long,lat) in degrees * 10^7

// ALT_UP(z)            - Gain z meters of altitude.
// ALT_DOWN(z)          - Drop z meters of altitude.
// SET_ALT(z)           - Set altitude to z.

// SPEED_INCREASE(x)    - Increases the target speed by x m/s
// SPEED_DECREASE(x)    - Decreases the target speed by x m/s
// SET_SPEED(x)         - Sets the target speed to x m/s

// REPEAT(n)            - Repeat all of the instructions until the matching END, n times
// REPEAT_FOREVER       - Repeat all of the instructions until the matching END, forever
// END                  - End the current REPEAT loop or Subroutine definition

// IF_EQ(val, x)        - Looks up a system value (listed below) and checks if it's equal to x.
//                        If so, runs commands until reaching ELSE or END.  If not, skips to ELSE 
//                        and runs until END, or just skips to END if there's no ELSE.
//                        Available IF commands: IF_EQ(equal), IF_NE(not equal), 
//                        IF_GT(val>x), IF_LT(val<x),IF_GE(val>=x), IF_LE(val<=x).
// ELSE                 - Starts a list of commands that get run if the preceding IF failed.

// PEN_UP               - While the pen is up, logo code execution does not stop to wait for the
//                        plane to move to each new position of the turtle before continuing.
//                        This allows you to use multiple logo instructions to get the turtle to
//                        the next goal location before commanding the plane to fly there by 
//                        putting the pen back down.
// PEN_DOWN             - When the pen is down, the plane moves to each new position of the turtle
//                        before more logo instructions are interpereted.
// PEN_TOGGLE           - Toggle the pen between up and down.

// SET_TURTLE(T)        - Choose to control either the plane's turtle, or the camera turtle.
//                        Use either SET_TURTLE(PLANE) or SET_TURTLE(CAMERA).


// Commands for Modifying Flags
// 
// FLAG_ON(F)           - Turn on flag F.  (See below for a list of flags.)
// FLAG_OFF(F)          - Turn off flag F.
// FLAG_TOGGLE(F)       - Toggle flag F.
// 
// The supported flags are the following:
// 
// F_TAKEOFF            - More quickly gain altitude at takeoff.
// F_INVERTED           - Fly with the plane upside down. (only if STABILIZE_INVERTED_FLIGHT is set to 1 in options.h)
// F_HOVER              - Hover the plane with the nose up. (only if STABILIZE_HOVER is set to 1 in options.h)
//                        NOTE: while hovering, no navigation is performed, and throttle is under manual control.
// F_TRIGGER            - Trigger an action to happen at this point in the flight.  (See the Trigger Action section of the options.h file.) 
// F_ALTITUDE_GOAL      - Climb or descend to the given altitude.
// F_CROSS_TRACK        - Navigate using cross-tracking.  Best used for longer flight legs.
// F_LAND               - Fly with the throttle off.


// Commands for Creating and Calling Subroutines
//
// TO(FUNC)             - Begin defining subroutine FUNC (requires #define FUNC N where N is an
//                        integer, unique among your defined subroutines.  End each subroutine
//                        definition with an END.
// DO(FUNC)             - Run subroutine FUNC.  When it finishes, control returns to the line after
//                        the DO() instruction.
// EXEC(FUNC)           - Call FUNC as though it were the beginning of the logo program.  This will never return.
//                        When/if FUNC finishes, logo will start back at the beginning of the program.
// DO(LOGO_MAIN) or
// EXEC(LOGO_MAIN)      - Restart at the top of the LOGO program
// DO_ARG(FUNC, PARAM)  - Run subroutine FUNC, using an integer value as a parameter.
// EXEC_ARG(FUNC, PARAM)- Exec subroutine FUNC, using an integer value as a parameter.
// 
// FD_PARAM             - From within a subroutine, call the FD command using the parameter
//                        passed to this subroutine as the distance.
// RT_PARAM             - From within a subroutine, call the RT command using the parameter
//                        passed to this subroutine as the angle.
// REPEAT_PARAM         - Start a REPEAT block, using the current subroutine's parameter as the
//                        number of times to repeat.
// DO_PARAM(FUNC)       - Call subroutine FUNC with a parameter equal to the current subroutine's
//                        parameter value.
// 
// PARAM_ADD(x)         - Adds x to the current subroutine's current parameter value.  Fun
//                        inside repeats inside subroutines!
// PARAM_SUB(x)         - Subtracts x from the current subroutine's current parameter value.
// PARAM_MUL(x)         - Multiplies the current subroutine's current parameter value by x.
// PARAM_DIV(x)         - Divides the current subroutine's current parameter value by x.
// PARAM_SET(x)         - Sets the current subroutine's current parameter value to x.
// 
// LOAD_TO_PARAM(val)   - Loads a system value (listed below) into the current subroutine's parameter value.
// 
// All parameter-related commands: 
//        FD_PARAM, BK_PARAM, RT_PARAM, LT_PARAM, SET_ANGLE_PARAM, 
//        EAST_PARAM, WEST_PARAM, NORTH_PARAM, SOUTH_PARAM, ALT_UP_PARAM, ALT_DOWN_PARAM, 
//        SET_X_POS_PARAM, SET_Y_POS_PARAM, SET_ALT_PARAM, 
//        SPEED_INCREASE_PARAM, SPEED_DECREASE_PARAM, SET_SPEED_PARAM
//        REPEAT_PARAM, DO_PARAM(FUNC), EXEC_PARAM(FUNC)
//        PARAM_SET(x), PARAM_ADD(x), PARAM_SUB(x), PARAM_MUL(x), PARAM_DIV(x)
//        IF_EQ_PARAM(x), IF_NE_PARAM(x), IF_GT_PARAM(x), IF_LT_PARAM(x), IF_GE_PARAM(x), IF_LE_PARAM(x)


// SET_INTERRUPT(f)     - Sets a user-defined logo function to be called at 40Hz.  Be careful not to modify
//                        the turtle location from within your interrupt function unless you really want to!
//                        Usually you'll just want your interrupt function to check some condition, and do
//                        something only if it's true.  (Like fly home only if you get too far away.)
// CLEAR_INTERRUPT      - Clears/disables the interrupt function.  Not usually needed.


// System Values for use with LOAD_TO_PARAM(val) and IF_XX() commands
// 
// DIST_TO_HOME         - in m
// DIST_TO_GOAL         - in m
// ALT                  - in m
// CURRENT_ANGLE        - in degrees. 0-359 (clockwise, 0=North)
// ANGLE_TO_HOME        - in degrees. 0-359 (clockwise, 0=North)
// ANGLE_TO_GOAL        - in degrees. 0-359 (clockwise, 0=North)
// REL_ANGLE_TO_HOME    - in degrees. -180-179 (0=heading directly towards Home. Home to the right of the nose of the plane is positive)
// REL_ANGLE_TO_GOAL    - in degrees. -180-179 (0=heading directly towards Goal. Goal to the right of the nose of the plane is positive)
// GROUND_SPEED         - in cm/s
// AIR_SPEED            - in cm/s
// AIR_SPEED_Z          - in cm/s
// WIND_SPEED           - in cm/s
// WIND_SPEED_X         - in cm/s
// WIND_SPEED_Y         - in cm/s
// WIND_SPEED_Z         - in cm/s
// WIND_FROM_ANGLE      - in degrees Wind from 0-359 (clockwise, 0=North)
// PARAM                - current param value
// XX_INPUT_CHANNEL     - channel value from 2000-4000 (any channel defined in options.h, e.g. THROTTLE_INPUT_CHANNEL)



////////////////////////////////////////////////////////////////////////////////
// Notes:
//  - Altitudes are relative to the starting point, and the initial altitude goal is 100 meters up.
//  - All angles are in degrees.
//  - Repeat commands and subroutines can be nested up to 12-deep.
//  - If the end of the list of instructions is reached, we start over at the top from the current location and angle.
//    This does not take up one of the 12 nested repeat levels.
//  - If you use many small FD() commands to make curves, I suggest enabling cross tracking: FLAG_ON(F_CROSS_TRACK)
//  - All Subroutines have to appear after the end of your main logo program.


////////////////////////////////////////////////////////////////////////////////
// Define the main flight plan as:
// 
// #define FOO 1
// 
// const struct logoInstructionDef instructions[] = {
//		instruction1
//		instruction2
//		etc.
//		END
//
//		TO (FOO)
//			etc.
//		END
//	};
// 
// and the Failsafe RTL course as:
// 
// #define BAR 2
// 
// const struct logoInstructionDef rtlInstructions[] = {
//		instruction1
//		instruction2
//		etc.
//		END
//
//		TO (BAR)
//			etc.
//		END
//	};


////////////////////////////////////////////////////////////////////////////////
// Main Flight Plan
//
// Fly a 100m square at an altitude of 100m, beginning above the origin, pointing North


const struct logoInstructionDef instructions[] = {

    USE_CURRENT_POS
    SET_SPEED(14)
    LOAD_TO_PARAM(ALT) // Check altitude to be more than 100m, if not climb)
    SET_ALT_PARAM
    IF_LE(ALT,100)
	SET_ALT(100)
    ELSE
        //LOAD_TO_PARAM(ALT)
        SET_ALT_PARAM
	END
};

////////////////////////////////////////////////////////////////////////////////
// RTL Flight Plan
// 
// On entering RTL mode, turn off the engine, fly home, and circle indefinitely until touching down

const struct logoInstructionDef rtlInstructions[] = {

	// Use cross-tracking for navigation
	FLAG_ON(F_CROSS_TRACK)

	// Turn off engine for RTL
	// Move this line down below the HOME to return home with power before circling unpowered.
	FLAG_ON(F_LAND)

	// Fly home
	HOME

	// Once we arrive home, aim the turtle in the
	// direction that the plane is already moving.
	USE_CURRENT_ANGLE

	REPEAT_FOREVER
		// Fly a circle (36-point regular polygon)
		REPEAT(36)
			RT(10)
			FD(8)
		END
	END
	
};


////////////////////////////////////////////////////////////////////////////////
// More Examples

/*
// Fly a 200m square starting at the current location and altitude, in the current direction
	REPEAT(4)
		FD(200)
		RT(90)
	END
*/

/*
// Fly a round-cornered square
	FLAG_ON(F_CROSS_TRACK)

	REPEAT(4)
		FD(170)
		REPEAT(6)
			LT(15)
			FD(10)
		END
	END
*/

/*
// Set the camera target to a point 100m North of the origin, then circle that point
	SET_TURTLE(CAMERA)
	HOME
	FD(100)
	SET_TURTLE(PLANE)

	FLAG_ON(F_CROSS_TRACK)

	HOME
	LT(90)

	REPEAT_FOREVER
		// Fly a circle (36-point regular polygon)
		REPEAT(36)
			RT(10)
			FD(20)
		END
	END
*/

/*
// Fly a giant, 2.5km diameter, 10-pointed star with external loops at each point
	FLAG_ON(F_CROSS_TRACK)

	REPEAT(10)
		FD(2000)

		REPEAT(18)
			RT(14) // == RT((180+72)/18)
			FD(50)
		END
	END
*/

/*
// Come in for an automatic landing at the HOME position
// from the current direction of the plane.
// 1. Aim for 32m altitude at 250m from HOME
// 2. Fly to 200m from HOME and turn off power
// 3. Aim for -32m altitude, 200m past home, which should
//    touch down very close to HOME.

	FLAG_ON(F_CROSS_TRACK)

	SET_ALT(32)

	PEN_UP
	HOME
	USE_ANGLE_TO_GOAL
	BK(250)
	PEN_DOWN
	FLAG_ON(F_LAND)
	PEN_UP
	HOME
	USE_ANGLE_TO_GOAL
	BK(200)
	PEN_DOWN
	SET_ALT(-32)
	PEN_UP
	HOME
	USE_ANGLE_TO_GOAL
	FD(200)
	PEN_DOWN
*/

/*
// Example of using some math on PARAM values to make cool spirals
#define SPIRAL_IN                   1
#define SPIRAL_OUT                  2
#define FWD_100_MINUS_PARAM_OVER_2  3

const struct logoInstructionDef instructions[] = {
	
DO_ARG(SPIRAL_IN, 10)
RT(100)
DO_ARG(SPIRAL_OUT,  70)

END



TO (SPIRAL_IN)
	REPEAT(30)
		DO_PARAM(FWD_100_MINUS_PARAM_OVER_2)
		RT_PARAM
		PARAM_ADD(2)
	END
END


TO (SPIRAL_OUT)
	REPEAT(30)
		PARAM_SUB(2)
		RT_PARAM
		DO_PARAM(FWD_100_MINUS_PARAM_OVER_2)
	END
END


TO (FWD_100_MINUS_PARAM_OVER_2)
	PARAM_MUL(-1)
	PARAM_ADD(100)
	PARAM_DIV(2)
	FD_PARAM
END
*/

/*
// Example of using an interrupt handler to stop the plane from getting too far away
// Notice mid-pattern if we get >200m away from home, and if so, fly home.
#define INT_HANDLER                 1

const struct logoInstructionDef instructions[] = {

SET_INTERRUPT(INT_HANDLER)

REPEAT_FOREVER
	FD(20)
	RT(10)
END

END


TO (INT_HANDLER)
	IF_GT(DIST_TO_HOME, 200)
		HOME
	END
END
*/

/*
// Example of using an interrupt handler to toggle between 2 flight plans.
// When starting the flightplan, decide whether to circle left or right, based on which direction
// initially turns towards home.  From then on, the circling direction can be changed by moving the
// rudder input channel to one side or the other.

#define CIRCLE_RIGHT                1
#define CIRCLE_LEFT                 2
#define INT_HANDLER_RIGHT           3
#define INT_HANDLER_LEFT            4

const struct logoInstructionDef instructions[] = {

IF_GT(REL_ANGLE_TO_HOME, 0)
	EXEC(CIRCLE_RIGHT)
ELSE
	EXEC(CIRCLE_LEFT)
END


TO (CIRCLE_RIGHT)
	USE_CURRENT_POS
	SET_INTERRUPT(INT_HANDLER_RIGHT)
	REPEAT_FOREVER
		FD(10)
		RT(10)
	END
END

TO (CIRCLE_LEFT)
	USE_CURRENT_POS
	SET_INTERRUPT(INT_HANDLER_LEFT)
	REPEAT_FOREVER
		FD(10)
		LT(10)
	END
END


TO (INT_HANDLER_RIGHT)
	IF_LT(RUDDER_INPUT_CHANNEL, 2800)
		EXEC(CIRCLE_LEFT)
	END
END

TO (INT_HANDLER_LEFT)
	IF_GT(RUDDER_INPUT_CHANNEL, 3200)
		EXEC(CIRCLE_RIGHT)
	END
END
};
*/
