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
#include "magnetometer.h"
#include "magnetometerOptions.h"
#include "libUDB.h" 
#include <stdio.h>
#include <stdlib.h>

#if (MAG_YAW_DRIFT == 1 && HILSIM != 1)
	#define HMC5883_COMMAND 0x3C
		
	void I2C_doneReadMagData( boolean I2CtrxOK );
	
	uint8_t hmc5883read_index[] = {0x03} ;	// Address of the first register to read
	uint8_t hmc5883write_index[] = {0x00} ;	// Address of the first register to read
	
	uint8_t enableMagRead[] =        { 0x10 , 0x20 , 0x00 } ;	// Continous measurament
	uint8_t enableMagCalibration[] = { 0x11 , 0x20 , 0x01 } ;	// Positive bias (Self Test) and single measurament
	uint8_t resetMagnetometer[]    = { 0x10 , 0x20 , 0x02 } ;	// Idle mode (Reset??)
	
	int16_t udb_magFieldBody[3] ;  					// magnetic field in the body frame of reference 
	int16_t udb_magOffset[3] = { 0 , 0 , 0 } ;  	// magnetic offset in the body frame of reference
	// To use static offsets, change udb_magOffset to udb_staticMagOffset on lines 334, 335, 336 below
	int16_t udb_staticMagOffset[3] = { 0 , 0 , 0 }; // Enter the static magnetic offset in the body frame of reference here.
	int16_t magGain[3] = { RMAX , RMAX , RMAX } ; 	// magnetometer calibration gains
	int16_t rawMagCalib[3] = { 0 , 0 , 0 } ;
	uint8_t magreg[6] ;  					// magnetometer read-write buffer
	int16_t magFieldRaw[3] ;
	int16_t I2interrupts = 0 ;
	
	int16_t mrindex ;  					// index into the read write buffer 
	int16_t magMessage = 0 ; 			// message type
	int16_t magCalibPause = 0 ;
		
	#define I2C_Normal		I2C2_Normal
	#define I2C_Read		I2C2_Read
	#define I2C_Write		I2C2_Write
	#define I2C_reset		I2C2_reset
	
	void rxMagnetometer(void)  		// service the magnetometer
	{
		#if ( LED_RED_MAG_CHECK == 1 )
			if ( magMessage == 7 )
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
			magMessage = 0 ; 			// start over again
			I2C_reset() ; 				// reset the I2C
			return ;
		}
	
		mrindex = 0 ;
	
		if ( magCalibPause == 0 )
		{
			magMessage++ ;
			if ( magMessage > 7 )
			{
				magMessage = 7 ;
			}
			switch ( magMessage )
			{ 
			case  1:    				// read the magnetometer in case it is still sending data, so as to NACK it
				I2C_Read(HMC5883_COMMAND, hmc5883read_index, 1, magreg, 6, &I2C_doneReadMagData); 
				break ;
			case  2:					// put magnetomter into the power up defaults on a reset
				I2C_Write(HMC5883_COMMAND, hmc5883write_index, 1, resetMagnetometer, 3, NULL);
				break ;
				case  3:  					// clear out any data that is still there
				I2C_Read(HMC5883_COMMAND, hmc5883read_index, 1, magreg, 6, &I2C_doneReadMagData);
				break ;
			case  4:  					// enable the calibration process
				magCalibPause = 2 ;
				I2C_Write(HMC5883_COMMAND, hmc5883write_index, 1, enableMagCalibration, 3, NULL);
				break ;
			case  5 : 					// read the calibration data
				I2C_Read(HMC5883_COMMAND, hmc5883read_index, 1, magreg, 6, &I2C_doneReadMagData);
				break ;
			case  6 :   				// enable normal continuous readings
				I2C_Write(HMC5883_COMMAND, hmc5883write_index, 1, enableMagRead, 3, NULL);
				break ;
			case 7 :  					// read the magnetometer data
				I2C_Read(HMC5883_COMMAND, hmc5883read_index, 1, magreg, 6, &I2C_doneReadMagData);
				break ;
			default  :
				magMessage = 0 ;
				break ;
			}
		}
		else
		{
			magCalibPause -- ;
		}
		return ;
	}
		
	int previousMagFieldRaw[3] = { 0 , 0 , 0 } ;
		
	void I2C_doneReadMagData( boolean I2CtrxOK )
	{	
		int vectorIndex ;
	
		if( I2CtrxOK == true )
		{
			magFieldRaw[0] = (magreg[0]<<8)+magreg[1] ; 
			magFieldRaw[1] = (magreg[2]<<8)+magreg[3] ; 
			magFieldRaw[2] = (magreg[4]<<8)+magreg[5] ;
			
			for ( vectorIndex = 0 ; vectorIndex < 6 ; vectorIndex++ ) magreg[vectorIndex] = 0;

			previousMagFieldRaw[0] = magFieldRaw[0] ;
			previousMagFieldRaw[1] = magFieldRaw[1] ;
			previousMagFieldRaw[2] = magFieldRaw[2] ;
		
			if ( magMessage == 7 )
			{
				// To use static offsets, Change udb_magOffset to udb_staticMagOffset
				udb_magFieldBody[0] = MAG_X_SIGN((__builtin_mulsu((magFieldRaw[MAG_X_AXIS]), magGain[MAG_X_AXIS] ))>>14)-(udb_magOffset[0]>>1) ;
				udb_magFieldBody[1] = MAG_Y_SIGN((__builtin_mulsu((magFieldRaw[MAG_Y_AXIS]), magGain[MAG_Y_AXIS] ))>>14)-(udb_magOffset[1]>>1) ;
				udb_magFieldBody[2] = MAG_Z_SIGN((__builtin_mulsu((magFieldRaw[MAG_Z_AXIS]), magGain[MAG_Z_AXIS] ))>>14)-(udb_magOffset[2]>>1) ;
		
				if ( ( abs(udb_magFieldBody[0]) < MAGNETICMAXIMUM ) &&
					 ( abs(udb_magFieldBody[1]) < MAGNETICMAXIMUM ) &&
					 ( abs(udb_magFieldBody[2]) < MAGNETICMAXIMUM ) )
				{
					magnetometer_callback_newdata();
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
					rawMagCalib[vectorIndex] = magFieldRaw[vectorIndex] ;
					if (  ( magFieldRaw[vectorIndex] > MAGNETICMINIMUM ) && ( magFieldRaw[vectorIndex] < MAGNETICMAXIMUM ) )
					{
						magGain[vectorIndex] = __builtin_divud( ((long) ( MAG_GAIN*RMAX)), magFieldRaw[vectorIndex] ) ;
					}
					else
					{
						magGain[vectorIndex] = RMAX ;
						magMessage = 0 ;  // invalid calibration, reset the magnetometer
		
					}
				}
			}
		}

		return ;
	}
#endif // MAG_YAW_DRIFT
