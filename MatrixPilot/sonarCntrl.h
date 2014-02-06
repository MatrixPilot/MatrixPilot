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


#ifndef SONARCNTRL_H
#define SONARCNTRL_H

	// The ff. sonar defines have been temporarily moved to option.h for ease of testing and debugging
	// #define EFFECTV_SONAR_ALTRANGE  		400 // 400 for MAXBOTIX MB1230 and 2200 for MB1260 XL.
	// #define MAXIMUM_SONAR_ALTRANGE      	750	// in centimeters, 750 (7.5 m) for MAXBOTIX MB1230 and 5000 (50 m) for MB1260 XL
	// #define SONAR_MINIMUM_VALREADS		1 	// Def 3, Number of validation readings threshold of a true reading.

	inline int16_t 	get_sonar_rawaglaltitude(void);
	inline int16_t 	get_sonar_aglaltitude(void);

	//int16_t sonar_rawaglaltitude;         		// distance to target in centimeters
	//int16_t sonar_aglaltitude; 					// calculated distance to ground in Earth's Z Plane allowing for tilt
	//uint32_t cos_sonarproll;      				// tilt of the plane in UDB fractional units * 2.
	#define SONAR_PWM_CM_EQCONSTNT		4451 	// 64536.0 / 14.5 (True for Maxbotix devices using PWM of 58 microseconds / centimeter).
	#define SONAR_NOREAD_TKNVAL			99999 	// Optional: return token value for no reading from PWM capture and sonar device.
	void calSonarAGLAltitude(void);

#endif

