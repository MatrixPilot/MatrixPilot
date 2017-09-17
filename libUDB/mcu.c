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


#include "libUDB.h"
#include "oscillator.h"
#include "interrupt.h"
#include "uart.h"
#include "mcu.h"
#include "options_ports.h"
#include <stdio.h>

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
#include <p33Fxxxx.h>
#ifdef __XC16__
#pragma config FNOSC = PRIPLL
#pragma config FCKSM = CSDCMD
#pragma config OSCIOFNC = OFF
#pragma config POSCMD = XT
#pragma config FWDTEN = OFF
#pragma config WINDIS = OFF
#pragma config GSS = OFF
#pragma config GWRP = OFF
#pragma config FPWRT = PWR1
#pragma config JTAGEN = OFF
#pragma config ICS = PGD2

#else // Not __XC16__
_FOSCSEL(FNOSC_PRIPLL); // pri plus PLL (primary osc  w/ PLL)
_FOSC(FCKSM_CSDCMD &
      OSCIOFNC_OFF &
      POSCMD_XT);
// Clock switching on startup is enabled, starts with fast RC.
// Clock switching after startup is disabled.
// Fail-Safe Clock Monitor is disabled.
// OSC2 pin has clock out function.
// Primary Oscillator XT mode.
_FWDT(FWDTEN_OFF &
      WINDIS_OFF);
_FGS(GSS_OFF &
     GCP_OFF &
     GWRP_OFF);
_FPOR(FPWRT_PWR1);
_FICD(JTAGEN_OFF &
      ICS_PGD2);
#endif // __XC16__

#elif (BOARD_TYPE == AUAV3_BOARD)

// DSPIC33EP512MU810 Configuration Bit Settings

#include <p33Exxxx.h>
#ifdef __XC16__

// FGS
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = OFF                // General Segment Code-Protect bit (General Segment Code protect is disabled)
#pragma config GSSK = OFF               // General Segment Key bits (General Segment Write Protection and Code Protection is Disabled)

// FOSCSEL
#pragma config FNOSC = FRC              // Initial Oscillator Source Selection Bits (Internal Fast RC) )
#pragma config IESO = OFF               // Two-speed Oscillator Start-up Enable bit (Start up with user-selected oscillator source)

// FOSC
#pragma config POSCMD = XT              // Primary Oscillator Mode Select bits (XT Crystal Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function bit (OSC2 is clock output)
#pragma config IOL1WAY = ON             // Peripheral pin select configuration (Allow only one reconfiguration)
#pragma config FCKSM = CSECMD           // Clock Switching Mode bits ( Clock enabled and Fail-safe Clock Monitor disabled)

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
_FOSCSEL(FNOSC_FRC);
//_FOSCSEL(FNOSC_PRIPLL & IESO_OFF);
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT & IOL1WAY_ON);
//_FWDT(FWDTEN_OFF & WINDIS_OFF & PLLKEN_ON & WDTPRE_PRI128 & PDTPOST_PS32768);
_FWDT(FWDTEN_OFF);
_FICD(ICS_PGD3);
_FPOR(ALTI2C1_ON & ALTI2C2_ON);
/*
//_FOSCSEL(FNOSC_FRC);
_FOSCSEL(FNOSC_PRIPLL & IESO_OFF);
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT & IOL1WAY_ON);
//_FWDT(FWDTEN_OFF & WINDIS_OFF & PLLKEN_ON & WDTPRE_PRI128 & PDTPOST_PS32768);
_FWDT(FWDTEN_OFF & WINDIS_OFF & PLLKEN_ON);
_FICD(ICS_PGD3);
_FPOR(ALTI2C1_ON & ALTI2C2_ON);
 */
#endif // __XC16__
#endif // BOARD_TYPE

#define INPUT_PIN  1
#define OUTPUT_PIN 0

int16_t defaultCorcon = 0;

