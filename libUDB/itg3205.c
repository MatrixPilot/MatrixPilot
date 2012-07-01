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

#define ITG3205_ADDRESS 0xD0		// Address of the gyroscope (0x68) left shifted (0xD0)

#if( BOARD_TYPE == UDB4_BOARD)
	#define USE_ITG3205_ON_I2C1  0
	#define USE_ITG3205_ON_I2C2  1
#elif (BOARD_TYPE == MADRE_BOARD)	
	#define USE_ITG3205_ON_I2C1  1
	#define USE_ITG3205_ON_I2C2  0	
#endif

void I2C_doneReadGyroData( boolean I2CtrxOK );

unsigned char itg3205reg_index[] = {0x15, 0x1B, 0x1D, 0x3E } ;	// Address of the first block of registers:SMPLRT_DIV[0], TEMP_OUT_H[1], GYRO_XOUT_H[2], PWR_MGM[3]
#define SMPLRT_DIV 		&itg3205reg_index[0]
#define TEMP_OUT_H		&itg3205reg_index[1]
#define GYRO_XOUT_H		&itg3205reg_index[2]
#define PWR_MGM			&itg3205reg_index[3]

// Values from 0x2C(BW_RATE) to 0x31(DATA_FORMAT)
unsigned char enableGyroRead[]	= { 0 , 0b00000011 , 0x01 } ;	// |Sample rate divider|Digital LP filter b7-b5:unused, b4-b3:Range selection, b2-b0: Digital LP filter config|Power managment (x axis clk reference)
unsigned char resetGyroscope[]	= { 0x00  } ;					// Value  for 0x3E (PWR_MGM) -> Reset 

int udb_gyroOffset[3] = { 0 , 0 , 0 } ;  		// acceleration offset in the body frame of reference
int gyroGain[3] = { RMAX , RMAX , RMAX } ; 		// magnetometer calibration gains
int rawGyroCalib[3] = { 0 , 0 , 0 } ;
unsigned char gyroreg[6] ;  					// magnetometer read-write buffer
int gyroMeasureRaw[3] ;

//int I2interrupts = 0 ;

int gyroindex ;  					// index into the read write buffer 
int gyroMessage = 0 ; 				// message type
//int I2messages = 0 ;

#if (USE_ITG3205_ON_I2C1 == 1)
	#define I2C_Normal		I2C1_Normal
	#define I2C_Read		I2C1_Read
	#define I2C_Write		I2C1_Write
	#define I2C_reset		I2C1_reset
#elif (USE_ITG3205_ON_I2C2 == 1)
	#define I2C_Normal		I2C2_Normal
	#define I2C_Read		I2C2_Read
	#define I2C_Write		I2C2_Write
	#define I2C_reset		I2C2_reset
#endif

void rxGyroscope(void)  		// service the gyroscope
{

#if ( LED_RED_GYRO_CHECK == 1 )
	if ( gyroMessage == 5 )
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
		gyroMessage = 0 ; 			// start over again
		I2C_reset() ; 				// reset the I2C
		return ;
	}

	gyroindex = 0 ;

	gyroMessage++ ;
	if ( gyroMessage > 5 )
	{
		gyroMessage = 5 ;
	}
	switch ( gyroMessage )
	{ 
		case  1:    				// read the gyroscope in case it is still sending data, so as to NACK it
			I2C_Read(ITG3205_ADDRESS, GYRO_XOUT_H, 1, gyroreg, 6, &I2C_doneReadGyroData); 
			break ;
		case  2:					// put magnetomter into the power up defaults on a reset
			I2C_Write(ITG3205_ADDRESS, PWR_MGM, 1, resetGyroscope, 1, NULL);
			break ;
		case  3:  					// clear out any data that is still there
			I2C_Read(ITG3205_ADDRESS, GYRO_XOUT_H, 1, gyroreg, 6, &I2C_doneReadGyroData);
			break ;
		case  4 :   				// enable normal continuous readings
			I2C_Write(ITG3205_ADDRESS, SMPLRT_DIV, 1, enableGyroRead, 2, &I2C_doneReadGyroData);
			break ;
		case  5 :  					// read the magnetometer data
			I2C_Read(ITG3205_ADDRESS, GYRO_XOUT_H, 1, gyroreg, 6, &I2C_doneReadGyroData);
			break ;
			
		default  :
			gyroMessage = 0 ;
			break ;
	}
	return ;
}



void I2C_doneReadGyroData( boolean I2CtrxOK )
{	
	int vectorIndex ;
	static int sample_count = 0;
	if( I2CtrxOK == true )
	{
		gyroMeasureRaw[0] = (gyroreg[0]<<8)+gyroreg[1] ; 
		gyroMeasureRaw[1] = (gyroreg[2]<<8)+gyroreg[3] ; 
		gyroMeasureRaw[2] = (gyroreg[4]<<8)+gyroreg[5] ;

		for ( vectorIndex = 0 ; vectorIndex < 6 ; vectorIndex++ ) gyroreg[vectorIndex] = 0;

		if ( gyroMessage == 4 ) I2C_Write(ITG3205_ADDRESS, PWR_MGM, 1, &enableGyroRead[2], 1, NULL);

		if ( gyroMessage == 5 )
		{
			udb_xrate.sum += gyroMeasureRaw[1];
			udb_yrate.sum += gyroMeasureRaw[0];
			udb_zrate.sum += gyroMeasureRaw[2];
			sample_count ++ ;
			
			if ( sample_count >= 48 )
			{
			
				udb_xrate.value = 	__builtin_divsd( udb_xrate.sum , sample_count ) ;
				udb_yrate.value = 	__builtin_divsd( udb_yrate.sum , sample_count ) ;
				udb_zrate.value = 	__builtin_divsd( udb_zrate.sum , sample_count ) ;
				udb_xrate.sum = 0;
				udb_yrate.sum = 0;
				udb_zrate.sum = 0;

				sample_count = 0;
				
			}	

			

		}

		
	}

	return ;
}


#endif
