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

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == MADRE_BOARD)

#define ADXL345_ADDRESS 0xA6		// Address of the accelerometer (0x53) left shifted (0xA6)

#if( BOARD_TYPE == UDB4_BOARD)
	#define USE_ADXL345_ON_I2C1  0
	#define USE_ADXL345_ON_I2C2  1
#elif (BOARD_TYPE == MADRE_BOARD)	
	#define USE_ADXL345_ON_I2C1  1
	#define USE_ADXL345_ON_I2C2  0	
#endif

void I2C_doneReadAccData( boolean I2CtrxOK );

unsigned char adxl345reg_index[] = {0x00, 0x1E, 0x2C, 0x32 } ;	// Address of the first block of registers:DEVID[0], offsets[1], device config[2], measure registers[3]
#define DEVID 		&adxl345reg_index[0]
#define OFSX		&adxl345reg_index[1]
#define BW_RATE		&adxl345reg_index[2]
#define DATAX0		&adxl345reg_index[3]

// Values from 0x2C(BW_RATE) to 0x31(DATA_FORMAT)
unsigned char enableAccRead[]			= { 0x0A , 0x08 , 0x00 , 0x00 , 0x02 , 0x0B } ;	// Continous measurament, 100Hz ODR,  Full resolution, Range +-16g
unsigned char enableAccCalibration[]	= { 0x0A , 0x00 , 0x00 , 0x00 , 0x02 , 0x00 } ;	// Positive bias (Self Test) and single measurament
unsigned char resetAccelerometer[]		= { 0x0A , 0x00 , 0x00 , 0x00 , 0x02 , 0x00 } ;	// Idle mode (Reset) 

int udb_accOffset[3] = { 0 , 0 , 0 } ;  	// acceleration offset in the body frame of reference
int accGain[3] = { RMAX , RMAX , RMAX } ; 	// magnetometer calibration gains
int rawAccCalib[3] = { 0 , 0 , 0 } ;
unsigned char accreg[6] ;  					// magnetometer read-write buffer
int accMeasureRaw[3] ;

//int I2interrupts = 0 ;

int accindex ;  					// index into the read write buffer 
int accMessage = 0 ; 			// message type
int accCalibPause = 0 ;
//int I2messages = 0 ;

#if (USE_ADXL345_ON_I2C1 == 1)
	#define I2C_Normal		I2C1_Normal
	#define I2C_Read		I2C1_Read
	#define I2C_Write		I2C1_Write
	#define I2C_reset		I2C1_reset
#elif (USE_ADXL345_ON_I2C2 == 1)
	#define I2C_Normal		I2C2_Normal
	#define I2C_Read		I2C2_Read
	#define I2C_Write		I2C2_Write
	#define I2C_reset		I2C2_reset
#endif

void udb_init_accelerometer(void)
{
//	rxAccelerometer();
	return ;
}

void rxAccelerometer(void)  		// service the accelerometer
{
//	I2messages++ ;
#if ( LED_RED_ACC_CHECK == 1 )
	if ( accMessage == 5 )
	{
		LED_RED = LED_OFF ;
	}
	else
	{
		LED_RED = LED_ON ;
	}
#endif

	if ( I2C_Normal() == false ) 	// if I2C is not ok
	{
		accMessage = 0 ; 			// start over again
		I2C_reset() ; 				// reset the I2C
		return ;
	}

	accindex = 0 ;

	if ( accCalibPause == 0 )
	{
		accMessage++ ;
		if ( accMessage > 5 )
		{
			accMessage = 5 ;
		}
		switch ( accMessage )
		{ 
		case  1:    				// read the accelerometer in case it is still sending data, so as to NACK it
			I2C_Read(ADXL345_ADDRESS, DATAX0, 1, accreg, 6, &I2C_doneReadAccData); 
			break ;
		case  2:					// put magnetomter into the power up defaults on a reset
			I2C_Write(ADXL345_ADDRESS, BW_RATE, 1, resetAccelerometer, 6, NULL);
			break ;
		case  3:  					// clear out any data that is still there
			I2C_Read(ADXL345_ADDRESS, DATAX0, 1, accreg, 6, &I2C_doneReadAccData);
			break ;
		case  4 :   				// enable normal continuous readings
			I2C_Write(ADXL345_ADDRESS, BW_RATE, 1, enableAccRead, 6, NULL);
			break ;
		case  5 :  					// read the magnetometer data
			I2C_Read(ADXL345_ADDRESS, DATAX0, 1, accreg, 6, &I2C_doneReadAccData);
			break ;
			
/*		case  4:  					// enable the calibration process
			magCalibPause = 2 ;
			I2C_Write(ADXL345_ADDRESS, hmc5883write_index, 1, enableMagCalibration, 3, NULL);
			break ;
		case  5 : 					// read the calibration data
			I2C_Read(ADXL345_ADDRESS, hmc5883read_index, 1, magreg, 6, &I2C_doneReadMagData);
			break ;
*/			
		default  :
			accMessage = 0 ;
			break ;
		}
	}
	else
	{
		accCalibPause -- ;
	}
	return ;
}


