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
#include "I2C.h"
#include "interrupt.h"
#include "mpu_spi.h"
#include "barometer.h"
#include "oscillator.h"
#include "delay.h"

#if (USE_BAROMETER_ALTITUDE == 1)

#define BMP085_ADDRESS 0xEE  // I2C address of BMP085
#define USE_BMP085_ON_I2C 2

// BMP085 oversampling can be set from 0 thru 3
#ifdef TEST_WITH_DATASHEET_VALUES
#define OSS 0
# else
#define OSS 3
//#define OSS 2
//#define OSS 1
//#define OSS 0
#endif

typedef union
{
	struct {
		int16_t ac1;
		int16_t ac2;
		int16_t ac3;
		uint16_t ac4;
		uint16_t ac5;
		uint16_t ac6;
		int16_t  b1;
		int16_t  b2;
		int16_t  mb;
		int16_t  mc;
		int16_t  md;
	};
	uint8_t buf[22];
} barCalib_union_t;

static barCalib_union_t bc; // barometer calibration constants
static int32_t b5; // this is the important number for calculating the temperature

static const uint8_t bmp085read_barCalib[] = { 0xAA };  // Address of the first register to read
static const uint8_t bmp085read_barData[]  = { 0xF6 };

static const uint8_t bmp085write_index[]   = { 0xF4 };  // Address of the command register to write
static uint8_t bmp085read_barTemp[]        = { 0x2E };
static uint8_t bmp085read_barPres[]        = { 0x34 + (OSS<<6) };

static uint8_t barData[3];

static int16_t barMessage = 0;      // message type, state machine counter
static int16_t barCalibPause = 0;

void ReadBarTemp_callback(boolean I2CtrxOK);
void ReadBarPres_callback(boolean I2CtrxOK);
void ReadBarCalib_callback(boolean I2CtrxOK);

#if (USE_BMP085_ON_I2C == 1)
	#define I2C_Normal      I2C1_Normal
	#define I2C_Read        I2C1_Read
	#define I2C_Write       I2C1_Write
	#define I2C_Reset       I2C1_Reset
#elif (USE_BMP085_ON_I2C == 2)
	#define I2C_Normal      I2C2_Normal
	#define I2C_Read        I2C2_Read
	#define I2C_Write       I2C2_Write
	#define I2C_Reset       I2C2_Reset
#endif


barometer_callback_funcptr barometer_callback = NULL;

uint8_t rxBarometer(barometer_callback_funcptr callback)  // service the barometer
{
	barometer_callback = callback;

	if (I2C_Normal() == false)  // if I2C is not ok
	{
		barMessage = 0;         // start over again
		I2C_Reset();            // reset the I2C
		return(BAROMETER_NEEDS_SERVICING);
	}

	if (barCalibPause == 0)
	{
		barMessage++;
		if (barMessage > 7)
		{
			barMessage = 4;
		}
		switch (barMessage)
		{ 
		case 1:
			return(BAROMETER_NEEDS_SERVICING);
		case 2:	
			return(BAROMETER_NEEDS_SERVICING);
		case 3:
			I2C_Read(BMP085_ADDRESS, bmp085read_barCalib, 1, bc.buf, 22, &ReadBarCalib_callback, I2C_MODE_WRITE_ADDR_READ);
			return(BAROMETER_SERVICE_CAN_PAUSE);
		case 4:
			I2C_Write(BMP085_ADDRESS, bmp085write_index, 1, bmp085read_barTemp, 1, NULL);
			return(BAROMETER_NEEDS_SERVICING);
		case 5:
			I2C_Read(BMP085_ADDRESS, bmp085read_barData, 1, barData, 2, &ReadBarTemp_callback, I2C_MODE_WRITE_ADDR_READ);
			return(BAROMETER_NEEDS_SERVICING);
		case 6:
			barCalibPause = 1;  // With OSS of 3, BMP180 needs 25.5 milliseconds to get the 3 oversamples 
			I2C_Write(BMP085_ADDRESS, bmp085write_index, 1, bmp085read_barPres, 1, NULL);
			return(BAROMETER_NEEDS_SERVICING);
		case 7:
			I2C_Read(BMP085_ADDRESS, bmp085read_barData, 1, barData, 3, &ReadBarPres_callback, I2C_MODE_WRITE_ADDR_READ);
			return(BAROMETER_SERVICE_CAN_PAUSE);
		default:
			barMessage = 0;
			return(BAROMETER_SERVICE_CAN_PAUSE);
		}
	}
	else
	{
		barCalibPause--;
		return(BAROMETER_NEEDS_SERVICING);
	}
}

