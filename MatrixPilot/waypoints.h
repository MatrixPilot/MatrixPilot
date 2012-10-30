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


////////////////////////////////////////////////////////////////////////////////
// Waypoint handling

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
#define WAYPOINT_RADIUS 		25

// Origin Location
// When using relative waypoints, the default is to interpret those waypoints as relative to the
// plane's power-up location.  Here you can choose to use any specific, fixed 3D location as the
// origin point for your relative waypoints.
//
// USE_FIXED_ORIGIN should be 0 to use the power-up location as the origin for relative waypoints.
// Set it to 1 to use a fixed location as the origin, no matter where you power up.
// FIXED_ORIGIN_LOCATION is the location to use as the origin for relative waypoints.  It uses the
// format { X, Y, Z } where:
// X is Logitude in degrees * 10^7
// Y is Latitude in degrees * 10^7
// Z is altitude above sea level, in meters, as a floating point value.
// 
// If you are using waypoints for an autonomous landing, it is a good idea to set the altitude value
// to be the altitude of the landing point, and then express the heights of all of the waypoints with
// respect to the landing point.
// If you are using OpenLog, an easy way to determine the altitude of your landing point is to
// examine the telemetry after a flight, take a look in the .csv file, it will be easy to spot the
// altitude, expressed in meters.

#define USE_FIXED_ORIGIN		1
// #define FIXED_ORIGIN_LOCATION	{ -797866871, 435341708, 182.0 }	// North Field field center launch point 168m above sea lev 
// #define FIXED_ORIGIN_LOCATION	{-797279923, 434894384, 168.4 }		// OMFC South Field field NW offset DC launch point 168m above sea lev
#define FIXED_ORIGIN_LOCATION	{-797278708, 434893087, 170.0}			// OMFC South Field field dead center launch point 168m above sea lev
// #define FIXED_ORIGIN_LOCATION	{-801586991, 429692064, 226.2 }		// Bennet Field, Springvale East field 
// #define FIXED_ORIGIN_LOCATION	{-801605927, 429691278, 226.2 }		// Bennet Field, Springvale West field 
// #define FIXED_ORIGIN_LOCATION	{-801595895, 429693672,  226.2}		// Bennet Field, Springvale Center field 
// #define FIXED_ORIGIN_LOCATION	{-801595895, 429693672,  226.2}		// Bennet Field, Springvale Center field 

////////////////////////////////////////////////////////////////////////////////
// Waypoint definitions
// 
// Define the main course as:
// 
// const struct waypointDef waypoints[] = {
//						waypoint1 ,
//						waypoint2 ,
//						etc.
//						} ;
// 
// and the Failsafe RTL course as:
// 
// const struct waypointDef rtlWaypoints[] = {
//						waypoint1 ,
//						waypoint2 ,
//						etc.
//						} ;
// 
// A waypoint is defined as { { X , Y , Z } , F , CAM_VIEW }
// where X, Y, and Z are the three coordinates of the waypoint,
// F stores the flags/options for this waypoint,
// and CAM_VIEW is a 3D location to aim the camera as {CAM_X, CAM_Y, CAM_Z}, or just
// CAM_VIEW_LAUNCH to aim the camera at the launch point.
// (This must be provided even if no camera is used.)
// 
// Each waypoint can be defined as either a Relative waypoint or an Absolute waypoint.
//
// For a Relative waypoint,
// X, Y, and Z are distances in meters, relative to the initialization location of the board.
// X is positive as you move east
// Y is positive as you move north
// Z is the height
// 
// For an Absolute waypoint,
// X is Longitude in degrees * 10^+7
// Y is Latitude in degrees * 10^+7
// Z is altitude in meters relative to the initialization location of the board.
// As an example, the absolute waypoint { { -1219950467, 374124664, 100 }, F_ABSOLUTE } represents a point
// 100 meters above Baylands Park in Sunnyvale, CA, and will fly there normally (not inverted, etc.)
// ( Longitude = -121.9950467 degrees, Latitude = 37.4124664 degrees. )
// 
// Currently F can be set to: F_NORMAL, or any combination of:
// F_ABSOLUTE		- Waypoints are Relative by default, unless F_ABSOLUTE is specified.
// 
// F_TAKEOFF		- More quickly gain altitude at takeoff.
// F_INVERTED		- Navigate to this waypoint with the plane upside down. (only if STABILIZE_INVERTED_FLIGHT is set to 1 in options.h)
// F_HOVER			- Hover the plane until reaching this waypoint. (only if STABILIZE_HOVER is set to 1 in options.h)
//					  NOTE: while hovering, no navigation is performed, and throttle is under manual control.
// F_LOITER			- After reaching this waypoint, continue navigating towards this same waypoint.  Repeat until leaving waypoint mode.
// F_TRIGGER		- Trigger an action to happen when this waypoint leg starts.  (See the Trigger Action section of the options.h file.) 
// F_ALTITUDE_GOAL	- Climb or descend to the given altitude, then continue to the next waypoint.
// F_CROSS_TRACK	- Navigate using cross-tracking.  Best used for longer waypoint legs.
// F_LAND			- Navigate towards this waypoint with the throttle off.
// 
// 
// NOTE: Please be very careful when including inverted or hovering legs in a waypoints list.  Even if your plane does not fly well
//       in these orientations, or if you fly these legs without power, the UDB will keep on trying to maintain these orientations
//       which could lead to a crash.  If you try to manually recover from this behavior, remember to switch out of waypoiont mode
//       first, to avoid fighting the stabilization code.
// 
// 
// Camera View Points are now part of a waypoint definition file. The waypoint definition structure requires
// a camera viewpoint even if you do not have a camera on your plane. (until we move to having a flight
// planning language).  As a default, you can use the predefined CAM_VIEW_LAUNCH which points at { 0, 0, 0 }.
// 
// Camera Viewpoints are exactly like waypoint definitions. They define a point at which
// the camera will look in 3 dimensions. If you are using a waypoint relative to the initialisation of your 
// plane, then the camera viewpoint should also be relative e.g. "{ 32 , -22, 0 )".
// Camera waypoints can be absolute LAT and LONG, and camera target height is height above initalisation.
// This is the same as a fixed or absolute waypoint.
// Finally, do not mix relative waypoints and absolute camera viewpoint in the same line. A line should
// either use both a fixed waypoint and fixed camera viewpoint, or both relative.
// 
// 
// You do not need to specify how many points you have, the compiler will count them for you.
// You can use the facilities of the compiler to do some simple calculations in defining the course.
//
// To use waypoints, make sure FLIGHT_PLAN_TYPE is set to FP_WAYPOINTS in options.h.


