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

#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_current;
union longww battery_mAh_used;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_voltage;	// battery_voltage._.W1 is in tenths of Volts
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
uint8_t rc_signal_strength;
#define MIN_RSSI   ((int32_t)((RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 65536))
#define RSSI_RANGE ((int32_t)((RSSI_MAX_SIGNAL_VOLTAGE-RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 100))
#endif


void init_analogs(void)
{
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	battery_current.WW = 0;
	battery_mAh_used.WW = 0;
#endif
#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	battery_voltage.WW = 0;
#endif
#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	rc_signal_strength = 0;
#endif
//	udb_init_ADC();
}

void calculate_analog_sensor_values(void)
{
/*
	printf("I %u V %u R %u\r\n", udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value,
	                             udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value,
	                             udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value);
 */
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	unsigned long I = udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value;
//	I *= 3300;
//	I *= 25;
//	I *= 10;
//	I /= 4098;

	I *= 16000;

//	battery_current.WW = I / 1000;
//	battery_mAh_used.WW += (battery_current.WW / 1440);

	battery_current._.W1 = I / 1000;

	static unsigned long Ah = 0L;
	Ah += (I / 144);
//	battery_mAh_used._.W1 += (battery_current._.W1 / 1440);
	battery_mAh_used._.W1 = Ah / 100000;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	unsigned long V = udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value;
//	V *= 3300;
//	V *= 8;
//	V *= 10;
//	V /= 4098;

//	V *= 3300;
//	V *= 10;
//	V /= 4098;

	V *= 100;
	V /= 1130;

// hm.....the actual voltage is battery voltage * 0.1818
// so, if the battery voltage is 14.4....you'll have 2.62 at the input of the ADC

//	battery_voltage.WW = V / 1000;
//	battery_voltage._.W1 = V / 1000;
	battery_voltage._.W1 = V;

#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	unsigned long R = udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value;
	R *= 3300;
	R *= 10;
	R /= 4098;
#endif

//	printf("I = %lu mA V = %lu mV R = %lu mV\r\n", I, V, R);

/*
I know where the problem is....it's my oversight.......
look at the schematics.......we have a divider 30k:10k........
additional to this, onboard we have another divider of 10k:10k.......
if you draw both resistor networks one after the other, you could see that the equivallent resistor in lower leg is 6.6666k,
thus what we have.....14.4 / ( 30 + 6.666) = 0.3827 * 6.666k = 2.62V.....which is the reading of your voltmeter
I have to secalculate the resistors for the next ACSP1 revision
I like to ask you for excuse about.......now we have a bit different equivalent divisor of 30:6.66 
 */

/*
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to current in tenths of Amps
	battery_current.WW = (udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_CURRENT) + (((int32_t)(CURRENT_SENSOR_OFFSET)) << 16);

	// mAh = mA / 144000 (increment per 40Hz tick is /40*60*60)
	// 90000/144000 == 900/1440
	battery_mAh_used.WW += (battery_current.WW / 1440);
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to voltage in tenths of Volts
	battery_voltage.WW = (udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_VOLTAGE) + (((int32_t)(VOLTAGE_SENSOR_OFFSET)) << 16);
#endif
 */

//#define RSSI_MIN_SIGNAL_VOLTAGE				0.5		// Voltage when RSSI should show 0%
//#define RSSI_MAX_SIGNAL_VOLTAGE				3.3		// Voltage when RSSI should show 100%
//#define MIN_RSSI   ((int32_t)((RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 65536))
//#define RSSI_RANGE ((int32_t)((RSSI_MAX_SIGNAL_VOLTAGE-RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 100))

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	union longww rssi_accum;
//	rssi_accum.WW = (((udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value + 32768) - (MIN_RSSI)) * (10000 / (RSSI_RANGE)));
	rssi_accum.WW = ((R - ((int32_t)(RSSI_MIN_SIGNAL_VOLTAGE * 10000))) / ((int32_t)(RSSI_MAX_SIGNAL_VOLTAGE * 100))) << 16;

	if (rssi_accum._.W1 < 0)
		rc_signal_strength = 0;
	else if (rssi_accum._.W1 > 100)
		rc_signal_strength = 100;
	else
		rc_signal_strength = (uint8_t)rssi_accum._.W1;
#endif
}
