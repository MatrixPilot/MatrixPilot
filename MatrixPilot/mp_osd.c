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


#include "defines.h"
#include "options_osd.h"
#include "mp_osd.h"

#if ((USE_OSD == OSD_NATIVE) && (SILSIM != 1))

#include <ctype.h>
#include <string.h>
#include "states.h"
#include "navigate.h"
#include "../libUDB/osd.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/ADchannel.h"
#include "../libDCM/deadReckoning.h"
#include "../libDCM/mathlibNAV.h"
#include "../libDCM/estWind.h"
#include "../libDCM/gpsData.h"
#include "../libDCM/rmat.h"

#define OSD_LOC_DISABLED    -1
#include "osd_layout.h"

#define VARIOMETER_LOW      15
#define VARIOMETER_HIGH     80

static const uint8_t heading_strings[16][4] = {
	{0x8F, 0x00, 0x00, 0xFF},   // E
	{0x8F, 0x98, 0x8F, 0xFF},   // ENE
	{0x98, 0x8F, 0x00, 0xFF},   // NE
	{0x98, 0x98, 0x8F, 0xFF},   // NNE
	{0x98, 0x00, 0x00, 0xFF},   // N
	{0x98, 0x98, 0xA1, 0xFF},   // NNW
	{0x98, 0xA1, 0x00, 0xFF},   // NW
	{0xA1, 0x98, 0xA1, 0xFF},   // WNW
	{0xA1, 0x00, 0x00, 0xFF},   // W
	{0xA1, 0x9D, 0xA1, 0xFF},   // WSW
	{0x9D, 0xA1, 0x00, 0xFF},   // SW
	{0x9D, 0x9D, 0xA1, 0xFF},   // SSW
	{0x9D, 0x00, 0x00, 0xFF},   // S
	{0x9D, 0x9D, 0x8F, 0xFF},   // SSE
	{0x9D, 0x8F, 0x00, 0xFF},   // SE
	{0x8F, 0x9D, 0x8F, 0xFF},   // ESE
};


#if (OSD_VIDEO_FORMAT == OSD_NTSC)
#define OSD_SPACING         4
#else
#define OSD_SPACING         5
#endif

// callsign
static const uint8_t callsign[] = OSD_CALL_SIGN;

static uint8_t osd_reset_cnt = 0;
static boolean osd_was_on = 0;

#if (OSD_SHOW_CONSOLE == 1)

static char msgbuf_0[21];
static char msgbuf_1[21];
static uint8_t osd_console_blanking_time = 0;

void osd_write_message(char* msg)
{
	strncpy(msgbuf_0, msgbuf_1, 20);
	strncpy(msgbuf_1, msg, 20);
	osd_console_blanking_time = 80;
}

static void osd_console_write_line(char* msg)
{
	uint8_t i;

	osd_spi_write(MAX7456_DMM, 1);      // DMM: Enable auto-increment mode
	for (i = 0; i < 20; i++)
	{
		char ch = tolower(msg[i]);
		if (!ch) break;

		if (ch == ' ') {
			ch = 138;
		} else {
//			ch = ch - 60;
			ch = ch + 42;
		}
		osd_spi_write_byte(ch);
	}
	osd_spi_write_byte(0xFF);           // Disable auto-increment mode 
}

static void osd_update_console(void)
{
	if (osd_console_blanking_time)
	{
		if (!--osd_console_blanking_time)
		{
			memset(msgbuf_0, ' ', 20);
			memset(msgbuf_1, ' ', 20);
		}
		osd_spi_write_location(OSD_LOC_CONSOLE_0);
		osd_console_write_line(msgbuf_0);
		osd_spi_write_location(OSD_LOC_CONSOLE_1);
		osd_console_write_line(msgbuf_1);
	}
}

#endif

#if (OSD_SHOW_HORIZON == 1)

static int16_t lastRoll = 0;
static int16_t lastPitch = 0;

