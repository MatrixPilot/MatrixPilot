/*********************************************************************
 *
 *                  DHCP Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        DHCP.h
 * Dependencies:    TCPIPStack.h
 *                  UDP.h
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
 * Author               Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     3/21/01  Original        (Rev 1.0)
 ********************************************************************/
#ifndef __DHCP_H
#define __DHCP_H

// UDP client port for DHCP Client transactions
#define DHCP_CLIENT_PORT                (68u)
// UDP listening port for DHCP Server messages
#define DHCP_SERVER_PORT                (67u)

#define BOOT_REQUEST                    (1u)	// BOOT_REQUEST DHCP type
#define BOOT_REPLY                      (2u)	// BOOT_REPLY DHCP type
#define BOOT_HW_TYPE                    (1u)	// BOOT_HW_TYPE DHCP type
#define BOOT_LEN_OF_HW_TYPE             (6u)	// BOOT_LEN_OF_HW_TYPE DHCP type

#define DHCP_MESSAGE_TYPE               (53u)	// DHCP Message Type constant
#define DHCP_MESSAGE_TYPE_LEN           (1u)	// Length of DHCP Message Type

#define DHCP_UNKNOWN_MESSAGE            (0u)	// Code for unknown DHCP message

#define DHCP_DISCOVER_MESSAGE           (1u)	// DCHP Discover Message
#define DHCP_OFFER_MESSAGE              (2u)	// DHCP Offer Message
#define DHCP_REQUEST_MESSAGE            (3u)	// DHCP Request message
#define DHCP_DECLINE_MESSAGE            (4u)	// DHCP Decline Message
#define DHCP_ACK_MESSAGE                (5u)	// DHCP ACK Message
#define DHCP_NAK_MESSAGE                (6u)	// DHCP NAK Message
#define DHCP_RELEASE_MESSAGE            (7u)	// DCHP Release message

#define DHCP_SERVER_IDENTIFIER          (54u)	// DHCP Server Identifier
#define DHCP_SERVER_IDENTIFIER_LEN      (4u)	// DHCP Server Identifier length

#define DHCP_OPTION_ACK_MESSAGE			(53u)	// DHCP_OPTION_ACK_MESSAGE Type
#define DHCP_PARAM_REQUEST_LIST         (55u)	// DHCP_PARAM_REQUEST_LIST Type
#define DHCP_PARAM_REQUEST_LIST_LEN     (4u)	// DHCP_PARAM_REQUEST_LIST_LEN Type
#define DHCP_PARAM_REQUEST_IP_ADDRESS       (50u)	// DHCP_PARAM_REQUEST_IP_ADDRESS Type
#define DHCP_PARAM_REQUEST_IP_ADDRESS_LEN   (4u)	// DHCP_PARAM_REQUEST_IP_ADDRESS_LEN Type
#define DHCP_SUBNET_MASK                (1u)	// DHCP_SUBNET_MASK Type
#define DHCP_ROUTER                     (3u)	// DHCP_ROUTER Type
#define DHCP_DNS						(6u)	// DHCP_DNS Type
#define DHCP_HOST_NAME					(12u)	// DHCP_HOST_NAME Type
#define DHCP_IP_LEASE_TIME              (51u)	// DHCP_IP_LEASE_TIME Type
#define DHCP_END_OPTION                 (255u)	// DHCP_END_OPTION Type

// DHCP or BOOTP Header structure
typedef struct __attribute__((aligned(2), packed))
{
	BYTE		MessageType;	// Message type for this message
	BYTE		HardwareType;	// Hardware type for this message
	BYTE		HardwareLen;	// Length of hardware type
	BYTE		Hops;			// Number of hops
	DWORD		TransactionID;	// DHCP Transaction ID
	WORD		SecondsElapsed;	// Number of elapsed seconds
	WORD		BootpFlags;		// BOOTP Flags
	IP_ADDR		ClientIP;		// Client IP
	IP_ADDR		YourIP;			// Your IP
	IP_ADDR		NextServerIP;	// Next Server IP
	IP_ADDR		RelayAgentIP;	// Relay Agent IP
	MAC_ADDR	ClientMAC;		// Client MAC Address
} BOOTP_HEADER;

// DHCP State Machine
typedef enum
{
	SM_DHCP_DISABLED = 0,		// DHCP is not currently enabled
	SM_DHCP_GET_SOCKET,			// DHCP is trying to obtain a socket
	SM_DHCP_SEND_DISCOVERY,		// DHCP is sending a DHCP Discover message
	SM_DHCP_GET_OFFER,			// DHCP is waiting for a DHCP Offer
	SM_DHCP_SEND_REQUEST,		// DHCP is sending a DHCP Send Reequest message
	SM_DHCP_GET_REQUEST_ACK,	// DCHP is waiting for a Request ACK message
	SM_DHCP_BOUND,				// DHCP is bound
	SM_DHCP_SEND_RENEW,			// DHCP is sending a DHCP renew message (first try)
	SM_DHCP_GET_RENEW_ACK,		// DHCP is waiting for a renew ACK
	SM_DHCP_SEND_RENEW2,		// DHCP is sending a DHCP renew message (second try)
	SM_DHCP_GET_RENEW_ACK2,		// DHCP is waiting for a renew ACK
	SM_DHCP_SEND_RENEW3,		// DHCP is sending a DHCP renew message (third try)
	SM_DHCP_GET_RENEW_ACK3		// DHCP is waiting for a renew ACK
} SM_DHCP;


void DHCPInit(BYTE vInterface);
void DHCPTask(void);
void DHCPServerTask(void);
void DHCPDisable(BYTE vInterface);
void DHCPEnable(BYTE vInterface);
BOOL DHCPIsEnabled(BYTE vInterface);
BOOL DHCPIsBound(BYTE vInterface);
BOOL DHCPStateChanged(BYTE vInterface);
BOOL DHCPIsServerDetected(BYTE vInterface);

extern BOOL bDHCPServerEnabled;

#endif
