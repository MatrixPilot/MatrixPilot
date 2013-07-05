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
#include "oscillator.h"
#include "interrupt.h"
#include "events.h"

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

#if (AIRFRAME_TYPE == AIRFRAME_QUAD)

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
_FOSCSEL(FNOSC_PRIPLL);            // medium speed XTAL plus PLL
_FOSC(	FCKSM_CSECMD &
		OSCIOFNC_ON &
		POSCMD_NONE );
_FWDT(	FWDTEN_OFF &
		WINDIS_OFF );
//_FGS(	GSS_OFF &
//		GCP_OFF &
//		GWRP_OFF );
_FPOR(	FPWRT_PWR1 );
_FICD(	JTAGEN_OFF &
		ICS_PGD2 );
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
#endif // BOARD_TYPE


int16_t defaultCorcon = 0;

volatile int16_t trap_flags __attribute__ ((persistent, near));
volatile int32_t trap_source __attribute__ ((persistent, near));
volatile int16_t osc_fail_count __attribute__ ((persistent, near));


uint16_t get_reset_flags(void)
{
	uint16_t oldRCON = RCON;
	RCON = 0;
	trap_flags = 0;
	trap_source = 0;
	osc_fail_count = 0;
	return oldRCON;
}

#if (BOARD_TYPE == AUAV3_BOARD)
// This method assigns all PPS registers
void configurePPS(void)
{
	// Unlock Registers
	__builtin_write_OSCCONL(OSCCON & ~(1 << 6));

	// CAN module 1 I/O
	_C1RXR = 96;
	_RP97R = 0b001110;

	// SPI1 SS, SCK, SDI, SDO
	//_RP84R = 0b000111;    // SS1 output RP84
	// in master mode, SS is not used by the SPI module; configure as GP output instead
	// LATE4 is SS1
	_RP127R = 0b000110;     // SCK1 input/output RP127: T1 white
	_SDI1R = 83;            // SDI1 input RPI83         T4 blue
	_RP82R = 0b000101;      // SDO1 output RP82         T2 red

	// SPI2: SCK2, SDI2, SDO2 are dedicated pins
	//_RP87R = 0b001010;    // SS2 output RP87
	// LATE7 is SS2

	// SPI3 SS, SCK, SDI, SDO
	//_RP66R = 0b100001;    // SS3 output RP66
	// LATD2 is SS3
	_RP65R = 0b100000;      // SCK3 output RP65
	_SDI3R = 76;            // SDI3 input RPI76
	_RP67R = 0b011111;      // SDO3 output RP67

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
	// #  MatrixPilot | AUAV3 silk          | AUAV3 Net | AUAV3 Port
	// -------------------------------------------------------------
	// 1: GPS           GPS                   GPS_RX,TX   PORT4
	// 2: USART         OUART1 (optoisolated) U1RX,TX     PORT1
	// 3: ---           UART3                 U3RX,TX     PORT3
	// 4: ---           OUART2 (optoisolated) U2RX,TX     PORT2

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

	// Lock Registers
	__builtin_write_OSCCONL(OSCCON | (1 << 6));
}

// This method configures TRISx for the digital IOs
void configureDigitalIO(void)   // AUAV3 board
{
	// TRIS registers have no effect on pins mapped to peripherals
	// TRIS assignments are made in the initialization methods for each function

	// port A
	TRISAbits.TRISA6 = 1;   // DIG2
	TRISAbits.TRISA7 = 1;   // DIG1
	TRISAbits.TRISA15 = 1;  // I4
	TRISAbits.TRISA14 = 1;  // I5
	TRISAbits.TRISA5 = 1;   // I6
	TRISAbits.TRISA4 = 1;   // I7

	// port B
	TRISBbits.TRISB2 = 0;   // LED1
	TRISBbits.TRISB3 = 0;   // LED2
	TRISBbits.TRISB4 = 0;   // LED3
	TRISBbits.TRISB5 = 0;   // LED4

	// port D
	TRISDbits.TRISD0 = 1;   // I1
	TRISDbits.TRISD11 = 1;  // I2
	TRISDbits.TRISD2 = 0;   // SS3
	TRISDbits.TRISD7 = 0;   // O4
	TRISDbits.TRISD8 = 1;   // I3

	// port E
	TRISEbits.TRISE0 = 1;   // O2
	TRISEbits.TRISE1 = 1;   // DIG0
	TRISEbits.TRISE2 = 0;   // SD01 (MPU6000)
	TRISEbits.TRISE3 = 1;   // SDI1 (MPU6000)
	TRISEbits.TRISE4 = 0;   // SS1  (MPU6000)
	TRISEbits.TRISE5 = 0;   // GPS_TX
	TRISEbits.TRISE6 = 1;   // GPS_RX
	TRISEbits.TRISE7 = 0;   // SS2  (AT45)

	// port F
	TRISFbits.TRISF0 = 1;   // CAN_RX
	TRISFbits.TRISF1 = 0;   // CAN_TX

	TRISFbits.TRISF2 = 1;   // U3_RX
	TRISFbits.TRISF3 = 0;   // U3_TX

	TRISFbits.TRISF4 = 1;   // U2_RX
	TRISFbits.TRISF5 = 0;   // U2_TX

	TRISFbits.TRISF8 = 1;   // I8
	TRISFbits.TRISF13 = 0;  // O7
	TRISFbits.TRISF12 = 0;  // O8

	// port G
	TRISGbits.TRISG0 = 0;   // O1
	TRISGbits.TRISG13 = 0;  // O3
	TRISGbits.TRISG14 = 0;  // O5
	TRISGbits.TRISG1 = 0;   // O6

// Configure the DIGx pins as outputs for scope tracing
	TRISAbits.TRISA6 = 0;   // DIG2
	TRISAbits.TRISA7 = 0;   // DIG1
	TRISEbits.TRISE1 = 0;   // DIG0
}
#else
void configureDigitalIO(void) // UDB4 and UDB5 boards
{
	_TRISD8 = 1;
#if (USE_PPM_INPUT == 0)
	_TRISD9 = _TRISD10 = _TRISD11 = _TRISD12 = _TRISD13 = _TRISD14 = _TRISD15 = _TRISD8;
#endif
	TRISF = 0b1111111111101100;
}
#endif

