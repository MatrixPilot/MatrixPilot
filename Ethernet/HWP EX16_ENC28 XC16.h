 /*********************************************************************
 *
 *	Hardware specific definitions for:
 *    - Explorer 16
 *    - PIC24F, PIC24H, dsPIC33F
 *    - Ethernet PICtail Plus (ENC28J60)
 *
 *********************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    Compiler.h
 * Processor:       PIC24F, PIC24H, dsPIC30F, dsPIC33F
 * Compiler:        Microchip C30 v3.24 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               Date		Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		09/16/2010	Regenerated for specific boards
 ********************************************************************/
#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

#include "Compiler.h"

// Define a macro describing this hardware set up (used in other files)
#define EXPLORER_16

// Set configuration fuses (but only in MainDemo.c where THIS_IS_STACK_APPLICATION is defined)
#if defined(THIS_IS_STACK_APPLICATION)
	#if defined(__PIC24FJ256GB210__)
		// PIC24FJ256GB210 PIM
		_CONFIG3(ALTPMP_ALPMPDIS & SOSCSEL_EC); 										// PMP in default location, disable Timer1 oscillator so that RC13 can be used as a GPIO
		_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT & IOL1WAY_OFF & PLL96MHZ_ON & PLLDIV_DIV2);	// Primary XT OSC with 96MHz PLL (8MHz crystal input), IOLOCK can be set and cleared
		_CONFIG1(FWDTEN_OFF & ICS_PGx2 & JTAGEN_OFF & ALTVREF_ALTVREDIS);				// Watchdog timer off, ICD debugging on PGEC2/PGED2 pins, JTAG off, AVREF and CVREF in default locations
	#elif defined(__PIC24FJ256GB110__)
		// PIC24FJ256GB110 PIM
		_CONFIG2(PLLDIV_DIV2 & PLL_96MHZ_ON & FNOSC_PRIPLL & IOL1WAY_OFF & POSCMOD_XT); // Primary XT OSC with 96MHz PLL (8MHz crystal input), IOLOCK can be set and cleared
		_CONFIG1(JTAGEN_OFF & ICS_PGx2 & FWDTEN_OFF);									// Watchdog timer off, ICD debugging on PGEC2/PGED2 pins, JTAG off
	#elif defined(__PIC24FJ256GA110__)
		// PIC24FJ256GA110 PIM
		_CONFIG2(FNOSC_PRIPLL & IOL1WAY_OFF & POSCMOD_XT);	// Primary XT OSC with PLL, IOLOCK can be set and cleared
		_CONFIG1(JTAGEN_OFF & ICS_PGx2 & FWDTEN_OFF);		// Watchdog timer off, ICD debugging on PGEC2/PGED2 pins, JTAG off
	#elif defined(__PIC24F__)
		// All other PIC24F PIMs
		_CONFIG2(FNOSC_PRIPLL & POSCMOD_XT)		// Primary XT OSC with 4x PLL
		_CONFIG1(JTAGEN_OFF & FWDTEN_OFF)		// JTAG off, watchdog timer off
	#elif defined(__dsPIC33F__) || defined(__PIC24H__) || defined(__dsPIC33E__)|| defined(__PIC24E__)
		// All dsPIC33F and PIC24H PIMs
		_FOSCSEL(FNOSC_PRIPLL)			// PLL enabled
		_FOSC(OSCIOFNC_OFF & POSCMD_XT)	// XT Osc
		_FWDT(FWDTEN_OFF)				// Disable Watchdog timer
		// JTAG should be disabled as well
	#endif
#endif


// Clock frequency values
// Create a PIC dependant macro for the maximum supported internal clock
#if defined(__PIC24F__) || defined(__PIC24FK__)
	#define MAXIMUM_PIC_FREQ		(32000000ul)
#else	// dsPIC33F, PIC24H
	#define MAXIMUM_PIC_FREQ		(80000000ul)
#endif

// These directly influence timed events using the Tick module.  They also are used for UART and SPI baud rate generation.
#define GetSystemClock()		(MAXIMUM_PIC_FREQ)			// Hz
#define GetInstructionClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Might need changing if using Doze modes.
#define GetPeripheralClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Divisor may be different if using a PIC32 since it's configurable.


// Hardware I/O pin mappings

// LEDs
#define LED0_TRIS			(TRISAbits.TRISA0)	// Ref D3
#define LED0_IO				(LATAbits.LATA0)
#define LED1_TRIS			(TRISAbits.TRISA1)	// Ref D4
#define LED1_IO				(LATAbits.LATA1)
#define LED2_TRIS			(TRISAbits.TRISA2)	// Ref D5
#define LED2_IO				(LATAbits.LATA2)
#define LED3_TRIS			(TRISAbits.TRISA3)	// Ref D6
#define LED3_IO				(LATAbits.LATA3)
#define LED4_TRIS			(TRISAbits.TRISA4)	// Ref D7
#define LED4_IO				(LATAbits.LATA4)
#define LED5_TRIS			(TRISAbits.TRISA5)	// Ref D8
#define LED5_IO				(LATAbits.LATA5)
#define LED6_TRIS			(TRISAbits.TRISA6)	// Ref D9
#define LED6_IO				(LATAbits.LATA6)
#define LED7_TRIS			(LATAbits.LATA7)	// Ref D10;  Note: This is multiplexed with BUTTON1, so this LED can't be used.  However, it will glow very dimmly due to a weak pull up resistor.
#define LED7_IO				(LATAbits.LATA7)
#define LED_GET()			(*((volatile unsigned char*)(&LATA)))
#define LED_PUT(a)			(*((volatile unsigned char*)(&LATA)) = (a))

// Momentary push buttons
#define BUTTON0_TRIS		(TRISDbits.TRISD13)	// Ref S4
#define	BUTTON0_IO			(PORTDbits.RD13)
#define BUTTON1_TRIS		(TRISAbits.TRISA7)	// Ref S5;  Note: This is multiplexed with LED7
#define	BUTTON1_IO			(PORTAbits.RA7)
#define BUTTON2_TRIS		(TRISDbits.TRISD7)	// Ref S6
#define	BUTTON2_IO			(PORTDbits.RD7)
#define BUTTON3_TRIS		(TRISDbits.TRISD6)	// Ref S3
#define	BUTTON3_IO			(PORTDbits.RD6)

