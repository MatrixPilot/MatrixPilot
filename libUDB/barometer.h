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


#ifndef BAROMETER_H
#define BAROMETER_H

enum BAROMETER_SERVICE_STATE {
  BAROMETER_SERVICE_CAN_PAUSE = 0,
  BAROMETER_NEEDS_SERVICING
};
#if (USE_BAROMETER_ALTITUDE == 1)
    typedef void (*barometer_callback_funcptr)(long pressure, int temperature, char status);

    uint8_t rxBarometer(barometer_callback_funcptr);  // service the barometer
    #elif (USE_BAROMETER_ALTITUDE == 2)
    extern int32_t barometer_agl_altitude;    // above ground level altitude - AGL in mm
    typedef void (*barometer_callback_funcptr)(long altitude, int temperature, char status);
    void MPL3115A2_Init(void);
    uint8_t rxBarometer(barometer_callback_funcptr);  // service the barometer
    #endif


#endif // BAROMETER_H