volatile uint16_t trap_flags __attribute__ ((persistent, near));
volatile uint32_t trap_source __attribute__ ((persistent, near));
volatile uint16_t osc_fail_count __attribute__ ((persistent, near));

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
static void configureANS(void) // disable all analog inputs
{
	ANSELA = 0x0000;
	ANSELB = 0x0000;
	ANSELC = 0x0000;
	ANSELD = 0x0000;
	ANSELE = 0x0000;
	ANSELG = 0x0000;
}

static void configurePPS(void)  // This method assigns all PPS registers
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
	_IC1R = IC_RPIN1;
	_IC2R = IC_RPIN2;
	_IC3R = IC_RPIN3;
	_IC4R = IC_RPIN4;
	_IC5R = IC_RPIN5;
	_IC6R = IC_RPIN6;
	_IC7R = IC_RPIN7;
	_IC8R = IC_RPIN8;

// OC1:8 are PWM module outputs
/* OC modules not currently used, but will be needed for multirotor ESC control
	OC_RPIN1 = 0b010000;
	OC_RPIN2 = 0b010001;
	OC_RPIN3 = 0b010010;
	OC_RPIN4 = 0b010011;
	OC_RPIN5 = 0b010100;
	OC_RPIN6 = 0b010101;
	OC_RPIN7 = 0b010110;
	OC_RPIN8 = 0b010111;
*/

	// UART mapping:
	// #  MatrixPilot | AUAV3 silk          | AUAV3 Net | AUAV3 Port
	// -------------------------------------------------------------
	// 1: GPS           GPS                   GPS_RX,TX   PORT4
	// 2: USART         OUART1 (optoisolated) U1RX,TX     PORT1
	// 3: ---           UART3                 U3RX,TX     PORT3
	// 4: ---           OUART2 (optoisolated) U2RX,TX     PORT2

	#define _UART_TO_PORT(x, y) \
		_U##x##RXR = PORT##y##_RP; \
		PORT##y##_RPI = UART##x##_RP;
	#define UART_TO_PORT(x, y) _UART_TO_PORT(x, y)

	UART_TO_PORT(1, GPS_PORT)
	UART_TO_PORT(2, TLM_PORT)
	#if (CONSOLE_UART != 0) && (CONSOLE_UART != 9)
		UART_TO_PORT(CONSOLE_UART, DBG_PORT)
	#endif // CONSOLE_UART

	#ifdef AUAV3
		#if (OSD_UART != 0)
			UART_TO_PORT(OSD_UART, OSD_PORT)
		#endif // OSD_UART
	#endif // AUAV3

	// Lock Registers
	__builtin_write_OSCCONL(OSCCON | (1 << 6));
}