#define UARTTX_TRIS			(TRISFbits.TRISF5)
#define UARTTX_IO			(PORTFbits.RF5)
#define UARTRX_TRIS			(TRISFbits.TRISF4)
#define UARTRX_IO			(PORTFbits.RF4)

// ENC28J60 I/O pins
#if defined(__PIC24FJ256GA110__) || defined(__dsPIC33E__)|| defined(__PIC24E__)	// PIC24FJ256GA110 must place the ENC28J60 on SPI2 because PIC rev A3 SCK1 output pin is a PPS input only (fixed on A5, but demos use SPI2 for simplicity)
	#define ENC_CS_TRIS			(TRISFbits.TRISF12)	// Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
	#define ENC_CS_IO			(LATFbits.LATF12)
	// SPI SCK, SDI, SDO pins are automatically controlled by the 
	// PIC24/dsPIC SPI module 
	#define ENC_SPI_IF			(IFS2bits.SPI2IF)
	#define ENC_SSPBUF			(SPI2BUF)
	#define ENC_SPISTAT			(SPI2STAT)
	#define ENC_SPISTATbits		(SPI2STATbits)
	#define ENC_SPICON1			(SPI2CON1)
	#define ENC_SPICON1bits		(SPI2CON1bits)
	#define ENC_SPICON2			(SPI2CON2)
#else	// SPI1 for all other processors
	#define ENC_CS_TRIS			(TRISDbits.TRISD14)	// Comment this line out if you are using the ENC424J600/624J600, MRF24WB0M, or other network controller.
	#define ENC_CS_IO			(LATDbits.LATD14)
	// SPI SCK, SDI, SDO pins are automatically controlled by the 
	// PIC24/dsPIC SPI module 
	#define ENC_SPI_IF			(IFS0bits.SPI1IF)
	#define ENC_SSPBUF			(SPI1BUF)
	#define ENC_SPISTAT			(SPI1STAT)
	#define ENC_SPISTATbits		(SPI1STATbits)
	#define ENC_SPICON1			(SPI1CON1)
	#define ENC_SPICON1bits		(SPI1CON1bits)
	#define ENC_SPICON2			(SPI1CON2)
#endif


