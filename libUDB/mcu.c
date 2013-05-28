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

#if (USE_CONSOLE != 0)
#include "console.h"
#include <stdio.h>
#endif // USE_CONSOLE

#if (BOARD_IS_CLASSIC_UDB)
#error Classic UDB boards are no not supported in this version

#elif (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD )
_FOSCSEL(FNOSC_PRIPLL); // pri plus PLL (primary osc  w/ PLL)
_FOSC(FCKSM_CSDCMD &
      OSCIOFNC_OFF &
      POSCMD_XT); 
// Clock switching on startup is enabled, starts with fast RC.
// Clock switching after startup is disabled.
// Fail-Safe Clock Monitor is disabled.
// OSC2 pin has clock out function.
// Primary Oscillator XT mode.
_FWDT(	FWDTEN_OFF &
		WINDIS_OFF ) ;
_FGS(	GSS_OFF &
		GCP_OFF &
		GWRP_OFF ) ;
_FPOR(	FPWRT_PWR1 ) ;
_FICD(	JTAGEN_OFF &
		ICS_PGD2 ) ;

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

int16_t defaultCorcon = 0 ;

volatile int16_t trap_flags __attribute__ ((persistent, near));
volatile int32_t trap_source __attribute__ ((persistent, near));
volatile int16_t osc_fail_count __attribute__ ((persistent, near)) ;

volatile int16_t stack_ptr __attribute__ ((persistent, near)) ;

volatile uint16_t active_inta __attribute__ ((persistent, near)) ;
volatile uint16_t active_intb __attribute__ ((persistent, near)) ;


uint16_t get_reset_flags(void)
{
	return RCON;
}

#if (BOARD_TYPE == AUAV3_BOARD )
// This method assigns all PPS registers
void configurePPS(void) 
{
    // Unlock Registers
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
//    _IC1R = 64; // IC1 on RP64/RD0
//    _IC2R = 75; // IC2 on RP75/RD11
//    _IC3R = 72; // IC3 on RP72/RD8
//    _IC4R = 31; // IC4 on RP31/RA15
//    _IC5R = 30; // IC5 on RP30/RA14
//    _IC6R = 21; // IC6 on RP21/RA5
//    _IC7R = 20; // IC7 on RP20/RA4
//    _IC8R = 104; // IC8 on RP104/RF8
    _IC1R = IC_RPIN1;
    _IC2R = IC_RPIN2;
    _IC3R = IC_RPIN3;
    _IC4R = IC_RPIN4;
    _IC5R = IC_RPIN5;
    _IC6R = IC_RPIN6;
    _IC7R = IC_RPIN7;
    _IC8R = IC_RPIN8;

    // temporarily assign REFCLK0 to OC1 pin for PLL testing
////    _RP112R = 0b010000; // OC1 output RP112
//    _RP112R = 0b110001; // REFCLK0 output RP112
//    REFOCONbits.RODIV = 7;  // divide by 128
//    REFOCONbits.ROON = 1;   // enable refclk output

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
    // #  MatrixPilot | AUAV3 silk          | AUAV3 Net
    // ------------------------------------------------
    // 1: GPS           GPS                   GPS_RX,TX
    // 2: USART         OUART1 (optoisolated) U1RX,TX
    // 3: ---           UART3                 U3RX,TX
    // 4: ---           OUART2 (optoisolated) U2RX,TX

    // UART1 RX, TX: This is the GPS UART in MatrixPilot
    // On the AUAV3, GPS_RX,TX are pins RPI86,RP85
    _U1RXR = 86;        // U1RX input RPI86
    _RP85R = 0b000001;  // U1TX output RP85

    // use UART3 to connect OpenLog, since it supplies 5V power
    // use OUART1 to connect to Xbee using separate 5V power source
#define TLM_PORT OUART1

    // UART2 RX, TX; This is the "USART" in MatrixPilot
    // On the AUAV3, the opto-uart port labeled "OUART1" is on nets U1RX,TX and pins RPI78,RP79
#if (TLM_PORT == OUART1)
    _U2RXR = 78;        // U2RX input RP178
    _RP79R = 0b000011;  // U2TX output RP79
#elif (TLM_PORT == UART3)
    _U2RXR = 98;        // U2RX input RP98
    _RP99R = 0b000011;  // U2TX output RP99
#endif

    // UART3 RX, TX
    // On the AUAV3, the uart port labeled "UART3" is on nets U3RX,TX and pins RP98,99
#if (TLM_PORT == OUART1)
    _U3RXR = 98;        // U3RX input RP98
    _RP99R = 0b011011;  // U3TX output RP99
#elif (TLM_PORT == UART3)
    _U3RXR = 78;        // U3RX input RP178
    _RP79R = 0b011011;  // U3TX output RP79
#endif

    // UART4 RX, TX
    // On the AUAV3, the opto-uart port labeled "OUART2" is on nets U2RX,TX and pins RP100,101
    _U4RXR = 100;       // U4RX input RP100
    _RP101R = 0b011101; // U4TX output RP101

    // Lock Registers
    __builtin_write_OSCCONL(OSCCON | (1 << 6));
}

