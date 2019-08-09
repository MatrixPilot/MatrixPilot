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


#include "defines.h"
#include "alt_agl.h"
#include "../libUDB/rangeIn.h"
#include "../libDCM/rmat.h"

#if (USE_RANGER_INPUT != 0)

int16_t range_to_target;               // distance to target in centimeters
int16_t height_above_ground_level;     // calculated distance to ground in Earth's Z Plane allowing for tilt
static fractional cos_pitch_roll;      // tilt of the plane in UDB fractional units * 2.


#if ( RANGER_TYPE == RANGER_LIDAR )
// USEABLE_RANGER_DISTANCE for Lidar
#define USEABLE_RANGER_DISTANCE               2000 // Reliable Lidar measurement distance (centimeters) for your specific landing area.
#define OUT_OF_RANGE_DISTANCE                 3000 // Distance in centimeters that denotes "out of range" for your Lidar device.
#define NO_READING_RECEIVED_DISTANCE          3100 // Distance denotes that no reliable range reading was returned from device
#define RANGER_SAMPLE_THRESHOLD                  1 // Number of readings before code deems "certain" of a true reading.
#define UDB_RANGER_PWM_UNITS_TO_CENTIMETERS  26246 // 64536.0 / 2.497 (Garmin Lidar LiteV3HP using PWM of 10 microseconds / centimeter).

#elif (RANGER_TYPE == RANGER_SONAR)
// USEABLE_RANGER_DISTANCE may well vary with type of ground cover (e.g. long grass may be less).
// Pete Hollands ran the code with #define SERIAL_OUTPUT SERIAL_UDB_SONAR while flying low
// over his landing area, which was a freshly cut straw field. Post flight, he anlaysed the CSV telemetry into a spreadsheet graph,
// and determined that all measurements below 4 meters were true, as long as there were at least 3 consecutive measurements,
// that were less than 4 meters (400 centimeters).
#define USEABLE_RANGER_DISTANCE               400 // Reliable Sonar measurement distance (centimeters) for your specific landing area.
#define OUT_OF_RANGE_DISTANCE                 750 // Distance in centimeters that denotes "out of range" for your Sonar device.
#define NO_READING_RECEIVED_DISTANCE         9999 // Distance denotes that no sonar reading was returned from sonar device
#define RANGER_SAMPLE_THRESHOLD                 3 // Number of readings before code deems "certain" of a true reading.
#define UDB_RANGER_PWM_UNITS_TO_CENTIMETERS  4451 // 64536.0 / 14.5 (True for Maxbotix devices using PWM of 58 microseconds / centimeter).

#else
#error  'incorrectly specified RANGER_TYPE'
#endif

static uint8_t good_sample_count = 0;   // Tracks the number of consequtive good samples up until SONAR_SAMPLE_THRESHOLD is reached.
static uint8_t no_readings_count = 0;   // Tracks number of UDB frames since last sonar reading was sent by sonar device

void calculate_height_above_ground_level(void)
{
	if (udb_flags._.range_updated == 1) 
	{	
		union longbbbb accum;

		no_readings_count  = 0;
		accum.WW = __builtin_mulss(get_range_value(), UDB_RANGER_PWM_UNITS_TO_CENTIMETERS) + 32768;
		range_to_target = accum._.W1;
		// RMAT 8 is the cosine of the tilt of the plane in pitch and roll	;
		cos_pitch_roll = rmat[8]; // rmat[8] can change in another thread of execution
		if (cos_pitch_roll < 8192) // tilt >  45 degrees
		{
			height_above_ground_level = OUT_OF_RANGE_DISTANCE;
            return;
		}
		if (range_to_target > USEABLE_RANGER_DISTANCE)
		{
			height_above_ground_level = OUT_OF_RANGE_DISTANCE;
			good_sample_count = 0; 
		}
		else 
		{
			good_sample_count++;
			if (good_sample_count > RANGER_SAMPLE_THRESHOLD) 
			{
				good_sample_count = RANGER_SAMPLE_THRESHOLD;
				accum.WW = (__builtin_mulss(cos_pitch_roll, range_to_target) + 8192)<<2;
				height_above_ground_level = accum._.W1; 
			}
			else
			{
				height_above_ground_level = OUT_OF_RANGE_DISTANCE;
			}
		}
		udb_flags._.range_updated = 0;
	}
	else
	{
		if (no_readings_count < 7)  // This assumes running at 40HZ UDB frame rate
		{
			no_readings_count++;
		}
		else
		{
			height_above_ground_level = NO_READING_RECEIVED_DISTANCE;
		}
	}
}

#else

void calculate_height_above_ground_level(void)
{
}

#endif // USE_RANGER_INPUT
