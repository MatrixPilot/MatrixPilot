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
#include "defines.h"
#include "options.h"
#include "debug.h"

#ifdef USE_DEBUG_U1
#include "uart1.h"
#endif
#ifdef USE_DEBUG_U2
#include "uart2.h"
#endif


#if (BOARD_IS_CLASSIC_UDB)
#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
_FOSC( CSW_FSCM_OFF & HS ) ;		// external high speed crystal
#elif ( CLOCK_CONFIG == FRC8X_CLOCK ) 
_FOSC(CSW_FSCM_OFF & FRC_PLL8);
#endif
_FWDT( WDT_OFF ) ;					// no watchdog timer


// Add compatibility for c30 V3.3
#ifndef BORV_20
#define BORV_20 BORV20
#endif
#ifndef _FICD
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

#elif ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)

#if ( CLOCK_CONFIG == FRC8X_CLOCK )
_FOSCSEL(FNOSC_FRCPLL); // fast RC plus PLL (Internal Fast RC (FRC) w/ PLL)
_FOSC(FCKSM_CSECMD &
      OSCIOFNC_ON &
      POSCMD_NONE); // Clock switching is enabled, Fail-Safe Clock Monitor is disabled,
// OSC2 pin has digital I/O function
// Primary Oscillator Disabled
#elif ( CLOCK_CONFIG == CRYSTAL_CLOCK )
_FOSCSEL(FNOSC_PRIPLL); // pri plus PLL (primary osc  w/ PLL)
_FOSC(FCKSM_CSDCMD &
      OSCIOFNC_OFF &
      POSCMD_XT); // Clock switching is enabled, Fail-Safe Clock Monitor is disabled,
// OSC2 pin has clock out function
// Primary Oscillator XT mode
#else
#error CLOCK_CONFIG must be one of [FRC8X_CLOCK, CRYSTAL_CLOCK]
#endif

_FWDT(FWDTEN_OFF &
      WINDIS_OFF); // Watchdog timer enabled/disabled by user software
// Watchdog Timer in Non-Window mode
_FGS(GSS_OFF &
     GCP_OFF &
     GWRP_OFF); // User program memory is not code-protected
// User program memory is not write-protected
_FPOR(FPWRT_PWR1); // POR Timer Value: Disabled
_FICD(JTAGEN_OFF &
      ICS_PGD2); // JTAG is Disabled
// Communicate on PGC2/EMUC2 and PGD2/EMUD2

#else // AIRFRAME_TYPE
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
#endif // AIRFRAME_TYPE

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
	
#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
//#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
#if (1)
    // reset values of PLLPRE, PLLPOST, PLLDIV are 0, 1, 0x30, yielding FOSC of about 45MHz
    //	CLKDIVbits.PLLPRE = 1 ;  // PLL prescaler: divide by 3, postscaler: div by 4(default), PLL divisor: x52, FRCdiv:1(default)
    //	PLLFBDbits.PLLDIV = 50 ; // FOSC = 32 MHz (FRC = 7.37MHz, N1=3, N2=4, M = 52)

#if ( CLOCK_CONFIG == FRC8X_CLOCK )
    CLKDIVbits.PLLPRE = 0; // PLL prescaler: divide by 2, postscaler: div by 4(default), PLL divisor: x43, FRCdiv:1(default)
    CLKDIVbits.PLLPOST = 0;
    PLLFBDbits.PLLDIV = 41; // FOSC = 79.23 MHz (FRC = 7.37MHz, N1=2, N2=2, M = 43)
#else
    CLKDIVbits.PLLPRE = 0; // PLL prescaler: divide by 2, postscaler: div by 4(default), PLL divisor: x40, FRCdiv:1(default)
    CLKDIVbits.PLLPOST = 0;
    PLLFBDbits.PLLDIV = 38; // FOSC = 80 MHz (XTAL=8MHz, N1=2, N2=2, M = 40)
#endif // CLOCK_CONFIG

#else // AIRFRAME_TYPE
	PLLFBDbits.PLLDIV = 30 ; // FOSC = 32 MHz (XT = 8.00MHz, N1=2, N2=4, M = 32)
#endif // AIRFRAME_TYPE

#endif // BOARD_TYPE
	
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
	udb_init_capture() ;
	
#if (MAG_YAW_DRIFT == 1)
	udb_init_I2C() ;
#endif
//*	
#ifdef USE_DEBUG_U1
#warning("Using UART1 as debug port")
	uart1_init();
#else	
	udb_init_GPS() ;
#endif

#ifdef USE_DEBUG_U2
#warning("Using UART2 as debug port")
	uart2_init();
#else	
	udb_init_USART() ;
#endif
// */
	udb_init_USART() ;

#if (BOARD_TYPE & AUAV2_BOARD)
    // AUAV2_BOARD uses MPU6000 for inertial sensors
//    delay_ms(100);
    MPU6000_init16();
#endif

    // initialize PWM outputs
	udb_init_pwm() ;
	
#if (USE_OSD == 1)
	udb_init_osd() ;
#endif
	
	SRbits.IPL = 0 ;	// turn on all interrupt priorities
	
	return ;
}

void udb_run(void)
{
#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
    //  nothing else to do... entirely interrupt driven
//    while (1)
    {
        // ISRs now start and stop the cpu timer
        //        // pause cpu counting timer while not in an ISR
        //        indicate_loading_main;

        // background task performs low priority tasks and idles when done
        run_background_task();
    }
    // Never returns
#else // AIRFRAME_TYPE
//	//  nothing else to do... entirely interrupt driven
//	while (1)
//	{
		// pause cpu counting timer while not in an ISR
		indicate_loading_main ;
//	}
//	// Never returns
#endif // AIRFRAME_TYPE
}


void udb_init_leds( void )
{
	
#if (BOARD_IS_CLASSIC_UDB == 1)
	TRISFbits.TRISF0 = 0 ;
    // set up LED pins as outputs
#elif (BOARD_TYPE == UDB4_BOARD)
	_TRISE1 = _TRISE2 = _TRISE3 = _TRISE4 = 0 ;
	_LATE1 = _LATE2 = _LATE3 = _LATE4 = LED_OFF ;
#elif (BOARD_TYPE & AUAV2_BOARD)
	_TRISB0 = _TRISB1 = _TRISB3 = _TRISB4 = 0 ;
	_LATB0 = _LATB1 = _LATB3 = _LATB4 = LED_OFF ;
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
	udb_xrate.offset = udb_xrate.value ;
	UDB_YACCEL.offset = UDB_YACCEL.value ;
	udb_yrate.offset = udb_yrate.value ;
	UDB_ZACCEL.offset = UDB_ZACCEL.value + ( Z_GRAVITY_SIGN ((int)(2*GRAVITY))) ; // same direction
	udb_zrate.offset = udb_zrate.value ;									
#ifdef VREF
	udb_vref.offset = udb_vref.value ;
#endif
	return ;
}
#endif // INITIALIZE_VERTICAL


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
