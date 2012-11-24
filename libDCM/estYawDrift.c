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


#include "libDCM_internal.h"

//	Compute actual and desired courses.
//	Actual course is simply the scaled GPS course over ground information.
//	Desired course is a "return home" course, which is simply the negative of the
//	angle of the vector from the origin to the location of the plane.

//	The origin is recorded as the location of the plane during power up of the control.


#if (MAG_YAW_DRIFT == 1)
void udb_magnetometer_callback_data_available( void )
{
	dcm_flags._.mag_drift_req = 1 ;
	return ;
}
#endif


void dcm_enable_yaw_drift_correction(boolean enabled)
{
	dcm_flags._.skip_yaw_drift = !enabled;
	return ;
}

extern signed char actual_dir ;
extern signed char calculated_heading ;

void estYawDrift(void)
{
	
	// Don't update Yaw Drift while hovering, since that doesn't work right yet
	if ( gps_nav_valid() && !dcm_flags._.skip_yaw_drift )
	{
		if ((estimatedWind[0] == 0 && estimatedWind[1] == 0) || air_speed_magnitudeXY < WIND_NAV_AIR_SPEED_MIN   )
		{
			dirovergndHGPS[0] = -cosine(actual_dir) ;
			dirovergndHGPS[1] = sine(actual_dir) ;
			dirovergndHGPS[2] = 0 ;
		}
		else
		{
			dirovergndHGPS[0] = -cosine(calculated_heading) ;
			dirovergndHGPS[1] = sine(calculated_heading) ;
			dirovergndHGPS[2] = 0 ;
		}
	}

	return ;
}
