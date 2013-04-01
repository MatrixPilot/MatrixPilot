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
struct relative2D { int16_t x ; int16_t y ; } ;
struct absolute2D { int32_t x ; int32_t y ; } ;
struct relative3D { int16_t x ; int16_t y ; int16_t z ; } ;
struct absolute3D { int32_t x ; int32_t y ; int32_t z ; } ;
struct waypoint3D { int32_t x ; int32_t y ; int16_t z ; } ;
struct fixedOrigin3D {int32_t x; int32_t y; float z;} ;

struct dcm_flag_bits {
			uint16_t unused					: 4 ;
			uint16_t rollpitch_req			: 1 ;
			uint16_t gps_history_valid		: 1 ;
			uint16_t dead_reckon_enable		: 1 ;
			uint16_t reckon_req				: 1 ;
			uint16_t first_mag_reading		: 1 ;
			uint16_t mag_drift_req			: 1 ;
			uint16_t yaw_req				: 1 ;
			uint16_t skip_yaw_drift			: 1 ;
			uint16_t nav_capable			: 1 ;
			uint16_t nmea_passthrough		: 1 ; // only used by ublox
			uint16_t init_finished			: 1 ;
			uint16_t calib_finished			: 1 ;
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

#define RADPERSEC ((int64_t)5632.0/SCALEGYRO)
// one radian per second, in AtoD/2 units

#define DEGPERSEC ((int64_t)98.3/SCALEGYRO)
// one degree per second, in AtoD/2 units

#define GRAVITYM ((int64_t)980.0) 
// 100 times gravity, meters/sec/sec

#define ACCELSCALE ((int32_t) ( GRAVITY/GRAVITYM ) )

#define CENTRISCALE ((int32_t) (((int64_t)519168.0)*GRAVITY)/((int64_t)RADPERSEC*GRAVITYM))
// scale factor in multiplying omega times velocity to get centrifugal acceleration

#define CENTRIFSAT ((int32_t) (GRAVITYM*RADPERSEC)/(GRAVITY*((int64_t)32)))
// saturation limit for the centrifugal adjustment to avoid numeric overflow

#define WIND_NAV_AIR_SPEED_MIN			200		// Minimum airspeed in cm/sec for wind navigation to apply
#define GPS_SPEED_MIN					150		// Minimum ground speed in cm/sec to use GPS for yaw drift compensation

#endif