// This method configures TRISx for the digital IOs
void configureDigitalIO(void)
{
    // TRIS registers have no effect on pins mapped to peripherals
    // TRIS assignments are made in the initialization methods for each function

    // port A
    TRISAbits.TRISA6 = 1; // DIG2
    TRISAbits.TRISA7 = 1; // DIG1
    TRISAbits.TRISA15 = 1; // I4
    TRISAbits.TRISA14 = 1; // I5
    TRISAbits.TRISA5 = 1; // I6
    TRISAbits.TRISA4 = 1; // I7

    // port B
    TRISBbits.TRISB2 = 0; // LED1
    TRISBbits.TRISB3 = 0; // LED2
    TRISBbits.TRISB4 = 0; // LED3
    TRISBbits.TRISB5 = 0; // LED4

    // port D
    TRISDbits.TRISD0 = 1; // I1
    TRISDbits.TRISD11 = 1; // I2
    TRISDbits.TRISD2 = 0; // SS3
    TRISDbits.TRISD7 = 0; // O4
    TRISDbits.TRISD8 = 1; // I3

    // port E
    TRISEbits.TRISE0 = 1; // O2
    TRISEbits.TRISE1 = 1; // DIG0
    TRISEbits.TRISE2 = 0; // SD01 (MPU6000)
    TRISEbits.TRISE3 = 1; // SDI1 (MPU6000)
    TRISEbits.TRISE4 = 0; // SS1  (MPU6000)
    TRISEbits.TRISE5 = 0; // GPS_TX
    TRISEbits.TRISE6 = 1; // GPS_RX
    TRISEbits.TRISE7 = 0; // SS2  (AT45)

    // port F
    TRISFbits.TRISF8 = 1; // I8
    TRISFbits.TRISF13 = 0; // O7
    TRISFbits.TRISF12 = 0; // O8

    // port G
    TRISGbits.TRISG0 = 0; // O1
    TRISGbits.TRISG13 = 0; // O3
    TRISGbits.TRISG14 = 0; // O5
    TRISGbits.TRISG1 = 0; // O6

// Configure the DIGx pins as outputs for scope tracing
    TRISAbits.TRISA6 = 0; // DIG2
    TRISAbits.TRISA7 = 0; // DIG1
    TRISEbits.TRISE1 = 0; // DIG0
}
#else
void configureDigitalIO(void)
{
	_TRISD8 = 1 ;
#if (USE_PPM_INPUT == 0)
	_TRISD9 = _TRISD10 = _TRISD11 = _TRISD12 = _TRISD13 = _TRISD14 = _TRISD15 = _TRISD8 ;
#endif
}
#endif