void init_leds(void)
{
#if (BOARD_TYPE == AUAV3_BOARD)
	_LATB2 = LED_OFF; _LATB3 = LED_OFF; _LATB4 = LED_OFF; _LATB5 = LED_OFF; 
	_TRISB2 = 0; _TRISB3 = 0; _TRISB4 = 0; _TRISB5 = 0;
#elif (BOARD_TYPE == AUAV4_BOARD)
#elif (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
	_LATE1 = LED_OFF; _LATE2 = LED_OFF; _LATE3 = LED_OFF; _LATE4 = LED_OFF;
	_TRISE1 = 0; _TRISE2 = 0; _TRISE3 = 0; _TRISE4 = 0;
#else
#error Invalid BOARD_TYPE
#endif
}

void mcu_init(void)
{
	defaultCorcon = CORCON;
	
	if (_SWR == 0)
	{
		// if there was not a software reset (trap error) clear the trap data
		trap_flags = 0;
		trap_source = 0;
		osc_fail_count = 0;
	}

#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
//#if (AIRFRAME_TYPE == AIRFRAME_QUAD)
#if (1)
	// reset values of PLLPRE, PLLPOST, PLLDIV are 0, 1, 0x30, yielding FOSC of about 45MHz
//	CLKDIVbits.PLLPRE = 1;  // PLL prescaler: divide by 3, postscaler: div by 4(default), PLL divisor: x52, FRCdiv:1(default)
//	PLLFBDbits.PLLDIV = 50; // FOSC = 32 MHz (FRC = 7.37MHz, N1=3, N2=4, M = 52)

	CLKDIVbits.PLLPRE = 0; // PLL prescaler: divide by 2, postscaler: div by 4(default), PLL divisor: x40, FRCdiv:1(default)
	CLKDIVbits.PLLPOST = 0;
	PLLFBDbits.PLLDIV = 38; // FOSC = 80 MHz (XTAL=8MHz, N1=2, N2=2, M = 40)

#else // AIRFRAME_TYPE
	PLLFBDbits.PLLDIV = 30; // FOSC = 32 MHz (XT = 8.00MHz, N1=2, N2=4, M = 32)
#endif // AIRFRAME_TYPE

#endif // BOARD_TYPE

#if (BOARD_TYPE == AUAV3_BOARD)
#if (USE_USB == 1)
	// Configuring the auxiliary PLL, since the primary
	// oscillator provides the source clock to the auxiliary
	// PLL, the auxiliary oscillator is disabled. Note that
	// the AUX PLL is enabled. The input 8MHz clock is divided
	// by 2, multiplied by 24 and then divided by 2. Wait till 
	// the AUX PLL locks.
	ACLKCON3 = 0x24C1;
	ACLKDIV3 = 0x7;
	ACLKCON3bits.ENAPLL = 1;
	while (ACLKCON3bits.APLLCK != 1);
#endif // USE_USB
	configurePPS();
#endif // BOARD_TYPE

	configureDigitalIO();
	init_leds();

#if (CONSOLE_UART != 0)
	init_console();
	printf("\r\n\r\nMatrixPilot " __TIME__ " " __DATE__ " @ %u mips\r\n", MIPS);
	if (_SWR == 1)
	{
		printf("S/W Reset: trap_flags %04x, trap_source %04x%04x, osc_fail_count %u\r\n", 
			trap_flags, 
			(unsigned int)(trap_source >> 16), 
			(unsigned int)(trap_source & 0xffff), 
			osc_fail_count);
//			printf("active_int %04X %04X, RCON %04X, stack %x, limit %x\r\n", active_inta, active_intb, RCON, stack_ptr, SPLIM); 
	}
#endif // CONSOLE_UART
}
