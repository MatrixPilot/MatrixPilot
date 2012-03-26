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

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == MADRE_BOARD)

#if ( MAG_YAW_DRIFT == 1)

const unsigned char enableMagRead[] =        { 0x3C , 0x00 , 0x10 , 0x20 , 0x00 } ;	// Continous measurament
const unsigned char enableMagCalibration[] = { 0x3C , 0x00 , 0x11 , 0x20 , 0x01 } ; // Positive bias (Self Test) and single measurament
const unsigned char resetMagnetometer[]    = { 0x3C , 0x00 , 0x10 , 0x20 , 0x02 } ;	// Idle mode (Reset??)

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

int udb_magFieldBody[3] ;  					// magnetic field in the body frame of reference 
int udb_magOffset[3] = { 0 , 0 , 0 } ;  	// magnetic offset in the body frame of reference
int magGain[3] = { RMAX , RMAX , RMAX } ; 	// magnetometer calibration gains
int rawMagCalib[3] = { 0 , 0 , 0 } ;
unsigned char magreg[6] ;  					// magnetometer read-write buffer
int magFieldRaw[3] ;

int I2ERROR = 0 ;
int I2interrupts = 0 ;

void (* I2C_state ) ( void ) = &I2C_idle ;
extern void send_debug_line( void );

#define I2CFSCL 400000	// Bus speed measured in Hz
#define I2CBRGVAL ((FREQOSC/(CLK_PHASES *I2CFSCL))-(FREQOSC/(CLK_PHASES * 10000000)))-1

#if			(BOARD_TYPE == UDB4_BOARD)				// USE I2C2
#define		I2CCON			I2C2CON
#define		I2CSTAT			I2C2STAT
#define		_I2CEN			I2C2CONbits.I2CEN
#define		_TRISSCL		_TRISA2
#define		_TRISSDA		_TRISA3
#define		_RSCL			_RA2
#define		_RSDA			_RA3
#define		I2CBRG			I2C2BRG
#define		_MI2CIP			_MI2C2IP
#define		_MI2CIF			_MI2C2IF
#define		_MI2CIE			_MI2C2IE	
#define		_MI2CInterrupt	_MI2C2Interrupt
#define		_I2CSEN			I2C2CONbits.SEN
#define		I2CTRN			I2C2TRN
#define		_I2CPEN			I2C2CONbits.PEN
#define		_I2CRCEN		I2C2CONbits.RCEN
#define		_I2CRSEN		I2C2CONbits.RSEN
#define		_I2CACKSTAT		I2C2STATbits.ACKSTAT
#define		_I2CACKDT		I2C2CONbits.ACKDT
#define		_I2CACKEN		I2C2CONbits.ACKEN

#elif		(BOARD_TYPE == MADRE_BOARD)				// USE I2C1
#define		I2CCON			I2C1CON
#define		I2CSTAT			I2C1STAT
#define		_I2CEN			I2C1CONbits.I2CEN
#define		_TRISSCL		_TRISB8
#define		_TRISSDA		_TRISB9
#define		_RSCL			_RB8
#define		_RSDA			_RB9
#define		I2CBRG			I2C1BRG
#define		_MI2CIP			_MI2C1IP
#define		_MI2CIF			_MI2C1IF
#define		_MI2CIE			_MI2C1IE	
#define		_MI2CInterrupt	_MI2C1Interrupt
#define		_I2CSEN			I2C1CONbits.SEN
#define		I2CTRN			I2C1TRN
#define		_I2CPEN			I2C1CONbits.PEN
#define		_I2CRCEN		I2C1CONbits.RCEN
#define		_I2CRSEN		I2C1CONbits.RSEN
#define		_I2CACKSTAT		I2C1STATbits.ACKSTAT
#define		_I2CACKDT		I2C1CONbits.ACKDT
#define		_I2CACKEN		I2C1CONbits.ACKEN
#endif

#define I2C_NORMAL ((( I2CCON & 0b0000000000011111 ) == 0) && ( (I2CSTAT & 0b0100010011000001) == 0 ))

void udb_init_I2C(void)
{
	_TRISSCL = _TRISSDA = 0 ;
	I2CBRG = I2CBRGVAL ; 
	_I2CEN = 1 ; 				// enable I2C

	_MI2CIP = 5 ; 				// I2C at priority 5
	_MI2CIF = 0 ; 				// clear the I2C master interrupt
	_MI2CIE = 1 ; 				// enable the interrupt
	return ;
}

int mrindex ;  					// index into the read write buffer 
int magMessage = 0 ; 			// message type

int magCalibPause = 0 ;

int I2messages = 0 ;

void rxMagnetometer(void)  		// service the magnetometer
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