void init_leds(void)
{
#if (BOARD_TYPE == AUAV3_BOARD )
    // port B
    _LATB2 = LED_OFF; _LATB3 = LED_OFF; _LATB4 = LED_OFF; _LATB5 = LED_OFF; 
    // port B
    TRISBbits.TRISB2 = 0; // LED1
    TRISBbits.TRISB3 = 0; // LED2
    TRISBbits.TRISB4 = 0; // LED3
    TRISBbits.TRISB5 = 0; // LED4
#elif (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD )
	_LATE1 = LED_OFF ;_LATE2 = LED_OFF ; _LATE3 = LED_OFF ;_LATE4 = LED_OFF ;
	_TRISE1 = 0 ;_TRISE2 = 0 ;_TRISE3 = 0 ;_TRISE4 = 0 ;
#else
#error Invalid BOARD_TYPE
#endif
}

void mcu_init(void)
{
	defaultCorcon = CORCON ;
	
	if ( _SWR == 0 )
	{
		// if there was not a software reset (trap error) clear the trap data
		trap_flags = 0 ;
		trap_source = 0 ;
		osc_fail_count = 0 ;

		stack_ptr = 0;
		active_inta = 0;
		active_intb = 0;
	}
	
// new RobD
	ANSELA = 0x0000;
	ANSELB = 0x0000;
	ANSELC = 0x0000;
	ANSELD = 0x0000;
	ANSELE = 0x0000;
	ANSELG = 0x0000;

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
	PLLFBDbits.PLLDIV = 30 ; // FOSC = 32 MHz (XT = 8.00MHz, N1=2, N2=4, M = 32)
#endif

#if (BOARD_TYPE == AUAV3_BOARD )
#if (MIPS == 64)
#warning Fast OSC selected
    // Configure the device PLL to obtain 64 MIPS operation. The crystal
    // frequency is 8MHz. Divide 8MHz by 2, multiply by 64 and divide by
    // 2. This results in Fosc of 128MHz. The CPU clock frequency is
    // Fcy = Fosc/2 = 64MHz. Wait for the Primary PLL to lock and then
    // configure the auxilliary PLL to provide 48MHz needed for USB 
    // Operation.
	PLLFBD = 62;				// M  = 64
#elif (MIPS == 32)
#warning Medium OSC selected
    // Configure the device PLL to obtain 32 MIPS operation. The crystal
    // frequency is 8MHz. Divide 8MHz by 2, multiply by 32 and divide by
    // 2. This results in Fosc of 64MHz. The CPU clock frequency is
    // Fcy = Fosc/2 = 32MHz. Wait for the Primary PLL to lock and then
    // configure the auxilliary PLL to provide 48MHz needed for USB 
    // Operation.
	PLLFBD = 30;				// M  = 32
#elif (MIPS == 16)
#warning Slow OSC selected
    // Configure the device PLL to obtain 16 MIPS operation. The crystal
    // frequency is 8MHz. Divide 8MHz by 2, multiply by 64 and divide by
    // 2. This results in Fosc of 32MHz. The CPU clock frequency is
    // Fcy = Fosc/2 = 16MHz. Wait for the Primary PLL to lock and then
    // configure the auxilliary PLL to provide 48MHz needed for USB 
    // Operation.
	PLLFBD = 14;				// M  = 16
#else
#error Invalid MIPS Configuration
#endif // MIPS
	CLKDIVbits.PLLPOST = 0;		// N1 = 2
	CLKDIVbits.PLLPRE = 0;		// N2 = 2
	OSCTUN = 0;			

    //	Initiate Clock Switch to Primary Oscillator with PLL (NOSC= 0x3)
	__builtin_write_OSCCONH(0x03);		
	__builtin_write_OSCCONL(0x01);
	while (OSCCONbits.COSC != 0x3);       
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
    
#if (USE_CONSOLE != 0)
	init_console();
    printf("\r\n\r\nMatrixPilot " __TIME__ " " __DATE__ " @ %u mips\r\n", MIPS);
	if ( _SWR == 1 )
	{
		printf("S/W Reset: trap_flags %04x, trap_source %04x%04x, osc_fail_count %u\r\n", 
			trap_flags, 
			(unsigned int)(trap_source >> 16), 
			(unsigned int)(trap_source & 0xffff), 
			osc_fail_count);
//			printf("active_int %04X %04X, RCON %04X, stack %x, limit %x\r\n", active_inta, active_intb, RCON, stack_ptr, SPLIM); 
	}
#endif // USE_CONSOLE
}
