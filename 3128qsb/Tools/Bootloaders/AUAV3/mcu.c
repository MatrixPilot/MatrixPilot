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


#include "defines.h"
#include "oscillator.h"
#include "HardwareProfile.h"
#include "mcu.h"
#include "../libFlashFS/AT45D.h"

#if 0

_FOSCSEL(FNOSC_FRC);
//_FOSCSEL(FNOSC_PRIPLL & IESO_OFF);
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT & IOL1WAY_ON);
//_FWDT(FWDTEN_OFF & WINDIS_OFF & PLLKEN_ON & WDTPRE_PRI128 & PDTPOST_PS32768);
_FWDT(FWDTEN_OFF);
_FICD(ICS_PGD3);
_FPOR(ALTI2C1_ON & ALTI2C2_ON);
//_FWDT(FWDTEN_OFF & WINDIS_OFF & PLLKEN_ON);

#endif // 0

_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT & OSCIOFNC_OFF & IOL1WAY_OFF);
_FWDT(FWDTEN_OFF);
_FPOR(FPWRT_PWR128 & BOREN_ON & ALTI2C1_ON & ALTI2C2_ON);
_FICD(ICS_PGD3 & RSTPRI_AF & JTAGEN_OFF);


// This method assigns all PPS registers
void configurePPS(void)
{
	// Unlock Registers
	__builtin_write_OSCCONL(OSCCON & ~(1 << 6));

	// SPI2: SCK2, SDI2, SDO2 are dedicated pins
	//_RP87R = 0b001010;    // SS2 output RP87
	// LATE7 is SS2

	// SPI3 SS, SCK, SDI, SDO
	//_RP66R = 0b100001;    // SS3 output RP66
	// LATD2 is SS3
	_RP65R = 0b100000;      // SCK3 output RP65
	_SDI3R = 76;            // SDI3 input RPI76
	_RP67R = 0b011111;      // SDO3 output RP67

	// Map UART 3 to PORT 2
//	_U3RXR = 100;
//	_RP101R = 0b011011;

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

	// CNPU bits enable input pin weak pull up resistors
	CNPUBbits.CNPUB0 = 1;   // ICSP PGD
	CNPUBbits.CNPUB1 = 1;   // ICSP PGC

	// port B
	TRISBbits.TRISB0 = 1;   // ICSP PGD
	TRISBbits.TRISB1 = 1;   // ICSP PGC

	TRISBbits.TRISB2 = 0;   // LED1
	TRISBbits.TRISB3 = 0;   // LED2
	TRISBbits.TRISB4 = 0;   // LED3
	TRISBbits.TRISB5 = 0;   // LED4

	// port D
	TRISDbits.TRISD2 = 0;   // SS3

	// port E
	TRISEbits.TRISE1 = 1;   // DIG0
	TRISEbits.TRISE5 = 0;   // GPS_TX
	TRISEbits.TRISE6 = 1;   // GPS_RX
	TRISEbits.TRISE7 = 0;   // SS2  (AT45)

	// port F
	TRISFbits.TRISF2 = 1;   // U3_RX
	TRISFbits.TRISF3 = 0;   // U3_TX

	TRISFbits.TRISF4 = 1;   // U2_RX
	TRISFbits.TRISF5 = 0;   // U2_TX
}

void mcu_init(void)
{
#if (MIPS == 64)
#warning Fast OSC selected
	PLLFBD = 62;                // M  = 64
#elif (MIPS == 32)
#warning Medium OSC selected
	PLLFBD = 30;                // M  = 32
#elif (MIPS == 16)
#warning Slow OSC selected
	PLLFBD = 14;                // M  = 16
#else
#error Invalid MIPS Configuration
#endif // MIPS
	CLKDIVbits.PLLPOST = 0;     // N1 = 2
	CLKDIVbits.PLLPRE = 0;      // N2 = 2
	OSCTUN = 0;

	__builtin_write_OSCCONH(0x03);  // Initiate Clock Switch to Primary Oscillator with PLL (NOSC= 0x3)
	__builtin_write_OSCCONL(0x01);  // Start clock switching
	while (OSCCONbits.COSC != 0x3);

	// Wait for Clock switch to occur and for PLL to lock
//	while (OSCCONbits.LOCK != 1);

	// new RobD - disable all analog inputs
	ANSELA = 0x0000;
	ANSELB = 0x0000;
	ANSELC = 0x0000;
	ANSELD = 0x0000;
	ANSELE = 0x0000;
	ANSELG = 0x0000;

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

	configurePPS();
	init_dataflash();
	configureDigitalIO();
	InitSwitch();

	_LATB2 = LED_OFF; _LATB3 = LED_OFF; _LATB4 = LED_OFF; _LATB5 = LED_OFF; 
	_TRISB2 = 0; _TRISB3 = 0; _TRISB4 = 0; _TRISB5 = 0;

	T1CONbits.TON = 0;      // ensure timer is off
	T1CONbits.TGATE = 0;
	T1CONbits.TCKPS = 3;    // set prescaler
	T1CONbits.TCS = 0;      // use the crystal to drive the clock
	PR1 = 0xFFFF;           // set period register
	T1CONbits.TON = 1;      // turn on timer 1
/*
	__C30_UART = CONSOLE_UART;

	U3BRG = BAUDRATEREG;
	U3MODE = 0;
	U3MODEbits.BRGH = BRGHX;
	U3STA = 0;
	U3MODEbits.UARTEN = 1;
	U3STAbits.UTXEN = 1;
	IFS5bits.U3RXIF = 0;

//	printf("\r\nAUAV3 " __TIME__ " " __DATE__ " @ %u mips\r\n", MIPS);
 */
}
