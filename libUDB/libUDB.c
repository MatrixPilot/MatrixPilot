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

#if (BOARD_IS_CLASSIC_UDB)
#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
_FOSC( CSW_FSCM_OFF & HS ) ;		// external high speed crystal
#elif ( CLOCK_CONFIG == FRC8X_CLOCK ) 
_FOSC(CSW_FSCM_OFF & FRC_PLL8);
#endif
_FWDT( WDT_OFF ) ;					// no watchdog timer


// Add compatibility for c30 V3.3
#ifdef BORV20
#define BORV_20 BORV20
#define _FICD(x) _ICD(x)
#endif


_FBORPOR( 	PBOR_ON &				// brown out detection on
			BORV_20 &				// brown out set to 2.0 V
			MCLR_EN &				// enable MCLR
			RST_PWMPIN &			// pwm pins as pwm
			PWMxH_ACT_HI &			// PWMH is active high
			PWMxL_ACT_HI ) ;		// PMWL is active high
_FGS( CODE_PROT_OFF ) ;				// no protection
_FICD( 0xC003 ) ;					// normal use of debugging port

#elif (BOARD_TYPE == UDB4_BOARD)
_FOSCSEL(FNOSC_PRIPLL) ;            // medium speed XTAL plus PLL
_FOSC(	FCKSM_CSECMD &
		OSCIOFNC_ON &
		POSCMD_NONE ) ;
_FWDT(	FWDTEN_OFF &
		WINDIS_OFF ) ;
_FGS(	GSS_OFF &
		GCP_OFF &
		GWRP_OFF ) ;
_FPOR(	FPWRT_PWR1 ) ;
_FICD(	JTAGEN_OFF &
		ICS_PGD2 ) ;

#elif (BOARD_TYPE == MADRE_BOARD)
_FOSCSEL(	FNOSC_PRIPLL & 			// Primary oscillator (XT, HS, EC) with PLL
			IESO_OFF);				// No Clock switching during startup
_FOSC(	FCKSM_CSDCMD & 				// Clock Switching and Fail Safe Clock Monitor disabled
		OSCIOFNC_OFF  & 			// Function of pin OSC2: OSC2 is used by the oscilator
		POSCMD_HS);					// Primary Oscillator Mode: High speed oscillator HS
_FWDT(	FWDTEN_OFF &				// Watchdog Timer disabled
		WINDIS_OFF);				// Watchdog Timer window mode	
_FGS(	GSS_OFF &					// Code security config
		GCP_OFF &
		GWRP_OFF ) ;
_FPOR(	ALTI2C_OFF);				// Use the BUS I2C on pins SDA1/SCL1
_FICD(	JTAGEN_OFF &
		ICS_PGD3 );					// JTAG OFF, use PGD3 pins

#endif


union udb_fbts_byte udb_flags ;

int defaultCorcon = 0 ;

#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_current ;
union longww battery_mAh_used ;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_voltage;	// battery_voltage._.W1 is in tenths of Volts
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
unsigned char rc_signal_strength ;
#define MIN_RSSI	((long)((RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 65536))
#define RSSI_RANGE	((long)((RSSI_MAX_SIGNAL_VOLTAGE-RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 100))
#endif


// Functions only included with nv memory.
#if(USE_NV_MEMORY == 1)
UDB_SKIP_FLAGS udb_skip_flags = {0,0,0};

void udb_skip_radio_trim()
{
	udb_skip_flags.skip_radio_trim = 1;
}

void udb_skip_imu_calibration()
{
	udb_skip_flags.skip_imu_cal = 1;
}

#endif


//#if(USE_NV_MEMORY == 1)
//if(udb_skip_flags.skip_radio_trim == 1)
//if(udb_skip_flags.skip_imu_cal == 1)
//#endif
//

void udb_init(void)
{
	defaultCorcon = CORCON ;
	
#if (BOARD_TYPE == UDB4_BOARD)
	PLLFBDbits.PLLDIV = 30 ; // FOSC = 32 MHz (XT = 8.00MHz, N1=2, N2=4, M = 32)
#elif (BOARD_TYPE == MADRE_BOARD)
	_PLLPRE = 5 -2;					// Prescaler of the PLL, (the value to set in the registry is the real value -2)
	_PLLDIV = 32 -2;				// Divider of the PLL, (the value to set in the registry is the real value -2)
	_PLLPOST = 0;					// Postscaler of the PLL, 0 = /2
	while(!OSCCONbits.LOCK);		// Wait the PLL to lock
#endif
	
	udb_flags.B = 0 ;
	
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	battery_current.WW = 0 ;
	battery_mAh_used.WW = 0 ;
#endif
	
#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	battery_voltage.WW = 0 ;
#endif
	
#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	rc_signal_strength = 0 ;
#endif
	
	udb_init_leds() ;
	udb_init_ADC() ;
	udb_init_clock() ;
#if (BOARD_SUPPORT_RADIO_IN == 1)
	udb_init_capture() ;
#endif

#if (MAG_YAW_DRIFT == 1)
	I2C1_init();
#endif

	udb_init_GPS() ;
	udb_init_USART() ;
	udb_init_pwm() ;
	
#if (USE_OSD == 1)
	udb_init_osd() ;
#endif
	
	SRbits.IPL = 0 ;	// turn on all interrupt priorities
	
	return ;
}


