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


#ifndef ESTALTITUDE_H
#define ESTALTITUDE_H


#if (USE_BAROMETER_ALTITUDE == 1)
void udb_barometer_callback(long pressure, int16_t temperature, char status);
#elif (USE_BAROMETER_ALTITUDE == 2)
void udb_barometer_callback(long altitude, int16_t temperature, char status);
#endif
#if (USE_LIDAR_ALTITUDE > 0)
void udb_lidar_callback(long altitude);
#endif
void altimeter_calibrate(void);
void estAltitude(void);
extern int32_t estimated_altitude;        // previous above ground height (millimeters)
//extern int32_t fusion,vze_fusion;                // Altitude fusion output (in mm)

long get_barometer_altitude(void);
long get_barometer_pressure(void);
int16_t get_barometer_temperature(void);


#endif // ESTALTITUDE_H