static void osd_update_horizon(void)
{
	// TODO: Change away from using roll degrees.  Use tangent as the slope.
	struct relative2D matrix_accum;
	int32_t earth_roll;
	int32_t earth_pitch;
	int8_t i;

	matrix_accum.x = rmat[8];
	matrix_accum.y = rmat[6];
	earth_roll = rect_to_polar(&matrix_accum);              // binary angle (0 - 256 = 360 degrees)
	earth_roll = (-earth_roll * BYTECIR_TO_DEGREE) >> 16;   // switch polarity, convert to -180 - 180 degrees
#if (OSD_HORIZON_ROLL_REVERSED == 1)
	earth_roll = -earth_roll;
#endif

	matrix_accum.y = rmat[7];
	earth_pitch = rect_to_polar(&matrix_accum);             // binary angle (0 - 256 = 360 degrees)
	earth_pitch = (-earth_pitch * BYTECIR_TO_DEGREE) >> 16; // switch polarity, convert to -180 - 180 degrees
#if (OSD_HORIZON_PITCH_REVERSED == 1)
	earth_pitch = -earth_pitch;
#endif

	for (i = -OSD_HORIZON_WIDTH; i < OSD_HORIZON_WIDTH; i++)
	{
		int8_t lastHeight;
		int16_t h = earth_roll * i - earth_pitch * 16 + 60;
		int8_t height = h / 120;
		int8_t subHeight = ((h % 120) * 16 / 120);
		if (h < 0) { height--; subHeight--; }
		subHeight &= 0x0F;

		h = lastRoll * i - lastPitch * 16 + 60;
		lastHeight = h / 120;
		if (h < 0) lastHeight--;

		if (height != 0 || OSD_SHOW_CENTER_DOT != 1 || (i != -1 && i != 0))
		{
			if (height >= -OSD_SPACING && height <= OSD_SPACING)
			{
				osd_spi_write_location(OSD_LOC(OSD_SPACING + 3 - height, 15+i));
				osd_spi_write(0x7, 0xC0 + subHeight);   // DMDI: Write a '-'
			}
		}

		if (lastHeight != 0 || OSD_SHOW_CENTER_DOT != 1 || (i != -1 && i != 0))
		{
			if (height != lastHeight && lastHeight >= -OSD_SPACING && lastHeight <= OSD_SPACING)
			{
				osd_spi_write_location(OSD_LOC(OSD_SPACING + 3 - lastHeight, 15+i));
				osd_spi_write(0x7, 0x00);               // DMDI: Write a ' '
			}
		}
	}
	lastRoll = earth_roll;
	lastPitch = earth_pitch;
}

#endif // OSD_SHOW_HORIZON

static void osd_write_arrow(int8_t dir_to_goal)
{
	int16_t d = dir_to_goal - 8;
	if (d < 0) d += 256;
	d = (15 - (d/16)) * 2;

	osd_spi_write(0x04,1);              // DMM: Enable auto-increment mode
	osd_spi_write_byte(0x50 + d);
	osd_spi_write_byte(0x51 + d);
	osd_spi_write_byte(0xFF);
}

