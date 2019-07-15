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

/**/
#include "../MatrixPilot/defines.h"
#include "../Config/options.h"
#include "Lidar.h"
#include "libUDB_internal.h"
#include "ConfigUDB5.h"
#include "../libDCM/rmat.h"
int32_t lidar0 = 0;// LidarLite value for a null altitude in half millimeters
lidar_callback_funcptr lidar_callback = NULL;
void rxLidar_on_PWM(lidar_callback_funcptr callback)  // service the Lidar altimeter on PWM
{
    lidar_callback = callback;
    LIDAR_Trigger = 0;//Trig the Lidar Lite PWM output
    fractional cos_pitch_roll ;  // tilt of the plane in UDB fractional units * 2.
	union longbbbb accum ;
	long altitude;  // in mm
        // If measurement less than 15 m then altitude could be valid
        accum.WW = udb_pwIn[7]  - (lidar0 << 1);// Offset correction for the LidarLite (mm);
                // RMAT 8 is the cosine of the tilt of the plane in pitch and roll	;
		cos_pitch_roll = rmat[8] ;
		if ( cos_pitch_roll > 16383 )
		{
			cos_pitch_roll = 16383 ;
		}
                
        if (accum.WW < 30000) {
		   accum.WW = __builtin_mulsu(cos_pitch_roll, accum._.W0) ;
		   altitude = (long)((accum._.W1 << 1) );
                    //raw uncorrected altitude :altitude = ((long)udb_pwIn[7] >> 1)
                }
		if (lidar_callback != NULL)
		{
			lidar_callback(altitude);   // Callback
            udb_flags._.lidar_updated = 1;
		}
        else
            udb_flags._.lidar_updated = 0;
            
}