// This method configures TRISx for the digital IOs
static void configureDigitalIO(void)   // AUAV3 board
{
	// TRIS registers have no effect on pins mapped to peripherals
	// TRIS assignments are made in the initialization methods for each function

	// port A
	TRISAbits.TRISA6  = INPUT_PIN;  // DIG2
	TRISAbits.TRISA7  = INPUT_PIN;  // DIG1
	TRISAbits.TRISA15 = INPUT_PIN;  // I4
	TRISAbits.TRISA14 = INPUT_PIN;  // I5
	TRISAbits.TRISA5  = INPUT_PIN;  // I6
	TRISAbits.TRISA4  = INPUT_PIN;  // I7

	// port B
	TRISBbits.TRISB2  = OUTPUT_PIN; // LED1
	TRISBbits.TRISB3  = OUTPUT_PIN; // LED2
	TRISBbits.TRISB4  = OUTPUT_PIN; // LED3
	TRISBbits.TRISB5  = OUTPUT_PIN; // LED4

	// port D
	TRISDbits.TRISD0  = INPUT_PIN;  // I1
	TRISDbits.TRISD11 = INPUT_PIN;  // I2
	TRISDbits.TRISD2  = OUTPUT_PIN; // SS3
	TRISDbits.TRISD7  = OUTPUT_PIN; // O4
	TRISDbits.TRISD8  = INPUT_PIN;  // I3

	// port E
	TRISEbits.TRISE0  = INPUT_PIN;  // O2
	TRISEbits.TRISE1  = INPUT_PIN;  // DIG0
	TRISEbits.TRISE2  = OUTPUT_PIN; // SD01 (MPU6000)
	TRISEbits.TRISE3  = INPUT_PIN;  // SDI1 (MPU6000)
	TRISEbits.TRISE4  = OUTPUT_PIN; // SS1  (MPU6000)
	TRISEbits.TRISE5  = OUTPUT_PIN; // GPS_TX
	TRISEbits.TRISE6  = INPUT_PIN;  // GPS_RX
	TRISEbits.TRISE7  = OUTPUT_PIN; // SS2  (AT45)

	// port F
	TRISFbits.TRISF0  = INPUT_PIN;  // CAN_RX
	TRISFbits.TRISF1  = OUTPUT_PIN; // CAN_TX
 
	// Pins F2, F3, F3, F4 are initialized by the Peripheral Module hardware when it is enabled.
	
	TRISFbits.TRISF8  = INPUT_PIN;  // I8
	TRISFbits.TRISF13 = OUTPUT_PIN; // O7
	TRISFbits.TRISF12 = OUTPUT_PIN; // O8

	// port G
	TRISGbits.TRISG0  = OUTPUT_PIN; // O1
	TRISGbits.TRISG13 = OUTPUT_PIN; // O3
	TRISGbits.TRISG14 = OUTPUT_PIN; // O5
	TRISGbits.TRISG1  = OUTPUT_PIN; // O6

// Configure the DIGx pins as outputs for scope tracing
//	TRISAbits.TRISA6  = OUTPUT_PIN; // DIG2
	TRISAbits.TRISA7  = OUTPUT_PIN; // DIG1
	TRISEbits.TRISE1  = OUTPUT_PIN; // DIG0

	// any pin configured as an input which is not permanantly connected
	// to a device should have an internal pullup or pulldown enabled

	// CNPU bits enable input pin weak pull up resistors
	CNPUAbits.CNPUA6  = 1;          // DIG2
	CNPUAbits.CNPUA7  = 1;          // DIG1
	CNPUEbits.CNPUE1  = 1;          // DIG0
}
#else
static void configureDigitalIO(void) // UDB4 and UDB5 boards
{
	// TODO: this needs to be updated to support PPM input on user defined input channel
	_TRISD8 = 1;
#if (USE_PPM_INPUT == 0)
	_TRISD9 = _TRISD10 = _TRISD11 = _TRISD12 = _TRISD13 = _TRISD14 = _TRISD15 = _TRISD8;
#endif
	TRISF = 0b1111111111101100;
}
#endif // BOARD_TYPE

static void init_leds(void)
{
#if (BOARD_TYPE == AUAV3_BOARD)
	_LATB2 = LED_OFF; _LATB3 = LED_OFF; _LATB4 = LED_OFF; _LATB5 = LED_OFF; 
	_TRISB2 = 0; _TRISB3 = 0; _TRISB4 = 0; _TRISB5 = 0;
#elif (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
	_LATE1 = LED_OFF; _LATE2 = LED_OFF; _LATE3 = LED_OFF; _LATE4 = LED_OFF;
	_TRISE1 = 0; _TRISE2 = 0; _TRISE3 = 0; _TRISE4 = 0;
#else
#error Invalid BOARD_TYPE
#endif // BOARD_TYPE
}

