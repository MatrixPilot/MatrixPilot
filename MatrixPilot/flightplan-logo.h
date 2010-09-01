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


////////////////////////////////////////////////////////////////////////////////
// UDB LOGO Waypoint handling

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
// Only applies for waypoint legs that do not use cross tracking.
#define WAYPOINT_RADIUS 		25

// Origin Location
// When using relative waypoints, the default is to interpret those waypoints as relative to the
// plane's power-up location.  Here you can choose to use any specific, fixed 2D location as the
// origin point for your relative waypoints.  (The code will still use the plane's power-up
// altitude as the altitude origin.)
//
// USE_FIXED_ORIGIN should be 0 to use the power-up location as the origin for relative waypoints.
// Set it to 1 to use a fixed location as the origin, no matter where you power up.
// FIXED_ORIGIN_LOCATION is the location to use as the origin for relative waypoints.  It uses the
// format { X, Y } where:
// X is Logitude in degrees * 10^7
// Y is Latitude in degrees * 10^7
// 
#define USE_FIXED_ORIGIN		0
#define FIXED_ORIGIN_LOCATION	{ -1219950467, 374124664 }	// A point in Baylands Park in Sunnyvale, CA


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
// you enter RTL mode, the state is reset and your rtlInstructions[] logo program is run instead.
// The following state is cleared when entering waypoint mode or RTL mode: (but not when your program
// ends and starts over)
//   - The plane turtle and camera turtle begin at the plane's current position and altitude.
//   - Both turtles begin pointing in the plane's current heading.
//   - The flags are all turned off.
//   - The pen is down, and the PLANE turtle is active.
// 
// To use UDB Logo, set FLIGHT_PLAN_TYPE to FP_LOGO in options.h.



////////////////////////////////////////////////////////////////////////////////
// Commands
// 
// Use the following commands to create your logo paths:
// 
// HOME				- Return the turtle to the origin, aiming North.
// 
// FD(x)			- Move the turtle forward x meters, in the turtle's current direction.
// BK(x)			- Move the turtle backwards x meters, in the turtle's current direction.
// 
// RT(x)			- Rotate the turtle to the right by x degrees.
// LT(x)			- Rotate the turtle to the left by x degrees.
// SET_ANGLE(x)		- Set the turtle to point x degrees clockwise from N.
// USE_CURRENT_ANGLE- Aim the turtle in the direction that the plane is currently headed.
// 
// EAST(x)			- Move the turtle x meters East.
// WEST(x)			- Move the turtle x meters West.
// SET_X_POS(x)		- Set the X value of the turtle (meters East of the origin) to x.
// 
// NORTH(y)			- Move the turtle y meters North.
// SOUTH(y)			- Move the turtle y meters South.
// SET_Y_POS(y)		- Set the Y value of the turtle (meters North of the origin) to y.
// 
// SET_POS(x, y)	- Set both x and y at the same time.
// 
// ALT_UP(z)		- Gain z meters of altitude.
// ALT_DOWN(z)		- Drop z meters of altitude.
// SET_ALT(z)		- Set altitude to z.
// 
// FLAG_ON(f)		- Turn on flag f.  (See below for a list of flags.)
// FLAG_OFF(f)		- Turn off flag f.
// FLAG_TOGGLE(f)	- Toggle flag f.
// 
// PEN_UP			- While the pen is up, logo code execution does not stop to wait for the
// 					  plane to move to each new position of the turtle before continuing.
//					  This allows you to use multiple logo instructions to get the turtle to
//					  the next goal location before commanding the plane to fly there by 
//					  putting the pen back down.
// PEN_DOWN			- When the pen is down, the plane moves to each new position of the turtle
//					  before more logo instructions are interpereted.
// PEN_TOGGLE		- Toggle the pen between up and down.
// 
// SET_TURTLE(T)	- Choose to control either the plane's turtle, or the camera turtle.
//					  Use either SET_TURTLE(PLANE) or SET_TURTLE(CAMERA).
// 
// REPEAT(n)		- Repeat all of the instructions until the matching END, n times
// REPEAT_FOREVER	- Repeat all of the instructions until the matching END, forever
// END				- End the current REPEAT loop


////////////////////////////////////////////////////////////////////////////////
// Flags
// 
// The supported flags are the following:
// 
// F_TAKEOFF		- More quickly gain altitude at takeoff.
// F_INVERTED		- Fly with the plane upside down. (only if STABILIZE_INVERTED_FLIGHT is set to 1 in options.h)
// F_HOVER			- Hover the plane with the nose up. (only if STABILIZE_HOVER is set to 1 in options.h)
//					  NOTE: while hovering, no navigation is performed, and throttle is under manual control.
// F_TRIGGER		- Trigger an action to happen at this point in the flight.  (See the Trigger Action section of the options.h file.) 
// F_ALTITUDE_GOAL	- Climb or descend to the given altitude.
// F_CROSS_TRACK	- Navigate using cross-tracking.  Best used for longer flight legs.
// F_LAND			- Fly with the throttle off.


////////////////////////////////////////////////////////////////////////////////
// Notes:
//	- Altitudes are relative to the starting point, and the initial altitude goal is 100 meters up.
//	- All angles are in degrees.
//	- Repeat commands can be nested up to 8-deep.
//	- If the end of the list of instructions is reached, we start over at the top from the current location and angle.
//    This does not take up one of the 8 nested repeat levels.
//	- If you use many small FD() commands to make curves, I suggest enabling cross tracking: FLAG_ON(F_CROSS_TRACK)
//  - You can define non-recursive logo functions/macros like this:
//    #define RECT(x, y)		REPEAT(2) FD(x) RT(64) FD(y) RT(64) END


////////////////////////////////////////////////////////////////////////////////
// Define the main flight plan as:
// 
// const struct logoInstructionDef instructions[] = {
//		instruction1
//		instruction2 
//		etc.
//	} ;
// 
// and the Failsafe RTL course as:
// 
// const struct logoInstructionDef rtlInstructions[] = {
//		instruction1
//		instruction2 
//		etc.
//	} ;


////////////////////////////////////////////////////////////////////////////////
// Main Flight Plan
//
// Fly a 100m square at an altitude of 100m, beginning above the origin, pointing North
const struct logoInstructionDef instructions[] = {
	
	SET_ALT(100)
	
	// Go Home and point North
	HOME
	
	REPEAT_FOREVER
		REPEAT(4)
			FD(100)
			RT(90)
		END
	END
	
} ;


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
			RT(14) // (180+72)/18
			FD(50)
		END
	END
*/
