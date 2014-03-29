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
#define WAYPOINT_RADIUS         25

#define CAM_VIEW_LAUNCH         { 0, 0, 0 }

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

#define USE_FIXED_ORIGIN        0
//#define FIXED_ORIGIN_LOCATION   { -1219950467, 374124664, 30.0 }    // A point in Baylands Park in Sunnyvale, CA

//#define USE_FIXED_ORIGIN        1
#define FIXED_ORIGIN_LOCATION   { 113480854, 472580108, 578 }    // Innsbruck

//printf("o_lon %li o_lat %li o_alt %li\r\n", o_lon, o_lat, o_alt); // o_lon 113480854 o_lat 472580108 o_alt 57763


////////////////////////////////////////////////////////////////////////////////
// Waypoint definitions
// 
// Define the main course as:
// 
// const struct waypointDef waypoints[] = {
//                      waypoint1 ,
//                      waypoint2 ,
//                      etc.
//                      };
// 
// and the Failsafe RTL course as:
// 
// const struct waypointDef rtlWaypoints[] = {
//                      waypoint1 ,
//                      waypoint2 ,
//                      etc.
//                      };
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
// (Longitude = -121.9950467 degrees, Latitude = 37.4124664 degrees.)
// 
// Currently F can be set to: F_NORMAL, or any combination of:
// F_ABSOLUTE       - Waypoints are Relative by default, unless F_ABSOLUTE is specified.
// 
// F_TAKEOFF        - More quickly gain altitude at takeoff.
// F_INVERTED       - Navigate to this waypoint with the plane upside down. (only if STABILIZE_INVERTED_FLIGHT is set to 1 in options.h)
// F_HOVER          - Hover the plane until reaching this waypoint. (only if STABILIZE_HOVER is set to 1 in options.h)
//                    NOTE: while hovering, no navigation is performed, and throttle is under manual control.
// F_LOITER         - After reaching this waypoint, continue navigating towards this same waypoint.  Repeat until leaving waypoint mode.
// F_TRIGGER        - Trigger an action to happen when this waypoint leg starts.  (See the Trigger Action section of the options.h file.) 
// F_ALTITUDE_GOAL  - Climb or descend to the given altitude, then continue to the next waypoint.
// F_CROSS_TRACK    - Navigate using cross-tracking.  Best used for longer waypoint legs.
// F_LAND           - Navigate towards this waypoint with the throttle off.
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
// plane, then the camera viewpoint should also be relative e.g. "{ 32 , -22, 0)".
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

/*
const struct waypointDef waypoints[] = {
		{ {   0,   0, 75 } , F_NORMAL, CAM_VIEW_LAUNCH } ,  // return to, and loiter 75 meters above the startup position
};
 */

/*
const struct waypointDef waypoints[] = {
		{ { 1000,    0  , 750 } , F_NORMAL,   CAM_VIEW_LAUNCH } ,
		{ { 1000, 1000  , 750 } , F_NORMAL,   CAM_VIEW_LAUNCH } ,
		{ {    0, 1000  , 750 } , F_INVERTED, CAM_VIEW_LAUNCH } ,
		{ {    0,    0  , 750 } , F_NORMAL,   CAM_VIEW_LAUNCH } ,
		{ {   50,   50  , 750 } , F_LOITER + F_TRIGGER + F_LAND, CAM_VIEW_LAUNCH } ,
};
 */

 // CORNER is the absolute value of the X or Y coordinate at the corners of the course. 
#define CORNER 1000

// CLEARANCE is an allowance for obstacles.
#define CLEARANCE 350

#define CAM_VIEW_2  { CORNER, CORNER, 0 } // Define a Camera ViewPoint to look at 100 ,100, 0

// Here is the T3 course definition:
/*
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
};
 */
/*
//const struct waypointDef waypoints[] = {
//		{ {    CORNER  ,    CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {    CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {  - CORNER  ,    CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {  - CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//};
//const struct waypointDef waypoints[] = {
//		{ {  - CORNER  ,    CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {  - CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {    CORNER  ,    CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {    CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//};

//const struct waypointDef waypoints[] = {
//		{ {  - CORNER  ,    CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {    CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {    2000,        CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {  - CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//};

//const struct waypointDef waypoints[] = {
//		{ {  - CORNER  ,    CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {    CORNER  ,    CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {  - CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//		{ {    CORNER  ,  - CORNER  , CLEARANCE } , F_NORMAL, CAM_VIEW_LAUNCH } ,
//};
 */

