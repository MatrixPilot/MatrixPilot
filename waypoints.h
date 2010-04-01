////////////////////////////////////////////////////////////////////////////////
// Waypoint handling

// You have the option of using either cross tracking,
// in which navigation is based on the distance of the plane
// to the line between the waypoints.
// Or you can use navigation directly toward the goal point.
// If you want to use cross tracking, set USE_CROSSTRACKING to 1,
// otherwise, to use navigation directly toward the goal,
// set USE_CROSSTRACKING to 0.
#define USE_CROSSTRACKING		0

// Move on to the next waypoint when getting within this distance of the current goal (in meters)
// Only applies if not using cross tracking.
#define WAYPOINT_RADIUS 		25


////////////////////////////////////////////////////////////////////////////////
// Waypoint definitions
// 
// Define the course as:
// 
// const struct waypointDef waypoints[] = {
//						waypoint1 ,
//						waypoint2 ,
//						etc.
//						} ;
// 
// A waypoint is defined as { { X , Y , Z } , F }
// where X, Y, and Z are the three coordinates of the waypoint, and F stores the flags/options for this waypoint.
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
// F_INVERTED		- Navigate to this waypoint with the plane upside down. (only if STABILIZE_INVERTED_FLIGHT is set to 1 in options.h)
// F_HOVER			- Hover the plane until reaching this waypoint. (only if STABILIZE_HOVER is set to 1 in options.h)
//					  NOTE: while hovering, no navigation is performed, and throttle is under manual control.
// F_LOITER			- After reaching this waypoint, continue navigating towards this same waypoint.  Repeat until leaving waypoint mode.
// F_TRIGGER		- Trigger an action to happen when this waypoint leg starts.  (See the Trigger Action section of the options.h file.) 
// F_ALTITUDE_GOAL	- Climb or descend to the given altitude, then continue to the next waypoint.
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


// By default the only waypoint is defined to be above the starting point.

const struct waypointDef waypoints[] = {
						{ {  0  ,  0  , 50 } , F_NORMAL, CAM_VIEW_LAUNCH } ,  // return to, and circle 50 meters above the startup position
						} ;



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


/*
// As a more complex example, here is the DIY Drones T3-2 contest course
// 
// Prior to flight, initialize the board at the center of the course.

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