//// ENC624J600 Interface Configuration
//// Comment out ENC100_INTERFACE_MODE if you don't have an ENC624J600 or 
//// ENC424J600.  Otherwise, choose the correct setting for the interface you 
//// are using.  Legal values are:
////  - Commented out: No ENC424J600/624J600 present or used.  All other 
////                   ENC100_* macros are ignored.
////	- 0: SPI mode using CS, SCK, SI, and SO pins
////  - 1: 8-bit demultiplexed PSP Mode 1 with RD and WR pins
////  - 2: *8-bit demultiplexed PSP Mode 2 with R/Wbar and EN pins
////  - 3: *16-bit demultiplexed PSP Mode 3 with RD, WRL, and WRH pins
////  - 4: *16-bit demultiplexed PSP Mode 4 with R/Wbar, B0SEL, and B1SEL pins
////  - 5: 8-bit multiplexed PSP Mode 5 with RD and WR pins
////  - 6: *8-bit multiplexed PSP Mode 6 with R/Wbar and EN pins
////  - 9: 16-bit multiplexed PSP Mode 9 with AL, RD, WRL, and WRH pins
////  - 10: *16-bit multiplexed PSP Mode 10 with AL, R/Wbar, B0SEL, and B1SEL 
////        pins
//// *IMPORTANT NOTE: DO NOT USE PSP MODE 2, 4, 6, OR 10 ON EXPLORER 16! 
//// Attempting to do so will cause bus contention with the LCD module which 
//// shares the PMP.  Also, PSP Mode 3 is risky on the Explorer 16 since it 
//// can randomly cause bus contention with the 25LC256 EEPROM.
//#define ENC100_INTERFACE_MODE			0
//
//// If using a parallel interface, direct RAM addressing can be used (if all 
//// addresses wires are connected), or a reduced number of pins can be used 
//// for indirect addressing.  If using an SPI interface or PSP Mode 9 or 10 
//// (multiplexed 16-bit modes), which require all address lines to always be 
//// connected, then this option is ignored. Comment out or uncomment this 
//// macro to match your hardware connections.
//#define ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING
//
//// ENC424J600/624J600 parallel indirect address remapping macro function.
//// This section translates SFR and RAM addresses presented to the 
//// ReadMemory() and WriteMemory() APIs in ENCX24J600.c to the actual 
//// addresses that must be presented on the parallel interface.  This macro 
//// must be modified to match your hardware if you are using an indirect PSP 
//// addressing mode (ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING is defined) and 
//// have some of your address lines tied off to Vdd.  If you are using the 
//// SPI interface, then this section can be ignored or deleted.
//#if (ENC100_INTERFACE_MODE == 1) || (ENC100_INTERFACE_MODE == 2) || (ENC100_INTERFACE_MODE == 5) || (ENC100_INTERFACE_MODE == 6) // 8-bit PSP
//	// Several TCP/IP evaluation boards use the PMA8/U2TX pin for UART communication.
//	// This function will remap the A8 address signal used in PMP modes 1, 2, 6 and 6
//	// to PMPCS1 (PMA14), which will allow continued UART2 functionality on these boards.
//	#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		((((a)&0x0100)<<6) | ((a)&0x00FF))
//#elif (ENC100_INTERFACE_MODE == 3) || (ENC100_INTERFACE_MODE == 4) // 16-bit PSP
//	#define ENC100_TRANSLATE_TO_PIN_ADDR(a)		(a)
//#endif
//
//// Auto-crossover pins on Fast 100Mbps Ethernet PICtail/PICtail Plus.  If 
//// your circuit doesn't have such a feature, delete these two defines.
//#if defined(__dsPIC33E__)|| defined (__PIC24E__)
//	#define ENC100_MDIX_TRIS				(TRISAbits.TRISA9)
//	#define ENC100_MDIX_IO					(LATAbits.LATA9)
//#else
//	#define ENC100_MDIX_TRIS				(TRISBbits.TRISB3)
//	#define ENC100_MDIX_IO					(LATBbits.LATB3)
//#endif
//
//// ENC624J600 I/O control and status pins
//// If a pin is not required for your selected ENC100_INTERFACE_MODE 
//// interface selection (ex: WRH/B1SEL for PSP modes 1, 2, 5, and 6), then 
//// you can ignore, delete, or put anything for the pin definition.  Also, 
//// the INT and POR pins are entirely optional.  If not connected, comment 
//// them out.
//#if defined(__dsPIC33FJ256GP710__) || defined(__PIC24HJ256GP610__)
//	#define ENC100_INT_TRIS				(TRISAbits.TRISA13)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
//	#define ENC100_INT_IO				(PORTAbits.RA13)
//#else
//	#define ENC100_INT_TRIS				(TRISEbits.TRISE9)		// INT signal is optional and currently unused in the Microchip TCP/IP Stack.  Leave this pin disconnected and comment out this pin definition if you don't want it.
//	#define ENC100_INT_IO				(PORTEbits.RE9)
//#endif
//#if (ENC100_INTERFACE_MODE >= 1)	// Parallel mode
//	// PSP control signal pinout
//	#define ENC100_CS_TRIS					(TRISAbits.TRISA5)	// CS is optional in PSP mode.  If you are not sharing the parallel bus with another device, tie CS to Vdd and comment out this pin definition.
//	#define ENC100_CS_IO					(LATAbits.LATA5)
//	#define ENC100_POR_TRIS					(TRISCbits.TRISC1)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//	#define ENC100_POR_IO					(LATCbits.LATC1)
//	#define ENC100_SO_WR_B0SEL_EN_TRIS		(TRISDbits.TRISD4)
//	#define ENC100_SO_WR_B0SEL_EN_IO		(LATDbits.LATD4)
//	#define ENC100_SI_RD_RW_TRIS			(TRISDbits.TRISD5)
//	#define ENC100_SI_RD_RW_IO				(LATDbits.LATD5)
//	#define ENC100_SCK_AL_TRIS				(TRISBbits.TRISB15)
//	#define ENC100_SCK_AL_IO				(LATBbits.LATB15)
//#else
//	// SPI pinout
//	#if defined(__PIC24FJ256GA110__) || defined(__dsPIC33E__)|| defined (__PIC24E__)	// Some processors must use SPI2 slot on Explorer 16.  If you don't have one of the specified processors but want to use SPI2 for some reason, you can use these definitions.
//		#define ENC100_CS_TRIS					(TRISFbits.TRISF12)	// CS is mandatory when using the SPI interface
//		#define ENC100_CS_IO					(LATFbits.LATF12)
//		#define ENC100_POR_TRIS					(TRISFbits.TRISF13)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//		#define ENC100_POR_IO					(LATFbits.LATF13)
//	#else	// All other PIC24s, dsPICs, and PIC32s use SPI1 slot (top most closest to LCD)
//		#define ENC100_CS_TRIS					(TRISDbits.TRISD14)	// CS is mandatory when using the SPI interface
//		#define ENC100_CS_IO					(LATDbits.LATD14)
//		#define ENC100_POR_TRIS					(TRISDbits.TRISD15)	// POR signal is optional.  If your application doesn't have a power disconnect feature, comment out this pin definition.
//		#define ENC100_POR_IO					(LATDbits.LATD15)
//	#endif
//#endif
//
//// ENC624J600 Bit Bang PSP I/O macros and pin configuration for address and 
//// data.  If using the SPI interface (ENC100_INTERFACE_MODE is 0) then this 
//// section is not used and can be ignored or deleted.  If using the PIC PMP
//// hardware module (if present), then ENC100_BIT_BANG_PMP must be commented 
//// out and the remaining definitions will be ignored/can be deleted.  
//// Otherwise, if you are using a parallel interface mode, but do not have a 
//// PMP (or want to interface using different pins), define 
//// ENC100_BIT_BANG_PMP and properly configure the applicable macros.
////#define ENC100_BIT_BANG_PMP
//#if defined(ENC100_BIT_BANG_PMP)
//	#if ENC100_INTERFACE_MODE == 1 || ENC100_INTERFACE_MODE == 2	// Dumultiplexed 8-bit address/data modes
//		// SPI2 CANNOT BE ENABLED WHEN ACCESSING THE ENC624J600 FOR THESE TWO MODES AS THE PINS OVERLAP WITH ADDRESS LINES.
//		#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A8 connected (A9-A14 tied to Vdd)
//			#if defined(__PIC24FJ256GB210__)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0xF9E7; ANSB &= 0x3FFF; ANSG &= 0xFCFF;} while(0)		// RE0-RE7, RF12, RD11, RD4, RD5 (AD0-AD7, A5, A8, WR, RD) pins are already digital only pins.
//			#else
//				#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile unsigned char*)&AD1PCFGH)[1] = 0xFF; ((volatile unsigned char*)&AD1PCFGL)[1] |= 0xC0;}while(0)	// Disable AN24-AN31 and AN14-AN15 analog inputs on RE0-RE7 and RB14-RB15 pins (ENCX24J600 AD0-AD7, A1-A0)
//			#endif
//			#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x3FFF; TRISFbits.TRISF12 = 0; TRISGbits.TRISG9 = 0; TRISDbits.TRISD11 = 0;}while(0)
//			#define ENC100_SET_ADDR_IO(a)		do{unsigned short _SetMacro = (a); LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATGbits.LATG9 = 0; LATA &= 0xF9E7; LATFbits.LATF12 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATAbits.LATA4 = 1; if(_SetMacro & 0x0010) LATAbits.LATA3 = 1; if(_SetMacro & 0x0020) LATFbits.LATF12 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//			#define ENC100_SET_AD_TRIS_IN()		(((volatile unsigned char*)&TRISE)[0] = 0xFF)
//			#define ENC100_SET_AD_TRIS_OUT()	(((volatile unsigned char*)&TRISE)[0] = 0x00)
//			#define ENC100_GET_AD_IO()			(((volatile unsigned char*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))
//		#else 	// All ENC624J600 address pins A0-A14 connected
//			#if defined(__PIC24FJ256GB210__)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0xF9E7; ANSB &= 0x03FF; ANSG &= 0xFCFF;} while(0)		// RE0-RE7, RF12, RD11, RD4, RD5 (AD0-AD7, A5, A14, WR, RD) pins are already digital only pins.
//			#else
//				#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile unsigned char*)&AD1PCFGH)[1] = 0xFF; ((volatile unsigned char*)&AD1PCFGL)[1] |= 0xFC;}while(0)	// Disable AN24-AN31 and AN10-AN15 analog inputs on RE0-RE7 and RB10-RB15 pins (ENCX24J600 AD0-AD7, A1-A0, A13-A10)
//			#endif
//			#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x03FF; TRISF &= 0xEFCF; TRISGbits.TRISG9 = 0; TRISDbits.TRISD11 = 0;}while(0)
//			#define ENC100_SET_ADDR_IO(a)		do{unsigned short _SetMacro = (a); LATA &= 0xF9E7; LATB &= 0x03FF; LATF &= 0xEFCF; LATGbits.LATG9 = 0; LATDbits.LATD11 = 0; if(_SetMacro & 0x0001) LATBbits.LATB15 = 1; if(_SetMacro & 0x0002) LATBbits.LATB14 = 1; if(_SetMacro & 0x0004) LATGbits.LATG9 = 1; if(_SetMacro & 0x0008) LATAbits.LATA4 = 1; if(_SetMacro & 0x0010) LATAbits.LATA3 = 1; if(_SetMacro & 0x0020) LATFbits.LATF12 = 1; if(_SetMacro & 0x0040) LATAbits.LATA10 = 1; if(_SetMacro & 0x0080) LATAbits.LATA9 = 1; if(_SetMacro & 0x0100) LATFbits.LATF5 = 1; if(_SetMacro & 0x0200) LATFbits.LATF4 = 1; if(_SetMacro & 0x0400) LATBbits.LATB13 = 1; if(_SetMacro & 0x0800) LATBbits.LATB12 = 1; if(_SetMacro & 0x1000) LATBbits.LATB11 = 1; if(_SetMacro & 0x2000) LATBbits.LATB10 = 1; if(_SetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//			#define ENC100_SET_AD_TRIS_IN()		(((volatile unsigned char*)&TRISE)[0] = 0xFF)
//			#define ENC100_SET_AD_TRIS_OUT()	(((volatile unsigned char*)&TRISE)[0] = 0x00)
//			#define ENC100_GET_AD_IO()			(((volatile unsigned char*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))
//		#endif
//	#elif ENC100_INTERFACE_MODE == 3 || ENC100_INTERFACE_MODE == 4	// Dumultiplexed 16-bit address/data modes
//		#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENC624J600 address pins A0-A7 connected (A8-A13 tied to Vdd)
//			#if defined(__PIC24FJ256GB210__)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0x79E7; ANSB &= 0x3FFF; ANSD &= 0xCF0F; ANSG &= 0xFCFC;}while(0)
//			#else
//				#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGH = 0xFFFF; AD1PCFGL = 0xFFFF; AD2PCFGL = 0xFFFF;}while(0)
//			#endif
//			#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISBbits.TRISB15 = 0; TRISBbits.TRISB14 = 0; TRISFbits.TRISF12 = 0; TRISGbits.TRISG9 = 0;}while(0)
//			#define ENC100_SET_ADDR_IO(a)		do{unsigned short _wSetMacro = (a); LATA &= 0xF9E7; LATBbits.LATB15 = 0; LATBbits.LATB14 = 0; LATFbits.LATF12 = 0; LATGbits.LATG9 = 0; if(_wSetMacro & 0x0001) LATBbits.LATB15 = 1; if(_wSetMacro & 0x0002) LATBbits.LATB14 = 1; if(_wSetMacro & 0x0004) LATGbits.LATG9 = 1; if(_wSetMacro & 0x0008) LATAbits.LATA4 = 1; if(_wSetMacro & 0x0010) LATAbits.LATA3 = 1; if(_wSetMacro & 0x0020) LATFbits.LATF12 = 1; if(_wSetMacro & 0x0040) LATAbits.LATA10 = 1; if(_wSetMacro & 0x0080) LATAbits.LATA9 = 1;}while(0)
//			#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//			#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//			#define ENC100_SET_AD_TRIS_IN()		do{((volatile unsigned char*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//			#define ENC100_SET_AD_TRIS_OUT()	do{((volatile unsigned char*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//			#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//			#define ENC100_GET_AD_IOL()			(((volatile unsigned char*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		do{unsigned short _wSetMacro = (data); ((volatile unsigned char*)&LATE)[0] = ((unsigned char*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//			#define ENC100_SET_AD_IOL(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))
//		#else 	// All ENC624J600 address pins A0-A13 connected
//			#if defined(__PIC24FJ256GB210__)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSA &= 0x79E7; ANSB &= 0x03FF; ANSD &= 0xCF0F; ANSG &= 0xFCFC;}while(0)
//			#else
//				#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGH = 0xFFFF; AD1PCFGL = 0xFFFF; AD2PCFGL = 0xFFFF;}while(0)
//			#endif
//			#define ENC100_SET_ADDR_TRIS_OUT()	do{TRISA &= 0xF9E7; TRISB &= 0x03FF; TRISF &= 0xEFCF; TRISGbits.TRISG9 = 0;}while(0)
//			#define ENC100_SET_ADDR_IO(a)		do{unsigned short _wSetMacro = (a); LATA &= 0xF9E7; LATB &= 0x03FF; LATF &= 0xEFCF; LATGbits.LATG9 = 0; if(_wSetMacro & 0x0001) LATBbits.LATB15 = 1; if(_wSetMacro & 0x0002) LATBbits.LATB14 = 1; if(_wSetMacro & 0x0004) LATGbits.LATG9 = 1; if(_wSetMacro & 0x0008) LATAbits.LATA4 = 1; if(_wSetMacro & 0x0010) LATAbits.LATA3 = 1; if(_wSetMacro & 0x0020) LATFbits.LATF12 = 1; if(_wSetMacro & 0x0040) LATAbits.LATA10 = 1; if(_wSetMacro & 0x0080) LATAbits.LATA9 = 1; if(_wSetMacro & 0x0100) LATFbits.LATF5 = 1; if(_wSetMacro & 0x0200) LATFbits.LATF4 = 1; if(_wSetMacro & 0x0400) LATBbits.LATB13 = 1; if(_wSetMacro & 0x0800) LATBbits.LATB12 = 1; if(_wSetMacro & 0x1000) LATBbits.LATB11 = 1; if(_wSetMacro & 0x2000) LATBbits.LATB10 = 1;}while(0)
//			#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//			#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//			#define ENC100_SET_AD_TRIS_IN()		do{((volatile unsigned char*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//			#define ENC100_SET_AD_TRIS_OUT()	do{((volatile unsigned char*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//			#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//			#define ENC100_GET_AD_IOL()			(((volatile unsigned char*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		do{unsigned short _wSetMacro = (data); ((volatile unsigned char*)&LATE)[0] = ((unsigned char*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//			#define ENC100_SET_AD_IOL(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))
//		#endif
//	#elif ENC100_INTERFACE_MODE == 5 || ENC100_INTERFACE_MODE == 6	// Mutliplexed 8-bit address/data modes
//		#if defined(ENC100_PSP_USE_INDIRECT_RAM_ADDRESSING)	// Only ENCX24J600 address pins AD0-AD8 connected (AD9-AD14 tied to Vdd)
//			#if defined(__PIC24FJ256GB210__)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSB &= 0x7FFF; ANSG &= 0xFEFF;} while(0)		// RE0-RE7, RD11, RD4, RD5 (AD0-AD7, AD8, WR, RD) pins are already digital only pins.  RB15, RG8 (AL, CS) needs to be made digital only.
//			#else
//				#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile unsigned char*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN24-AN31 analog inputs on RE0-RE7 pins (ENCX24J600 AD0-AD7)
//			#endif
//			#define ENC100_SET_AD_TRIS_IN()		do{((volatile unsigned char*)&TRISE)[0] = 0xFF;}while(0)
//			#define ENC100_SET_AD_TRIS_OUT()	do{((volatile unsigned char*)&TRISE)[0] = 0x00; TRISDbits.TRISD11 = 0;}while(0)
//			#define ENC100_GET_AD_IO()			(((volatile unsigned char*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		do{unsigned short _wSetMacro = (data); ((volatile unsigned char*)&LATE)[0] = (unsigned char)_wSetMacro; LATDbits.LATD11 = 0; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//			#define ENC100_SET_AD_IOL(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))
//		#else 	// All ENCX24J600 address pins AD0-AD14 connected
//			// This pinout is bad for doing 8-bit bit-bang operations with all address lines.  The Fast 100Mbps Ethernet PICtail Plus hardware is wired for PMP hardware support, which requires this pinout.  However, if you are designing a custom board, you can simplify these read/write operations dramatically if you wire things more logically by putting all 15 I/O pins, in order, on PORTB or PORTD.  Such a change would enhance performance.
//			// UART2 CANNOT BE USED OR ENABLED FOR THESE TWO MODES AS THE PINS OVERLAP WITH ADDRESS LINES.
//			#if defined(__PIC24FJ256GB210__)
//				#define ENC100_INIT_PSP_BIT_BANG()	do{ANSB &= 0x43FF; ANSG &= 0xFEFF;} while(0) // Set pins as digital I/Os (not analog).  RD11, RD5, RD4, RE0-RE7, RF4, RF5 are all digital-only pins and therefore no writes to ANSD, ANSE, or ANSF are needed.
//			#else
//				#define ENC100_INIT_PSP_BIT_BANG()	do{AD1PCFGL |= 0x3C00; ((volatile unsigned char*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN10-AN13 and AN24-AN31 analog inputs on RB10-RB13 and RE0-RE7 pins (ENCX24J600 AD13-AD10 and AD0-AD7)
//			#endif
//			#define ENC100_SET_AD_TRIS_IN()		do{((volatile unsigned char*)&TRISE)[0] = 0xFF;}while(0)
//			#define ENC100_SET_AD_TRIS_OUT()	do{((volatile unsigned char*)&TRISE)[0] = 0x00; TRISFbits.TRISF5 = 0; TRISFbits.TRISF4 = 0; TRISB &= 0x43FF; TRISDbits.TRISD11 = 0;}while(0)
//			#define ENC100_GET_AD_IO()			(((volatile unsigned char*)&PORTE)[0])
//			#define ENC100_SET_AD_IO(data)		do{unsigned short _wSetMacro = (data); ((volatile unsigned char*)&LATE)[0] = (unsigned char)_wSetMacro; LATFbits.LATF5 = 0; LATFbits.LATF4 = 0; LATB &= 0x43FF; LATDbits.LATD11 = 0; if(_wSetMacro & 0x0100) LATFbits.LATF5 = 1; if(_wSetMacro & 0x0200) LATFbits.LATF4 = 1; if(_wSetMacro & 0x0400) LATBbits.LATB13 = 1; if(_wSetMacro & 0x0800) LATBbits.LATB12 = 1; if(_wSetMacro & 0x1000) LATBbits.LATB11 = 1;  if(_wSetMacro & 0x2000) LATBbits.LATB10 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD11 = 1;}while(0)
//			#define ENC100_SET_AD_IOL(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))
//		#endif
//	#elif ENC100_INTERFACE_MODE == 9 || ENC100_INTERFACE_MODE == 10	// Mutliplexed 16-bit address/data modes
//		// All ENC624J600 adddress/data pins AD0-AD15 connected (required for 16-bit data, so there is no differentiation for indirect versus direct addressing mode)
//		// This pinout is awful for doing 16-bit bit-bang operations.  The Fast 100Mbps Ethernet PICtail Plus hardware is wired for PMP hardware support, which requires this pinout.  However, if you are designing a custom board, you can simplify these read/write operations dramatically if you wire things more logically by putting all 16 I/O pins, in order, on PORTB or PORTD.  Such a change would enhance performance.
//		#if defined(__PIC24FJ256GB210__)
//			#define ENC100_INIT_PSP_BIT_BANG()	do{ANSBbits.ANSB15 = 0; ANSCbits.ANSC13 = 0; ANSD &= 0xCF0F; ANSGbits.ANSG8 = 0;}while(0)	// Set pins as digital I/Os (not analog).  RA15 and RE0-RE7 are all digital-only pins and therefore no writes to ANSA or ANSE are needed.
//		#else
//			#define ENC100_INIT_PSP_BIT_BANG()	do{((volatile unsigned char*)&AD1PCFGH)[1] = 0xFF;}while(0)	// Disable AN24-AN31 analog inputs on RE0-RE7 pins (ENCX24J600 AD0-AD7)
//		#endif
//		#define ENC100_WRH_B1SEL_TRIS		ENC100_SO_WR_B0SEL_EN_TRIS
//		#define ENC100_WRH_B1SEL_IO			ENC100_SO_WR_B0SEL_EN_IO
//		#define ENC100_SET_AD_TRIS_IN()		do{((volatile unsigned char*)&TRISE)[0] = 0xFF; TRISAbits.TRISA15 = 1; TRISCbits.TRISC13 = 1; TRISD |= 0x30C0; TRISGbits.TRISG0 = 1; TRISGbits.TRISG1 = 1;}while(0)
//		#define ENC100_SET_AD_TRIS_OUT()	do{((volatile unsigned char*)&TRISE)[0] = 0x00; TRISAbits.TRISA15 = 0; TRISCbits.TRISC13 = 0; TRISD &= 0xCF3F; TRISGbits.TRISG0 = 0; TRISGbits.TRISG1 = 0;}while(0)
//		#define ENC100_GET_AD_IOH()			(PORTGbits.RG0 | (PORTGbits.RG1<<1) | (PORTCbits.RC13<<2) | (PORTAbits.RA15<<3) | (PORTDbits.RD12<<4) | (PORTDbits.RD13<<5) | (PORTDbits.RD6<<6) | (PORTDbits.RD7<<7))
//		#define ENC100_GET_AD_IOL()			(((volatile unsigned char*)&PORTE)[0])
//		#define ENC100_SET_AD_IO(data)		do{unsigned short _wSetMacro = (data); ((volatile unsigned char*)&LATE)[0] = ((unsigned char*)&_wSetMacro)[0]; LATG &= 0xFFFC; LATCbits.LATC13 = 0; LATAbits.LATA15 = 0; LATD &= 0xCF3F; if(_wSetMacro & 0x0100) LATGbits.LATG0 = 1; if(_wSetMacro & 0x0200) LATGbits.LATG1 = 1; if(_wSetMacro & 0x0400) LATCbits.LATC13 = 1; if(_wSetMacro & 0x0800) LATAbits.LATA15 = 1; if(_wSetMacro & 0x1000) LATDbits.LATD12 = 1; if(_wSetMacro & 0x2000) LATDbits.LATD13 = 1; if(_wSetMacro & 0x4000) LATDbits.LATD6 = 1; if(_wSetMacro & 0x8000) LATDbits.LATD7 = 1;}while(0)
//		#define ENC100_SET_AD_IOL(data)		(((volatile unsigned char*)&LATE)[0] = (unsigned char)(data))
//	#endif
//#endif
//
//// ENC624J600 SPI SFR register selection (controls which SPI peripheral to 
//// use on PICs with multiple SPI peripherals).  If a parallel interface is 
//// used (ENC100_INTERFACE_MODE is >= 1), then the SPI is not used and this 
//// section can be ignored or deleted.
//#if defined(__PIC24FJ256GA110__)	// The PIC24FJ256GA110 must use SPI2 slot on Explorer 16.  If you don't have a PIC24FJ256GA110 but want to use SPI2 for some reason, you can use these definitions.
//	#define ENC100_ISR_ENABLE		(IEC3bits.INT4IE)
//	#define ENC100_ISR_FLAG			(IFS3bits.INT4IF)
//	#define ENC100_ISR_POLARITY		(INTCON2bits.INT4EP)
//	#define ENC100_ISR_PRIORITY		(IPC13bits.INT4IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(IFS1bits.SPI2IF)
//	#define ENC100_SSPBUF			(SPI2BUF)
//	#define ENC100_SPISTAT			(SPI2STAT)
//	#define ENC100_SPISTATbits		(SPI2STATbits)
//	#define ENC100_SPICON1			(SPI2CON1)
//	#define ENC100_SPICON1bits		(SPI2CON1bits)
//	#define ENC100_SPICON2			(SPI2CON2)
//#elif defined (__dsPIC33E__) || defined (__PIC24E__)	
//	#define ENC100_ISR_ENABLE		(IEC1bits.INT2IE)
//	#define ENC100_ISR_FLAG			(IFS1bits.INT2IF)
//	#define ENC100_ISR_POLARITY		(INTCON2bits.INT2EP)
//	#define ENC100_ISR_PRIORITY		(IPC7bits.INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(IFS0bits.SPI2IF)
//	#define ENC100_SSPBUF			(SPI2BUF)
//	#define ENC100_SPISTAT			(SPI2STAT)
//	#define ENC100_SPISTATbits		(SPI2STATbits)
//	#define ENC100_SPICON1			(SPI2CON1)
//	#define ENC100_SPICON1bits		(SPI2CON1bits)
//	#define ENC100_SPICON2			(SPI2CON2)
//#else	// All other PIC24s and dsPICs use SPI1 slot (top most closest to LCD)
//	#define ENC100_ISR_ENABLE		(IEC1bits.INT2IE)
//	#define ENC100_ISR_FLAG			(IFS1bits.INT2IF)
//	#define ENC100_ISR_POLARITY		(INTCON2bits.INT2EP)
//	#define ENC100_ISR_PRIORITY		(IPC7bits.INT2IP)
//	#define ENC100_SPI_ENABLE		(ENC100_SPISTATbits.SPIEN)
//	#define ENC100_SPI_IF			(IFS0bits.SPI1IF)
//	#define ENC100_SSPBUF			(SPI1BUF)
//	#define ENC100_SPISTAT			(SPI1STAT)
//	#define ENC100_SPISTATbits		(SPI1STATbits)
//	#define ENC100_SPICON1			(SPI1CON1)
//	#define ENC100_SPICON1bits		(SPI1CON1bits)
//	#define ENC100_SPICON2			(SPI1CON2)
//#endif