/*
#define BIG_ENDIAN      0
#define LITTLE_ENDIAN   1

int TestByteOrder()
{
	int_16_t word = 0x0001;
	int8_t* byte = (int8_t*)&word;
	return(byte[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}
 */

static void byteswaparray(uint8_t* ary, int16_t len)
{
	int16_t i;
	uint8_t tmp;

	for (i = 0; i < len; i += 2)
	{
		tmp = ary[i];
		ary[i] = ary[i+1];
		ary[i+1] = tmp;
	}
}

void ReadBarCalib_callback(boolean I2CtrxOK)
{	
	if (I2CtrxOK == true)
	{
		byteswaparray(bc.buf, 22);
#ifdef TEST_WITH_DATASHEET_VALUES
		bc.ac1 = 408;
		bc.ac2 = -72;
		bc.ac3 = -14383;
		bc.ac4 = 32741;
		bc.ac5 = 32757;
		bc.ac6 = 23153;
		bc.b1 = 6190;
		bc.b2 = 4;
		bc.mb = -32768;
		bc.mc = -8711;
		bc.md = 2868;
#endif
	}
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
static int32_t bmp085CalcTemperature( uint16_t ut)
{
	int32_t x1, x2;

	x1 = (((int32_t)ut - (int32_t)bc.ac6)*(int32_t)bc.ac5) >> 15;
	x2 = ((int32_t)bc.mc << 11) / (x1 + bc.md);
	b5 = x1 + x2;

	return ((b5 + 8) >> 4); 
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085CalcTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
static int32_t bmp085CalcPressure(int32_t up)
{
	int32_t x1, x2, x3, b3, b6, p;
	uint32_t b4, b7;

	b6 = b5 - 4000;
	// Calculate B3
	x1 = ((int32_t)bc.b2 * ((b6 * b6) >> 12)) >> 11;
	x2 = ((int32_t)bc.ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = ((((int32_t)bc.ac1*4 + x3) << OSS) + 2) >> 2;
	// Calculate B4
	x1 = ((int32_t)bc.ac3 * b6) >> 13;
	x2 = ((int32_t)bc.b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = ((uint32_t)bc.ac4 * (uint32_t)(x3 + 32768)) >> 15;
	b7 = ((uint32_t)up - b3) * (50000UL>>OSS);
	if (b7 < 0x80000000) {
		p = (b7 << 1) / b4;
	} else {
		p = (b7 / b4) << 1;
	}
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p = p + ((x1 + x2 + (int32_t)3791) >> 4);

	return p;
}

void ReadBarTemp_callback(boolean I2CtrxOK)
{
	int32_t temperature;
	uint16_t ut;

	if (I2CtrxOK == true)
	{
		ut = (barData[0] << 8 | barData[1]);
#ifdef TEST_WITH_DATASHEET_VALUES
		ut = 27898;
#endif
		temperature = bmp085CalcTemperature(ut);
	}
	else
	{
		// the operation failed - we should probably do something about it...
	}
}

void ReadBarPres_callback(boolean I2CtrxOK)
{
	int32_t pressure;

	if (I2CtrxOK == true)
	{
		pressure = ((int32_t)barData[0] << 16 | (int32_t)barData[1] << 8 | (int32_t)barData[2]) >> (8-OSS);
#ifdef TEST_WITH_DATASHEET_VALUES
		pressure = 23843;
#endif
		pressure = bmp085CalcPressure(pressure);
		if (barometer_callback != NULL)
		{
			barometer_callback(pressure, ((b5 + 8) >> 4), 0);   // Callback
		}
	}
	else
	{
		// the operation failed - we should probably do something about it...
	}
}

#elif (USE_BAROMETER_ALTITUDE == 2)
#define USE_MPL3115A2_ON_I2C 2
#define MPL3115_ADDRESS 0xC0  // I2C address of MPL3115A2
static unsigned char MPL3115A2_CTRL_REG1[]       = { 0x26 };
//static unsigned char MPL3115A2_INT_SOURCE_REG[]  = { 0x12 };
static unsigned char MPL3115A2_PT_DATA_CFG[]     = { 0x13 };
static unsigned char MPL3115A2_CTRL_REG3[]       = { 0x28 };
static unsigned char MPL3115A2_CTRL_REG4[]       = { 0x29 };
static unsigned char MPL3115A2_CTRL_REG5[]       = { 0x2A };
static unsigned char MPL3115A2_REGISTER_STATUS[] = { 0x00 };
//static unsigned char MPL3115A2_STD_BY[]           = { 0x00 };//
static unsigned char MPL3115A2_ALT_OSR[]         = { 0xA8 };//MPL3115A2_CTRL_REG1_ALT+MPL3115A2_CTRL_REG1_OS132ms+STD_BY
static unsigned char MPL3115A2_PT_CFG[]          = { 0x07 };//MPL3115A2_PT_DATA_CFG_DREM+MPL3115A2_MPL3115A2_PT_DATA_CFG_PDEFE
static unsigned char MPL3115A2_REG3_CFG[]        = { 0x00 };//MPL3115A2_IPOL2 Active low, internal Pull-up
static unsigned char MPL3115A2_REG4_CFG[]        = { 0x80 };//MPL3115A2_INT_EN_DRDY
static unsigned char MPL3115A2_REG5_CFG[]        = { 0x00 };//MPL3115A2_INT_CFG_DRDY on INT2
//static unsigned char MPL3115A2_ACTIVE[]          = { 0xA9 };//MPL3115A2_CTRL_REG1_ALT+MPL3115A2_CTRL_REG1_OS132+MPL3115A2_CTRL_REG1_SBYB
static unsigned char MPL3115A2_OST[]             = { 0xAA };//MPL3115A2_CTRL_REG1_ALT+OS132ms+OST
//static unsigned char MPL3115A2_SRC_DRDY[]        = { 0x80 };//MPL3115A2 INT Data Ready
static unsigned char barData[6];

static int barMessage = 0;      // message type, state machine counter
void ReadBarAltitude_callback(boolean I2CtrxOK);

#if (USE_MPL3115A2_ON_I2C == 1)
	#define I2C_Normal		I2C1_Normal
	#define I2C_Read		I2C1_Read
	#define I2C_Write		I2C1_Write
	#define I2C_Reset		I2C1_Reset
#elif (USE_MPL3115A2_ON_I2C == 2)
	#define I2C_Normal		I2C2_Normal
	#define I2C_Read		I2C2_Read
	#define I2C_Write		I2C2_Write
	#define I2C_Reset		I2C2_Reset
#endif


barometer_callback_funcptr barometer_callback = NULL;

// MPL115A2 Initialization and configuration

void MPL3115A2_Init(void)
{   //Write (device_adress,*Register,Reg byte number,*data,data byte number, data pointer)
    // Std by
    I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG1, 1, MPL3115A2_ALT_OSR, 1, NULL);
    delay_us(300);//Waiting for first message transmission 9 tics * 10탎 * 3 bytes = 720 탎 + 10% margin = 300 탎
    // Read data and status to clear all interrupts
//    I2C_Read(MPL3115_ADDRESS, MPL3115A2_REGISTER_STATUS, 1, barData, 6, &ReadBarAltitude_callback, I2C_MODE_WRITE_ADDR_READ);
//    delay_us(800);//Waiting for first message transmission 9 tics * 10탎 * 8 bytes = 720 탎 + 10% margin = 300 탎
    // interrupt on data ready, raise event on new altitude
    I2C_Write(MPL3115_ADDRESS, MPL3115A2_PT_DATA_CFG, 1, MPL3115A2_PT_CFG, 1, NULL);//PT_DATA_CFG=06
    delay_us(300);
    // interrupt on data ready, interrupt active low and internal pull_up interface on INT1 & INT2
    I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG3, 1, MPL3115A2_REG3_CFG, 1, NULL);//REG3=0x11
    delay_us(300);
    // interrupt on data ready, route interrupt on INT2
    I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG4, 1, MPL3115A2_REG4_CFG, 1, NULL);
     delay_us(300);
    // interrupt on data ready, raise event on new altitude
    I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG5, 1, MPL3115A2_REG5_CFG, 1, NULL);
     delay_us(300);
    // Barometer activate altitude mode and OS=b101, 32 samples, 132 ms minimum time between sample, 6Hz sample rate
    I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG1, 1, MPL3115A2_OST, 1, NULL);
    delay_us(300);

}
uint8_t rxBarometer(barometer_callback_funcptr callback)  // service the MPL3115A2 barometer
{
	barometer_callback = callback;

	if (I2C_Normal() == false)  // if I2C is not ok
	{
		I2C_Reset();            // reset the I2C
		return(BAROMETER_NEEDS_SERVICING);
	}
		barMessage++;
		if (barMessage > 2)
		{
			barMessage = 1;
		}
		switch (barMessage)
		{ 
		case 1:
	// Baro activation : enable INT and set OST (One Shot Trigger)

    // Read data and status to clear all interrupts
            I2C_Read(MPL3115_ADDRESS, MPL3115A2_REGISTER_STATUS, 1, barData, 6, &ReadBarAltitude_callback, I2C_MODE_WRITE_ADDR_READ);
            //delay_us(800);//Waiting for first message transmission 9 tics * 10탎 * 8 bytes = 720 탎 + 10% margin = 800 탎
        	return(BAROMETER_NEEDS_SERVICING);
		case 2:
            I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG1, 1, MPL3115A2_OST, 1, NULL);
            //delay_us(300);
//        	return(BAROMETER_NEEDS_SERVICING);
//		case 3:
            _INT1IE = 1; // Enable INT1 Interrupt Service Routine
			return(BAROMETER_SERVICE_CAN_PAUSE);
		default:
			barMessage = 0;
			return(BAROMETER_SERVICE_CAN_PAUSE);
		}

}
#if (MPU_SPI == 1)
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
	_INT3IF = 0; // Clear the INT3 interrupt flag
        _INT3IE = 0; // Disable INT3 Interrupt Service Routine
	indicate_loading_inter;
	interrupt_save_set_corcon;
