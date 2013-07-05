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
#include "gpsParseCommon.h"
#include "../libUDB/heartbeat.h"


// seconds
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
#define DR_PERIOD ((int16_t)(1.1 * (HEARTBEAT_HZ/GPS_RATE)))
//#warning("disabled drift correction of IMUvelocity and IMUlocation")
//#define DR_PERIOD (0)
#else
#define DR_PERIOD (int16_t)((HEARTBEAT_HZ/GPS_RATE)+4)
#endif

// seconds
#define DR_TIMESTEP (1.0/HEARTBEAT_HZ)

#define MAX16 (4.0*RMAX)

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)

#warning("Including integrate_loc_cm function")

// estimate position in cm, using only GPSloc_cm, GPSvelocity and accelEarth
union longww IMUcmx = { 0 };
union longww IMUcmy = { 0 };
union longww IMUcmz = { 0 };

// estimated velocity
union longww IMUvx = { 0 };
union longww IMUvy = { 0 };
union longww IMUvz = { 0 };

// integral of acceleration
union longww integralAccelx = { 0 };
union longww integralAccely = { 0 };
union longww integralAccelz = { 0 };

fractional cmErrorEarth[]  = { 0, 0, 0 };
fractional velErrorEarth[] = { 0, 0, 0 };

extern struct relative3D GPSloc_cm;

#define A2DV            ((DR_TIMESTEP * GRAVITYM * MAX16) / GRAVITY)

// cm/sec * V2X = centimeters; at 400Hz, V2X = SCALE_VAL/400 in 1.15 fractional form
// result of fractional multiply must be right shifted by SCALE_SHIFT
//FIXME: HEARTBEAT_HZ: SCALE_SHIFT and SCALE_VAL should be calculated from HEARTBEAT_HZ
// optimal SCALE_SHIFT is floor(log2(HEARTBEAT_HZ = 1/DR_TIMESTEP)) (assuming INT_TAU >= 1)
#define SCALE_SHIFT     8
#define SCALE_VAL       256
#define V2X             (SCALE_VAL * DR_TIMESTEP * MAX16)

// seconds
#define INT_TAU         (2.5)
#define INT_TAU_INV     ((unsigned int)(MAX16 / INT_TAU))

// at 400 Hz with 2.5 second integration time constant, filter gain is 1/1000
#define SCALE_SHIFT2    8
#define SCALE_VAL2      256
// 1/seconds^2
#define INT_FILTER_GAIN ((int)((SCALE_VAL2 * DR_TIMESTEP * MAX16) / INT_TAU))

int integrate_clock = DR_PERIOD;

void integrate_loc_cm(void)
{
	if (dcm_flags._.dead_reckon_enable == 1)  // wait for startup of GPS
	{
		// integrate the accelerometers to update IMU velocity
		// accelEarth is acceleration-offset in earth frame
		integralAccelx.WW += __builtin_mulss(((int)(A2DV)), accelEarth[0]);
		integralAccely.WW += __builtin_mulss(((int)(A2DV)), accelEarth[1]);
		integralAccelz.WW += __builtin_mulss(((int)(A2DV)), accelEarth[2]);

		// integrate IMU velocity to update the IMU location
		IMUcmx.WW += (__builtin_mulss(((int)(V2X)), integralAccelx._.W1) >> SCALE_SHIFT);
		IMUcmy.WW += (__builtin_mulss(((int)(V2X)), integralAccely._.W1) >> SCALE_SHIFT);
		IMUcmz.WW += (__builtin_mulss(((int)(V2X)), integralAccelz._.W1) >> SCALE_SHIFT);

		if (integrate_clock > 0)
		// apply drift adjustments only while valid GPS data is in force.
		// this is done with a countdown clock that gets reset each time new data comes in.
		{
			integrate_clock --;

			// without these terms IMUcm doesn't track with GPS
			integralAccelx.WW += (__builtin_mulss(INT_FILTER_GAIN, velErrorEarth[0]) >> SCALE_SHIFT2);
			integralAccely.WW += (__builtin_mulss(INT_FILTER_GAIN, velErrorEarth[1]) >> SCALE_SHIFT2);
			integralAccelz.WW += (__builtin_mulss(INT_FILTER_GAIN, velErrorEarth[2]) >> SCALE_SHIFT2);

			IMUcmx.WW += (__builtin_mulss(INT_FILTER_GAIN, cmErrorEarth[0]) >> SCALE_SHIFT2);
			IMUcmy.WW += (__builtin_mulss(INT_FILTER_GAIN, cmErrorEarth[1]) >> SCALE_SHIFT2);
			IMUcmz.WW += (__builtin_mulss(INT_FILTER_GAIN, cmErrorEarth[2]) >> SCALE_SHIFT2);

			IMUvx.WW = integralAccelx.WW + __builtin_mulus(INT_TAU_INV, cmErrorEarth[0]);
			IMUvy.WW = integralAccely.WW + __builtin_mulus(INT_TAU_INV, cmErrorEarth[1]);
			IMUvz.WW = integralAccelz.WW + __builtin_mulus(INT_TAU_INV, cmErrorEarth[2]);
		}
		else
		{
			IMUvx.WW = integralAccelx.WW;
			IMUvy.WW = integralAccely.WW;
			IMUvz.WW = integralAccelz.WW;
		}

		if (gps_nav_valid() && (dcm_flags._.integrate_req == 1))
		{
			// compute error indications and restart the dead reckoning clock to apply them
			dcm_flags._.integrate_req = 0;
			integrate_clock = DR_PERIOD;

			cmErrorEarth[0] = GPSloc_cm.x - IMUcmx._.W1;
			cmErrorEarth[1] = GPSloc_cm.y - IMUcmy._.W1;
			cmErrorEarth[2] = GPSloc_cm.z - IMUcmz._.W1;

			velErrorEarth[0] = GPSvelocity.x - integralAccelx._.W1;
			velErrorEarth[1] = GPSvelocity.y - integralAccely._.W1;
			velErrorEarth[2] = GPSvelocity.z - integralAccelz._.W1;
		}
	}
	else
	{
		integralAccelx.WW = 0;
		integralAccely.WW = 0;
		integralAccelz.WW = 0;

		IMUcmx.WW = 0;
		IMUcmy.WW = 0;
		IMUcmz.WW = 0;
	}
}

#endif // AIRFRAME_TYPE
