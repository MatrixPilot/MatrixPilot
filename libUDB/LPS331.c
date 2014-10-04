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
#include "barometer.h"
#include "LPS331.h"

#ifdef USE_LPS331

#define I2C_ADDRESS 0x5C  // I2C address of LPS331 with A0 tied low
//#define I2C_ADDRESS 0x5D  // I2C address of LPS331 with A0 tied high
#define USE_I2C_BUS 2

#if (USE_I2C_BUS == 1)
	#define I2C_Normal      I2C1_Normal
	#define I2C_Read        I2C1_Read
	#define I2C_Write       I2C1_Write
	#define I2C_Reset       I2C1_Reset
#elif (USE_I2C_BUS == 2)
	#define I2C_Normal      I2C2_Normal
	#define I2C_Read        I2C2_Read
	#define I2C_Write       I2C2_Write
	#define I2C_Reset       I2C2_Reset
#endif

//boolean I2C2_Write(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback);
//boolean I2C2_Read(uint8_t addr, const uint8_t* cmd, uint8_t cmd_len, uint8_t* data, uint16_t data_len, I2C_callbackFunc callback, uint16_t mode);

static barometer_callback_funcptr barometer_callback = NULL;
static uint8_t buf[10];

static void ReadWhoAmI_callback(boolean I2CtrxOK)
{
	if (I2CtrxOK == true)
	{
		printf("LPS331 WhoAmI return 0x%02X\r\n", buf[0]);
		if (buf[0] == 0xBB)
		{
		}
		else
		{
		}
	}
}

static void ReadData_callback(boolean I2CtrxOK)
{
	int i;

	if (I2CtrxOK == true)
	{
		printf("LPS331 data:");
		for (i = 0; i < 5; i++)
		{
			printf(" 0x%02X", buf[i]);
		}
		printf("\r\n");

/*
		int temperature;
		temperature = bmp085CalcTemperature(ut);
		long pressure;
		pressure = ((long)barData[0] << 16 | (long)barData[1] << 8 | (long)barData[2]) >> (8-OSS);
		pressure = bmp085CalcPressure(pressure);
		if (barometer_callback != NULL)
		{
			barometer_callback(pressure, temperature, 0);
		}
 */
	}
}

int rxBarometer(barometer_callback_funcptr callback)  // service the barometer
{
	static int state = 0;

	barometer_callback = callback;

	if (I2C_Normal())
	{

static const unsigned char lps331_whoami[]       = { LPS331_WHO_AM_I };
static const unsigned char lps331_ctrl_reg1[]    = { LPS331_CTRL_REG1 };
static const unsigned char lps331_press_out_xl[] = { (LPS331_PRESS_OUT_XL | (1 << 7)) };

		switch (state++)
		{ 
		case 0: // on first pass we get the barometer calibration constants
//			I2C_Read(I2C_ADDRESS, bmp085read_barCalib, 1, bc.buf, 22, &ReadBarCalib_callback, I2C_MODE_WRITE_ADDR_READ);
			I2C_Read(I2C_ADDRESS, lps331_whoami, 1, buf, 1, &ReadWhoAmI_callback, I2C_MODE_WRITE_ADDR_READ);
			break;
		case 1:
//			I2C_Write(I2C_ADDRESS, bmp085write_index, 1, bmp085read_barTemp, 1, NULL);
			buf[0] = 0b11100000;  // turns on sensor and enables continuous output
			                      // active mode, 12.5 Hz output data rate
			I2C_Write(I2C_ADDRESS, lps331_ctrl_reg1, 1, buf, 1, NULL);
			break;
		case 2:
			// assert MSB to enable register address auto-increment
			I2C_Read(I2C_ADDRESS, lps331_press_out_xl, 1, buf, 5, &ReadData_callback, I2C_MODE_WRITE_ADDR_READ);
		case 3:
			break;
		case 4:
//			I2C_Read(I2C_ADDRESS, bmp085read_barData, 1, barData, 2, &ReadBarTemp_callback, I2C_MODE_WRITE_ADDR_READ);
			break;
		case 5:
//			I2C_Write(I2C_ADDRESS, bmp085write_index, 1, bmp085read_barPres, 1, NULL);
			break;
		case 6:
		case 7:
			break;
		case 8:
//			I2C_Read(I2C_ADDRESS, bmp085read_barData, 1, barData, 3, &ReadBarPres_callback, I2C_MODE_WRITE_ADDR_READ);
			state = 1;  // on next pass go back and read the temperature
			break;
		default:
			return 0;   // barometer driver has finished a cycle
		}
	}
	else
	{
		state = 0;
		I2C_Reset();
	}
	return 1;           // barometer driver is busy with the bus
}

