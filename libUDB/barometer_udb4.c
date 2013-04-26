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
#include "I2C.h"
#include "barometer.h"

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == AUAV3_BOARD)

#if (BAROMETER_ALTITUDE == 1)

#define BMP085_ADDRESS 0xEE  // I2C address of BMP085

#if( BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == AUAV3_BOARD)
	#define USE_BMP085_ON_I2C1  0
	#define USE_BMP085_ON_I2C2  1
#elif (BOARD_TYPE == MADRE_BOARD)	
	#define USE_BMP085_ON_I2C1  1
	#define USE_BMP085_ON_I2C2  0	
#endif

// BMP085 oversampling can be set from 0 thru 3
//#define OSS 3
//#define OSS 2
//#define OSS 1
#define OSS 0

typedef union 
{
	struct {
		int ac1;
		int ac2; 
		int ac3; 
		unsigned int ac4;
		unsigned int ac5;
		unsigned int ac6;
		int b1; 
		int b2;
		int mb;
		int mc;
		int md;
	};
	unsigned char buf[22];
} barCalib_union_t;

static barCalib_union_t bc;
static long b5;

static const unsigned char bmp085read_barCalib[] = { 0xAA } ;	// Address of the first register to read
unsigned char bmp085read_barTemp[]  = { 0x2E } ;
unsigned char bmp085read_barPres[]  = { 0x34 + (OSS<<6) } ;
static const unsigned char bmp085read_barData[]  = { 0xF6 } ;
static const unsigned char bmp085read_index[]    = { 0xAA } ;	// Address of the first calibration register to read
static const unsigned char bmp085write_index[]   = { 0xF4 } ;	// Address of the command register to write

static unsigned char barData[3];

static int barMessage = 0 ; 			// message type, state machine counter
static int barCalibPause = 0 ;

void ReadBarTemp_callback( boolean I2CtrxOK );
void ReadBarPres_callback( boolean I2CtrxOK );
void ReadBarCalib_callback( boolean I2CtrxOK );

#if (USE_BMP085_ON_I2C1 == 1)
	#define I2C_Normal		I2C1_Normal
	#define I2C_Read		I2C1_Read
	#define I2C_Write		I2C1_Write
	#define I2C_reset		I2C1_reset
#elif (USE_BMP085_ON_I2C2 == 1)
	#define I2C_Normal		I2C2_Normal
	#define I2C_Read		I2C2_Read
	#define I2C_Write		I2C2_Write
	#define I2C_reset		I2C2_reset
#endif


barometer_callback_funcptr barometer_callback = NULL;

void rxBarometer(barometer_callback_funcptr callback)  // service the barometer
{
	barometer_callback = callback;

	if ( I2C_Normal() == false ) 	// if I2C is not ok
	{
		barMessage = 0 ; 			// start over again
		I2C_reset() ; 				// reset the I2C
		return ;
	}

	if ( barCalibPause == 0 )
	{
		barMessage++ ;
		if ( barMessage > 7 )
		{
			barMessage = 4 ;
		}
		switch ( barMessage )
		{ 
		case  1:    				// read the barometer in case it is still sending data, so as to NACK it
			break ;
		case  2:					// put magnetomter into the power up defaults on a reset
			break ;
		case  3:  					// clear out any data that is still there
			I2C_Read(BMP085_ADDRESS, bmp085read_barCalib, 1, bc.buf, 22, &ReadBarCalib_callback, I2C_MODE_WRITE_ADDR_READ);
			break ;
		case  4:  					// enable the calibration process
			barCalibPause = 2 ;		// probably not required
			I2C_Write(BMP085_ADDRESS, bmp085write_index, 1, bmp085read_barTemp, 1, NULL);
			break ;
		case  5 :
			I2C_Read(BMP085_ADDRESS, bmp085read_barData, 1, barData, 2, &ReadBarTemp_callback, I2C_MODE_WRITE_ADDR_READ);
			break ;
		case  6 :
			barCalibPause = 2 ;		// probably not required
			I2C_Write(BMP085_ADDRESS, bmp085write_index, 1, bmp085read_barPres, 1, NULL);
			break ;
		case 7 :
			I2C_Read(BMP085_ADDRESS, bmp085read_barData, 1, barData, 3, &ReadBarPres_callback, I2C_MODE_WRITE_ADDR_READ);
			break ;
		default  :
			barMessage = 0 ;
			break ;
		}
	}
	else
	{
		barCalibPause -- ;
	}
	return ;
}



