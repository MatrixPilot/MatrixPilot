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


#include "../../libUDB/libUDB.h"
#include "../../libUDB/heartbeat.h"
#include "../../libUDB/osd.h"
//#include "../../libUDB/ADchannel.h"
#include "../../MatrixPilot/osd_layout.h" // Defines whether using NTSC or PAL Video
#include "font_data.h"

#if (BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD)
#include "../../libUDB/mpu6000.h" // required for UDB5 and AUAV3 for heartbeat
#endif

int charPosition = 0;
boolean didDisplay = 0;
int countdown = HEARTBEAT_HZ;
char skip = 0;


void osd_update_glyph(void)
{
	osd_spi_write(0x9, charPosition);   // CMAH: set glyph to overwrite

	unsigned char i;
	for (i = 0; i < 54; i++)
	{
		osd_spi_write(0xA, i);          // CMAL: set the 4-px chunk of the glyph to overwrite
		osd_spi_write(0xB, font_data[charPosition * 64 + i]); // CMDI: update the data representing the 4-px chunk of the glyph
	}

	osd_spi_write(0x8, 0xA0);           // CMM: write glyph to NVRAM

	charPosition++;

	udb_led_toggle(LED_GREEN);          // Flash the green LED after each char is updated
}

// Called at HEARTBEAT_HZ
void udb_heartbeat_callback(void)
{
	if (countdown)
	{
		// delay for countdown/HEARTBEAT_HZ seconds
		countdown--;
		if (countdown == 0)
		{
			osd_spi_write(0x0, 0x00);   // VM0: disable display of OSD image
		}
		return;
	}

	if (udb_heartbeat_counter % (HEARTBEAT_HZ/40) == 0)
	{
		if (!skip)
		{
			if (charPosition < 256)
			{
				osd_update_glyph();
			}
		}
		skip = !skip;
	}
}

// Called every 1/40 second at low priority
void udb_heartbeat_40hz_callback(void)
{
	if (udb_heartbeat_counter % 20 == 0)
	{
		if (charPosition == 256 && !didDisplay)
		{
			LED_GREEN = LED_ON;
			LED_GREEN = LED_ON;

			osd_spi_write(0x04, 0);     // DMM set to 0
#if (OSD_VIDEO_FORMAT == OSD_NTSC)
			osd_spi_write(0x0, 0x08);   // VM0: enable display of OSD image, NTSC
#else
			osd_spi_write(0x0, 0x48);   // VM0: enable display of OSD image, PAL
#endif

			int row;
			for (row = 0; row < 11; row++)
			{
				osd_spi_write_location(OSD_LOC(row + 1, 3));
				osd_spi_write(0x04, 1); // DMM: Enable auto-increment mode
				int col;
				for (col = 0; col < 24; col++)
				{
					osd_spi_write_byte(row * 24 + col);
				}
				osd_spi_write_byte(0xFF);
				didDisplay = 1;
			}
		}
		else
		{
			udb_led_toggle(LED_RED);
		}
	}
}

int main(void)
{
	mcu_init();
	udb_init();

	while (1)
	{
		udb_run();
	}
	return 0;
}


// Unused callbacks
void udb_background_callback_triggered(void) {}
void udb_gps_callback_received_byte(uint8_t rxchar) {}
int16_t udb_gps_callback_get_byte_to_send(void) { return 0; }
void udb_serial_callback_received_byte(uint8_t rxchar) {}
int16_t udb_serial_callback_get_byte_to_send(void) { return 0; }
void udb_magnetometer_callback_data_available(void) {}
void udb_callback_read_sensors(void) {}
void udb_callback_radio_did_turn_off(void) {}


void init_analogs(void) {}
//void init_events(void) {}
void radioIn_init(void) {}
#if (BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD)
	void MPU6000_init16(callback_fptr_t fptr); // required for UDB5 and AUAV3 for heartbeat
#else
	void MPU6000_init16(void) {}
	int16_t vref_adj;
#endif

int16_t failSafePulses = 0;
int16_t noisePulses = 0;

void init_gps(void) {}
void udb_init_ADC(void) {}
void calculate_analog_sensor_values(void) {}

//struct ADchannel udb_xaccel;
//struct ADchannel udb_yaccel;
//struct ADchannel udb_zaccel;
//struct ADchannel udb_xrate;
//struct ADchannel udb_yrate;
//struct ADchannel udb_zrate;

void start_pwm_outputs(void) {}
void servoOut_init(void) {}

void udb_init_GPS(void) {}
void udb_init_USART(void) {}
void udb_eeprom_init(void) {}

void udb_gyros_auto_zero_latch_up(void) {}
void udb_gyros_auto_zero_latch_down(void) {}