#endif // USE_LPS331

/*
// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
//#define LPS331AP_ADDRESS_SA0_LOW  0b1011100
//#define LPS331AP_ADDRESS_SA0_HIGH 0b1011101

// Pololu board pulls SA0 high, so default assumption is that it is high
//static byte address = LPS331AP_ADDRESS_SA0_HIGH;

static bool autoDetectAddress(void)
{
  // try each possible address and stop if reading WHO_AM_I returns the expected response
  address = LPS331AP_ADDRESS_SA0_LOW;
  if (testWhoAmI()) return true;
  address = LPS331AP_ADDRESS_SA0_HIGH;
  if (testWhoAmI()) return true;

  return false;
}

static bool testWhoAmI(void)
{
  return (readReg(LPS331_WHO_AM_I) == 0xBB);
}

//sa0 = LPS331_SA0_AUTO

// sets or detects slave address; returns bool indicating success
bool init(byte sa0)
{
  switch(sa0)
  {
    case LPS331_SA0_LOW:
      address = LPS331AP_ADDRESS_SA0_LOW;
      return testWhoAmI();

    case LPS331_SA0_HIGH:
      address = LPS331AP_ADDRESS_SA0_HIGH;
      return testWhoAmI();

    default:
      return autoDetectAddress();
  }
}

// turns on sensor and enables continuous output
void enableDefault(void)
{
  // active mode, 12.5 Hz output data rate
  writeReg(LPS331_CTRL_REG1, 0b11100000);
}

// writes register
void writeReg(byte reg, byte value)
{
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

// reads register
byte readReg(byte reg)
{
  byte value;

  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission(false); // restart
  Wire.requestFrom(address, (byte)1);
  value = Wire.read();
  Wire.endTransmission();

  return value;
}

// reads pressure in millibars (mbar)/hectopascals (hPa)
float readPressureMillibars(void)
{
  return (float)readPressureRaw() / 4096;
}

// reads pressure in inches of mercury (inHg)
float readPressureInchesHg(void)
{
  return (float)readPressureRaw() / 138706.5;
}

// reads pressure and returns raw 24-bit sensor output
long readPressureRaw(void)
{
  Wire.beginTransmission(address);
  // assert MSB to enable register address auto-increment
  Wire.write(LPS331_PRESS_OUT_XL | (1 << 7));
  Wire.endTransmission();
  Wire.requestFrom(address, (byte)3);

  while (Wire.available() < 3);

  uint8_t pxl = Wire.read();
  uint8_t pl = Wire.read();
  uint8_t ph = Wire.read();

  // combine bytes
  // GCC performs an arithmetic right shift for signed negative
  // numbers, but this code will not work if you port it to a
  // compiler that does a logical right shift instead.
  return (int32_t)ph << 16 | (uint16_t)pl << 8 | pxl;
}

// reads temperature in degrees C
float readTemperatureC(void)
{
  return 42.5 + (float)readTemperatureRaw() / 480;
}

// reads temperature in degrees F
float readTemperatureF(void)
{
  return 108.5 + (float)readTemperatureRaw() / 480 * 1.8;
}

// reads temperature and returns raw 16-bit sensor output
int readTemperatureRaw(void)
{
  Wire.beginTransmission(address);
  // assert MSB to enable register address auto-increment
  Wire.write(LPS331_TEMP_OUT_L | (1 << 7));
  Wire.endTransmission();
  Wire.requestFrom(address, (byte)2);

  while (Wire.available() < 2);

  uint8_t tl = Wire.read();
  uint8_t th = Wire.read();

  // combine bytes
  // GCC performs an arithmetic right shift for signed negative
  // numbers, but this code will not work if you port it to a
  // compiler that does a logical right shift instead.
  return (int16_t)th << 8 | tl;
}

// converts pressure in mbar to altitude in meters, using 1976 US
// Standard Atmosphere model (note that this formula only applies to a
// height of 11 km, or about 36000 ft)
// If altimeter setting (QNH, barometric pressure adjusted to sea
// level) is given, this function returns an indicated altitude
// compensated for actual regional pressure; otherwise, it returns
// the pressure altitude above the standard pressure level of 1013.25
// mbar or 29.9213 inHg
float pressureToAltitudeMeters(float pressure_mbar, float altimeter_setting_mbar)
{
  return (1 - pow(pressure_mbar / altimeter_setting_mbar, 0.190263)) * 44330.8;
}

// converts pressure in inHg to altitude in feet; see notes above
float pressureToAltitudeFeet(float pressure_inHg, float altimeter_setting_inHg)
{
  return (1 - pow(pressure_inHg / altimeter_setting_inHg, 0.190263)) * 145442;
}
 */
 
