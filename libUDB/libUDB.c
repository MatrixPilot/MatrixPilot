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
#include "mpu6000.h"
//#include "defines.h"
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

#elif ((BOARD_TYPE == UDB4_BOARD) || BOARD_TYPE == UDB5_BOARD || (BOARD_TYPE == AUAV2_BOARD))

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
#ifdef AUAV3
_FGS(GSS_OFF &
     GCP_OFF &
     GWRP_OFF); // User program memory is not code-protected
#endif
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
//_FGS(	GSS_OFF &
//		GCP_OFF &
//		GWRP_OFF ) ;
_FPOR(	FPWRT_PWR1 ) ;
_FICD(	JTAGEN_OFF &
		ICS_PGD2 ) ;
#endif // AIRFRAME_TYPE
#elif (BOARD_TYPE == AUAV3_BOARD)

// DSPIC33EP512MU810 Configuration Bit Settings

#include <p33Exxxx.h>
#ifdef __XC16__

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = PRIPLL           // Initial Oscillator Source Selection Bits (Primary Oscillator (XT, HS, EC) with PLL)
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration (Allow only one reconfiguration)
#pragma config FCKSM = CSDCMD           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler Bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler bit (1:128)
#pragma config PLLKEN = ON              // PLL Lock Wait Enable bit (Clock switch to PLL source will wait until the PLL lock signal is valid.)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable bit (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // Power-on Reset Timer Value Select bits (128ms)
#pragma config BOREN = ON               // Brown-out Reset (BOR) Detection Enable bit (BOR is enabled)
#pragma config ALTI2C1 = ON             // Alternate I2C pins for I2C1 (ASDA1/ASCK1 pins are selected as the I/O pins for I2C1)
#pragma config ALTI2C2 = ON             // Alternate I2C pins for I2C2 (ASDA2/ASCK2 pins are selected as the I/O pins for I2C2)

// FICD
#pragma config ICS = PGD3               // ICD Communication Channel Select bits (Communicate on PGEC3 and PGED3)
#pragma config RSTPRI = PF              // Reset Target Vector Select bit (Device will obtain reset instruction from Primary flash)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)

// FAS
#pragma config AWRP = OFF               // Auxiliary Segment Write-protect bit (Auxiliary program memory is not write-protected)
#pragma config APL = OFF                // Auxiliary Segment Code-protect bit (Aux Flash Code protect is disabled)
#pragma config APLK = OFF               // Auxiliary Segment Key bits (Aux Flash Write Protection and Code Protection is Disabled)

#else // __XC16__

//_FOSCSEL(FNOSC_FRC);
_FOSCSEL(FNOSC_PRIPLL);
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT);
_FWDT(FWDTEN_OFF);
_FICD(ICS_PGD3);
_FPOR(ALTI2C1_ON & ALTI2C2_ON);

#endif // __XC16__

#endif


union udb_fbts_byte udb_flags ;

int16_t defaultCorcon = 0 ;


volatile int16_t trap_flags __attribute__ ((persistent));
volatile int32_t trap_source __attribute__ ((persistent));
volatile int16_t osc_fail_count __attribute__ ((persistent)) ;


#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_current ;
union longww battery_mAh_used ;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
union longww battery_voltage;	// battery_voltage._.W1 is in tenths of Volts
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
uint8_t rc_signal_strength ;
#define MIN_RSSI	((int32_t)((RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 65536))
#define RSSI_RANGE	((int32_t)((RSSI_MAX_SIGNAL_VOLTAGE-RSSI_MIN_SIGNAL_VOLTAGE)/3.3 * 100))
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

#if (BOARD_TYPE == AUAV3_BOARD )
// This method assigns all PPS registers

