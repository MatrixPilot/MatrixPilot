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


#include "libUDB_internal.h"
#include "analogs.h"

union longww battery_current;
union longww battery_mAh_used;

union longww battery_voltage;	// battery_voltage._.W1 is in tenths of Volts

uint8_t rc_signal_strength;
#define MIN_RSSI   ((int32_t)((RSSI_MIN_SIGNAL_VOLTAGE) * 10000))
#define RSSI_RANGE ((int32_t)((RSSI_MAX_SIGNAL_VOLTAGE-RSSI_MIN_SIGNAL_VOLTAGE) * 10000))
#define RSSI_STEP (RSSI_RANGE / 256)

void init_analogs(void)
{
	battery_current.WW = 0;
	battery_mAh_used.WW = 0;
	battery_voltage.WW = 0;
	rc_signal_strength = 0;
}

void calculate_analog_sensor_values(void)
{
	uint32_t v = 0;
	if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	{
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to current in tenths of Amps
	v = udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value;
	//v = udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].input;
	v += (int32_t)32768;
	battery_current.WW = v;
	//v *= CURRENT_SENSOR_SCALE;
	//battery_current.WW = (udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value + (int32_t)32768) * (CURRENT_SENSOR_SCALE) + (CURRENT_SENSOR_OFFSET);
	//battery_current.WW /= 100;

	// mAh = mA / 144000 (increment per 40Hz tick is /40*60*60)
	// 90000/144000 == 900/1440
	battery_mAh_used.WW += (battery_current.WW / 1440);
	}

	if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	{
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to voltage in tenths of Volts
		battery_voltage.WW = (udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_VOLTAGE);
		// Convert to tenths of Volts and round last digit
		battery_voltage.WW /= 10000;
		battery_voltage.WW *= 10;

		battery_voltage.WW += (VOLTAGE_SENSOR_OFFSET);
	}

	if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	{
		uint32_t rssi_accum;
		rssi_accum = udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value + 32768;
		rssi_accum -= MIN_RSSI;
#if RSSI_RAW_SIGNAL
		rssi_accum /= RSSI_STEP;
		if (rssi_accum > 255)
		{
			rssi_accum = 255;
		}
		rc_signal_strength = (uint8_t)rssi_accum;
		union longww rssi;
		rssi.WW = rc_signal_strength;
		if (rssi._.W1 > 100)
			rc_signal_strength = 100;
#else
		rssi_accum = ((rssi_accum * 100) / (RSSI_RANGE));
		if (rssi_accum > 100)
		{
			rssi_accum = 100;
		}
		rc_signal_strength = (uint8_t)rssi_accum;
#endif
	}
}