// 25LC256 I/O pins
#if defined(__PIC24FJ256GB110__)
	// PIC24FJ256GB110 USB PIM has RD12 pin on Explorer 16 schematic 
	// remapped and actually connected to PIC24FJ256GB110 pin 90 (RG0).  
	#define EEPROM_CS_TRIS		(TRISGbits.TRISG0)
	#define EEPROM_CS_IO		(LATGbits.LATG0)
#elif defined(__PIC24FJ256GB210__)
	// PIC24FJ256GB210 USB PIM has RD12 pin on Explorer 16 schematic 
	// remapped and actually connected to PIC24FJ256GB210 pin 90 (RG0) when 
	// JP1 on PIM has pins 1-2 shorted (USB).  When JP1 pins 2-3 are shorted 
	// (PMP), PIC pin 90 does connect to RD12.  To make the PIM work with 
	// either jumper setting, we will drive both RG0 and RD12 simultaneously
	// as chip select to the same states.  For an actual application, you'd 
	// want to specify only the single necessary pin as this double 
	// assignment operation generates inefficient code by the C compiler.
	#define EEPROM_CS_TRIS		TRISGbits.TRISG0 = TRISDbits.TRISD12
	#define EEPROM_CS_IO		LATGbits.LATG0 = LATDbits.LATD12