void configurePPS(void) {
    // configure PPS registers

    //*************************************************************
    // Unlock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));

    // CAN module 1 I/O
    _C1RXR = 96;
    _RP97R = 0b001110;

    // SPI1 SS, SCK, SDI, SDO
    //    _RP84R = 0b000111;     // SS1 output RP84
    // in master mode, SS is not used by the SPI module; configure as GP output instead
    // LATE4 is SS1
    _RP127R = 0b000110; // SCK1 input/output RP127: T1 white
    _SDI1R = 83;        // SDI1 input RPI83         T4 blue
    _RP82R = 0b000101;  // SDO1 output RP82         T2 red

    // SPI2: SCK2, SDI2, SDO2 are dedicated pins
    //    _RP87R = 0b001010;     // SS2 output RP87
    // LATE7 is SS2

    // SPI3 SS, SCK, SDI, SDO
    //    _RP66R = 0b100001;     // SS3 output RP66
    // LATD2 is SS3
    _RP65R = 0b100000; // SCK3 output RP65
    _SDI3R = 76;        // SDI3 input RPI76
    _RP67R = 0b011111;  // SDO3 output RP67

    // INTG (MPU6000 interrupt)
    _INT1R = 124; // RPI124/RG12

    // IC1:8 are Input Capture module inputs
    _IC1R = 64; // IC1 on RP64
    _IC2R = 75; // IC2 on RP75
    _IC3R = 72; // IC3 on RP72
    _IC4R = 31; // IC4 on RP31
    _IC5R = 30; // IC5 on RP30
    _IC6R = 21; // IC6 on RP21
    _IC7R = 20; // IC7 on RP20
    _IC8R = 104; // IC8 on RP104

//    // OC1:8 are PWM module outputs
//
//    _RP112R = 0b010000; // OC1 output RP112
//    _RP80R = 0b010001; // OC2 output RP80
//    _RP125R = 0b010010; // OC3 output RP125
//    _RP71R = 0b010011; // OC4 output RP71
//    _RP126R = 0b010100; // OC5 output RP126
//    _RP113R = 0b010101; // OC6 output RP113
//    _RP109R = 0b010110; // OC7 output RP109
//    _RP108R = 0b010111; // OC8 output RP108

    // UART mapping:
    // #  MatrixPilot | AUAV3               | AUAV3 Net
    // ------------------------------------------------
    // 1: GPS           GPS                   GPS_RX,TX
    // 2: USART         TLM (optoisolated)    U1RX,TX
    // 3: ---           UART3                 U3RX,TX
    // 4: ---           OSD (optoisolated)    U2RX,TX

    // UART1 RX, TX: This is the GPS UART in MatrixPilot
    // On the AUAV3, GPS_RX,TX are pins RPI86,RP85
    _U1RXR = 86;        // U1RX input RPI86
    _RP85R = 0b000001;  // U1TX output RP85

    // UART2 RX, TX; This is the "USART" in MatrixPilot
    // On the AUAV3, the opto-uart port labeled "OUART1" is on nets U1RX,TX and pins RPI78,RP79
    _U2RXR = 78;        // U2RX input RP178
    _RP79R = 0b000011;  // U2TX output RP79

    // UART3 RX, TX
    // On the AUAV3, the uart port labeled "UART3" is on nets U3RX,TX and pins RP98,99
    _U3RXR = 98;        // U3RX input RP98
    _RP99R = 0b011011;  // U3TX output RP99

    // UART4 RX, TX
    // On the AUAV3, the opto-uart port labeled "OUART2" is on nets U2RX,TX and pins RP100,101
    _U4RXR = 100;       // U4RX input RP100
    _RP101R = 0b011101; // U4TX output RP101


    //*************************************************************
    // Lock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON | (1 << 6));
}

// This method configures TRISx for the digital IOs

void configureDigitalIO(void) {
    // port A
    TRISAbits.TRISA6 = 1; // DIG2
    TRISAbits.TRISA7 = 1; // DIG1

    // port E
    TRISEbits.TRISE1 = 1; // DIG0

    // TRIS registers have no effect on pins mapped to peripherals
    // and TRIS assignments are made in the initialization methods for each function

}
#endif

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

