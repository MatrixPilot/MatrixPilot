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


#ifndef DCM_DEFINES_H
#define DCM_DEFINES_H


// Types
struct relative2D { int x ; int y ; } ;
struct absolute2D { long x ; long y ; } ;
struct relative3D { int x ; int y ; int z ; } ;
struct absolute3D { long x ; long y ; long z ; } ;
struct waypoint3D { long x ; long y ; int z ; } ;
struct fixedOrigin3D {long x; long y; float z;} ;

struct dcm_flag_bits {
			unsigned int unused					: 5 ;
			unsigned int gps_history_valid		: 1 ;
			unsigned int dead_reckon_enable		: 1 ;
			unsigned int reckon_req				: 1 ;
			unsigned int first_mag_reading		: 1 ;
			unsigned int mag_drift_req			: 1 ;
			unsigned int yaw_req				: 1 ;
			unsigned int skip_yaw_drift			: 1 ;
			unsigned int nav_capable			: 1 ;
			unsigned int nmea_passthrough		: 1 ; // only used by ublox
			unsigned int init_finished			: 1 ;
			unsigned int calib_finished			: 1 ;
			} ;


// Defines
// GPS Type
#define GPS_STD				1
#define GPS_UBX_2HZ			2
#define GPS_UBX_4HZ			4
#define GPS_MTEK			5

#define GPS_RATE			((GPS_TYPE == GPS_MTEK) ? 4 : GPS_TYPE)

// If GPS data has not been received for this many state machine cycles, consider the GPS lock to be lost.
#define GPS_DATA_MAX_AGE	9

#define LONGDEG_2_BYTECIR 305 // = (256/360)*((256)**4)/(10**7)
#define COURSEDEG_2_BYTECIR 466 // = (256/360)*((256)**2)/(10**2)

#define LONGDEG_2_BYTECIR 305
// = (256/360)*((256)**4)/(10**7)

#define RADPERSEC ((long long)5632.0/SCALEGYRO)
// one radian per second, in AtoD/2 units

#define DEGPERSEC ((long long)98.3/SCALEGYRO)
// one degree per second, in AtoD/2 units

#define GRAVITYM ((long long)980.0) 
// 100 times gravity, meters/sec/sec

#define ACCELSCALE ((long) ( GRAVITY/GRAVITYM ) )

#define CENTRISCALE ((long) (((long long)519168.0)*GRAVITY)/((long long)RADPERSEC*GRAVITYM))
// scale factor in multiplying omega times velocity to get centrifugal acceleration

#define CENTRIFSAT ((long) (GRAVITYM*RADPERSEC)/(GRAVITY*((long long)32)))
// saturation limit for the centrifugal adjustment to avoid numeric overflow

#define WIND_NAV_AIR_SPEED_MIN			200		// Minimum airspeed in cm/sec for wind navigation to apply
#define GPS_SPEED_MIN					150		// Minimum ground speed in cm/sec to use GPS for yaw drift compensation

#endif
