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
// const struct waypointDef waypoints[] = {
//						waypoint1 ,
//						waypoint2 ,
//
//						etc.
//							}
// 
// A waypoint is defined as { { X , Y , Z } , F }
// where X, Y, and Z are the three coordinates of the waypoint in meters, 
// relative to the initialization location of the board.
// X is positive as you move east
// Y is positive as you move north
// Z is the height
// F stores the flags/options for this waypoint.  Currently can be set to: F_NORMAL, or any combination of:
// 
// F_INVERTED	- Navigate to this waypoint with the plane upside down. (only if STABILIZE_INVERTED_FLIGHT is set to 1 in options.h)
// F_HOVER		- Hover the plane until reaching this waypoint. (only if STABILIZE_HOVER is set to 1 in options.h)
//				  NOTE: while hovering, no navigation is performed, and throttle is under manual control.
// F_LOITER		- After reaching this waypoint, continue navigating towards this same waypoint.  Repeat until leaving waypoint mode.
// F_LAND		- Navigate towards this waypoint with the throttle off.
// 
// 
// NOTE: Please be very careful when including inverted or hovering legs in a waypoints list.  Even if your plane does not fly well
//       in these orientations, or if you fly these legs without power, the UDB will keep on trying to maintain these orientations
//       which could lead to a crash.  If you try to manually recover from this behavior, remember to switch out of waypoiont mode
//       first, to avoid fighting the stabilization code.
// 
// 
// You do not need to specify how many points you have, the compiler will count them for you.
// You can use the facilities of the compiler to do some simple calculations in defining the course.



// By default the only waypoint is defined to be above the starting point.

const struct waypointDef waypoints[] = {
						{ {  0  ,  0  , 50 } , F_NORMAL } ,  // return to, and circle 50 meters above the startup position
						} ;



// This is an example course that makes a 100 meter square, 75 meters above the starting point, and then lands.
// 
// We first go to the south east corner of the square.
// Then on to the north east corner.
// The plane then uses ailerons to flip upside down, and heads towards the north west corner.
// Then we flip back over and head back to the south west corner.  
// We then turn off the motor and head towards the middle of the square.
// When we fly past the middle, we turn back towards the middle, still without throttle, over and over until "landing".
// 
// Note that this is likely not going to be a very smooth landing...

/*
const struct waypointDef waypoints[] = {
						{ { 100,   0  , 75 } , F_NORMAL } ,
						{ { 100, 100  , 75 } , F_NORMAL } ,
						{ {   0, 100  , 75 } , F_INVERTED } ,
						{ {   0,   0  , 75 } , F_NORMAL } ,
						{ {  50,  50  , 75 } , F_LOITER + F_LAND } ,
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
									
// Here is the T3 course definition:
const struct waypointDef waypoints[] = {
						{ {    CORNER  ,    CORNER  , CLEARANCE + 100 } , F_NORMAL } ,
						{ {    CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL } ,
						{ {  - CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL } ,
						{ {  - CORNER  ,  - CORNER  , CLEARANCE +  25 } , F_NORMAL } ,
						{ {    CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL } ,
						{ {    CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL } ,
						{ {  - CORNER  ,    CORNER  , CLEARANCE + 100 } , F_NORMAL } ,
						{ {  - CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL } ,
						{ {    CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL } ,
						{ {    CORNER  ,  - CORNER  , CLEARANCE +  25 } , F_NORMAL } ,
						{ {  - CORNER  ,    CORNER  , CLEARANCE +  50 } , F_NORMAL } ,
						{ {  - CORNER  ,  - CORNER  , CLEARANCE +  75 } , F_NORMAL } ,
										} ;
*/