static void osd_setup_screen(void)
{
#if (OSD_LOC_ALTITUDE != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_ALTITUDE);
	osd_spi_write(0x7, 0xA6);           // Altitude symbol
#endif

#if (OSD_LOC_CPU_LOAD != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_CPU_LOAD);
	osd_spi_write(0x7, 0xBD);           // CPU symbol
	osd_spi_write_location(OSD_LOC_CPU_LOAD+3);
	osd_spi_write(0x7, 0xA5);           // % symbol
#endif

#if (OSD_LOC_DIST_TO_GOAL != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_DIST_TO_GOAL);
	osd_spi_write(0x7, 0xA7);           // Distance symbol
#endif

#if (OSD_LOC_HEADING_NUM != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_HEADING_NUM);
	osd_spi_write(0x7, 0xAB);           // Direction symbol
	osd_spi_write_location(OSD_LOC_HEADING_NUM+4);
	osd_spi_write(0x7, 0x4D);           // Degrees symbol
#endif

#if (OSD_LOC_AIR_SPEED_M_S != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_AIR_SPEED_M_S+3);
	osd_spi_write(0x7, 0xDD);           // m/s symbol
#endif

#if (OSD_LOC_AIR_SPEED_MI_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_AIR_SPEED_MI_HR+3);
	osd_spi_write(0x7, 0xDF);           // mi/hr symbol
#endif

#if (OSD_LOC_AIR_SPEED_KM_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_AIR_SPEED_KM_HR+3);
	osd_spi_write(0x7, 0xDE);           // km/hr symbol
#endif

#if (OSD_LOC_GROUND_SPEED_M_S != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GROUND_SPEED_M_S+3);
	osd_spi_write(0x7, 0xDD);           // m/s symbol
#endif

#if (OSD_LOC_GROUND_SPEED_MI_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GROUND_SPEED_MI_HR+3);
	osd_spi_write(0x7, 0xDF);           // mi/hr symbol
#endif

#if (OSD_LOC_GROUND_SPEED_KM_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GROUND_SPEED_KM_HR+3);
	osd_spi_write(0x7, 0xDE);           // km/hr symbol
#endif

#if (OSD_SHOW_CENTER_DOT == 1)
	osd_spi_write_location(OSD_LOC(OSD_SPACING + 3, 14));
	osd_spi_write(0x7, 0x4E);           // center dot
	osd_spi_write_location(OSD_LOC(OSD_SPACING + 3, 15));
	osd_spi_write(0x7, 0x4F);           // center dot
#endif

#if (OSD_SHOW_HORIZON == 1)
	osd_spi_write_location(OSD_LOC(OSD_SPACING + 3, 14-OSD_HORIZON_WIDTH));
	osd_spi_write(0x7, 0xF1);           // horizon center
	osd_spi_write_location(OSD_LOC(OSD_SPACING + 3, 15+OSD_HORIZON_WIDTH));
	osd_spi_write(0x7, 0xF0);           // horizon center
#endif

#if (OSD_LOC_CALLSIGN_HORIZ != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_CALLSIGN_HORIZ);
	osd_spi_write_string(callsign);     // callsign
#endif

#if (OSD_LOC_CALLSIGN_VERT != OSD_LOC_DISABLED)
	osd_spi_write_vertical_string_at_location(OSD_LOC_CALLSIGN_VERT, callsign); // callsign
#endif
}

static void osd_update_values_phase_0(void)
{
#if (OSD_LOC_ALTITUDE != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_ALTITUDE+1);
	osd_spi_write_number(IMUlocationz._.W1, 0, 0, NUM_FLAG_SIGNED, 0, 0); // Altitude
#endif

#if (OSD_LOC_CPU_LOAD != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_CPU_LOAD+1);
	osd_spi_write_number(udb_cpu_load(), 2, 0, 0, 0, 0); // CPU
#endif

#if (OSD_LOC_VARIO_NUM != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_VARIO_NUM);
	osd_spi_write_number(IMUvelocityz._.W1, 0, 0, NUM_FLAG_SIGNED, 0, 0); // Variometer
#endif

#if (OSD_LOC_VARIO_ARROW != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_VARIO_ARROW);
	if (IMUvelocityz._.W1 <= -VARIOMETER_HIGH)
		osd_spi_write(0x7, 0xD4);                   // Variometer down fast
	else if (IMUvelocityz._.W1 <= -VARIOMETER_LOW)
		osd_spi_write(0x7, 0xD2);                   // Variometer down slowly
	else if (IMUvelocityz._.W1 < VARIOMETER_LOW)
		osd_spi_write(0x7, 0x00);                   // Variometer flat (was 0xD0)
	else if (IMUvelocityz._.W1 < VARIOMETER_HIGH)
		osd_spi_write(0x7, 0xD1);                   // Variometer up slowly
	else if (IMUvelocityz._.W1 >= VARIOMETER_HIGH)
		osd_spi_write(0x7, 0xD3);                   // Variometer up fast
#endif

#if (OSD_LOC_AP_MODE != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_AP_MODE);
	if (!state_flags._.pitch_feedback)
		osd_spi_write(0x7, 0x97);                   // M : Manual Mode
	else if (!state_flags._.GPS_steering)
		osd_spi_write(0x7, 0x9D);                   // S : Stabilized Mode
	else if (udb_flags._.radio_on && !state_flags._.rtl_hold)
		osd_spi_write(0x7, 0xA1);                   // W : Waypoint Mode
	else if (state_flags._.rtl_hold && udb_flags._.radio_on)
		osd_spi_write(0x7, 0x92);                   // H : RTL Hold, has signal
	else
		osd_spi_write(0x7, 0x9C);                   // R : RTL Mode, lost signal
#endif
}

