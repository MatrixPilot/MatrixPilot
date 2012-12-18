/*********************************************************************
 *
 *                  MAC Module Defs for Microchip Stack
 *
 *********************************************************************
 * FileName:        MAC.h
 * Dependencies:    StackTsk.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     4/27/01     Original        (Rev 1.0)
 * Nilesh Rajbharti     11/27/01    Added SLIP
 * Nilesh Rajbharti     2/9/02      Cleanup
 * Nilesh Rajbharti     5/22/02     Rev 2.0 (See version.log for detail)
 * Howard Schlunder     6/28/04     Added ENC28J60 specific features
 * Howard Schlunder		11/29/04	Added Get/SetLEDConfig macros
 ********************************************************************/
#ifndef __MAC_H
#define __MAC_H

#include "HardwareProfile.h"

#if defined(WF_CS_TRIS)
	// Do not use the DMA and other goodies that Microchip Ethernet modules have
	#define NON_MCHP_MAC
#endif

#if defined(ENC_CS_TRIS) && defined(WF_CS_TRIS)
	#error "Error in HardwareProfile.h.  Must select either the ENC28J60 or the MRF24WB10 but not both ENC_CS_TRIS and WF_CS_TRIS."
#endif
#if defined(ENC100_INTERFACE_MODE) && defined(WF_CS_TRIS)
	#error "Error in HardwareProfile.h.  Must select either the ENCX24J600 or the MRF24WB10 but not both ENC100_INTERFACE_MODE and WF_CS_TRIS."
#endif
#if defined(ENC100_INTERFACE_MODE) && defined(ENC_CS_TRIS)
	#error "Error in HardwareProfile.h.  Must select either the ENC28J60 or the ENCX24J600 but not both ENC_CS_TRIS and ENC100_INTERFACE_MODE."
#endif



#if !defined(ENC_CS_TRIS) && !defined(WF_CS_TRIS) && !defined(ENC100_INTERFACE_MODE) && \
	 (defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60) || \
	  defined(_18F97J60) ||  defined(_18F96J65) ||  defined(_18F96J60) ||  defined(_18F87J60) ||  defined(_18F86J65) ||  defined(_18F86J60) ||  defined(_18F67J60) ||  defined(_18F66J65) ||  defined(_18F66J60))
	#include "TCPIP Stack/ETH97J60.h"
#elif defined(ENC_CS_TRIS) || defined(WF_CS_TRIS)
	#include "TCPIP Stack/ENC28J60.h"
#elif defined(ENC100_INTERFACE_MODE)
	#include "TCPIP Stack/ENCX24J600.h"
	#define PHYREG WORD
#elif defined(__PIC32MX__) && defined(_ETH)
	// extra includes for PIC32MX with embedded ETH Controller
#else
	#error No Ethernet/WiFi controller defined in HardwareProfile.h.  Defines for an ENC28J60, ENC424J600/624J600, or WiFi MRF24WB10 must be present.
#endif


#define MAC_TX_BUFFER_SIZE			(1500ul)

// A generic structure representing the Ethernet header starting all Ethernet
// frames
typedef struct  __attribute__((aligned(2), packed))
{
	MAC_ADDR        DestMACAddr;
	MAC_ADDR        SourceMACAddr;
	WORD_VAL        Type;
} ETHER_HEADER;


#define MAC_IP      	(0x00u)
#define MAC_ARP     	(0x06u)
#define MAC_UNKNOWN 	(0xFFu)


#if !defined(STACK_USE_HTTP2_SERVER)
	#define RESERVED_HTTP_MEMORY 0ul
#endif

#if !defined(STACK_USE_SSL)
	#define RESERVED_SSL_MEMORY 0ul
#endif

#if defined(WF_CS_TRIS)
    #define MAX_PACKET_SIZE     (1514ul)
#endif

// MAC RAM definitions
#if defined(ENC100_INTERFACE_MODE)
	#define RESERVED_CRYPTO_MEMORY	(128ul)
	#define RAMSIZE			(24*1024ul)
	#define TXSTART 		(0x0000ul)
	#define RXSTART 		((TXSTART + 1518ul + TCP_ETH_RAM_SIZE + RESERVED_HTTP_MEMORY + RESERVED_SSL_MEMORY + RESERVED_CRYPTO_MEMORY + 1ul) & 0xFFFE)
	#define	RXSTOP			(RAMSIZE-1ul)
	#define RXSIZE			(RXSTOP-RXSTART+1ul)
	#define BASE_TX_ADDR	(TXSTART)
	#define BASE_TCB_ADDR	(BASE_TX_ADDR + 1518ul)
	#define BASE_HTTPB_ADDR (BASE_TCB_ADDR + TCP_ETH_RAM_SIZE)
	#define BASE_SSLB_ADDR	(BASE_HTTPB_ADDR + RESERVED_HTTP_MEMORY)
	#define BASE_CRYPTOB_ADDR	(BASE_SSLB_ADDR + RESERVED_SSL_MEMORY)