int previousaccMeasureRaw[3] = { 0 , 0 , 0 } ;


void I2C_doneReadAccData( boolean I2CtrxOK )
{	
	int vectorIndex ;

	if( I2CtrxOK == true )
	{
		accMeasureRaw[0] = (accreg[1]<<8)+accreg[0] ; 
		accMeasureRaw[1] = (accreg[3]<<8)+accreg[2] ; 
		accMeasureRaw[2] = (accreg[5]<<8)+accreg[4] ;

		for ( vectorIndex = 0 ; vectorIndex < 6 ; vectorIndex++ ) accreg[vectorIndex] = 0;


/*
		previousaccMeasureRaw[0] = accMeasureRaw[0] ;
		previousaccMeasureRaw[1] = accMeasureRaw[1] ;
		previousaccMeasureRaw[2] = accMeasureRaw[2] ;

		if ( magMessage == 7 )
		{
			udb_magFieldBody[0] = MAG_X_SIGN((__builtin_mulsu((accMeasureRaw[MAG_X_AXIS]), magGain[MAG_X_AXIS] ))>>14)-(udb_magOffset[0]>>1) ;
			udb_magFieldBody[1] = MAG_Y_SIGN((__builtin_mulsu((accMeasureRaw[MAG_Y_AXIS]), magGain[MAG_Y_AXIS] ))>>14)-(udb_magOffset[1]>>1) ;
			udb_magFieldBody[2] = MAG_Z_SIGN((__builtin_mulsu((accMeasureRaw[MAG_Z_AXIS]), magGain[MAG_Z_AXIS] ))>>14)-(udb_magOffset[2]>>1) ;

			if ( ( abs(udb_magFieldBody[0]) < MAGNETICMAXIMUM ) &&
				 ( abs(udb_magFieldBody[1]) < MAGNETICMAXIMUM ) &&
				 ( abs(udb_magFieldBody[2]) < MAGNETICMAXIMUM ) )
			{
				udb_magnetometer_callback_data_available();
			}
			else
			{
				magMessage = 0 ; // invalid reading, reset the magnetometer
			}
		}
		else if ( magMessage == 5 )		// Calibration data
		{
			for ( vectorIndex = 0 ; vectorIndex < 3 ; vectorIndex++ )
			{
				rawMagCalib[vectorIndex] = accMeasureRaw[vectorIndex] ;
				if (  ( accMeasureRaw[vectorIndex] > MAGNETICMINIMUM ) && ( accMeasureRaw[vectorIndex] < MAGNETICMAXIMUM ) )
				{
					magGain[vectorIndex] = __builtin_divud( ((long) ( MAG_GAIN*RMAX)), accMeasureRaw[vectorIndex] ) ;
				}
				else
				{
					magGain[vectorIndex] = RMAX ;
					magMessage = 0 ;  // invalid calibration, reset the magnetometer
	
				}
			}
		}
		*/
	}
	return ;
}


#endif