static void osd_update_values_phase_1(void)
{
	int8_t earth_yaw;
	int8_t dir_to_goal;
	int16_t dist_to_goal;
	struct relative2D curHeading;
    
#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED || RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
#if (OSD_LOC_RSSI != OSD_LOC_DISABLED && OSD_FLASH_RSSI != 0)
	static char f_OSD_RSSI_flash = 0;        // To keep track on show/hide OSD_RSSI
#endif
#endif

	curHeading.x = -rmat[1];
	curHeading.y = rmat[4];
	earth_yaw = rect_to_polar(&curHeading);  // 0-255 (0=East,  ccw)

	if (state_flags._.GPS_steering)
	{
		dir_to_goal = desired_dir - earth_yaw;
		dist_to_goal = abs(tofinish_line);
	}
	else
	{
		struct relative2D toGoal;
		toGoal.x = 0 - IMUlocationx._.W1;
		toGoal.y = 0 - IMUlocationy._.W1;
		dir_to_goal = rect_to_polar (&toGoal) - earth_yaw;
		dist_to_goal = toGoal.x;
	}

#if (OSD_LOC_DIST_TO_GOAL != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_DIST_TO_GOAL+1);
	osd_spi_write_number(dist_to_goal, 0, 0, 0, 0, 0);  // Distance to wp/home
#endif

#if (OSD_LOC_ARROW_TO_GOAL != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_ARROW_TO_GOAL);
	osd_write_arrow(dir_to_goal);
#endif

#if (OSD_LOC_HEADING_NUM != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_HEADING_NUM+1);
//			earth_yaw                                           // 0-255 (0=East,  ccw)
	int16_t angle = (earth_yaw * 180 + 64) >> 7;        // 0-359 (0=East,  ccw)
	angle = -angle + 90;                                // 0-359 (0=North, clockwise)
	if (angle < 0) angle += 360;                        // 0-359 (0=North, clockwise)
	osd_spi_write_number(angle, 3, 0, NUM_FLAG_ZERO_PADDED, 0, 0); // heading
#endif

#if (OSD_LOC_HEADING_CARDINAL != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_HEADING_CARDINAL);
	osd_spi_write_string(heading_strings[((uint8_t)(earth_yaw+8))>>4]); // heading
#endif

#if (OSD_LOC_VERTICAL_ANGLE_HOME != OSD_LOC_DISABLED)
	// Vertical angle from origin to plane
	int16_t verticalAngle = 0;
	if (dist_to_goal != 0)
	{
		struct relative2D componentsToPlane;
		componentsToPlane.x = dist_to_goal;
		componentsToPlane.y = IMUlocationz._.W1;
		verticalAngle = rect_to_polar(&componentsToPlane);              // binary angle (0 - 256 = 360 degrees)
		verticalAngle = (verticalAngle * BYTECIR_TO_DEGREE) >> 16;      // switch polarity, convert to -180 - 180 degrees
	}
	osd_spi_write_location(OSD_LOC_VERTICAL_ANGLE_HOME);
	osd_spi_write_number(verticalAngle, 0, 0, NUM_FLAG_SIGNED, 0, 0x4D);// Footer: Degree symbol
#endif

#if (OSD_LOC_ROLL_RATE != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_ROLL_RATE);
	osd_spi_write_number(abs(omegagyro[1])/DEGPERSEC, 3, 0, 0, 0, 0);   // roll rate in degrees/sec/sec
#endif

#if (OSD_LOC_PITCH_RATE != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_PITCH_RATE);
	osd_spi_write_number(abs(omegagyro[0])/DEGPERSEC, 3, 0, 0, 0, 0);   // pitch rate in degrees/sec/sec
#endif

#if (OSD_LOC_YAW_RATE != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_YAW_RATE);
	osd_spi_write_number(abs(omegagyro[2])/DEGPERSEC, 3, 0, 0, 0, 0);   // yaw rate in degrees/sec/sec
#endif

#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)

#if (OSD_LOC_BATT_CURRENT != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_BATT_CURRENT);
	osd_spi_write_number(battery_current._.W1, 3, 1, 0, 0, 0xB4);   // tenths of Amps being used right now
#endif

#if (OSD_LOC_BATT_USED != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_BATT_USED);
	osd_spi_write_number(battery_mAh_used._.W1, 4, 0, 0, 0, 0xB7);  // mAh used so far
#endif

#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)

#if (OSD_LOC_BATT_VOLTAGE != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_BATT_VOLTAGE);
	osd_spi_write_number(battery_voltage._.W1, 3, 1, 0, 0, 0xA0);   // tenths of Volts
#endif

#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED || RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)

#if (OSD_LOC_RSSI != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_RSSI);
#if (OSD_FLASH_RSSI != 0)
	if(rc_signal_strength < OSD_FLASH_MIN_RSSI)         // If we has low RC signal strength
	{
		if ( f_OSD_RSSI_flash != 0 )                    // If blinking flag is 1
		{                                               // then shows RSSI
			osd_spi_write_number(rc_signal_strength, 3, 0, 0, 0, 0xB3);     // RC Receiver signal strength as 0-100%
			f_OSD_RSSI_flash = 0;                       // Next time RSSI info will be erased
		}
		else                                            // else
		{
			osd_spi_erase_chars(3);                     // erase RSSI info
			f_OSD_RSSI_flash = 1;                       // Next time RSSI info will be showed
		}
	}
	else
	{
		osd_spi_write_number(rc_signal_strength, 3, 0, 0, 0, 0xB3);     // RC Receiver signal strength as 0-100%
	}
#else
	osd_spi_write_number(rc_signal_strength, 3, 0, 0, 0, 0xB3);     // RC Receiver signal strength as 0-100%
#endif // (OSD_FLASH_RSSI != 0)
#endif // (OSD_LOC_RSSI != OSD_LOC_DISABLED)

#endif // (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
}