/*	if ( _I2CEN == 0 ) 				// if I2C is off
	{
		I2C_state = &I2C_idle ; 	// disable response to any interrupts
		_RSCL = _RSDA = 1 ; 		// pull SDA and SCL high
		udb_init_I2C() ; 			// turn the I2C back on
		magMessage = 0 ; 			// start over again
		return ;
	}
*/

	if (  I2C_NORMAL )
	{
	}
	else
	{
		I2C_state = &I2C_idle ;		// disable the response to any more interrupts
		magMessage = 0 ; 			// start over again
		I2ERROR = I2CSTAT ; 		// record the error for diagnostics
		
		_I2CEN = 0 ;  				// turn off the I2C
		_MI2CIF = 0 ; 				// clear the I2C master interrupt
		_MI2CIE = 0 ; 				// disable the interrupt
		_RSCL = _RSDA = 0 ; 		// pull SDA and SCL low <------------ why?
		_RSCL = _RSDA = 1 ; 		// pull SDA and SCL high
		udb_init_I2C() ; 			// turn the I2C back on

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
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		case  2:					// put magnetomter into the power up defaults on a reset
			for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
			{
				magreg[magregIndex] = resetMagnetometer[magregIndex] ;
			}
			I2C_state = &I2C_writeMagCommand ;
			_MI2CIF = 1 ;
			break ;
		case  3:  					// clear out any data that is still there
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		case  4:  					// enable the calibration process
			for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
			{
				magreg[magregIndex] = enableMagCalibration[magregIndex] ;
			}
			I2C_state = &I2C_writeMagCommand ;
			magCalibPause = 2 ;
			_MI2CIF = 1 ;
			break ;
		case  5 : 					// read the calibration data
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		case  6 :   				// enable normal continuous readings
			for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
			{
				magreg[magregIndex] = enableMagRead[magregIndex] ;
			}
			I2C_state = &I2C_writeMagCommand ;
			_MI2CIF = 1 ;
			break ;
		case 7 :  					// read the magnetometer data
			I2C_state = &I2C_readMagData ;
			_MI2CIF = 1 ;
			break ;
		default  :
//			I2C_state = &I2C_idle ;
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
	_MI2CIF = 0 ; 					// clear the interrupt
	(* I2C_state) () ; 				// execute the service routine
	interrupt_restore_corcon ;
	return ;
}


void I2C_writeMagCommand(void)
{
	I2C_state = &I2C_WriteMagData ;
	_I2CSEN = 1 ;
	return ;
}

void I2C_WriteMagData(void)
{
	I2CTRN = magreg[mrindex++] ;
	if ( mrindex > 4)		// Hazard! I2C_state MUST changhe before the I2C module complete sending byte
	{
		I2C_state = &I2C_stopWriteMagData ;
	}
	return ;
}

void I2C_stopWriteMagData(void)
{
	I2C_state = &I2C_idle ;
	_I2CPEN = 1 ;
	return ;
}


void I2C_readMagData(void)
{
	if ( _I2CRCEN == 0 )
	{
		I2C_state = &I2C_send3C ;
		_I2CSEN = 1 ;
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
	_I2CRSEN = 1;  // repeated start
}

void I2C_startReadMagData(void)
{
	I2C_state = &I2C_recen ;
	I2CTRN = 0x3D ;
	return ;
}

void I2C_recen(void)
{
	if ( _I2CACKSTAT == 1 )	// magnetometer not responding
	{
		magMessage = 0 ; 	// start over
		_I2CPEN = 1; 		// stop the bus
		I2C_state = &I2C_idle ; 
	}
	else
	{
		_I2CRCEN = 1 ;
		I2C_state = &I2C_recstore ;
	}

	return ;
}

void I2C_rerecen(void)
{
	_I2CRCEN = 1 ;
	I2C_state = &I2C_recstore ;
	return ;
}

void I2C_recstore(void)
{
	magreg[mrindex++] = I2CRCV ;
	if ( mrindex > 6 )
	{
		_I2CACKDT = 1 ;		// Send a NACK, Slave stop to send me data please!
		I2C_state = &I2C_stopReadMagData ;
	}
	else
	{
		_I2CACKDT = 0 ;		// Send an ACK to the slave, need more data
		I2C_state = &I2C_rerecen ;	// Have to set RCEN =1 again to read another byte
	}
	_I2CACKEN = 1 ;			// Write the ACKDT on the bus

	return ;
}


void I2C_stopReadMagData(void)
{
	_I2CPEN = 1;
	I2C_state = &I2C_doneReadMagData ;
	return ;
}

int previousMagFieldRaw[3] = { 0 , 0 , 0 } ;


void I2C_doneReadMagData(void)
{
	int vectorIndex ;
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
//		I2C_state = &I2C_idle ;
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
//		I2C_state = &I2C_idle ;
	}
	return ;
}



void I2C_idle(void)
{
	return ;
}

#endif

#endif
