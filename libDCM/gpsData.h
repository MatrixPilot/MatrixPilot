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


#ifdef USE_EXTENDED_NAV
extern struct relative3D_32 GPSlocation;
#else
extern struct relative3D GPSlocation;
#endif // USE_EXTENDED_NAV

extern struct relative3D GPSvelocity;
extern int16_t cos_lat;
extern int16_t gps_data_age;
extern int8_t actual_dir;
extern uint16_t ground_velocity_magnitudeXY;
extern int16_t forward_acceleration;
extern uint16_t air_speed_magnitudeXY;
extern uint16_t air_speed_3DGPS;
extern int8_t calculated_heading;           // takes into account wind velocity

extern volatile union intbb hilsim_airspeed;  // referenced in estWind and deadReckoning modules

extern union longbbbb lat_origin, lon_origin, alt_origin;  // (COULD THIS BETTER BE A VECTOR??)

///////////////////////////////////////////////////////////////////////////////
// WRAP ALL THIS UP INTO A STRUCTURE
extern volatile union longbbbb lat_gps, lon_gps, alt_sl_gps;        // latitude, longitude, altitude   (COULD THIS BETTER BE A VECTOR??)
extern volatile uint8_t hdop;                                       // horizontal dilution of precision
extern volatile uint8_t svs;    // referenced by telemetry and OSD modules  // number of satellites
// these are only exported for telemetry output
extern volatile union intbb week_no;
extern volatile union intbb sog_gps;                                // speed over ground
extern volatile union uintbb cog_gps;                               // course over ground
extern volatile union intbb climb_gps;                              // climb
extern volatile union longbbbb tow;
