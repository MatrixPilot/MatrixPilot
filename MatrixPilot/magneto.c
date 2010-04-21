#include "p30f4011.h"
#include "defines.h"
#include "definesRmat.h"
#include "magnetometerOptions.h"

const unsigned char enableMagRead[] =        { 0x3C , 0x00 , 0x10 , 0x20 , 0x00 } ;
const unsigned char enableMagCalibration[] = { 0x3C , 0x00 , 0x11 , 0x20 , 0x01 } ;

void I2C_readMagData(void) ;
void I2C_writeMagCommand(void) ;

void I2C_startReadMagData(void) ;
void I2C_stopReadMagData(void) ;
void I2C_doneReadMagData(void) ;
void I2C_recen(void) ;
void I2C_recstore(void) ;

void I2C_WriteMagData(void) ;
void I2C_stopWriteMagData(void) ;

void I2C_idle(void) ;

int magFieldBody[3] ;  // magnetic field in the body frame of reference 
int magOffset[3] = { 0 , 0 , 0 } ;  // magnetic offset in the body frame of reference
int magGain[3] = { RMAX , RMAX , RMAX } ; // magnetometer calibration gains
unsigned char magreg[6] ;  // magnetometer read-write buffer

void (* I2C_state ) ( void ) = &I2C_idle ;

#define I2CBRGVAL 35 // 100 Khz

#define DECLINATIONANGLE ((signed char)(MAGNETICDECLINATION*128/180))

fractional declinationVector[2] ;

void init_I2C(void)
{
	flags._.first_mag_reading = 1 ;
	declinationVector[0] = cosine(DECLINATIONANGLE) ;
	declinationVector[1] = sine(DECLINATIONANGLE) ;

	I2CBRG = I2CBRGVAL ; 
	I2CCONbits.I2CEN = 1 ; // enable I2C

	IPC3bits.MI2CIP = 3 ; // I2C at priority 3
	IFS0bits.MI2CIF = 0 ; // clear the I2C master interrupt
	IEC0bits.MI2CIE = 1 ; // enable the interrupt
	return ;
}

int mrindex ;  // index into the read write buffer 
int magMessage = 0 ; // message type

void rxMagnetometer(void)  // service the magnetometer
{
	int magregIndex ;
	mrindex = 0 ;
	magMessage++ ;
	if ( magMessage > 4 )
	{
		magMessage = 4 ;
	}
	switch ( magMessage )
	{ 
	case  1:  // enable the calibration process
		for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
		{
			magreg[magregIndex] = enableMagCalibration[magregIndex] ;
		}
		I2C_state = &I2C_writeMagCommand ;
		IFS0bits.MI2CIF = 1 ;
		break ;
	case  2 :  // read the calibration data
		I2C_state = &I2C_readMagData ;
		IFS0bits.MI2CIF = 1 ;
		break ;
	case 3 :   // enable normal continuous readings
		for ( magregIndex = 0 ; magregIndex < 5 ; magregIndex++ )
		{
			magreg[magregIndex] = enableMagRead[magregIndex] ;
		}
		I2C_state = &I2C_writeMagCommand ;
		IFS0bits.MI2CIF = 1 ; 
		break ;
	case 4 :  // read the magnetometer data
		I2C_state = &I2C_readMagData ;
		IFS0bits.MI2CIF = 1 ;
		break ;
	default  :
		I2C_state = &I2C_idle ;
		break ;
	}
	return ;
}

void __attribute__((__interrupt__,__no_auto_psv__)) _MI2CInterrupt(void)
{
	IFS0bits.MI2CIF = 0 ; // clear the interrupt
	if ( (I2CSTAT & 0b0000010011000000) == 0 )  // everything normal ?
	{
		(* I2C_state) () ;
	}
	else
	{
		I2CSTAT = I2CSTAT & 0b1111101100111111 ; // clear the error 
		I2CCONbits.PEN = 1 ;  // stop the bus
		I2C_state = &I2C_idle ; 
	}
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
	I2C_state = &I2C_startReadMagData ;
	I2CCONbits.SEN = 1 ;
	return ;
}
void I2C_startReadMagData(void)
{
	I2C_state = &I2C_recen ;
	I2CTRN = 0x3D ;
	return ;
}

void I2C_recen(void)
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
		I2C_state = &I2C_recen ;
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

void I2C_doneReadMagData(void)
{
	int vectorIndex ;
	int magFieldRaw[3] ;
	magFieldRaw[0] = (magreg[0]<<8)+magreg[1] ; 
	magFieldRaw[1] = (magreg[2]<<8)+magreg[3] ; 
	magFieldRaw[2] = (magreg[4]<<8)+magreg[5] ;

	if ( magMessage == 4 )
	{
		magFieldBody[0] = MAG_X_SIGN((__builtin_mulsu((magFieldRaw[MAG_X_AXIS]), magGain[MAG_X_AXIS] ))>>14)-(magOffset[0]>>1) ;
		magFieldBody[1] = MAG_Y_SIGN((__builtin_mulsu((magFieldRaw[MAG_Y_AXIS]), magGain[MAG_Y_AXIS] ))>>14)-(magOffset[1]>>1) ;
		magFieldBody[2] = MAG_Z_SIGN((__builtin_mulsu((magFieldRaw[MAG_Z_AXIS]), magGain[MAG_Z_AXIS] ))>>14)-(magOffset[2]>>1) ;
		I2C_state = &I2C_idle ;
		flags._.mag_drift_req = 1 ;
	}
	else if ( magMessage == 2 )
	{
		for ( vectorIndex = 0 ; vectorIndex < 3 ; vectorIndex++ )
		{
			if ( magFieldRaw[vectorIndex] > 0 )
			{
				magGain[vectorIndex] = __builtin_divud( ((long) ( 700.0*RMAX)), magFieldRaw[vectorIndex] ) ;
			}
			else
			{
				magGain[vectorIndex] = RMAX ;
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

