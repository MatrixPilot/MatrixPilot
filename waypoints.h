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


// By default the only waypoint is defined to be above the starting point.

const struct relative3D waypoints[] = {
						{  0  ,  0  , 50 } ,  // return to, and circle 50 meters above the startup position
						} ;



/*
// As a more complex example, here is the DIY Drones T3-2 contest course

// Prior to flight, initialize the board at the center of the course.

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

*/
