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
#include "delay.h"

#include "uart1.h"
#include "I2C.h"
//#include "bmp085.h"
//#include "scp1000.h"
#include "mpu6000.h"

//#include "magnetometer.h"
//#include "barometer.h"
//#include "..\libDCM\estYawDrift.h"
//#include "..\libDCM\estAltitude.h"

#include <math.h>
#include <stdio.h>

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#endif


int trigger_one_hertz = 0;
int trigger_forty_hertz = 0;


void testproc_init(void)
{
#if (BAROMETER_ALTITUDE == 1)
//	scp1000_init();
//	i2c2_init();
//	bmp085_init();
//	udb_init_I2C();
#endif
	MPU6000_init();

	//test_HMC5843();			// display configuration and ID registers
	LED_RED = LED_ON;	
	delay_ms(1000);
	LED_RED = LED_OFF;
}

#if (BAROMETER_ALTITUDE == 1)
int bmp085_test(void)
{
	const float p0 = 101325;     // Pressure at sea level (Pa)
//	const float p0 = 101660;     // Pressure at sea level (Pa)  -- currently according to BMCC weather station

//float ground_altitude = 308.0;	// altitude at valley heights
//float ground_altitude = 372.0;		// altitude at springwood council
	long temperature;
	long pressure;
	float altitude;

	temperature = bmp085GetTemperature();
	pressure = bmp085GetPressure();
 	altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));

//	long sea_level_pressure;
//	sea_level_pressure = (pressure / pow((1 - (ground_altitude/44330.0)), 5.255));
//	printf( "Temperature = %ld deg C, Pressure = %ld Pa, Altitude = %f m\r\n", temperature, pressure, (double)altitude);
//	printf( "Temperature = %.1f deg C, Pressure = %.2f mB, Altitude = %.2f m\r\n", (double)temperature / 10.0, (double)pressure / 100.0, (double)altitude);
	printf( "T = %.1f C, P = %.2f mB, A = %.2f m\r\n", (double)temperature / 10.0, (double)pressure / 100.0, (double)altitude);
// 	altitude = (float)44330 * (1 - pow(((float) sea_level_pressure/p0), 0.190295));
//	printf( "T = %.1f C, P = %.2f mB, A = %.2f m\r\n", (double)temperature / 10.0, (double)sea_level_pressure / 100.0, (double)altitude);
	return 0;
}

void shared_i2c_test(void)
{
	static int toggle = 0;
	static int counter = 0;

	if (toggle) {
#if (MAG_YAW_DRIFT == 1)
		rxMagnetometer(udb_magnetometer_callback) ;
#endif
		toggle = 0;
	} else {
#if (BAROMETER_ALTITUDE == 1)
		rxBarometer(udb_barometer_callback);
#endif
		if (counter++ > 6) {
			counter = 0;
			toggle = 1;
		}
	}
}

void scp1000_test(void)
{
	const float p0 = 101325;     // Pressure at sea level (Pa)
	long temperature;
	long pressure;
	float altitude;
	int status;

	temperature = scp1000_get_temperature();
	pressure = scp1000_get_pressure();
	status = scp1000_get_status();
 	altitude = (float)44330 * (1 - pow(((float) pressure/p0), 0.190295));

	printf( "T = %.1f C, P = %.2f mB, A = %.2f m, [Status %u]\r\n", (double)temperature, 
    	                                                            (double)pressure / 100.0, 
	                                                                (double)altitude, 
        	                                                        status);
}
#endif

extern struct ADchannel udb_xaccel, udb_yaccel , udb_zaccel ; // x, y, and z accelerometer channels
extern struct ADchannel udb_xrate , udb_yrate, udb_zrate ;  // x, y, and z gyro channels
extern struct ADchannel udb_vref ; // reference voltage

void IDG_print(void)
{
//	printf( "axyz %06i %06i %06i gxyz %06i %06i %06i t %u\r\n", accelX, accelY, accelZ, gyroX, gyroY, gyroZ, mpu_temp);
	printf( "axyz %06i %06i %06i gxyz %06i %06i %06i\r\n", udb_xaccel.input, udb_yaccel.input, udb_zaccel.input, udb_xrate.input, udb_yrate.input, udb_zrate.input);
}

void testproc_loop(void) // currently called continuously
{
	if (trigger_forty_hertz) {
		static int beatcnt = 0;
		if (beatcnt++ % 10) {
//			shared_i2c_test();
//			uart_test();

//		scp1000_test();
//		bmp085_test();

#ifdef USE_DEBUG_IO
			IDG_print();
	LED_BLUE = LED_ON;	MPU6000_read();
	LED_BLUE = LED_OFF;	MPU6000_print();
#endif
//			udb_led_toggle(LED_BLUE);
		}
		trigger_forty_hertz = 0;
	}

	if (trigger_one_hertz) {
		udb_led_toggle(LED_ORANGE);
		trigger_one_hertz = 0;
	}
}