////////////////////////////////////////////////////////////////////////////////
// waypoints[]
// 
// By default the only waypoint is defined to be 75 meters above the starting point.


/* const struct waypointDef waypoints[] = {
{ {   0,   0, 75 } , F_NORMAL, CAM_VIEW_LAUNCH } ,   // return to, and loiter 75 meters above the startup position
} ; 
*/
//   OMFC Southfield LOS FLYING CLOCKWISE  15 way points  // for glider
const struct waypointDef waypoints[] = {
                                        { { -274, -94, 105 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -427, 41, 110 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { -148, 255, 115 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { 12, 93, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { -292, -109, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { -395, 32, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { -152, 241, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        { { 10, 101, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 7
                                        { { -273, -79, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 8
                                        { { -406, 44, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 9
                                        { { -141, 247, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 10
                                        { { 20, 87, 120 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 11
                                        { { -267, -112, 115 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 12
                                        { { -406, 32, 115 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 13
                                        { { -150, 234, 105 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 14
                                        { { 11, 91, 105 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 15
                                        }
;


////////////////////////////////////////////////////////////////////////////////
// rtlWaypoints[]
// 
// This set of waypoints describes what to do when entering RTL mode.
// By default the only RTL waypoint is defined to be at the starting point, and to fly there unpowered.
// 
// Note that this set of waypoints is only used if FAILSAFE_TYPE is set to FAILSAFE_RTL in options.h.
// 
// WARNING: If you set this not to include the F_LAND flag, then be very careful during ground testing
// and after flights, since turning off the transmitter will cause the throttle to come on.

const struct waypointDef rtlWaypoints[] = {
		{ { 0, 0,  50 } , F_LOITER + F_LAND, CAM_VIEW_LAUNCH } ,
} ;




////////////////////////////////////////////////////////////////////////////////
// Example waypoint lists
//
// Here are more examples of waypoints lists, commented out.  Note that these are
// all definitions of waypoints[], and if you want to use any of these as rtl waypoints,
// you'll have to change them to define rtlWaypoints[] instead of waypoints[].



////////////////////////////////////////////////////////////////////////////////
// This is an example course that makes a 100 meter square, 75 meters above the starting point, and
// then takes a photo and lands.
// 
// We first go to the south east corner of the square.
// Then on to the north east corner.
// The plane then uses ailerons to flip upside down, and heads towards the north west corner.
// Then we flip back over and head back to the south west corner.  
// We then take a photo, turn off the motor and head towards the middle of the square.
// When we fly past the middle, we turn back towards the middle, still without throttle, over and over until "landing".
// 
// Note that this is not likely to be an especially smooth landing...

/*
const struct waypointDef waypoints[] = {
		{ { 100,   0  , 75 } , F_NORMAL,   CAM_VIEW_LAUNCH } ,
		{ { 100, 100  , 75 } , F_NORMAL,   CAM_VIEW_LAUNCH } ,
		{ {   0, 100  , 75 } , F_INVERTED, CAM_VIEW_LAUNCH } ,
		{ {   0,   0  , 75 } , F_NORMAL,   CAM_VIEW_LAUNCH } ,
		{ {  50,  50  , 75 } , F_LOITER + F_TRIGGER + F_LAND, CAM_VIEW_LAUNCH } ,
} ;
*/


////////////////////////////////////////////////////////////////////////////////
// DIY Drones T3-2 Course
// 
// As a more complex example, here is the DIY Drones T3-2 contest course
// 
// Prior to flight, initialize the board at the center of the course.

/*
// CORNER is the absolute value of the X or Y coordinate at the corners of the course. 
#define CORNER 100

// CLEARANCE is an allowance for obstacles.
#define CLEARANCE 25

#define CAM_VIEW_2  { CORNER, CORNER, 0 } // Define a Camera ViewPoint to look at 100 ,100, 0

// Here is the T3 course definition:

const struct waypointDef waypoints[] = {
		{ {    CORNER  ,    CORNER  , CLEARANCE + 100 } , F_NORMAL, CAM_VIEW_LAUNCH } ,
		{ {    CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL, CAM_VIEW_LAUNCH } ,
		{ {  - CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL, CAM_VIEW_LAUNCH } ,
		{ {  - CORNER  ,  - CORNER  , CLEARANCE +  25 } , F_NORMAL, CAM_VIEW_LAUNCH } ,
		{ {    CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL, CAM_VIEW_LAUNCH } ,
		{ {    CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL, CAM_VIEW_LAUNCH } ,
		{ {  - CORNER  ,    CORNER  , CLEARANCE + 100 } , F_NORMAL, CAM_VIEW_LAUNCH } ,
		{ {  - CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL, CAM_VIEW_2 } ,
		{ {    CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL, CAM_VIEW_2 } ,
		{ {    CORNER  ,  - CORNER  , CLEARANCE +  25 } , F_NORMAL, CAM_VIEW_2 } ,
		{ {  - CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL, CAM_VIEW_2 } ,
		{ {  - CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL, CAM_VIEW_2 } ,
} ;
*/
/* Options for OMFC LOS flight: 
//  CLOCKWISE PATTERN TEMPLATES
// 1)	Relative  coordinates,  clockwise rectangular repetitive rounds, with   4 wps.
const struct waypointDef waypoints[] = {
                                        { { -96, -35, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -149, 33, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { 38, 132, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { 91, 71, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        }
;

// 2)	Relative  coordinates,  clockwise rectangular flight 3 rounds, with   13 wps, landing on last.
const struct waypointDef waypoints[] = {
                                        { { -88, -18, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -134, 37, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { 31, 133, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { 73, 85, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { -100, -15, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { -139, 39, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { 33, 140, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        { { 79, 86, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 7
                                        { { -105, -21, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 8
                                        { { -145, 39, 42 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 9
                                        { { 34, 145, 37 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 10
                                        { { 88, 85, 20 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 11
                                        { { 68, 68, 12 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 12
                                        { { 39, 39, 5 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 13
                                        { { 15, 17, 1 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 14
                                        { { -9, -4, 0 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 15                                        
                                         }
;
       
// 3) Relative  coordinates,  clockwise  flight, with rectangular  6 waypoints, inverted between 2 and 3.
const struct waypointDef waypoints[] = {
                                        { { -87, -18, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -121, 34, 65 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { -73, 80, 65 } , F_NORMAL  +  F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -6, 109, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { 55, 117, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { 92, 77, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        }
 ;
// 4) Relative  coordinates,  CLOCKWISE  flight, with rectangular  6 waypoints, inverted between 2 and 3
//     hover between 6 and 7; and inverted between 8 and launch point.
//     IMPORTANT: Plane must be 3D capable to hover.
const struct waypointDef waypoints[] = {
                                        { { -87, -18, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -119, 32, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { -62, 73, 45 } , F_NORMAL  +  F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -1, 99, 40 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { 55, 117, 40 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { 92, 77, 40 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { 53, 38, 35 } , F_NORMAL + F_HOVER , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        { { 39, 27, 35 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 7
                                        { { 25, 16, 40 } , F_NORMAL  +  F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 8
                                        }
;
// 5) Relative  coordinates,  clockwise  flight, with half star pattern  7 waypoints, inverted between 2 and 3.
const struct waypointDef waypoints[] = {
                                        { { -98, -27, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -80, 35, 65 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { -104, 92, 75 } , F_NORMAL + F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -36, 91, 75 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { -1, 142, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { 39, 102, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { 101, 86, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        }
;

//  COUNTER-CLOCKWISE PATTERN TEMPLATES

// 6)	Relative  coordinates,  counter-clockwise rectangular repetitive rounds, with   4 wps.
const struct waypointDef waypoints[] = {
                                        { { 74, 97, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { 27, 148, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { -141, 34, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -99, -20, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        }
 ;
// 7)	Relative  coordinates,  COUNTER-CLOCKWISE rectangular flight 3 rounds, with   13 wps, landing on last.
const struct waypointDef waypoints[] = {
                                        { { 79, 89, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { 34, 133, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { -150, 30, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -108, -13, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { 88, 94, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { 38, 136, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { -157, 30, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        { { -110, -18, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 7
                                        { { 95, 94, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 8
                                        { { 42, 141, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 9
                                        { { -174, 17, 40 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 10
                                        { { -122, -42, 20 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 11
                                        { { -82, -28, 12 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 12
                                        { { -51, -15, 5 } , F_NORMAL  + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 13
                                        { { -16, -2, 1 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 14
                                        { { 15, 10, 0 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 15
                                        }
;

// 8)	Relative  coordinates,  COUNTER-CLOCKWISE  flight, with rectangular  6 waypoints, inverted between 2 and 3.
const struct waypointDef waypoints[] = {
                                        { { 88, 67, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { 54, 105, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { 2, 91, 60 } , F_NORMAL  +  F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -73, 57, 65 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { -114, 14, 65 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { -83, -29, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        }
;
// 9)	Relative  coordinates,  COUNTER-CLOCKWISE  flight, with rectangular 8 waypoints, inverted between 2 and 3.
//     HOVER between 6 and 7; and inverted between 8 and launch point.
//     IMPORTANT: Plane must be 3D capable to hover.
const struct waypointDef waypoints[] = {
                                        { { 88, 67, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { 54, 105, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { 2, 91, 45 } , F_NORMAL  +  F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -71, 54, 40 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { -114, 14, 40 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { -83, -29, 40 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { -58, -22, 35 } , F_NORMAL + F_HOVER , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        { { -39, -15, 35 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 7
                                        { { -12, -5, 40 } , F_NORMAL  +  F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 8
                                        }
;
// 10)	Relative  coordinates,  COUNTER-CLOCKWISE  flight, with HALF STAR pattern  7 waypoints, inverted between 3 and 4.
const struct waypointDef waypoints[] = {
                                        { { 94, 87, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { 44, 101, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { 6, 134, 60 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -32, 97, 65 } , F_NORMAL   +  F_INVERTED , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { -88, 91, 70 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { -67, 37, 65 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { -86, -18, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        }
;
//  SOUTHFIELD rtlWaypoints PATTERN TEMPLATES
//  1.	Relative  coordinates  CLOCKWISE
const struct waypointDef rtlWaypoints[] = {
                                        { { -61, 30, 45 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -28, 105, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { 45, 131, 37 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { 84, 98, 20 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { 68, 68, 12 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { 39, 39, 5 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { 15, 17, 1 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        { { 0, 0, 0 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 7
                                        }
;

//  2.	Relative  coordinates  COUNTER-CLOCKWISE
const struct waypointDef rtlWaypoints[] = {
                                        { { -36, 50, 55 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 0
                                        { { -110, 36, 50 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 1
                                        { { -162, -8, 37 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 2
                                        { { -124, -45, 20 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 3
                                        { { -91, -41, 12 } , F_NORMAL , CAM_VIEW_LAUNCH } , //Waypoint 4
                                        { { -60, -30, 5 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 5
                                        { { -26, -14, 1 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 6
                                        { { 0, 0, 0 } , F_NORMAL + F_LAND , CAM_VIEW_LAUNCH } , //Waypoint 7
                                        }
;

*/
