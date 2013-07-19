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


/*
union longbbbb lat_gps, lon_gps, alt_sl_gps, tow;  // latitude, longitude, altitude
union intbb sog_gps, cog_gps, climb_gps, week_no;   // speed over ground, course over ground, climb
union intbb as_sim;
uint8_t hdop;                                       // horizontal dilution of precision
union longbbbb lat_origin, lon_origin, alt_origin;
//union longbbbb xpg, ypg, zpg;                     // gps x, y, z position
//union intbb    xvg, yvg, zvg;                     // gps x, y, z velocity
//uint8_t mode1, mode2;                             // gps mode1, mode2
uint8_t svs;                                        // number of satellites
int16_t cos_lat = 0;
int16_t gps_data_age;
uint8_t *gps_out_buffer = 0;
int16_t gps_out_buffer_length = 0;
int16_t gps_out_index = 0;
 */

// the following variables are shared internally between the various GPS parsing modules only
//
//extern union intbb payloadlength;
//extern int16_t store_index;
//extern uint8_t un;
//extern uint8_t svs_;
extern union longbbbb lat_gps_;
extern union longbbbb lon_gps_;
extern union longbbbb alt_sl_gps_;
//extern union longbbbb sog_gps_;
//extern union longbbbb cog_gps_;
//extern union longbbbb climb_gps_;
extern union intbb hdop_;
extern union longbbbb tow_;
//

//extern void (*msg_parse)(uint8_t inchar);
//extern void (*gps_startup_sequence)(int16_t gpscount);
//extern boolean (*gps_nav_valid)(void);
//extern void (*gps_commit_data)(void);


int16_t calculate_week_num(int32_t date);
int32_t calculate_time_of_week(int32_t time);