static void init_pll(void)
{
#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
#if (MIPS == 16)
//No warning given for default MIPS speed
	CLKDIVbits.PLLPRE = 0;  // PLL prescaler: N1 = 2 (default)
	CLKDIVbits.PLLPOST = 1; // PLL postscaler: N2 = 4 (default)
	PLLFBDbits.PLLDIV = 30; // FOSC = 32 MHz (XTAL=8MHz, N1=2, N2=4, M = 32)
#elif (MIPS == 32)
#warning 32 MIPS selected
	CLKDIVbits.PLLPRE = 0;  // PLL prescaler: N1 = 2 (default)
	CLKDIVbits.PLLPOST = 0; // PLL postscaler: N2 = 2
	PLLFBDbits.PLLDIV = 30; // FOSC = 64 MHz (XTAL=8MHz, N1=2, N2=2, M = 32)
#elif (MIPS == 40)
#warning 40 MIPS selected
	CLKDIVbits.PLLPRE = 0;  // PLL prescaler: N1 = 2 (default)
	CLKDIVbits.PLLPOST = 0; // PLL postscaler: N2 = 2
	PLLFBDbits.PLLDIV = 38; // FOSC = 80 MHz (XTAL=8MHz, N1=2, N2=2, M = 40)
#else
#error "invalid MIPS Configuration"
#endif // MIPS
//Change GFM#9: in accordance with Microchip recommandation
// Cf. DS70188E-page 17-16    
while(OSCCONbits.LOCK != 1) {}; // Wait for PLL to lock
#endif // BOARD_TYPE

#if (BOARD_TYPE == AUAV3_BOARD)
#if (MIPS == 70)
#warning 70 MIPS selected
	// Configure the device PLL to obtain 70 MIPS operation. The crystal
	// frequency is 8MHz. Divide 8MHz by 2, multiply by 70 and divide by
	// 2. This results in Fosc of 140MHz. The CPU clock frequency is
	// Fcy = Fosc/2 = 70MHz.
	PLLFBD = 68;                // M  = 70
#elif (MIPS == 64)
#warning 64 MIPS selected
	PLLFBD = 62;                // M  = 64
#elif (MIPS == 40)
#warning 40 MIPS selected
	PLLFBD = 38;                // M  = 40
#elif (MIPS == 32)
#warning 32 MIPS selected
	PLLFBD = 30;                // M  = 32
#elif (MIPS == 16)
// No warning given for default MIPS speed
	PLLFBD = 14;                // M  = 16
#else
#error Invalid MIPS Configuration
#endif // MIPS
	CLKDIVbits.PLLPOST = 0;     // N1 = 2
	CLKDIVbits.PLLPRE = 0;      // N2 = 2
	OSCTUN = 0;

	// Initiate Clock Switch to Primary Oscillator with PLL (NOSC= 0x3)
	__builtin_write_OSCCONH(0x03);
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0x3);     // Wait for the Primary PLL to lock
#if (USE_USB == 1)
	// Configuring the auxiliary PLL.
	// Since the primary oscillator provides the source clock to the
	// auxiliary PLL, the auxiliary oscillator is disabled.
	// Note that the AUX PLL is enabled. The input 8MHz clock is divided
	// by 2, multiplied by 24 and then divided by 2.
	ACLKCON3 = 0x24C1;
	ACLKDIV3 = 0x7;
	ACLKCON3bits.ENAPLL = 1;
	while (ACLKCON3bits.APLLCK != 1);   // Wait till the AUX PLL locks.
#endif // USE_USB
#endif // BOARD_TYPE
}

void mcu_init(void)
{
	defaultCorcon = CORCON;

	if (_SWR == 0)  // if there was not a software reset (trap error) clear the trap data
	{
		trap_flags = 0;
		trap_source = 0;
		osc_fail_count = 0;
	}
	init_pll();
#if (BOARD_TYPE == AUAV3_BOARD)
	configureANS();
	configurePPS();
#endif // BOARD_TYPE
	configureDigitalIO();
	init_leds();
#if (CONSOLE_UART != 0)
	init_uart();
	DPRINT("\r\n\r\nMatrixPilot " __TIME__ " " __DATE__ " @ %u mips\r\n", MIPS);
	if (_SWR == 1)
	{
		DPRINT("S/W Reset: trap_flags %04x, trap_source %04x%04x, osc_fail_count %u\r\n", 
		    trap_flags, 
		    (unsigned int)(trap_source >> 16), 
		    (unsigned int)(trap_source & 0xffff), 
		    osc_fail_count);
	}
#endif // CONSOLE_UART
}
