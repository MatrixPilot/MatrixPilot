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


#include "libUDB.h"
#include "ADchannel.h"
#include "analogs.h"

#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)
union longww battery_current;
union longww battery_mAh_used;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)
union longww battery_voltage;	// battery_voltage._.W1 is in tenths of Volts
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED || RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
uint8_t rc_signal_strength;
#define MIN_RSSI   ((int32_t)((RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 65536))
#define RSSI_RANGE ((int32_t)((RSSI_MAX_SIGNAL_VOLTAGE-RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 100))
#endif


void init_analogs(void)
{
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)
	battery_current.WW = 0;
	battery_mAh_used.WW = 0;
#endif
#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)
	battery_voltage.WW = 0;
#endif
#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED || RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	rc_signal_strength = 0;
#endif
}

void calculate_analog_sensor_values(void)
{
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to current in tenths of Amps
	battery_current.WW = (udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_CURRENT) + (((int32_t)(CURRENT_SENSOR_OFFSET)) << 16);
#endif
    
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED || USE_CASTLE_LINK_THROTTLE == 1)
	// mAh = mA / 144000 (increment per 40Hz tick is /40*60*60)
	// 90000/144000 == 900/1440
	battery_mAh_used.WW += (battery_current.WW / 1440);
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to voltage in tenths of Volts
	battery_voltage.WW = (udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_VOLTAGE) + (((int32_t)(VOLTAGE_SENSOR_OFFSET)) << 16);
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	union longww rssi_accum;
	rssi_accum.WW = (((udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value + 32768) - (MIN_RSSI)) * (10000 / (RSSI_RANGE)));
	if (rssi_accum._.W1 < 0)
		rc_signal_strength = 0;
	else if (rssi_accum._.W1 > 100)
		rc_signal_strength = 100;
	else
		rc_signal_strength = (uint8_t)rssi_accum._.W1;
#endif
	
#if (RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	int16_t tempRSSI = (udb_pwIn[RSSI_INPUT_CHANNEL]-2000) / 20;
	if (tempRSSI < 0) tempRSSI = 0;
	if (tempRSSI > 100) tempRSSI = 100;
	rc_signal_strength = tempRSSI;
#endif
}