static void osd_update_values_phase_2(void)
{
#if (OSD_SHOW_HORIZON == 1)
	osd_update_horizon();
#endif
#if (OSD_SHOW_CONSOLE == 1)
	osd_update_console();
#endif
}

static void osd_update_values_phase_3(void)
{
#if (OSD_FLASH_GPS != 0)
	static char f_OSD_NUM_SATS_flash = 0;        // To keep track on show or hide OSD_NUM_SATS
#endif

#if (OSD_LOC_AIR_SPEED_M_S != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_AIR_SPEED_M_S);
	osd_spi_write_number(air_speed_3DIMU/100, 3, 0, 0, 0, 0);   // speed in m/s
#endif

#if (OSD_LOC_AIR_SPEED_MI_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_AIR_SPEED_MI_HR);
	osd_spi_write_number(air_speed_3DIMU/45, 3, 0, 0, 0, 0);    // speed in mi/hr
#endif

#if (OSD_LOC_AIR_SPEED_KM_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_AIR_SPEED_KM_HR);
	osd_spi_write_number(air_speed_3DIMU/28, 3, 0, 0, 0, 0);    // speed in km/hr
#endif

#if (OSD_LOC_GROUND_SPEED_M_S != OSD_LOC_DISABLED || OSD_LOC_GROUND_SPEED_MI_HR != OSD_LOC_DISABLED || OSD_LOC_GROUND_SPEED_KM_HR != OSD_LOC_DISABLED)
	uint16_t ground_speed_3DIMU = 
		vector3_mag(IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1);
#endif

#if (OSD_LOC_GROUND_SPEED_M_S != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GROUND_SPEED_M_S);
	osd_spi_write_number(ground_speed_3DIMU/100, 3, 0, 0, 0, 0);// speed in m/s
#endif

#if (OSD_LOC_GROUND_SPEED_MI_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GROUND_SPEED_MI_HR);
	osd_spi_write_number(ground_speed_3DIMU/45, 3, 0, 0, 0, 0); // speed in mi/hr
#endif

#if (OSD_LOC_GROUND_SPEED_KM_HR != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GROUND_SPEED_KM_HR);
	osd_spi_write_number(ground_speed_3DIMU/28, 3, 0, 0, 0, 0); // speed in km/hr
#endif

#if (OSD_LOC_VERTICAL_ACCEL != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_VERTICAL_ACCEL);
	union longww gravity_z;
	gravity_z.WW = __builtin_mulss(GRAVITY, rmat[8]) << 2;
	osd_spi_write_number((ZACCEL_VALUE - gravity_z._.W1)/(100*ACCELSCALE), 3, 0, NUM_FLAG_SIGNED, 0, 0); // vertical acceleration rate in units of m/sec/sec
#endif

#if (OSD_LOC_VERTICAL_WIND_SPEED != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_VERTICAL_WIND_SPEED);
	osd_spi_write_number(estimatedWind[2]/10, 4, 1, NUM_FLAG_SIGNED, 0, 0); // vertical wind speed in m/s
#endif

#if (OSD_LOC_TOTAL_ENERGY != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_TOTAL_ENERGY);
	osd_spi_write_number(total_energy, 4, 0, NUM_FLAG_SIGNED, 0, 0);    // total energy in meters above the origin
#endif

#if (OSD_AUTO_HIDE_GPS == 1)
	boolean showGPS = (IMUlocationz._.W1 < 20 || ground_velocity_magnitudeXY < 150);
#else
	boolean showGPS = 1;
#endif

#if (OSD_LOC_NUM_SATS != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_NUM_SATS);
#if (OSD_FLASH_GPS != 0)
	if(svs <= OSD_FLASH_MIN_SVS)                            // I want to run follow code when SVS is LOW
	{
		if ( (f_OSD_NUM_SATS_flash != 0) && showGPS)        // If blinking flag is 1 and showGPS
		{                                                   // then shows SVS and sat picture
			osd_spi_write_number(svs, 0, 0, 0, 0xEB, 0);    // Num satelites locked, with SatDish icon header
			f_OSD_NUM_SATS_flash = 0;                       // Next time GPS info will be erased
		}
		else                                                // else
		{
			osd_spi_erase_chars(3);                         // erase GPS SVS info
			f_OSD_NUM_SATS_flash = 1;                       // Next time GPS info will be showed
		}
	}
	else                                                    // SVS is > than OSD_FLASH_MIN_SVS so do as old code
	{
		if (showGPS)
		{
			osd_spi_write_number(svs, 0, 0, 0, 0xEB, 0);    // Num satelites locked, with SatDish icon header
		}
		else
		{
			osd_spi_erase_chars(3);
		}
	}
#else
		if (showGPS)
		{
			osd_spi_write_number(svs, 0, 0, 0, 0xEB, 0);    // Num satelites locked, with SatDish icon header
		}
		else
		{
			osd_spi_erase_chars(3);
		}
#endif // (OSD_FLASH_GPS != 0)
#endif // (OSD_LOC_NUM_SATS != OSD_LOC_DISABLED)

#if (OSD_LOC_GPS_LAT != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GPS_LAT);
	if (showGPS)
	{
		osd_spi_write_number(labs(lat_gps.WW/10), 8, 6, 0, 0, (lat_gps.WW >= 0) ? 0x98 : 0x9D); // Footer: N/S
	}
	else
	{
		osd_spi_erase_chars(9);
	}
#endif

#if (OSD_LOC_GPS_LONG != OSD_LOC_DISABLED)
	osd_spi_write_location(OSD_LOC_GPS_LONG);
	if (showGPS)
	{
		osd_spi_write_number(labs(lon_gps.WW/10), 9, 6, 0, 0, (lon_gps.WW >= 0) ? 0x8F : 0xA1); // Footer: E/W
	}
	else
	{
		osd_spi_erase_chars(10);
	}
#endif
}

