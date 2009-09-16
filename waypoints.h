
// You have the option of using either cross tracking,
// in which navigation is based on the distance of the plane
// to the line between the waypoints.
// Or you can use navigation directly toward the goal point.
// If you want to use cross tracking, define CROSSTRACKING,
// otherwise, to use navigation directly toward the goal,
// comment out the following #define CROSSTRACKING:

#define CROSSTRACKING

// waypoint definitions

// Define the course as:
// const struct relative3D waypoints[] = {
//						waypoint1 ,
//						waypoint2 ,
//
//						etc.
//							}

// A waypoint is defined as { X , Y , Z } 
// where X, Y, and Z are the three coordinates of the waypoint in meters, 
// relative to the initialization location of the board.
// X is positive as you move east
// Y is positive as you move north
// Z is the height
//
// You do not need to specify how many points you have, the compiler will count them for you.
// You can use the facilities of the compiler to do some simple calculations in defining the course.

// As an example, here is the T3 contest course

// Prior to fligh, initialize the board at the center of the course.

// CORNER is the absolute value of the X or Y coordinate at the corners of the course.
 
#define CORNER 100

// CLEARANCE is an allowance for obstacles.

#define CLEARANCE 25
									
// Here is the T3 course definition:

const struct relative3D waypoints[] = {

						{    CORNER  ,    CORNER  , CLEARANCE + 100 } ,
						{    CORNER  ,  - CORNER  , CLEARANCE +  75 } ,
						{  - CORNER  ,    CORNER  , CLEARANCE +  50 } ,
						{  - CORNER  ,  - CORNER  , CLEARANCE +  25 } ,
						{    CORNER  ,    CORNER  , CLEARANCE +  50 } ,
						{    CORNER  ,  - CORNER  , CLEARANCE +  75 } ,
						{  - CORNER  ,    CORNER  , CLEARANCE + 100 } ,
						{  - CORNER  ,  - CORNER  , CLEARANCE +  75 } ,
						{    CORNER  ,    CORNER  , CLEARANCE +  50 } ,
						{    CORNER  ,  - CORNER  , CLEARANCE +  25 } ,
						{  - CORNER  ,    CORNER  , CLEARANCE +  50 } ,
						{  - CORNER  ,  - CORNER  , CLEARANCE +  75 } ,

										} ;	

// Here is how you can implement return to launch at 50 meters, using a single waypoint:

/*

const struct relative3D waypoints[] = {
						{    0  ,    0  , 50 } ,
										} ;	

*/

