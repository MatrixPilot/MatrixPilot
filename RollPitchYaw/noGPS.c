// This file is part of the MatrixPilot RollPitchYaw demo.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2013 MatrixPilot Team
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


#include "../libDCM/libDCM.h"
#include "../libDCM/gpsParseCommon.h"

// GPS MatrixPilot global variables (these are the GPS system outputs)
//vect3_16t estimatedWind = { 0, 0, 0 };
//struct relative3D_32 GPSlocation = { 0, 0, 0 };
struct relative3D GPSvelocity = { 0, 0, 0 };
int16_t cos_lat = 0;
//int16_t gps_data_age;
//int8_t actual_dir;
uint16_t ground_velocity_magnitudeXY = 0;
int16_t forward_acceleration = 0;
//uint16_t air_speed_magnitudeXY = 0;
uint16_t air_speed_3DGPS = 0;
//int8_t calculated_heading;           // takes into account wind velocity
union longbbbb lat_origin, lon_origin, alt_origin;
volatile union longbbbb lat_gps, lon_gps, alt_sl_gps;        // latitude, longitude, altitude

void dead_reckon(struct relative3D gps_velocity) {}
int16_t udb_gps_callback_get_byte_to_send(void) { return -1; }
void udb_gps_callback_received_byte(uint8_t rxchar) {}
void gps_startup_sequence(int16_t gpscount) {}

///////////////////////////////////////////////////////////////////////////////

void calculate_analog_sensor_values(void) {}
void init_analogs(void) {}
void udb_init_capture(void) {}
void udb_init_ADC(void) {}