#if (BOARD_TYPE == AUAV3_BOARD )
        configurePPS();
        configureDigitalIO();
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
	udb_init_clock() ;
	udb_init_capture() ;
	
#if (MAG_YAW_DRIFT == 1 && HILSIM != 1)
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
	udb_init_pwm() ;
	
#if (USE_OSD == 1)
	udb_init_osd() ;
#endif


#if (BOARD_TYPE & AUAV2_BOARD || BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD)
    // AUAV2_BOARD uses MPU6000 for inertial sensors
//    delay_ms(100);
    MPU6000_init16();
#endif

	udb_init_ADC() ;
	SRbits.IPL = 0 ;	// turn on all interrupt priorities
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
#elif (BOARD_TYPE == AUAV3_BOARD )
    // port B
    _LATB2 = LED_OFF; _LATB3 = LED_OFF; _LATB4 = LED_OFF; _LATB5 = LED_OFF; 
    // port B
    TRISBbits.TRISB2 = 0; // LED1
    TRISBbits.TRISB3 = 0; // LED2
    TRISBbits.TRISB4 = 0; // LED3
    TRISBbits.TRISB5 = 0; // LED4


#endif
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
	UDB_YACCEL.offset = UDB_YACCEL.value - ( Y_GRAVITY_SIGN ((int16_t)(2*GRAVITY)) ); // opposite direction
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
	UDB_ZACCEL.offset = UDB_ZACCEL.value + ( Z_GRAVITY_SIGN ((int16_t)(2*GRAVITY))) ; // same direction
	udb_zrate.offset = udb_zrate.value ;									
#ifdef VREF
	udb_vref.offset = udb_vref.value ;
#endif
}
#endif // INITIALIZE_VERTICAL


void udb_servo_record_trims(void)
{
	int16_t i;
	for (i=0; i <= NUM_INPUTS; i++)
		udb_pwTrim[i] = udb_pwIn[i] ;
}


// saturation logic to maintain pulse width within bounds
int16_t udb_servo_pulsesat ( int32_t pw )
{
	if ( pw > SERVOMAX ) pw = SERVOMAX ;
	if ( pw < SERVOMIN ) pw = SERVOMIN ;
	return (int16_t)pw ;
}


void calculate_analog_sensor_values( void )
{
#if (ANALOG_CURRENT_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to current in tenths of Amps
	battery_current.WW = (udb_analogInputs[ANALOG_CURRENT_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_CURRENT) + (((int32_t)(CURRENT_SENSOR_OFFSET)) << 16) ;
	
	// mAh = mA / 144000 (increment per 40Hz tick is /40*60*60)
	// 90000/144000 == 900/1440
	battery_mAh_used.WW += (battery_current.WW / 1440) ;
#endif

#if (ANALOG_VOLTAGE_INPUT_CHANNEL != CHANNEL_UNUSED)
	// Shift up from [-2^15 , 2^15-1] to [0 , 2^16-1]
	// Convert to voltage in tenths of Volts
	battery_voltage.WW = (udb_analogInputs[ANALOG_VOLTAGE_INPUT_CHANNEL-1].value + (int32_t)32768) * (MAX_VOLTAGE) + (((int32_t)(VOLTAGE_SENSOR_OFFSET)) << 16) ;
#endif

#if (ANALOG_RSSI_INPUT_CHANNEL != CHANNEL_UNUSED)
	union longww rssi_accum ;
	rssi_accum.WW = (((udb_analogInputs[ANALOG_RSSI_INPUT_CHANNEL-1].value + 32768) - (MIN_RSSI)) * (10000 / (RSSI_RANGE))) ;
	if (rssi_accum._.W1 < 0)
		rc_signal_strength = 0 ;
	else if (rssi_accum._.W1 > 100)
		rc_signal_strength = 100 ;
	else
		rc_signal_strength = (uint8_t)rssi_accum._.W1 ;
#endif
}


uint16_t udb_get_reset_flags(void)
{
	return RCON ;
}
