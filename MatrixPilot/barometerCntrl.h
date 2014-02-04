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


#ifndef BAROMETERCNTRL_H
#define BAROMETERCNTRL_H

void udb_barometer_callback(int32_t pressure, int16_t temperature, int8_t status);
void barometerCalibrate(void);
void barometerCalibrationUpdate(void);
void estBarometerAltitude(void);
void initOriginVars(void);
int32_t setBarOriginAlt(float barpres,int16_t bartemp);

inline float get_barometer_pressure(void);
inline int16_t get_barometer_temperature(void);
inline float get_barometer_pressureorgn(void);
inline int16_t get_barometer_temperatureorgn(void);
inline int32_t get_barometer_altitudeorgn(void);
inline float get_barometer_rtavepressure(void);
inline int16_t get_barometer_rtavetemperature(void);
inline int32_t get_barometer_aslaltitude(void);

int32_t calcSHABarASLAlt(float barpres,int16_t bartemp,float gnd_barpres,int16_t gnd_bartemp);
int32_t calcSMABarASLAlt(float barpres,int16_t bartemp,float gnd_barpres,int16_t gnd_bartemp);
int32_t calcNScBarASLAlt(float barpres);
int32_t calcBarASLAlt(float barpres,int16_t bartemp,float gnd_barpres,int16_t gnd_bartemp);


#endif // BAROMETERCNTRL_H

