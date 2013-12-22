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
extern int16_t get_sonar_value(void);   	// Get the raw pwm units from the sonar device driver

#if (USE_SONAR == 1 && HILSIM != 1)

	int16_t sonar_rawaglaltitude;         	// distance to target in centimeters
	int16_t sonar_aglaltitude; 				// calculated distance to ground in Earth's Z Plane allowing for tilt
	static fractional cos_pitch_roll;      	// tilt of the plane in UDB fractional units * 2.
	uint32_t cos_sonarproll;      			// tilt of the plane in UDB fractional units * 2.

	// The ff. sonar defines were move to option.h
	// #define EFFECTV_SONAR_ALTRANGE  	400 	// 400 for MAXBOTIX MB1230 and 2200 for MB1260 XL.
	// #define MAXIMUM_SONAR_ALTRANGE      750 	// in centimeters, 750 (7.5 m) for MAXBOTIX MB1230 and 5000 (50 m) for MB1260 XL
	// #define SONAR_MINIMUM_VALREADS		1 		// Def 3, Number of validation readings threshold of a true reading.

	#define SONAR_PWM_CM_EQCONSTNT		4451 	// 64536.0 / 14.5 (True for Maxbotix devices using PWM of 58 microseconds / centimeter).
	#define SONAR_NOREAD_TKNVAL			99999 	// Optional: return token value for no reading from PWM capture and sonar device.
	extern int16_t udb_pwm_sonar;					// Raw pwm units from sonar device, from radioIn.c
	static uint8_t valReadIndex  = 0;  		// valid reads index, loop until SONAR_MINIMUM_VALREADS is reached.
	static uint8_t noReadIndex = 0;   			// tracks number of no readings from sonar


	void calSonarAGLAltitude(void)
	{
		if (udb_flags._.sonar_updated == 1) 
		{	
			union longbbbb accum;
	
			noReadIndex  = 0;
			accum.WW = __builtin_mulss(get_sonar_value(), SONAR_PWM_CM_EQCONSTNT) + 32768;
			sonar_rawaglaltitude = accum._.W1;
			// RMAT 8 is the cosine of the tilt of the plane in pitch and roll	;
			cos_pitch_roll = rmat[8];
			if (cos_pitch_roll > 16383)
			{
				cos_pitch_roll = 16383;
			}
			if (sonar_rawaglaltitude > EFFECTV_SONAR_ALTRANGE)
			{
				sonar_aglaltitude = MAXIMUM_SONAR_ALTRANGE;
				valReadIndex = 0; 
			}
			else 
			{
				valReadIndex++;
				if (valReadIndex > SONAR_MINIMUM_VALREADS) 
				{
					valReadIndex = SONAR_MINIMUM_VALREADS;
					cos_pitch_roll = cos_pitch_roll <<1 ; 
					accum.WW = __builtin_mulss(cos_pitch_roll, sonar_rawaglaltitude) ;
					sonar_aglaltitude = accum._.W1 <<1 ; 
				}
				else
				{
					sonar_aglaltitude = MAXIMUM_SONAR_ALTRANGE;
				}
			}
			udb_flags._.sonar_updated = 0;
			udb_flags._.sonar_print_telemetry = 1;
		}
		else
		{
			if (noReadIndex < 7)  // This assumes runnig at 40HZ UDB frame rate
			{
			 	noReadIndex++;
			}
			else
			{
		    	sonar_aglaltitude = (int16_t)SONAR_NOREAD_TKNVAL;
			}
		}
	}
	
	#else
	void calSonarAGLAltitude(void)
	{
	}
#endif // USE_SONAR	

/*  template:

#if (USE_SONAR == 1)

	int16_t sonar_rawaglaltitude;         	// distance to target in centimeters
	int16_t sonar_aglaltitude; 				// calculated distance to ground in Earth's Z Plane allowing for tilt
	static fractional cos_pitch_roll;      	// tilt of the plane in UDB fractional units * 2.
	uint32_t cos_sonarproll;      			// tilt of the plane in UDB fractional units * 2.
	
	
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

	void calSonarAGLAltitude(void)
	{
		if (udb_flags._.sonar_updated == 1) 
		{	
			union longbbbb accum;
	
			no_readings_count  = 0;
			accum.WW = __builtin_mulss(get_sonar_value(), UDB_SONAR_PWM_UNITS_TO_CENTIMETERS) + 32768;
			sonar_rawaglaltitude = accum._.W1;
			// RMAT 8 is the cosine of the tilt of the plane in pitch and roll	;
			cos_pitch_roll = rmat[8];
			if (cos_pitch_roll > 16383)
			{
				cos_pitch_roll = 16383;
			}
			if (sonar_rawaglaltitude > USEABLE_SONAR_DISTANCE)
			{
				sonar_aglaltitude = OUT_OF_RANGE_DISTANCE;
				good_sample_count = 0; 
			}
			else 
			{
				good_sample_count++;
				if (good_sample_count > SONAR_SAMPLE_THRESHOLD) 
				{
					good_sample_count = SONAR_SAMPLE_THRESHOLD;
					accum.WW = __builtin_mulss(cos_pitch_roll, sonar_rawaglaltitude);
					sonar_aglaltitude = accum._.W1 << 2; 
				}
				else
				{
					sonar_aglaltitude = OUT_OF_RANGE_DISTANCE;
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
		    	sonar_aglaltitude = NO_READING_RECEIVED_DISTANCE;
			}
		}
		#if SERIAL_OUTPUT_FORMAT == SERIAL_UDB_SONAR 
			cos_sonarproll = 0;
			cos_sonarproll = (uint32_t)cos_pitch_roll;
		#endif
	}
	
#else
	void calSonarAGLAltitude(void)
	{
	}

#endif // USE_SONAR
*/

