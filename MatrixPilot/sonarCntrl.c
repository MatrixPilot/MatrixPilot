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
#include "sonarCntrl.h"
#include "../libUDB/sonarIn.h"
#include "../libDCM/estAltitude.h"
#include "../libDCM/rmat.h"

#if (USE_SONAR_INPUT != 0)

int16_t sonar_distance;             // distance to target in centimeters
int16_t sonar_height_to_ground;     // calculated distance to ground in Earth's Z Plane allowing for tilt
static fractional cos_pitch_roll;   // tilt of the plane in UDB fractional units * 2.


// USEABLE_SONAR_DISTANCE may well vary with type of ground cover (e.g. long grass may be less).
// Pete Hollands ran the code with #define SERIAL_OUTPUT SERIAL_UDB_SONAR while flying low
// over his landing area, which was a freshly cut straw field. Post flight, he anlaysed the CSV telemetry into a spreadsheet graph,
// and determined that all measurements below 4 meters were true, as long as there were at least 3 consecutive measurements,
// that were less than 4 meters (400 centimeters).
#define USEABLE_SONAR_DISTANCE               400 // Reliable Sonar measurement distance (centimeters) for your specific landing area.
#define OUT_OF_RANGE_DISTANCE                750 // Distance in centimeters that denotes "out of range" for your Sonar device.
#define NO_READING_RECEIVED_DISTANCE        9999 // Distance denotes that no sonar reading was returned from sonar device
#define SONAR_SAMPLE_THRESHOLD                 3 // Number of readings before code deems "certain" of a true reading.
#define UDB_SONAR_PWM_UNITS_TO_CENTIMETERS  4451 // 64536.0 / 14.5 (True for Maxbotix devices using PWM of 58 microseconds / centimeter).

static uint8_t good_sample_count = 0;   // Tracks the number of consequtive good samples up until SONAR_SAMPLE_THRESHOLD is reached.
static uint8_t no_readings_count = 0;   // Tracks number of UDB frames since last sonar reading was sent by sonar device

void calculate_sonar_height_above_ground(void)
{
	if (udb_flags._.sonar_updated == 1) 
	{	
		union longbbbb accum;

		no_readings_count  = 0;
		accum.WW = __builtin_mulss(get_sonar_value(), UDB_SONAR_PWM_UNITS_TO_CENTIMETERS) + 32768;
		sonar_distance = accum._.W1;
		// RMAT 8 is the cosine of the tilt of the plane in pitch and roll	;
		cos_pitch_roll = rmat[8];
		if (cos_pitch_roll > 16383)
		{
			cos_pitch_roll = 16383;
		}
		if (sonar_distance > USEABLE_SONAR_DISTANCE)
		{
			sonar_height_to_ground = OUT_OF_RANGE_DISTANCE;
			good_sample_count = 0; 
		}
		else 
		{
			good_sample_count++;
			if (good_sample_count > SONAR_SAMPLE_THRESHOLD) 
			{
				good_sample_count = SONAR_SAMPLE_THRESHOLD;
				accum.WW = __builtin_mulss(cos_pitch_roll, sonar_distance);
				sonar_height_to_ground = accum._.W1 << 2; 
			}
			else
			{
				sonar_height_to_ground = OUT_OF_RANGE_DISTANCE;
			}
		}
		udb_flags._.sonar_updated = 0;
		udb_flags._.sonar_print_telemetry = 1;
	}
	else
	{
		if (no_readings_count < 7)  // This assumes runnig at 40HZ UDB frame rate
		{
			no_readings_count++;
		}
		else
		{
			sonar_height_to_ground = NO_READING_RECEIVED_DISTANCE;
		}
	}
}

#else

void calculate_sonar_height_above_ground(void)
{
}

#endif // USE_SONAR_INPUT