/*        //Read INT Source
	I2C_Read(MPL3115_ADDRESS, MPL3115A2_INT_SOURCE_REG, 1, barData, 1, &ReadBarAltitude_callback, I2C_MODE_READ_ONLY);
        delay_us(300);
        // Test if data ready
        if ((barData[0] && MPL3115A2_SRC_DRDY)  )
        {
       //Read data
	I2C_Read(MPL3115_ADDRESS, MPL3115A2_REGISTER_STATUS, 1, barData, 6, &ReadBarAltitude_callback, I2C_MODE_WRITE_ADDR_READ);
        delay_us(800);//Waiting for first message transmission 9 tics * 10탎 * 8 bytes = 720 탎 + 10% margin = 300 탎
        }
 */
    // Reset OST
        I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG1, 1, MPL3115A2_ALT_OSR, 1, NULL);
//        delay_us(300);
	interrupt_restore_corcon;
}
#else
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
	_INT1IF = 0; // Clear the INT1 interrupt flag
        _INT1IE = 0; // Disable INT1 Interrupt Service Routine
	indicate_loading_inter;
	interrupt_save_set_corcon;
/*        //Read INT Source
	I2C_Read(MPL3115_ADDRESS, MPL3115A2_INT_SOURCE_REG, 1, barData, 1, &ReadBarAltitude_callback, I2C_MODE_READ_ONLY);
        delay_us(300);
        // Test if data ready
        if ((barData[0] && MPL3115A2_SRC_DRDY)  )
        {
        //Read data
	I2C_Read(MPL3115_ADDRESS, MPL3115A2_REGISTER_STATUS, 1, barData, 6, &ReadBarAltitude_callback, I2C_MODE_WRITE_ADDR_READ);
        delay_us(800);//Waiting for first message transmission 9 tics * 10탎 * 8 bytes = 720 탎 + 10% margin = 300 탎
        }
*/
    // Reset OST
        I2C_Write(MPL3115_ADDRESS, MPL3115A2_CTRL_REG1, 1, MPL3115A2_ALT_OSR, 1, NULL);
//        delay_us(300);
	interrupt_restore_corcon;
}
#endif
void ReadBarAltitude_callback(boolean I2CtrxOK)
{
	long altitude;  // (signed long integer 32 bits) in mm
	long temperature;// in 1/256 캜

	if (I2CtrxOK == true)
	{
                altitude = ((long)barData[1] << 24); // Taking into account the sign
                altitude = ((long)barData[1] >> 6 | (long)barData[2] << 10 | (long)barData[3] << 2) ;
//                altitude = altitude * 1000 / 1024;// the exact altitude value must be corrected by the factor 1/1.024
		temperature = ((long)barData[4] << 8) + (long)barData[5] ;
#ifdef TEST_WITH_DATASHEET_VALUES
		altitude = 23843;
#endif
		if (barometer_callback != NULL)
		{
			barometer_callback(altitude, temperature, 0);   // Callback
		}
	}
	else
	{
		// the operation failed - we should probably do something about it...
	}
}

#endif // USE_BAROMETER_ALTITUDE
