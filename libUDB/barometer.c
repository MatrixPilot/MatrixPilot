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

#if (USE_BAROMETER == 1 && HILSIM != 1)

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
	#define BMP085_COMMAND 0xEE  // I2C address of BMP085
	static barCalib_union_t bc;
	static long b5;
	
	static unsigned char bmp085read_barCalib[] = { 0xAA } ;	// Address of the first register to read
	static unsigned char bmp085read_barTemp[]  = { 0x2E } ;
	static unsigned char bmp085read_barPres[]  = { 0x34 + (OSS<<6) } ;
	static unsigned char bmp085read_barData[]  = { 0xF6 } ;
	static unsigned char bmp085write_index[]   = { 0xF4 } ;	// Address of the command register to write
	
	static unsigned char barData[3];
		
	static int  brindex ;  					// index into the read write buffer 
	static int  barMessage = 0 ; 			// message type, state machine counter
	static int  barCalibPause = 0 ;
	
	void ReadBarTemp_callback( boolean I2CtrxOK );
	void ReadBarPres_callback( boolean I2CtrxOK );
	void ReadBarCalib_callback( boolean I2CtrxOK );
	#define I2C_Normal		I2C2_Normal
	#define I2C_Read		I2C2_Read
	#define I2C_Write		I2C2_Write
	#define I2C_reset		I2C2_reset
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
	
		brindex = 0 ;
	
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
			case  2:					// put barometer into the power up defaults on a reset
				break ;
			case  3:  					// clear out any data that is still there
				I2C_Read(BMP085_COMMAND, bmp085read_barCalib, 1, bc.buf, 22, &ReadBarCalib_callback);
				break ;
			case  4:  					// enable the calibration process
				barCalibPause = 2 ;		// probably not required
				I2C_Write(BMP085_COMMAND, bmp085write_index, 1, bmp085read_barTemp, 1, NULL);
				break ;
			case  5 :
				I2C_Read(BMP085_COMMAND, bmp085read_barData, 1, barData, 2, &ReadBarTemp_callback);
				break ;
			case  6 :
				barCalibPause = 2 ;		// probably not required
				I2C_Write(BMP085_COMMAND, bmp085write_index, 1, bmp085read_barPres, 1, NULL);
				break ;
			case 7 :
				I2C_Read(BMP085_COMMAND, bmp085read_barData, 1, barData, 3, &ReadBarPres_callback);
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
	
	// there has to be a better (optimised) way of doing this with a dsPIC (asm guru ?)
	static void byteswaparray(unsigned char* ary, int len)
	{
		uint16_t i;
		uint8_t tmp;
	
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
		}
	}
	// Calculate temperature given ut.
	// Value returned originally in units of 0.1 deg C
	static int32_t bmp085CalcTemperature(uint16_t ut)
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
	static int32_t bmp085CalcPressure(uint32_t up)
	{
	    int32_t x1, x2, x3, b3, b5, b6, p;
	    uint32_t b4, b7, tmp;
	    //int32_t tmp;
	    b6 = b5 - 4000;
		// Calculate B3
	    x1 = (bc.b2 * (b6 * b6 >> 12)) >> 11;
	    x2 = bc.ac2 * b6 >> 11;
	    x3 = x1 + x2;

		// better (?) than:
		//b3 = (((bc.ac1 * 4 + x3) << OSS) + 2) / 4;
	    tmp = bc.ac1;
	    tmp = (tmp*4 + x3)<<OSS;
	    b3 = (tmp+2)/4;
	    x1 = bc.ac3 * b6 >> 13;
	    x2 = (bc.b1 * (b6 * b6 >> 12)) >> 16;
	    x3 = ((x1 + x2) + 2) >> 2;
	    b4 = (bc.ac4 * (uint32_t) (x3 + 32768)) >> 15;
	    b7 = ((uint32_t)up - b3) * (50000 >> OSS);
	    p = b7 < 0x80000000 ? (b7 * 2) / b4 : (b7 / b4)*2;
	
	    x1 = (p >> 8) * (p >> 8);
	    x1 = (x1 * 3038) >> 16;
	    x2 = (-7357 * p) >> 16;
	    p = p + ((x1 + x2 + 3791) >> 4);
		return p;
	}
	
	void ReadBarTemp_callback( boolean I2CtrxOK )
	{
		int32_t temperature;
		int16_t ut;
	
		if( I2CtrxOK == true )
		{
			ut = (int16_t)(barData[0] << 8 | barData[1]);
			temperature = bmp085CalcTemperature(ut);
			udb_flags._.healthy = true;
		}
		else
		{
			// the operation failed 
			udb_flags._.healthy = false;
		}
	}
	
	void ReadBarPres_callback( boolean I2CtrxOK )
	{
		uint32_t pressure;
	
		if( I2CtrxOK == true )
		{
		  	pressure = ((uint32_t) barData[0] << 16 
					 | (uint32_t)barData[1] << 8 
					 | (uint32_t)barData[2]) >> (8-OSS);
			pressure = bmp085CalcPressure(pressure);
			if (barometer_callback != NULL)
			{
				barometer_callback(pressure, ((b5 + 8) >> 4), 0);   // Callback
			}
			udb_flags._.healthy = true;
		}
		else
		{
			// the operation failed
			udb_flags._.healthy = false;
		}
	}

#endif // USE_BAROMETER

// GOOFY'S PARKING LOT:
	
			//long x1, x2, x3, b3, b6, p;
			//unsigned long b4, b7; 
			//long x1, x2, x3, b3, b6, b7, p;
			//unsigned long b4;
			/*

		static int32_t bmp085CalcTemperature(uint16_t ut)
		{
			int32_t x1, x2;
			int16_t t;
		
			x1 = (((int32_t)ut - (int32_t)bc.ac6)*(int32_t)bc.ac5) >> 15;
			x2 = ((int32_t)bc.mc << 11) / (x1 + bc.md);
			b5 = x1 + x2;
			t = (b5 + 8) >> 4; 
			return t; 
		}

		static int32_t bmp085CalcTemperature(uint16_t ut)
		{
			int32_t x1, x2;
		
			x1 = (((int32_t)ut - (int32_t)bc.ac6)*(int32_t)bc.ac5) >> 15;
			x2 = ((int32_t)bc.mc << 11) / (x1 + bc.md);
			b5 = x1 + x2;
		
			return ((b5 + 8) >> 4); 
		}

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
			*/

			/*
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
			*/

