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

void udb_barometer_callback(int32_t pressure, int16_t temperature, uint8_t status);
void barometerCalibrate(void);
void estBarometerAltitude(void);

void initFlags(void);

int32_t calcSHABarASLAlt(int32_t pres_rt, int32_t pres_ogn, int16_t temp_rt, int16_t temp_ogn);
int32_t estBarAltOffset(int32_t estaslalt );
int32_t calcFOGAlt(int32_t alt_rt, int32_t alt_ogn);

#endif // BAROMETERCNTRL_H

// GOOFY'S PARKING

/*  **Deprecated**
int32_t calcSMABarASLAlt(int32_t pres_rt, int32_t pres_orgn, int16_t temp_orgn);
int32_t calcBarASLAlt(int32_t pres_rt, int32_t pres_orgn, int16_t temp_orgn);
int32_t calcNScBarASLAlt(int32_t pres_rt);
 *
*/

