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

#if (BOARD_IS_CLASSIC_UDB == 1)

#if ( MAG_YAW_DRIFT == 1 )

const unsigned char enableMagRead[] =        { 0x3C , 0x00 , 0x10 , 0x20 , 0x00 } ;
const unsigned char enableMagCalibration[] = { 0x3C , 0x00 , 0x11 , 0x20 , 0x01 } ;
const unsigned char resetMagnetometer[]    = { 0x3C , 0x00 , 0x10 , 0x20 , 0x02 } ;

void I2C_readMagData(void) ;
void I2C_writeMagCommand(void) ;

void I2C_startReadMagData(void) ;
void I2C_stopReadMagData(void) ;
void I2C_doneReadMagData(void) ;
void I2C_recen(void) ;
void I2C_recstore(void) ;
void I2C_send3C(void) ;
void I2C_send03(void) ;
void I2C_restart(void) ;

void I2C_WriteMagData(void) ;
void I2C_stopWriteMagData(void) ;

void I2C_idle(void) ;

int udb_magFieldBody[3] ;  // magnetic field in the body frame of reference 
int udb_magOffset[3] = { 0 , 0 , 0 } ;  // magnetic offset in the body frame of reference
int magGain[3] = { RMAX , RMAX , RMAX } ; // magnetometer calibration gains
int rawMagCalib[3] = { 0 , 0 , 0 } ;
unsigned char magreg[6] ;  // magnetometer read-write buffer

int I2ERROR = 0 ;
int I2interrupts = 0 ;

void (* I2C_state ) ( void ) = &I2C_idle ;

#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#define I2CBRGVAL 35 // 100 Khz
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#define I2CBRGVAL 129 // 100 Khz
#endif


#define I2C_NORMAL ((( I2CCON & 0b0000000000011111 ) == 0) && ( (I2CSTAT & 0b0100010011000001) == 0 ))

void udb_init_I2C(void)
{
	_TRISF2 = _TRISF3 = 0 ;
	I2CBRG = I2CBRGVAL ; 
	_I2CEN = 1 ; // enable I2C

	_MI2CIP = 5 ; // I2C at priority 5
	_MI2CIF = 0 ; // clear the I2C master interrupt
	_MI2CIE = 1 ; // enable the interrupt
	
	return ;
}

int mrindex ;  // index into the read write buffer 
int magMessage = 0 ; // message type

int magCalibPause = 0 ;

int I2messages = 0 ;

void rxMagnetometer(void)  // service the magnetometer
{
	int magregIndex ;
	
	I2messages++ ;
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
	if ( _I2CEN == 0 ) // I2C is off
	{
		I2C_state = &I2C_idle ; // disable response to any interrupts
		_RF2 = _RF3 = 1 ; // pull SDA and SCL high
		udb_init_I2C() ; // turn the I2C back on
		magMessage = 0 ; // start over again
		return ;
	}
	if (  I2C_NORMAL )
	{
	}
	else
	{
		I2C_state = &I2C_idle ;	// disable the response to any more interrupts
		magMessage = 0 ; // start over again
		I2ERROR = I2CSTAT ; // record the error for diagnostics
		_I2CEN = 0 ;  // turn off the I2C
		_MI2CIF = 0 ; // clear the I2C master interrupt
		_MI2CIE = 0 ; // disable the interrupt
		_RF2 = _RF3 = 0 ; // pull SDA and SCL low
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
		case  1:    // read the magnetometer in case it is still sending data, so as to NACK it
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		case  2:	// put magnetomter into the power up defaults on a reset
			for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
			{
				magreg[magregIndex] = resetMagnetometer[magregIndex] ;
			}
			I2C_state = &I2C_writeMagCommand ;
			_MI2CIF = 1 ;
			break ;
		case  3:  // clear out any data that is still there
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		case  4:  // enable the calibration process
			for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
			{
				magreg[magregIndex] = enableMagCalibration[magregIndex] ;
			}
			I2C_state = &I2C_writeMagCommand ;
			magCalibPause = 2 ;
			_MI2CIF = 1 ;
			break ;
		case  5 :  // read the calibration data
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		case  6 :   // enable normal continuous readings
			for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
			{
				magreg[magregIndex] = enableMagRead[magregIndex] ;
			}
			I2C_state = &I2C_writeMagCommand ;
			_MI2CIF = 1 ; 
			break ;
		case 7 :  // read the magnetometer data
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		default  :
			I2C_state = &I2C_idle ;
			break ;
		}
	}
	else
	{
		magCalibPause -- ;
	}
	return ;
}


