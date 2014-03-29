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


#include "libDCM.h"
#include "gpsParseCommon.h"
#include "mathlibNAV.h"
#include "estWind.h"


#if (WIND_ESTIMATION == 1)

static vect3_16t groundVelocityHistory = { 0, 0, 0 };
static vect3_16t fuselageDirectionHistory = { 0, 0, 0 };

#define MINROTATION ((int16_t)(0.2 * RMAX))

void estWind(vect3_16t* est_wind, struct relative3D gps_velocity)
{
	vect3_16t groundVelocity;
	vect3_16t groundVelocitySum;
	vect3_16t groundVelocityDiff;
	vect3_16t fuselageDirection;
	vect3_16t fuselageDirectionSum;
	vect3_16t fuselageDirectionDiff;
	uint16_t magVelocityDiff;
	uint16_t magDirectionDiff;
	int8_t angleVelocityDiff;
	int8_t angleDirectionDiff;
	int8_t thetaDiff;
	int16_t costhetaDiff;
	int16_t sinthetaDiff;
	union longww longaccum;
	struct relative2D xy;
	uint16_t estimatedAirspeed;

	if (dcm_flags._.skip_yaw_drift) return;

	groundVelocity.x = gps_velocity.x >>= 1;
	groundVelocity.y = gps_velocity.y >>= 1;
	groundVelocity.z = gps_velocity.z >>= 1;

	fuselageDirection.x = -(rmat[1] >>= 1);
	fuselageDirection.y =  (rmat[4] >>= 1);
	fuselageDirection.z = -(rmat[7] >>= 1);

//	for (index = 0; index < 3; index++)
//	{
//		groundVelocity >>= 1;
//		fuselageDirection >>= 1;
		groundVelocitySum.x  = groundVelocity.x + groundVelocityHistory.x;
		groundVelocitySum.y  = groundVelocity.y + groundVelocityHistory.y;
		groundVelocitySum.z  = groundVelocity.z + groundVelocityHistory.z;
		groundVelocityDiff.x = groundVelocity.x - groundVelocityHistory.x;
		groundVelocityDiff.y = groundVelocity.y - groundVelocityHistory.y;
		groundVelocityDiff.z = groundVelocity.z - groundVelocityHistory.z;
		fuselageDirectionSum.x  = fuselageDirection.x + fuselageDirectionHistory.x;
		fuselageDirectionSum.y  = fuselageDirection.y + fuselageDirectionHistory.y;
		fuselageDirectionSum.z  = fuselageDirection.z + fuselageDirectionHistory.z;
		fuselageDirectionDiff.x = fuselageDirection.x - fuselageDirectionHistory.x;
		fuselageDirectionDiff.y = fuselageDirection.y - fuselageDirectionHistory.y;
		fuselageDirectionDiff.z = fuselageDirection.z - fuselageDirectionHistory.z;
//	}

	xy.x = fuselageDirectionDiff.x;
	xy.y = fuselageDirectionDiff.y;
	angleDirectionDiff = rect_to_polar(&xy);

	xy.x = groundVelocityDiff.x;
	xy.y = groundVelocityDiff.y;
	angleVelocityDiff = rect_to_polar(&xy);

	thetaDiff = angleVelocityDiff - angleDirectionDiff;
	costhetaDiff = cosine(thetaDiff);
	sinthetaDiff = sine(thetaDiff);

	magDirectionDiff = vector3_mag(fuselageDirectionDiff.x,
	                               fuselageDirectionDiff.y,
	                               fuselageDirectionDiff.z);

	magVelocityDiff = vector3_mag(groundVelocityDiff.x,
	                              groundVelocityDiff.y,
	                              groundVelocityDiff.z);

	if (magDirectionDiff > MINROTATION)
	{
		longaccum._.W1 = magVelocityDiff >> 2;
		longaccum._.W0 = 0;
#if (HILSIM == 1)
		estimatedAirspeed = hilsim_airspeed.BB; // use the simulation as a pitot tube
#else
		estimatedAirspeed = __builtin_divud(longaccum.WW, magDirectionDiff);
#endif
		longaccum.WW = (__builtin_mulss(costhetaDiff, fuselageDirectionSum.x)
		              - __builtin_mulss(sinthetaDiff, fuselageDirectionSum.y)) << 2;
		longaccum.WW = (__builtin_mulus(estimatedAirspeed, longaccum._.W1)) << 2;
		est_wind->x = est_wind->x + 
		    ((groundVelocitySum.x - longaccum._.W1 - est_wind->x) >> 4);

		longaccum.WW = (__builtin_mulss(sinthetaDiff, fuselageDirectionSum.x)
		              + __builtin_mulss(costhetaDiff, fuselageDirectionSum.y)) << 2;
		longaccum.WW = (__builtin_mulus(estimatedAirspeed, longaccum._.W1)) << 2;
		est_wind->y = est_wind->y +
		    ((groundVelocitySum.y - longaccum._.W1 - est_wind->y) >> 4);

		longaccum.WW = (__builtin_mulus(estimatedAirspeed, fuselageDirectionSum.z)) << 2;
		est_wind->z = est_wind->z +
		((groundVelocitySum.z - longaccum._.W1 - est_wind->z) >> 4);

//		for (index = 0; index < 3; index++)
//		{
			groundVelocityHistory = groundVelocity;
			fuselageDirectionHistory = fuselageDirection;
//		}
	}
}

#else

void estWind(vect3_16t* est_wind, struct relative3D gps_velocity)
{
	est_wind->x = 0;
	est_wind->y = 0;
	est_wind->z = 0;
}

#endif // WIND_ESTIMATION