void udb_run(void)
{
	//  nothing else to do... entirely interrupt driven
	while (1)
	{
		// pause cpu counting timer while not in an ISR
		indicate_loading_main ;
	}
	// Never returns
}


void udb_init_leds( void )
{
	
#if (BOARD_IS_CLASSIC_UDB == 1)
	TRISFbits.TRISF0 = 0 ;
	
#elif (BOARD_TYPE == UDB4_BOARD)
	_TRISE1 = _TRISE2 = _TRISE3 = _TRISE4 = 0 ;
	_LATE1 = _LATE2 = _LATE3 = _LATE4 = LED_OFF ;

#elif (BOARD_TYPE == MADRE_BOARD)
	_TRISA7 = _TRISA10 = 0 ;
	_LATA7 = _LATA10 = LED_OFF ;
#endif
	
	return ;
}

#ifdef INITIALIZE_VERTICAL // for VTOL, vertical initialization
void udb_a2d_record_offsets(void)
{
#if(USE_NV_MEMORY == 1)
	if(udb_skip_flags.skip_imu_cal == 1)
		return;
#endif

	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value ;
	udb_xrate.offset = udb_xrate.value ;
	UDB_YACCEL.offset = UDB_YACCEL.value - ( Y_GRAVITY_SIGN ((int)(2*GRAVITY)) ); // opposite direction
	udb_yrate.offset = udb_yrate.value ;
	UDB_ZACCEL.offset = UDB_ZACCEL.value ; 
	udb_zrate.offset = udb_zrate.value ;
#ifdef VREF
	udb_vref.offset = udb_vref.value ;
#endif
	return ;
}
#else  // horizontal initialization
void udb_a2d_record_offsets(void)
{
#if(USE_NV_MEMORY == 1)
	if(udb_skip_flags.skip_imu_cal == 1)
		return;
#endif

	// almost ready to turn the control on, save the input offsets
	UDB_XACCEL.offset = UDB_XACCEL.value ;
//	UDB_XACCEL.offset = 0 ;
	udb_xrate.offset = udb_xrate.value ;
	UDB_YACCEL.offset = UDB_YACCEL.value ;
//	UDB_YACCEL.offset = 0 ;
	udb_yrate.offset = udb_yrate.value ;
//	UDB_ZACCEL.offset = UDB_ZACCEL.value + ( Z_GRAVITY_SIGN ((int)(2*GRAVITY))) ; // same direction
	UDB_ZACCEL.offset = UDB_ZACCEL.value + ( Z_GRAVITY_SIGN ((int)(GRAVITY))) ;
	udb_zrate.offset = udb_zrate.value ;									

/*	UDB_XACCEL.offset = 32 ;
	udb_xrate.offset = 16 ;
	UDB_YACCEL.offset = 89 ;
	udb_yrate.offset = 52 ;
	UDB_ZACCEL.offset = -1218 ;
	udb_zrate.offset = 38 ;	*/							
#ifdef VREF
	udb_vref.offset = udb_vref.value ;
#endif
	return ;
}
#endif


void udb_servo_record_trims(void)
{
	int i;
	for (i=0; i <= NUM_INPUTS; i++)
		udb_pwTrim[i] = udb_pwIn[i] ;
	
	return ;
}


// saturation logic to maintain pulse width within bounds
int udb_servo_pulsesat ( long pw )
{
	if ( pw > SERVOMAX ) pw = SERVOMAX ;
	if ( pw < SERVOMIN ) pw = SERVOMIN ;
	return (int)pw ;
}


void calculate_analog_sensor_values( void )
{
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to current in tenths of Amps
	battery_current.WW = (udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value + (long)32768) * (MAX_CURRENT) + (((long)(CURRENT_SENSOR_OFFSET)) << 16) ;
	
	// mAh = mA / 144000 (increment per 40Hz tick is /40*60*60)
	// 90000/144000 == 900/1440
	battery_mAh_used.WW += (battery_current.WW / 1440) ;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to voltage in tenths of Volts
	battery_voltage.WW = (udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value + (long)32768) * (MAX_VOLTAGE) + (((long)(VOLTAGE_SENSOR_OFFSET)) << 16) ;
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	union longww rssi_accum ;
	rssi_accum.WW = (((udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value + 32768) - (MIN_RSSI)) * (10000 / (RSSI_RANGE))) ;
	if (rssi_accum._.W1 < 0)
		rc_signal_strength = 0 ;
	else if (rssi_accum._.W1 > 100)
		rc_signal_strength = 100 ;
	else
		rc_signal_strength = (unsigned char)rssi_accum._.W1 ;
#endif
}