#else
	#define EEPROM_CS_TRIS		(TRISDbits.TRISD12)
	#define EEPROM_CS_IO		(LATDbits.LATD12)
#endif
#define EEPROM_SCK_TRIS		(TRISGbits.TRISG6)
#define EEPROM_SDI_TRIS		(TRISGbits.TRISG7)
#define EEPROM_SDO_TRIS		(TRISGbits.TRISG8)
#define EEPROM_SPI_IF		(IFS2bits.SPI2IF)
#define EEPROM_SSPBUF		(SPI2BUF)
#define EEPROM_SPICON1		(SPI2CON1)
#define EEPROM_SPICON1bits	(SPI2CON1bits)
#define EEPROM_SPICON2		(SPI2CON2)
#define EEPROM_SPISTAT		(SPI2STAT)
#define EEPROM_SPISTATbits	(SPI2STATbits)

// LCD Module I/O pins.  NOTE: On the Explorer 16, the LCD is wired to the 
// same PMP lines required to communicate with an ENCX24J600 in parallel 
// mode.  Since the LCD does not have a chip select wire, if you are using 
// the ENC424J600/624J600 in parallel mode, the LCD cannot be used.
#if !defined(ENC100_INTERFACE_MODE) || (ENC100_INTERFACE_MODE == 0)	// SPI only
	#define LCD_DATA_TRIS		(*((volatile unsigned char*)&TRISE))
	#define LCD_DATA_IO			(*((volatile unsigned char*)&LATE))
	#define LCD_RD_WR_TRIS		(TRISDbits.TRISD5)
	#define LCD_RD_WR_IO		(LATDbits.LATD5)
	#define LCD_RS_TRIS			(TRISBbits.TRISB15)
	#define LCD_RS_IO			(LATBbits.LATB15)
	#define LCD_E_TRIS			(TRISDbits.TRISD4)
	#define LCD_E_IO			(LATDbits.LATD4)
