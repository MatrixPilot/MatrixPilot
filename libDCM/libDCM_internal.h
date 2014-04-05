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
#include <stdio.h>
void dcm_init_rmat(void);

void estYawDrift(void);
void estimateWind(void);

void gps_commit_data(void);

void gpsoutline(const char* message);
void gpsoutbin(int16_t length, const uint8_t* msg);

void dcm_run_imu_step(void);    // This needs to be run every 25ms

#if (USE_BAROMETER == 1)
	void udb_barometer_callback(int32_t pressure, int16_t temperature, uint8_t status);
#endif