/*
#define BIG_ENDIAN      0
#define LITTLE_ENDIAN   1

int TestByteOrder()
{
   short int word = 0x0001;
   char *byte = (char *) &word;
   return(byte[0] ? LITTLE_ENDIAN : BIG_ENDIAN);
}
 */

// there has to be a better (optimised) way of doing this with a dsPIC (asm guru ?)
static void byteswaparray(unsigned char* ary, int len)
{
	int i;
	unsigned char tmp;

	for (i = 0; i < len; i += 2) {
		tmp = ary[i];
		ary[i] = ary[i+1];
		ary[i+1] = tmp;
	}
}

void ReadBarCalib_callback( boolean I2CtrxOK )
{	
	if( I2CtrxOK == true )
	{
		byteswaparray(bc.buf, 22);
/*
		ac1 = (barCalib[0]  << 8) + barCalib[1];
		ac2 = (barCalib[2]  << 8) + barCalib[3];
		ac3 = (barCalib[4]  << 8) + barCalib[5];
		ac4 = (barCalib[6]  << 8) + barCalib[7];
		ac5 = (barCalib[8]  << 8) + barCalib[9];
		ac6 = (barCalib[10] << 8) + barCalib[11];
		b1  = (barCalib[12] << 8) + barCalib[13];
		b2  = (barCalib[14] << 8) + barCalib[15];
		mb  = (barCalib[16] << 8) + barCalib[17];
		mc  = (barCalib[18] << 8) + barCalib[19];
		md  = (barCalib[20] << 8) + barCalib[21];
 */
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
static long bmp085CalcTemperature(int ut)
{
	long x1, x2;

	x1 = (((long)ut - (long)bc.ac6)*(long)bc.ac5) >> 15;
	x2 = ((long)bc.mc << 11) / (x1 + bc.md);
	b5 = x1 + x2;

	return ((b5 + 8) >> 4); 
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085CalcTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
static long bmp085CalcPressure(long up)
{
	long x1, x2, x3, b3, b6, b7, p;
	unsigned long b4;

	b6 = b5 - 4000;
	// Calculate B3
	x1 = (bc.b2 * ((b6 * b6) >> 12)) >> 11;
	x2 = (bc.ac2 * b6) >> 11;
	x3 = x1 + x2;
	b3 = (((bc.ac1*4 + x3) << OSS) + 2) >> 2;
	// Calculate B4
	x1 = (bc.ac3 * b6) >> 13;
	x2 = (bc.b1 * ((b6 * b6) >> 12)) >> 16;
	x3 = ((x1 + x2) + 2) >> 2;
	b4 = (bc.ac4 * (unsigned long)(x3 + 32768)) >> 15;
	b7 = ((unsigned long)up - b3) * (50000>>OSS);
	if (b7 < 0x80000000) {
		p = (b7 << 1) / b4;
	} else {
		p = (b7 / b4) << 1;
	}
	x1 = (p >> 8) * (p >> 8);
	x1 = (x1 * 3038) >> 16;
	x2 = (-7357 * p) >> 16;
	p = p + ((x1 + x2 + 3791) >> 4);

	return p;
}

void ReadBarTemp_callback( boolean I2CtrxOK )
{
	long temperature;
	int ut;

	if( I2CtrxOK == true )
	{
//		byteswaparray((unsigned char*)&barData, 2);
//		temperature = ((long) (long)barData[0] << 8 | (long)barData[1]);
  		ut = ((int) (int)barData[0] << 8 | (int)barData[1]);
		temperature = ut;
#ifdef TEST_WITH_DATASHEET_VALUES
		temperature = 27898;
#endif
		temperature = bmp085CalcTemperature(temperature);
	}
	else
	{
		// the operation failed - we should probably do something about it...
	}
}

void ReadBarPres_callback( boolean I2CtrxOK )
{
	long pressure;

	if( I2CtrxOK == true )
	{
	  	pressure = ((long) barData[0] << 16 | (long)barData[1] << 8 | (long)barData[2]) >> (8-OSS);
#ifdef TEST_WITH_DATASHEET_VALUES
		pressure = 23843;
#endif
		pressure = bmp085CalcPressure(pressure);
		if (barometer_callback != NULL) {
			barometer_callback(pressure, ((b5 + 8) >> 4), 0);		// Callback
		}
	}
	else
	{
		// the operation failed - we should probably do something about it...
	}
}


#endif // (BAROMETER_ALTITUDE == 1)

#endif // (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == AUAV3_BOARD)
