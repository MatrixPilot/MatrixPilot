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


extern volatile union intbb week_no;
extern volatile union intbb sog_gps;        // speed over ground
extern volatile union uintbb cog_gps;       // course over ground, units: degrees * 100, range [0-35999]
extern volatile union intbb climb_gps;      // climb
extern volatile union intbb hilsim_airspeed;// referenced in estWind and deadReckoning modules
extern volatile union longbbbb tow;
extern volatile uint8_t hdop;               // horizontal dilution of precision
extern volatile uint8_t svs;                // number of satellites
//extern union longbbbb as_sim_;
extern union longbbbb xpg, ypg, zpg;        // gps x, y, z position
extern union intbb xvg, yvg, zvg;           // gps x, y, z velocity
//extern uint8_t mode1, mode2;              // gps mode1, mode2
extern int16_t cos_lat;

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
extern union longbbbb date_gps_;
extern union longbbbb time_gps_;

extern uint16_t ground_velocity_magnitudeXY;
extern uint16_t air_speed_magnitudeXY;
extern int8_t calculated_heading;           // takes into account wind velocity
extern int16_t gps_data_age;
extern int8_t actual_dir;
extern uint16_t gps_parse_errors;

extern int16_t forward_acceleration;
extern uint16_t air_speed_3DGPS;


//extern void (*msg_parse)(uint8_t gpschar);
//extern void (*gps_startup_sequence)(int16_t gpscount);
//extern boolean (*gps_nav_valid)(void);
//extern void (*gps_commit_data)(void);
void gps_commit_data(void);
void gpsoutline(const char* message);
void gpsoutbin(int16_t length, const uint8_t* msg);

void gps_init(void);
void gps_parse_common(void);
void gps_update_basic_data(void);
boolean gps_nav_capable_check_set(void);
void HILSIM_set_gplane(fractional gplane[]);
void HILSIM_set_omegagyro(void);

int32_t get_gps_date(void);
int32_t get_gps_time(void);

int16_t calculate_week_num(int32_t date);
int32_t calculate_time_of_week(int32_t time);


// FIXME: This should be handled internally, along with DCM calibration
// Count down from 1000 at 40Hz
void gps_startup_sequence(int16_t gpscount);

// Is our gps data good enough for navigation?
boolean gps_nav_valid(void);

// Called once each time the GPS reports a new location.
// After dead reckoning is complete, this callback may go away.
void dcm_callback_gps_location_updated(void);   // Callback

int16_t udb_gps_callback_get_byte_to_send(void);