#elif defined(WF_CS_TRIS)
	#define RAMSIZE 		(14170ul - 8192ul - RESERVED_HTTP_MEMORY - RESERVED_SSL_MEMORY)
	#define TXSTART 		(RAMSIZE - (4ul + MAX_PACKET_SIZE + 4ul))
	#define RXSTART 		(0ul)
	#define RXSTOP			((TXSTART-2ul) | 0x0001ul)
	#define RXSIZE			(RXSTOP-RXSTART+1ul)
	#define BASE_TX_ADDR	(TXSTART + 4ul)
	#define BASE_SCRATCH_ADDR (BASE_TX_ADDR + (MAX_PACKET_SIZE + 4ul))
	#define BASE_HTTPB_ADDR  (BASE_SCRATCH_ADDR)
	#define BASE_SSLB_ADDR	(BASE_HTTPB_ADDR + RESERVED_HTTP_MEMORY)
	#define BASE_TCB_ADDR	(BASE_SSLB_ADDR + RESERVED_SSL_MEMORY)
#elif defined(__PIC32MX__) && defined(_ETH) && !defined(ENC_CS_TRIS)
	#define BASE_TX_ADDR	(MACGetTxBaseAddr())
	#define BASE_HTTPB_ADDR	(MACGetHttpBaseAddr())
	#define BASE_SSLB_ADDR	(MACGetSslBaseAddr())
	#define RXSIZE			(EMAC_RX_BUFF_SIZE)
	#define RAMSIZE			(2*RXSIZE)	// not used but silences the compiler
#else	// ENC28J60 or PIC18F97J60 family internal Ethernet controller
	#define RAMSIZE			(8*1024ul)
	#define TXSTART 		(RAMSIZE - (1ul+1518ul+7ul) - TCP_ETH_RAM_SIZE - RESERVED_HTTP_MEMORY - RESERVED_SSL_MEMORY)
	#define RXSTART			(0ul)						// Should be an even memory address; must be 0 for errata
	#define	RXSTOP			((TXSTART-2ul) | 0x0001ul)	// Odd for errata workaround
	#define RXSIZE			(RXSTOP-RXSTART+1ul)
	#define BASE_TX_ADDR	(TXSTART + 1ul)
	#define BASE_TCB_ADDR	(BASE_TX_ADDR + (1514ul+7ul))
	#define BASE_HTTPB_ADDR (BASE_TCB_ADDR + TCP_ETH_RAM_SIZE)
	#define BASE_SSLB_ADDR	(BASE_HTTPB_ADDR + RESERVED_HTTP_MEMORY)
#endif

#if (RXSIZE < 1400) || (RXSIZE > RAMSIZE)
	#error Warning, Ethernet RX buffer is tiny.  Reduce TCP socket count, the size of each TCP socket, or move sockets to a different RAM
#endif

WORD	MACCalcRxChecksum(WORD offset, WORD len);
WORD 	CalcIPBufferChecksum(WORD len);

void	MACPowerDown(void);
void	MACEDPowerDown(void);
void 	MACPowerUp(void);
#if defined(ENC_CS_TRIS) || defined(ENC100_INTERFACE_MODE) || \
	(defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60) || \
	  defined(_18F97J60) ||  defined(_18F96J65) ||  defined(_18F96J60) ||  defined(_18F87J60) ||  defined(_18F86J65) ||  defined(_18F86J60) ||  defined(_18F67J60) ||  defined(_18F66J65) ||  defined(_18F66J60))
void	WritePHYReg(BYTE Register, WORD Data);
PHYREG	ReadPHYReg(BYTE Register);
#endif
void	SetRXHashTableEntry(MAC_ADDR DestMACAddr);

// ENC28J60 specific
void	SetCLKOUT(BYTE NewConfig);
BYTE	GetCLKOUT(void);

