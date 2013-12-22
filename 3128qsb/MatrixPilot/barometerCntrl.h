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

void udb_barometer_callback(long pressure, int temperature, char status);
void barometerCalOne(void);
void barometerCalTwo(void);
void barometerCalVal(void);
void estBarometerAltitude(void);


inline long get_barometer_aslaltitude(void);
inline long get_barometer_pressure(void);
inline int get_barometer_temperature(void);
inline long get_barometer_gndaltitude(void);
inline long get_barometer_aglaltitude(void);

#endif // BAROMETERCNTRL_H