void __attribute__((__interrupt__,__no_auto_psv__)) _MI2CInterrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;
	
	IFS0bits.MI2CIF = 0 ; // clear the interrupt
	(* I2C_state) () ; // execute the service routine
	
	interrupt_restore_corcon ;
	return ;
}


void I2C_writeMagCommand(void)
{
	I2C_state = &I2C_WriteMagData ;
	I2CCONbits.SEN = 1 ;
	return ;
}


void I2C_WriteMagData(void)
{
	I2CTRN = magreg[mrindex++] ;
	if ( mrindex > 4)
	{
		I2C_state = &I2C_stopWriteMagData ;
	}
	return ;
}


void I2C_stopWriteMagData(void)
{
	I2C_state = &I2C_idle ;
	I2CCONbits.PEN = 1 ;
	return ;
}


void I2C_readMagData(void)
{
	if ( I2CCONbits.RCEN == 0 )
	{
		I2C_state = &I2C_send3C ;
		I2CCONbits.SEN = 1 ;
	}
	return ;
}

void I2C_send3C(void)
{
	I2C_state = &I2C_send03 ;
	I2CTRN = 0x3C ;
}

void I2C_send03(void)
{
	I2C_state = &I2C_restart ;
	I2CTRN = 0x03 ;
}

void I2C_restart(void)
{
	I2C_state = &I2C_startReadMagData ;
	I2CCONbits.RSEN = 1;  // repeated start
}


void I2C_startReadMagData(void)
{
	I2C_state = &I2C_recen ;
	I2CTRN = 0x3D ;
	return ;
}


void I2C_recen(void)
{
	if ( I2CSTATbits.ACKSTAT == 1 )  // magnetometer not responding
	{
		magMessage = 0 ; // start over
		I2CCONbits.PEN = 1; // stop the bus
		I2C_state = &I2C_idle ; 
	}
	else
	{
		I2CCONbits.RCEN = 1 ;
		I2C_state = &I2C_recstore ;
	}
	return ;
}


void I2C_rerecen(void)
{
	I2CCONbits.RCEN = 1 ;
	I2C_state = &I2C_recstore ;
	return ;
}


void I2C_recstore(void)
{
	magreg[mrindex++] = I2CRCV ;
	if ( mrindex > 6 )
	{
		I2CCONbits.ACKDT = 1 ;
		I2C_state = &I2C_stopReadMagData ;
	}
	else
	{
		I2CCONbits.ACKDT = 0 ;
		I2C_state = &I2C_rerecen ;
	}
	I2CCONbits.ACKEN = 1 ;
	return ;
}


void I2C_stopReadMagData(void)
{
	I2CCONbits.PEN = 1;
	I2C_state = &I2C_doneReadMagData ;
	return ;
}


int previousMagFieldRaw[3] = { 0 , 0 , 0 } ;


void I2C_doneReadMagData(void)
{
	int vectorIndex ;
	int magFieldRaw[3] ;
	magFieldRaw[0] = (magreg[0]<<8)+magreg[1] ; 
	magFieldRaw[1] = (magreg[2]<<8)+magreg[3] ; 
	magFieldRaw[2] = (magreg[4]<<8)+magreg[5] ;

	previousMagFieldRaw[0] = magFieldRaw[0] ;
	previousMagFieldRaw[1] = magFieldRaw[1] ;
	previousMagFieldRaw[2] = magFieldRaw[2] ;

	if ( magMessage == 7 )
	{
		udb_magFieldBody[0] = MAG_X_SIGN((__builtin_mulsu((magFieldRaw[MAG_X_AXIS]), magGain[MAG_X_AXIS] ))>>14)-(udb_magOffset[0]>>1) ;
		udb_magFieldBody[1] = MAG_Y_SIGN((__builtin_mulsu((magFieldRaw[MAG_Y_AXIS]), magGain[MAG_Y_AXIS] ))>>14)-(udb_magOffset[1]>>1) ;
		udb_magFieldBody[2] = MAG_Z_SIGN((__builtin_mulsu((magFieldRaw[MAG_Z_AXIS]), magGain[MAG_Z_AXIS] ))>>14)-(udb_magOffset[2]>>1) ;
		I2C_state = &I2C_idle ;
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
	else if ( magMessage == 5 )
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
		I2C_state = &I2C_idle ;
	}
	return ;
}


void I2C_idle(void)
{
	return ;
}

#endif

#endif