/******************************************************************************
 * Macro:        	void SetLEDConfig(WORD NewConfig)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           NewConfig - xxx0: Pulse stretching disabled
 *								xxx2: Pulse stretch to 40ms (default)
 *								xxx6: Pulse stretch to 73ms
 *								xxxA: Pulse stretch to 139ms
 *
 *								xx1x: LEDB - TX
 *								xx2x: LEDB - RX (default)
 *								xx3x: LEDB - collisions
 *								xx4x: LEDB - link
 *								xx5x: LEDB - duplex
 *								xx7x: LEDB - TX and RX
 *								xx8x: LEDB - on
 *								xx9x: LEDB - off
 *								xxAx: LEDB - blink fast
 *								xxBx: LEDB - blink slow
 *								xxCx: LEDB - link and RX
 *								xxDx: LEDB - link and TX and RX
 *								xxEx: LEDB - duplex and collisions
 *
 *								x1xx: LEDA - TX
 *								x2xx: LEDA - RX
 *								x3xx: LEDA - collisions
 *								x4xx: LEDA - link (default)
 *								x5xx: LEDA - duplex
 *								x7xx: LEDA - TX and RX
 *								x8xx: LEDA - on
 *								x9xx: LEDA - off
 *								xAxx: LEDA - blink fast
 *								xBxx: LEDA - blink slow
 *								xCxx: LEDA - link and RX
 *								xDxx: LEDA - link and TX and RX
 *								xExx: LEDA - duplex and collisions
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Writes the value of NewConfig into the PHLCON PHY register.
 *					The LED pins will beginning outputting the new
 *					configuration immediately.
 *
 * Note:
 *****************************************************************************/
#define SetLEDConfig(NewConfig)		WritePHYReg(PHLCON, NewConfig)


/******************************************************************************
 * Macro:        	WORD GetLEDConfig(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          WORD -	xxx0: Pulse stretching disabled
 *							xxx2: Pulse stretch to 40ms (default)
 *							xxx6: Pulse stretch to 73ms
 *							xxxA: Pulse stretch to 139ms
 *
 *							xx1x: LEDB - TX
 *							xx2x: LEDB - RX (default)
 *							xx3x: LEDB - collisions
 *							xx4x: LEDB - link
 *							xx5x: LEDB - duplex
 *							xx7x: LEDB - TX and RX
 *							xx8x: LEDB - on
 *							xx9x: LEDB - off
 *							xxAx: LEDB - blink fast
 *							xxBx: LEDB - blink slow
 *							xxCx: LEDB - link and RX
 *							xxDx: LEDB - link and TX and RX
 *							xxEx: LEDB - duplex and collisions
 *
 * 							x1xx: LEDA - TX
 *							x2xx: LEDA - RX
 *							x3xx: LEDA - collisions
 *							x4xx: LEDA - link (default)
 *							x5xx: LEDA - duplex
 *							x7xx: LEDA - TX and RX
 *							x8xx: LEDA - on
 *							x9xx: LEDA - off
 *							xAxx: LEDA - blink fast
 *							xBxx: LEDA - blink slow
 *							xCxx: LEDA - link and RX
 *							xDxx: LEDA - link and TX and RX
 *							xExx: LEDA - duplex and collisions
 *
 * Side Effects:    None
 *
 * Overview:        Returns the current value of the PHLCON register.
 *
 * Note:            None
 *****************************************************************************/
#define GetLEDConfig()		ReadPHYReg(PHLCON).Val


void MACInit(void);
void MACProcess(void);
BOOL MACIsLinked(void);

BOOL MACGetHeader(MAC_ADDR *remote, BYTE* type);
void MACSetReadPtrInRx(WORD offset);
PTR_BASE MACSetWritePtr(PTR_BASE address);
PTR_BASE MACSetReadPtr(PTR_BASE address);
BYTE MACGet(void);
WORD MACGetArray(BYTE *val, WORD len);
void MACDiscardRx(void);
WORD MACGetFreeRxSize(void);
void MACMemCopyAsync(PTR_BASE destAddr, PTR_BASE sourceAddr, WORD len);
BOOL MACIsMemCopyDone(void);

void MACPutHeader(MAC_ADDR *remote, BYTE type, WORD dataLen);
BOOL MACIsTxReady(void);
void MACPut(BYTE val);
void MACPutArray(BYTE *val, WORD len);
void MACFlush(void);


// ROM function variants for PIC18
#if defined(__18CXX)
	void MACPutROMArray(ROM BYTE *val, WORD len);
#else
	#define MACPutROMArray(a,b)	MACPutArray((BYTE*)a,b)
#endif

// PIC32MX with embedded ETHC functions
#if defined(__PIC32MX__) && defined(_ETH)
	PTR_BASE MACGetTxBaseAddr(void);
	PTR_BASE MACGetHttpBaseAddr(void);
	PTR_BASE MACGetSslBaseAddr(void);
#endif

	
#endif
