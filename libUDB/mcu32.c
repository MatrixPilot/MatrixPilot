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


#if defined(__PIC32MX__)
#include <p32xxxx.h>
#include <plib.h>
#endif

#if (CONSOLE_UART != 0)
#include "console.h"
#include <stdio.h>
#endif // CONSOLE_UART

#if (BOARD_TYPE == AUAV4_BOARD)

// set the configuration fuses for the processor
#if defined(__PIC32MX__)
//	#pragma config FPBDIV = DIV_2
//	#pragma config POSCMOD = XT

    #pragma config UPLLEN   = ON        // USB PLL Enabled
//    #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
    #pragma config FPLLMUL  = MUL_20        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
//    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FPBDIV   = DIV_4         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
    #pragma config DEBUG    = ON            // Background Debugger Enable
#endif // __PIC32MX__

#else
#endif // BOARD_TYPE


int16_t defaultCorcon = 0;

volatile int16_t trap_flags __attribute__ ((persistent, near));
volatile int32_t trap_source __attribute__ ((persistent, near));
volatile int16_t osc_fail_count __attribute__ ((persistent, near));
volatile int16_t stack_ptr __attribute__ ((persistent, near));


uint16_t get_reset_flags(void)
{
	return RCON;
}

// This method configures TRISx for the digital IOs
#if (BOARD_TYPE == AUAV3)
void configureDigitalIO(void)	// AUAV3 board
{
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
//	yellow led TRISEbits.TRISE0 = 1;   // O2
//	red led    TRISEbits.TRISE1 = 1;   // DIG0
//	white led  TRISEbits.TRISE2 = 0;   // SD01 (MPU6000)
//	green led  TRISEbits.TRISE3 = 1;   // SDI1 (MPU6000)
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

int print_memmap(void)
{
    printf("BMXPUPBA 0x%x\r\n", BMXPUPBA);
    printf("BMXDKPBA 0x%x\r\n", BMXDKPBA);
    printf("BMXDUPBA 0x%x\r\n", BMXDUPBA);
    printf("RAM memory size BMXDRMSZ 0x%x (%d KB)\r\n", BMXDRMSZ, (BMXDRMSZ>>10));
    printf("FLASH memory size BMXPFMSZ 0x%x (%d KB)\r\n", BMXPFMSZ,(BMXPFMSZ>>10)); 
    printf("BOOT memory size BMXBOOTSZ 0x%x (%d KB)\r\n", BMXBOOTSZ,(BMXBOOTSZ>>10));
//    DBPUTS("Program terminated. Click HALT and then RESET to stop the microcontroller. \r\n");
    return 0;
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
		stack_ptr = 0;
	}

#if defined(__PIC32MX__)
	{
		int  value;

		value = SYSTEMConfigWaitStatesAndPB( GetSystemClock() );

		// Enable the cache for the best performance
		CheKseg0CacheOn();

		INTEnableSystemMultiVectoredInt();

		value = OSCCON;
		while (!(value & 0x00000020))
		{
			value = OSCCON;    // Wait for PLL lock to stabilize
		}
	}
#else
	// Enable multi-vectored interrupts
	INTEnableSystemMultiVectoredInt();

	// Enable optimal performance
	SYSTEMConfigPerformance(GetSystemClock());
	mOSCSetPBDIV(OSC_PB_DIV_2);	// match the PBus to the fuse setting
//	mOSCSetPBDIV(OSC_PB_DIV_1);				// Use 1:1 CPU Core:Peripheral clocks
#endif

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD)
	PLLFBDbits.PLLDIV = 30; // FOSC = 32 MHz (XT = 8.00MHz, N1=2, N2=4, M = 32)
#endif

#if (BOARD_TYPE == AUAV3_BOARD)
#if (MIPS == 64)
#warning Fast OSC selected
	// Configure the device PLL to obtain 64 MIPS operation. The crystal
	// frequency is 8MHz. Divide 8MHz by 2, multiply by 64 and divide by
	// 2. This results in Fosc of 128MHz. The CPU clock frequency is
	// Fcy = Fosc/2 = 64MHz. Wait for the Primary PLL to lock and then
	// configure the auxilliary PLL to provide 48MHz needed for USB 
	// Operation.
	PLLFBD = 62;                // M  = 64
#elif (MIPS == 32)
#warning Medium OSC selected
	// Configure the device PLL to obtain 32 MIPS operation. The crystal
	// frequency is 8MHz. Divide 8MHz by 2, multiply by 32 and divide by
	// 2. This results in Fosc of 64MHz. The CPU clock frequency is
	// Fcy = Fosc/2 = 32MHz. Wait for the Primary PLL to lock and then
	// configure the auxilliary PLL to provide 48MHz needed for USB 
	// Operation.
	PLLFBD = 30;                // M  = 32
#elif (MIPS == 16)
#warning Slow OSC selected
	// Configure the device PLL to obtain 16 MIPS operation. The crystal
	// frequency is 8MHz. Divide 8MHz by 2, multiply by 64 and divide by
	// 2. This results in Fosc of 32MHz. The CPU clock frequency is
	// Fcy = Fosc/2 = 16MHz. Wait for the Primary PLL to lock and then
	// configure the auxilliary PLL to provide 48MHz needed for USB 
	// Operation.
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
	while (OSCCONbits.COSC != 0x3);

	// new RobD
	ANSELA = 0x0000;
	ANSELB = 0x0000;
	ANSELC = 0x0000;
	ANSELD = 0x0000;
	ANSELE = 0x0000;
	ANSELG = 0x0000;

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


void ReadSector(uint16_t sector, uint8_t* buffer) {};
void WriteSector(uint16_t sector, uint8_t* buffer) {};

void init_dataflash(void) {};


//void CloseSPI1(void) {}
//void ConfigIntSPI1(void) {}
//void CloseSPI2(void) {}
//void ConfigIntSPI2(void) {}

void __delay32(unsigned long cycles) {}


#if defined(__PIC32MX__)
#else
void __delay_us(void) {}
void __delay_ms(void) {}
#endif
