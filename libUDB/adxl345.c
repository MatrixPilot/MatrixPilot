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

void I2C_doneReadAccData( boolean I2CtrxOK ) ;


unsigned char adxl345reg_index[] = {0x00, 0x1E, 0x2C, 0x32 } ;	// Address of the first block of registers:DEVID[0], offsets[1], device config[2], measure registers[3]
#define DEVID 		&adxl345reg_index[0]
#define OFSX		&adxl345reg_index[1]
#define BW_RATE		&adxl345reg_index[2]
#define DATAX0		&adxl345reg_index[3]

// Values from 0x2C(BW_RATE) to 0x31(DATA_FORMAT)
unsigned char enableAccRead[]			= { 0x0F , 0x08 , 0x00 , 0x00 , 0x02 , 0x0B } ;	// Continous measurament, 50Hz ODR,  Full resolution, Range +-16g
unsigned char enableAccCalibration[]	= { 0x0F , 0x00 , 0x00 , 0x00 , 0x02 , 0x00 } ;	// Positive bias (Self Test) and single measurament
unsigned char resetAccelerometer[]		= { 0x0F , 0x00 , 0x00 , 0x00 , 0x02 , 0x00 } ;	// Idle mode (Reset) 

#define ADXL_GAIN_X 5280./(310.)
#define ADXL_GAIN_Y 5280./(318.)
#define ADXL_GAIN_Z 5280./(286.)

unsigned char accreg[6] ;  					// magnetometer read-write buffer
int accMeasureRaw[3] ;

//int I2interrupts = 0 ;

int accindex ;  					// index into the read write buffer 
int accMessage = 0 ; 			// message type

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
		default  :
			accMessage = 0 ;
			break ;
	}
	return ;
}


void I2C_doneReadAccData( boolean I2CtrxOK )
{	
	int vectorIndex ;
	static int sample_count = 0;

	if( I2CtrxOK == true )
	{
		accMeasureRaw[0] = (accreg[1]<<8)+accreg[0]; 
		accMeasureRaw[1] = (accreg[3]<<8)+accreg[2]; 
		accMeasureRaw[2] = (accreg[5]<<8)+accreg[4];

		for ( vectorIndex = 0 ; vectorIndex < 6 ; vectorIndex++ ) accreg[vectorIndex] = 0;

		if ( accMessage == 5 )
		{
			udb_xaccel.sum += accMeasureRaw[1]+84;
			udb_yaccel.sum += accMeasureRaw[0]+22;
			udb_zaccel.sum += accMeasureRaw[2]-1215;

			sample_count ++ ;
	
			//	When there is a chance that read_gyros() and read_accel() will execute soon,
			//  have the new average values ready.
			if ( sample_count >= 22 )
			{	
				udb_xaccel.value =  (int)((float)(__builtin_divsd( udb_xaccel.sum , sample_count ))*ADXL_GAIN_X) ;
				udb_yaccel.value =  (int)((float)(__builtin_divsd( udb_yaccel.sum , sample_count ))*ADXL_GAIN_Y) ;
				udb_zaccel.value =  (int)((float)(__builtin_divsd( udb_zaccel.sum , sample_count ))*ADXL_GAIN_Z) ;
//				udb_xaccel.value =  5280 ;
//				udb_yaccel.value =  0 ;
//				udb_zaccel.value =  0 ;
				udb_xaccel.sum = udb_yaccel.sum = udb_zaccel.sum = 0 ;
				sample_count = 0 ;
			}
		}
	}
	return ;
}


#endif