#endif


//// Serial Flash/SRAM/UART PICtail Plus attached to SPI2 (middle pin group)
//// This daughter card is not in production, but if you custom attach an SPI 
//// RAM or SPI Flash chip to your board, then use these definitions as a 
//// starting point.
//#define SPIRAM_CS_TRIS			(TRISGbits.TRISG9)
//#define SPIRAM_CS_IO			(LATGbits.LATG9)
//#define SPIRAM_SCK_TRIS			(TRISGbits.TRISG6)
//#define SPIRAM_SDI_TRIS			(TRISGbits.TRISG7)
//#define SPIRAM_SDO_TRIS			(TRISGbits.TRISG8)
//#define SPIRAM_SPI_IF			(IFS2bits.SPI2IF)
//#define SPIRAM_SSPBUF			(SPI2BUF)
//#define SPIRAM_SPICON1			(SPI2CON1)
//#define SPIRAM_SPICON1bits		(SPI2CON1bits)
//#define SPIRAM_SPICON2			(SPI2CON2)
//#define SPIRAM_SPISTAT			(SPI2STAT)
//#define SPIRAM_SPISTATbits		(SPI2STATbits)
//
//#define SPIFLASH_CS_TRIS		(TRISBbits.TRISB8)
//#define SPIFLASH_CS_IO			(LATBbits.LATB8)
//#define SPIFLASH_SCK_TRIS		(TRISGbits.TRISG6)
//#define SPIFLASH_SDI_TRIS		(TRISGbits.TRISG7)
//#define SPIFLASH_SDI_IO			(PORTGbits.RG7)
//#define SPIFLASH_SDO_TRIS		(TRISGbits.TRISG8)
//#define SPIFLASH_SPI_IF			(IFS2bits.SPI2IF)
//#define SPIFLASH_SSPBUF			(SPI2BUF)
//#define SPIFLASH_SPICON1		(SPI2CON1)
//#define SPIFLASH_SPICON1bits	(SPI2CON1bits)
//#define SPIFLASH_SPICON2		(SPI2CON2)
//#define SPIFLASH_SPISTAT		(SPI2STAT)
//#define SPIFLASH_SPISTATbits	(SPI2STATbits)