// Innsbruck inverted/reversed T3 course
const struct waypointDef waypoints[] = {
		{ {  113236490, 472638400, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH } ,
		{ {  113579820, 472496320, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH } ,
		{ {  113579820, 472638400, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH } ,
		{ {  113236490, 472496320, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH } ,
};

/*
#if 0
const struct waypointDef waypoints[] = {
		{ { 1749200000, -370100000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Auckland, New Zealand
		{ {-1495000000, -175000000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Papeete, Tahiti

		{ { -796000000,  89200000,  350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Balboa, Panama
};

#else
const struct waypointDef waypoints[] = {
{{ -1084960970, -14971790, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1085621970, -15479560, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1086944070, -16495040, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1088266300, -17510430, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1089588680, -18525730, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1090911210, -19540920, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1092233890, -20556020, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1093556750, -21571000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1094879780, -22585870, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1096203000, -23600620, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1097526410, -24615250, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1104146650, -29686290, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1110772970, -34753370, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1124048000, -44872930, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1137359850, -54968430, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1150716840, -65034350, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1164127360, -75065080, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1177599760, -85054970, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1191142440, -94998290, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1204763800, -104889190, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1218472240, -114721770, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1288615200, -162793860, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1362189450, -208443970, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1440190010, -250764450, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1523469450, -288732190, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1612578750, -321219090, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1707548960, -347044630, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1792331240, -365084960, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1688623680, -374433830, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1583599560, -374575810, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1479815420, -365502790, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1495000000, -175000000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Papeete, Tahiti
};
#endif
 */
/*
// Great Circle path from Balboa, Panama to Papeete, Tahiti
const struct waypointDef waypoints[] = {
{{ -796000000, 89200000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Balboa, Panama
{{ -835161020, 73919350, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -874049130, 58299710, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -912719640, 42414930, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -951230580, 26338510, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -989641910, 10143610, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1028014780, -6096800, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1066410780, -22309860, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1104891220, -38422650, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1143516430, -54362060, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1182345010, -70054690, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1221433010, -85426770, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1260833090, -100404140, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1300593610, -114912270, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1340757590, -128876450, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1381361710, -142221950, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1422435170, -154874430, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1463998610, -166760370, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1495000000, -175000000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Papeete, Tahiti
};
 */
/*
// Great Circle path from Agena to Auckland, New Zealand
const struct waypointDef waypoints[] = {
{{ -1110659870, -21779540, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1111319780, -22289050, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1112639740, -23307990, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1113959900, -24326800, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1115280250, -25345490, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1116600810, -26364040, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1117921590, -27382450, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1119242600, -28400710, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1120563830, -29418830, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1121885310, -30436780, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1123207030, -31454580, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1126512500, -33998320, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1129819710, -36540940, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1133128790, -39082340, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1136439880, -41622450, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1143068590, -46698400, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1149706900, -51768130, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1156355860, -56830940, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1163016510, -61886130, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1169689910, -66933000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1176377100, -71970860, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1189797110, -82016700, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1203284960, -92017930, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1216849110, -101968760, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1230498040, -111863290, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1244240220, -121695530, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1314646440, -169703280, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1388645960, -215174160, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1467233420, -257182670, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1551245570, -294685100, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1641190980, -326533950, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1737022000, -351537510, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1762086750, -368577930, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{  1749200000, -370100000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Auckland, New Zealand
};
 */
/*
// Great Circle path from Auckland, New Zealand to Agena
const struct waypointDef waypoints[] = {
{{  1749200000, -370100000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Auckland, New Zealand
{{ -1799618430, -362986430, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1749498190, -353735220, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1700635020, -342465080, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1653168210, -329310830, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1607181590, -314417500, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1562708130, -297935070, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1519736800, -280014060, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1478220460, -260802130, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1438083680, -240441730, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1399229940, -219068600, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1361547760, -196810970, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1324915680, -173789440, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1289206100, -150117210, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1254287990, -125900620, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1220028900, -101239980, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1186296070, -76230390, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1152957120, -50962790, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ -1119880280, -25524940, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
};
 */
/*
// Great Circle path from Rockhampton, to waypoint near north cape, New Zealand (Agena)
const struct waypointDef waypoints[] = {
{{ 1505000000, -235000000, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH }, // Rockhampton, Australia
{{ 1543362510, -257816480, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1583194260, -279588400, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1624619430, -300167570, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1667740580, -319395980, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1712629250, -337107220, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
{{ 1759315090, -353129070, 350 } , F_ABSOLUTE, CAM_VIEW_LAUNCH },
};
 */

// As an example, the absolute waypoint { { -1219950467, 374124664, 100 }, F_ABSOLUTE } represents a point
// 100 meters above Baylands Park in Sunnyvale, CA, and will fly there normally (not inverted, etc.)
// (Longitude = -121.9950467 degrees, Latitude = 37.4124664 degrees.)


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
};


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
};
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
};
*/
