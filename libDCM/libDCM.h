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


#ifndef LIB_DCM_H
#define LIB_DCM_H


#include "../libUDB/libUDB.h"
#include "libDCM_defines.h"

////////////////////////////////////////////////////////////////////////////////
// libDCM.h defines the API for accessing the location and orientation information
// from the DCM algorithm and GPS.
// 
// Requires libUDB.
// 
// This library is designed to use libUDB, but to remain independent of the 
// sepcifics of the MatrixPilot application.


////////////////////////////////////////////////////////////////////////////////
// Functions
void dcm_init( void ) ;
void dcm_calibrate(void) ;
void dcm_set_origin_location(long o_long, long o_lat, long o_alt) ;

// Called once each time the GPS reports a new location.
// After dead reckoning is complete, this callback may go away.
void dcm_callback_gps_location_updated(void) ;				// Callback

// Allows disabling yaw drift estimation.
// Starts off enabled.  Call this to disable and to then re-enable.
void dcm_enable_yaw_drift_correction(boolean enabled) ;

// Implement this callback to prepare the pwOut values.
// It is called at 40Hz (once every 25ms).
void dcm_servo_callback_prepare_outputs(void) ;				// Callback

// Convert an absolute location to relative
struct relative3D dcm_absolute_to_relative(struct waypoint3D absolute) ;

// FIXME: This should be handled internally, along with DCM calibration
// Count down from 1000 at 40Hz
void gps_startup_sequence( int gpscount ) ;

// Is our gps data good enough for navigation?
boolean gps_nav_valid(void) ;


// Rotation utility functions
int cosine ( signed char angle ) ;
int sine ( signed char angle ) ;
signed char arcsine ( int y ) ;
signed char rect_to_polar ( struct relative2D *xy ) ;
int rect_to_polar16 ( struct relative2D *xy ) ;
void rotate( struct relative2D *xy , signed char angle ) ;

// integer and long integer square roots
unsigned int sqrt_int ( unsigned int ) ;
unsigned int sqrt_long ( unsigned long int ) ;

// magnitudes of 2 and 3 component vectors
unsigned int vector2_mag( int , int ) ;
unsigned int vector3_mag( int , int , int ) ;


////////////////////////////////////////////////////////////////////////////////
// Vars
extern union dcm_fbts_word { struct dcm_flag_bits _ ; int W ; } dcm_flags ;

// Outside of libDCM, these should all be treated as read-only
extern fractional rmat[] ;
extern fractional omegaAccum[] ;
extern fractional omegagyro[] ;
extern fractional accelEarth[] ;

extern struct relative3D GPSlocation ;
extern struct relative3D GPSvelocity ;
extern struct relative2D velocity_thru_air ; // derived horizontal velocity relative to air in cm/sec
extern int    estimatedWind[3] ;			// wind velocity vectors in cm / sec

extern unsigned int air_speed_3DIMU ;
extern int total_energy ;

extern union longww IMUlocationx , IMUlocationy , IMUlocationz ;
extern union longww IMUvelocityx , IMUvelocityy , IMUvelocityz ;
#define IMUheight IMUlocationz._.W1

extern signed char calculated_heading ; // takes into account wind velocity
extern int gps_data_age ;

extern unsigned int ground_velocity_magnitudeXY ;
extern unsigned int air_speed_magnitudeXY;

extern union longbbbb lat_gps , long_gps , alt_sl_gps ;
extern union longbbbb lat_origin , long_origin , alt_origin ;

#endif