////----------------------------
//// MRF24WB0M WiFi I/O pins
////----------------------------
//// If you have a MRF24WB0M WiFi PICtail, you must uncomment one of 
//// these two lines to use it.  SPI1 is the top-most slot in the Explorer 16 
//// (closer to the LCD and prototyping area) while SPI2 corresponds to 
//// insertion of the PICtail into the middle of the side edge connector slot.
//#define MRF24WB0M_IN_SPI1
////#define MRF24WB0M_IN_SPI2
//
//// PIC24FJ256GA110 PIM on Explorer 16 must use SPI2, not SPI1
//#if defined(MRF24WB0M_IN_SPI1) && defined(__PIC24FJ256GA110__)
//	#undef MRF24WB0M_IN_SPI1
//	#define MRF24WB0M_IN_SPI2
//#endif
//
//// dsPIC33E/PIC24E PIM on Explorer 16 must use SPI2, not SPI1
//#if defined(MRF24WB0M_IN_SPI1) && (defined(__dsPIC33E__) || defined(__PIC24E__))
//	#undef MRF24WB0M_IN_SPI1
//	#define MRF24WB0M_IN_SPI2
//#endif
//
//#if defined(MRF24WB0M_IN_SPI1) && !defined(__32MX460F512L__) && !defined(__32MX795F512L__) && !defined(__PIC24FJ256GA110__)
//	// MRF24WB0M in SPI1 slot
//	#define WF_CS_TRIS			(TRISBbits.TRISB2)
//	#define WF_CS_IO			(LATBbits.LATB2)
//	#define WF_SDI_TRIS			(TRISFbits.TRISF7)
//	#define WF_SCK_TRIS			(TRISFbits.TRISF6)
//	#define WF_SDO_TRIS			(TRISFbits.TRISF8)
//	#define WF_RESET_TRIS		(TRISFbits.TRISF0)
//	#define WF_RESET_IO			(LATFbits.LATF0)
//	#if defined(__dsPIC33FJ256GP710__) || defined(__PIC24HJ256GP610__) || defined (__dsPIC33E__)||defined(__PIC24E__)
//		#define WF_INT_TRIS	    (TRISAbits.TRISA12)
//		#define WF_INT_IO		(PORTAbits.RA12)
//	#else
//		#define WF_INT_TRIS	    (TRISEbits.TRISE8)  // INT1
//		#define WF_INT_IO		(PORTEbits.RE8)
//	#endif
//	#define WF_HIBERNATE_TRIS	(TRISFbits.TRISF1)
//	#define	WF_HIBERNATE_IO		(PORTFbits.RF1)
//	#define WF_INT_EDGE			(INTCON2bits.INT1EP)
//	#define WF_INT_IE			(IEC1bits.INT1IE)
//	#define WF_INT_IF			(IFS1bits.INT1IF)
//
//	#define WF_SSPBUF			(SPI1BUF)
//	#define WF_SPISTAT			(SPI1STAT)
//	#define WF_SPISTATbits		(SPI1STATbits)
//	#define WF_SPICON1			(SPI1CON1)
//	#define WF_SPICON1bits		(SPI1CON1bits)
//	#define WF_SPICON2			(SPI1CON2)
//	#define WF_SPI_IE			(IEC0bits.SPI1IE)
////	#define WF_SPI_IP			(IPC2bits.SPI1IP)
//	#define WF_SPI_IF			(IFS0bits.SPI1IF)
//
//#elif defined(MRF24WB0M_IN_SPI2) && !defined(__32MX460F512L__) && !defined(__32MX795F512L__)
//	// MRF24WB0M in SPI2 slot
//	#define WF_CS_TRIS			(TRISGbits.TRISG9)
//	#define WF_CS_IO			(LATGbits.LATG9)
//	#define WF_SDI_TRIS			(TRISGbits.TRISG7)
//	#define WF_SCK_TRIS			(TRISGbits.TRISG6)
//	#define WF_SDO_TRIS			(TRISGbits.TRISG8)
//	#define WF_RESET_TRIS		(TRISGbits.TRISG0)
//	#define WF_RESET_IO			(LATGbits.LATG0)
//	#if defined(__PIC24FJ256GB110__) || defined(__PIC24FJ256GB210__)
//		#define WF_INT_TRIS			(TRISCbits.TRISC3)	// INT3
//		#define WF_INT_IO			(PORTCbits.RC3)
//	#else
//		#define WF_INT_TRIS			(TRISAbits.TRISA14)	// INT3
//		#define WF_INT_IO			(PORTAbits.RA14)
//	#endif
//	#define WF_HIBERNATE_TRIS		(TRISGbits.TRISG1)
//	#define	WF_HIBERNATE_IO			(PORTGbits.RG1)
//	#define WF_INT_EDGE			(INTCON2bits.INT3EP)
//	#define WF_INT_IE			(IEC3bits.INT3IE)
//	#define WF_INT_IF			(IFS3bits.INT3IF)
//
//	#define WF_SSPBUF			(SPI2BUF)
//	#define WF_SPISTAT			(SPI2STAT)
//	#define WF_SPISTATbits		(SPI2STATbits)
//	#define WF_SPICON1			(SPI2CON1)
//	#define WF_SPICON1bits		(SPI2CON1bits)
//	#define WF_SPICON2			(SPI2CON2)
//	#define WF_SPI_IE			(IEC2bits.SPI2IE)
////	#define WF_SPI_IP			(IPC8bits.SPI2IP)
//	#define WF_SPI_IF			(IFS2bits.SPI2IF)
//
//#endif


// Select which UART the STACK_USE_UART and STACK_USE_UART2TCP_BRIDGE 
// options will use.  You can change these to U1BRG, U1MODE, etc. if you 
// want to use the UART1 module instead of UART2.
#define UBRG				U2BRG
#define UMODE				U2MODE
#define USTA				U2STA
#define BusyUART()			BusyUART2()
#define CloseUART()			CloseUART2()
#define ConfigIntUART(a)	ConfigIntUART2(a)
#define DataRdyUART()		DataRdyUART2()
#define OpenUART(a,b,c)		OpenUART2(a,b,c)
#define ReadUART()			ReadUART2()
#define WriteUART(a)		WriteUART2(a)
#define getsUART(a,b,c)		getsUART2(a,b,c)
#define putsUART(a)			putsUART2((unsigned int*)a)
#define getcUART()			getcUART2()
#define putcUART(a)			do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
#define putrsUART(a)		putrsUART2(a)


#endif // #ifndef HARDWARE_PROFILE_H