// what is the assumed polling rate here? (looks like HEARTBEAT_HZ is important) - RobD
void mp_osd_run_step(uint16_t init_counter) // currently gets called with 'udb_heartbeat_counter'
{
	static uint8_t osd_phase = 0;
	boolean osd_on = (OSD_MODE_SWITCH_INPUT_CHANNEL == CHANNEL_UNUSED || udb_pwIn[OSD_MODE_SWITCH_INPUT_CHANNEL] >= 3000 || !udb_flags._.radio_on);
	int16_t countdown = 0;

	if (!dcm_flags._.init_finished && init_counter < 100)  // TODO: this will need updating for increased HEARTBEAT_HZ
	{
		countdown = 100 - init_counter;
	}
	if (countdown == 61)
	{
		osd_spi_write_byte(0xFF);       // Terminate sending a string, in case that was happening (Prep for reset)
	}
	else if (countdown == 60)
	{
		osd_spi_write(0x0, 0x02);       // VM0: Reset the OSD
	}
	else if (countdown == 48)
	{
		osd_spi_write(0x04, 0);         // DMM set to 0
	}
	else if (countdown < 48)
	{
		if (!osd_was_on && osd_on)
		{
#if (OSD_VIDEO_FORMAT == OSD_NTSC)
			osd_spi_write(0x0, 0x08);   // VM0: enable display of OSD image, NTSC
#else
			osd_spi_write(0x0, 0x48);   // VM0: enable display of OSD image, PAL
#endif
			int8_t i;
			for (i=0; i<16; i++) {
				osd_spi_write(0x10+i, 0b00000111);  // RB0-15 set to 10%-80%
			}
            
			osd_phase = 0;
			osd_setup_screen();
			osd_was_on = 1;
		}
		else if (osd_was_on && !osd_on) // just turned off
		{
			osd_spi_write(0x04, 4);     // DMM set to 6 (Blank screen)

#if (OSD_VIDEO_FORMAT == OSD_NTSC)
			osd_spi_write(0x0, 0x00);   // VM0: disable display of OSD image, NTSC
#else
			osd_spi_write(0x0, 0x40);   // VM0: disable display of OSD image, PAL
#endif
			osd_was_on = 0;
		}
		if (osd_on)
		{
			if (!osd_phase)
			{
				osd_reset_cnt++;
			}
			if (!osd_reset_cnt)
			{
				osd_spi_write(MAX7456_DMM, 0x04);    // DMM set to clear display memory
#if (OSD_VIDEO_FORMAT == OSD_NTSC)
				osd_spi_write(0x0, 0x08);            // VM0: enable display of OSD image, NTSC
#else
				osd_spi_write(0x0, 0x48);            // VM0: enable display of OSD image, PAL
#endif
				osd_setup_screen();
			}
			else
			{
				// work around for a bug whereby the offsets randomly get set to zero
				osd_spi_write(MAX7456_VOS, OSD_VERTICAL_OFFSET);
				osd_spi_write(MAX7456_HOS, OSD_HORIZONTAL_OFFSET);

//				osd_update_values();
				switch (osd_phase)
				{
					case 0:
						osd_update_values_phase_0();
						break;
					case 1:
						osd_update_values_phase_1();
						break;
					case 2:
						osd_update_values_phase_2();
						break;
					case 3:
						osd_update_values_phase_3();
						break;
				}
				osd_phase = (osd_phase+1) % 4;
			}
		}
	}
}

#else

void mp_osd_run_step(uint16_t init_counter) {}

#endif // USE_OSD
