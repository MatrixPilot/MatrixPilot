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

#include "options.h"
#include "p33fj256gp710a.h"
#include "../libUDB/libUDB_defines.h" // needed for FREQOSC
#include "Compiler.h"


// Clock frequency values
// These directly influence timed events using the Tick module.  They also are used for UART and SPI baud rate generation.
#define GetSystemClock()		(FREQOSC)			// Hz
#define GetInstructionClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Might need changing if using Doze modes.
#define GetPeripheralClock()	(GetSystemClock()/2)	// Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Divisor may be different if using a PIC32 since it's configurable.


// Hardware I/O pin mappings
// These are just dummy values to keep the compiler quiet for stock modules that use them like Telnet
#define BUTTON0_IO			(0)
#define BUTTON1_IO			(0)
#define BUTTON2_IO			(0)
#define BUTTON3_IO			(0)
#define LED0_IO				(0)
#define LED1_IO				(0)
#define LED2_IO				(0)
#define LED3_IO				(0)
#define LED4_IO				(0)
#define LED5_IO				(0)
#define LED6_IO				(0)
#define LED7_IO				(0)

#define LED_TCP_CONNECTED	LED_BLUE

#if (USE_ETHERNET_NETWORK_LINK == 1)
	// ENC28J60 I/O pins
	#define ENC_CS_TRIS			(TRISBbits.TRISB15) // RB15 == AN15
	#define ENC_CS_IO			(LATBbits.LATB15)
	// SPI SCK, SDI, SDO pins are automatically controlled by the 
	// PIC24/dsPIC SPI module 
	#define ENC_SPI_IF			(IFS2bits.SPI2IF)
	#define ENC_SSPBUF			(SPI2BUF)
	#define ENC_SPISTAT			(SPI2STAT)
	#define ENC_SPISTATbits		(SPI2STATbits)
	#define ENC_SPICON1			(SPI2CON1)
	#define ENC_SPICON1bits		(SPI2CON1bits)
	#define ENC_SPICON2			(SPI2CON2)
#else
	// If both Ethernet and WiFi are wired up, lets make sure to disable Ethernet
	//#define BOTH_WIFI_AND_ETHERNET_ARE_WIRED_UP
	#define DISABLE_ENC_CS_TRIS		(TRISBbits.TRISB15)
	#define DISABLE_ENC_CS_IO		(LATBbits.LATB15)
#endif

#if (USE_WIFI_NETWORK_LINK == 1)
	//----------------------------
	// MRF24W WiFi I/O pins
	//----------------------------
	#define MRF24W_IN_SPI2
	#define MRF24WG
	
	#define WF_CS_TRIS			(TRISGbits.TRISG9)
	#define WF_CS_IO			(LATGbits.LATG9)
	#define WF_SDI_TRIS			(TRISGbits.TRISG7)
	#define WF_SCK_TRIS			(TRISGbits.TRISG6)
	#define WF_SDO_TRIS			(TRISGbits.TRISG8)
	#define WF_RESET_TRIS		(TRISCbits.TRISC1)	// A16 - must also set AD1PCFGHbits
	#define WF_RESET_IO			(LATCbits.LATC1)	
	#define WF_INT_TRIS			(TRISAbits.TRISA12)	// IN1 (_INT1Interrupt)- must also set AD1PCFGHbits
	#define WF_INT_IO			(PORTAbits.RA12)
	#define WF_HIBERNATE_TRIS	(TRISCbits.TRISC2)	// A17 - must also set AD1PCFGHbits
	#define	WF_HIBERNATE_IO		(PORTCbits.RC2)
	#define WF_INT_EDGE			(INTCON2bits.INT1EP)
	#define WF_INT_IE			(IEC1bits.INT1IE)
	#define WF_INT_IF			(IFS1bits.INT1IF)
	
	#define WF_SSPBUF			(SPI2BUF)
	#define WF_SPISTAT			(SPI2STAT)
	#define WF_SPISTATbits		(SPI2STATbits)
	#define WF_SPICON1			(SPI2CON1)
	#define WF_SPICON1bits		(SPI2CON1bits)
	#define WF_SPICON2			(SPI2CON2)
	#define WF_SPI_IE			(IEC2bits.SPI2IE)
	#define WF_SPI_IF			(IFS2bits.SPI2IF)

#else
	// If both Ethernet and WiFi are wired up, lets make sure to disable WiFi
	//#define BOTH_WIFI_AND_ETHERNET_ARE_WIRED_UP
	
	#define DISABLE_WF_CS_TRIS	(TRISGbits.TRISG9)
	#define DISABLE_WF_CS_IO	(LATGbits.LATG9)
	#define DISABLE_WF_RESET_TRIS (TRISCbits.TRISC1)	// A17 - must also set AD1PCFGHbits
	#define DISABLE_WF_RESET_IO	(LATCbits.LATC1)	
#endif



#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
// Select which UART the STACK_USE_UART and STACK_USE_UART2TCP_BRIDGE 
// options will use.  You can change these to U1BRG, U1MODE, etc. if you 
// want to use the UART1 module instead of UART2.
// GPS uses UART1, logging and telemetry uses UART2
#define UARTTX_TRIS			(TRISFbits.TRISF5)
#define UARTTX_IO			(PORTFbits.RF5)
#define UARTRX_TRIS			(TRISFbits.TRISF4)
#define UARTRX_IO			(PORTFbits.RF4)

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

#else

#define MAX_UDP_SOCKETS (1) 		// dummy value to keep compiler quiet when Network interface is disabled
#define MAX_HTTP_CONNECTIONS (1)	// dummy value to keep compiler quiet when Network interface is disabled

#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))

#endif // #ifndef HARDWARE_PROFILE_H


