/*********************************************************************
 *
 *  Internet Control Message Protocol (ICMP) Server
 *  Module for Microchip TCP/IP Stack
 *   -Provides "ping" diagnostics
 *	 -Reference: RFC 792
 *
 *********************************************************************
 * FileName:        ICMP.c
 * Dependencies:    IP, ARP
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
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Howard Schlunder		03/16/07	Original
 ********************************************************************/
#define __ICMP_C

#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_ICMP_SERVER) || defined(STACK_USE_ICMP_CLIENT)

#if defined(STACK_USE_ICMP_CLIENT)

// ICMP Timeout Value
#define ICMP_TIMEOUT	(4ul*TICK_SECOND)

// ICMP Packet Structure
typedef struct
{
	BYTE vType;
	BYTE vCode;
	WORD wChecksum;
	WORD wIdentifier;
	WORD wSequenceNumber;
	WORD wData;
} ICMP_PACKET;

// ICMP Sequence Number
static WORD wICMPSequenceNumber;
// ICMP tick timer variable
static DWORD ICMPTimer;

// ICMP Flag structure
static struct
{
	unsigned char bICMPInUse:1;         // Indicates that the ICMP Client is in use
	unsigned char bReplyValid:1;        // Indicates that a correct Ping response to one of our pings was received
	unsigned char bRemoteHostIsROM:1;   // Indicates that a remote host name was passed as a ROM pointer argument
} ICMPFlags = {0x00};

// ICMP Static Variables
static union
{
	union
	{
		ROM BYTE *szROM;
		BYTE *szRAM;
	} RemoteHost;
	NODE_INFO ICMPRemote;
} StaticVars;

// ICMP State Machine Enumeration
static enum
{
	SM_IDLE = 0,
	SM_DNS_SEND_QUERY,
	SM_DNS_GET_RESPONSE,
	SM_ARP_SEND_QUERY,
	SM_ARP_GET_RESPONSE,
	SM_ICMP_SEND_ECHO_REQUEST,
	SM_ICMP_GET_ECHO_RESPONSE
} ICMPState;

#endif

/*********************************************************************
 * Function:        void ICMPProcess(void)
 *
 * PreCondition:    MAC buffer contains ICMP type packet.
 *
 * Input:           *remote: Pointer to a NODE_INFO structure of the 
 *					ping requester
 *					len: Count of how many bytes the ping header and 
 *					payload are in this IP packet
 *
 * Output:          Generates an echo reply, if requested
 *					Validates and sets ICMPFlags.bReplyValid if a 
 *					correct ping response to one of ours is received.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void ICMPProcess(NODE_INFO *remote, WORD len)
{
	DWORD_VAL dwVal;

    // Obtain the ICMP header Type, Code, and Checksum fields
    MACGetArray((BYTE*)&dwVal, sizeof(dwVal));
	
	// See if this is an ICMP echo (ping) request
	if(dwVal.w[0] == 0x0008u)
	{
		// Validate the checksum using the Microchip MAC's DMA module
		// The checksum data includes the precomputed checksum in the 
		// header, so a valid packet will always have a checksum of 
		// 0x0000 if the packet is not disturbed.
		if(MACCalcRxChecksum(0+sizeof(IP_HEADER), len))
			return;
	
		// Calculate new Type, Code, and Checksum values
		dwVal.v[0] = 0x00;	// Type: 0 (ICMP echo/ping reply)
		dwVal.v[2] += 8;	// Subtract 0x0800 from the checksum
		if(dwVal.v[2] < 8u)
		{
			dwVal.v[3]++;
			if(dwVal.v[3] == 0u)
				dwVal.v[2]++;
		}
	
	    // Wait for TX hardware to become available (finish transmitting 
	    // any previous packet)
	    while(!IPIsTxReady());

		// Position the write pointer for the next IPPutHeader operation
		// NOTE: do not put this before the IPIsTxReady() call for WF compatbility
	    MACSetWritePtr(BASE_TX_ADDR + sizeof(ETHER_HEADER));
        	
		// Create IP header in TX memory
		IPPutHeader(remote, IP_PROT_ICMP, len);
	
		// Copy ICMP response into the TX memory
		MACPutArray((BYTE*)&dwVal, sizeof(dwVal));
		MACMemCopyAsync(-1, -1, len-4);
		while(!MACIsMemCopyDone());
	
		// Transmit the echo reply packet
	    MACFlush();
	}
#if defined(STACK_USE_ICMP_CLIENT)
	else if(dwVal.w[0] == 0x0000u)	// See if this an ICMP Echo reply to our request
	{
		// Get the sequence number and identifier fields
		MACGetArray((BYTE*)&dwVal, sizeof(dwVal));
		
		// See if the identifier matches the one we sent
		if(dwVal.w[0] != 0xEFBE)	
			return;
	
		if(dwVal.w[1] != wICMPSequenceNumber)
			return;

		// Validate the ICMP checksum field
	    IPSetRxBuffer(0);
		if(CalcIPBufferChecksum(sizeof(ICMP_PACKET)))	// Two bytes of payload were sent in the echo request
			return;
		
		// Flag that we received the response and stop the timer ticking
		ICMPFlags.bReplyValid = 1;
		ICMPTimer = TickGet() - ICMPTimer;
	}
#endif
}

#if defined(STACK_USE_ICMP_CLIENT)
/*********************************************************************
 * Function:        void ICMPSendPing(DWORD dwRemoteIP)
 *
 * PreCondition:    ICMPBeginUsage() returned TRUE
 *
 * Input:           dwRemoteIP: IP Address to ping.  Must be stored 
 *								big endian.  Ex. 192.168.0.1 should be
 *								passed as 0x0100A8C0.
 *
 * Output:          Begins the process of transmitting an ICMP echo 
 *					request.  This normally involves an ARP 
 *					resolution procedure first.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void ICMPSendPing(DWORD dwRemoteIP)
{
	ICMPFlags.bReplyValid = 0;
	ICMPTimer = TickGet();
	StaticVars.ICMPRemote.IPAddr.Val = dwRemoteIP;
	ICMPState = SM_ARP_SEND_QUERY;
}

#if defined(STACK_USE_DNS)
/*********************************************************************
 * Function:        void ICMPSendPingToHost (BYTE * szRemoteHost)
 *
 * PreCondition:    ICMPBeginUsage() returned TRUE
 *
 * Input:           szRemoteHost: Host name to ping.  Must be stored 
 *								  in RAM if being called by PIC18.
 *								  Ex. www.microchip.com
 *
 * Output:          Begins the process of transmitting an ICMP echo 
 *					request.  This normally involves an ARP 
 *					resolution procedure first.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void ICMPSendPingToHost(BYTE * szRemoteHost)
{
	ICMPFlags.bReplyValid = 0;
	ICMPTimer = TickGet();
	ICMPFlags.bRemoteHostIsROM = 0;
	StaticVars.RemoteHost.szRAM = szRemoteHost;
	ICMPState = SM_DNS_SEND_QUERY;
}

#if defined(__18CXX)

/*********************************************************************
 * Function:        void ICMPSendPingToHostROM (ROM BYTE * szRemoteHost)
 *
 * PreCondition:    ICMPBeginUsage() returned TRUE
 *
 * Input:           szRemoteHost: Host name to ping.  Must be stored 
 *								  in ROM. Should only be called by PIC18.
 *								  Ex. www.microchip.com
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Begins the process of transmitting an ICMP echo 
 *					request.  This normally involves an ARP 
 *					resolution procedure first.
 *
 * Note:            None
 ********************************************************************/
void ICMPSendPingToHostROM(ROM BYTE * szRemoteHost)
{
	ICMPFlags.bReplyValid = 0;
	ICMPTimer = TickGet();
	ICMPFlags.bRemoteHostIsROM = 1;
	StaticVars.RemoteHost.szROM = szRemoteHost;
	ICMPState = SM_DNS_SEND_QUERY;
}

#endif
#endif

/*********************************************************************
 * Function:        LONG ICMPGetReply(void)
 *
 * PreCondition:    ICMPBeginUsage() returned TRUE and ICMPSendPing() 
 *					was called
 *
 * Input:           None
 *
 * Output:          -3: Could not resolve hostname (DNS timeout or 
 *			    	    hostname invalid)
 *					-2: No response received yet
 *					-1: Operation timed out (longer than ICMP_TIMEOUT) 
 *						has elapsed.
 *					>=0: Number of TICKs that elapsed between 
 *						 initial ICMP transmission and reception of 
 *						 a valid echo.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
LONG ICMPGetReply(void)
{
	ICMP_PACKET ICMPPacket;

	switch(ICMPState)
	{
#if defined(STACK_USE_DNS)
		case SM_DNS_SEND_QUERY:
			// Obtain DNS module ownership
			if(!DNSBeginUsage())
				break;
			
			// Send DNS query
			if(ICMPFlags.bRemoteHostIsROM)
				DNSResolveROM(StaticVars.RemoteHost.szROM, DNS_TYPE_A);
			else
				DNSResolve(StaticVars.RemoteHost.szRAM, DNS_TYPE_A);
			
			ICMPState = SM_DNS_GET_RESPONSE;
			break;
				
		case SM_DNS_GET_RESPONSE:
			// See if DNS is done, and if so, get the remote IP address
			if(!DNSIsResolved(&StaticVars.ICMPRemote.IPAddr))
				break;
			
			// Free the DNS module
			DNSEndUsage();
			
			// Return error code if the DNS query failed
			if(StaticVars.ICMPRemote.IPAddr.Val == 0x00000000ul)
			{
				ICMPState = SM_IDLE;
				return -3;
			}

			ICMPState = SM_ARP_SEND_QUERY;	
			// No break;	
#endif

		case SM_ARP_SEND_QUERY:
			ARPResolve(&StaticVars.ICMPRemote.IPAddr);
			ICMPState = SM_ARP_GET_RESPONSE;
			break;
			
		case SM_ARP_GET_RESPONSE:
			// See if the ARP reponse was successfully received
			if(!ARPIsResolved(&StaticVars.ICMPRemote.IPAddr, &StaticVars.ICMPRemote.MACAddr))
				break;
			
			ICMPState = SM_ICMP_SEND_ECHO_REQUEST;
			// No break; 
		
		case SM_ICMP_SEND_ECHO_REQUEST:
		    if(!IPIsTxReady())
		    	break;

			// Set up the ping packet
			ICMPPacket.vType = 0x08;	// 0x08: Echo (ping) request
			ICMPPacket.vCode = 0x00;
			ICMPPacket.wChecksum = 0x0000;
			ICMPPacket.wIdentifier = 0xEFBE;
			wICMPSequenceNumber++; 
			ICMPPacket.wSequenceNumber = wICMPSequenceNumber;
			ICMPPacket.wData = 0x2860;
			ICMPPacket.wChecksum = CalcIPChecksum((BYTE*)&ICMPPacket, sizeof(ICMPPacket));
		
			// Record the current time.  This will be used as a basis for 
			// finding the echo response time, which exludes the ARP and DNS 
			// steps
			ICMPTimer = TickGet();

			// Position the write pointer for the next IPPutHeader operation
		    MACSetWritePtr(BASE_TX_ADDR + sizeof(ETHER_HEADER));
		
			// Create IP header in TX memory
			IPPutHeader(&StaticVars.ICMPRemote, IP_PROT_ICMP, sizeof(ICMPPacket));
			MACPutArray((BYTE*)&ICMPPacket, sizeof(ICMPPacket));
			MACFlush();

			// Echo sent, advance state
			ICMPState = SM_ICMP_GET_ECHO_RESPONSE;
			break;

		case SM_ICMP_GET_ECHO_RESPONSE:
			// See if the echo was successfully received
			if(ICMPFlags.bReplyValid)
				return (LONG)ICMPTimer;
		
			break;
		
		// SM_IDLE or illegal/impossible state:
		default:
			return -1;
	}

	// See if the DNS/ARP/echo request timed out
	if(TickGet() - ICMPTimer > ICMP_TIMEOUT)
	{
		// Free DNS module if we have it in use
		#if defined(STACK_USE_DNS)
			if(ICMPState == SM_DNS_GET_RESPONSE)
				DNSEndUsage();
		#endif
		
		// Stop ICMP echo test and return error to caller
		ICMPState = SM_IDLE;
		return -1;
	}

	// Still working.  No response to report yet.
	return -2;
}


/*********************************************************************
 * Function:        BOOL ICMPBeginUsage(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          TRUE: You have successfully gained ownership of 
 *						  the ICMP client module and can now use the 
 *						  ICMPSendPing() and ICMPGetReply() functions.
 *					FALSE: Some other application is using the ICMP 
 *						   client module.  Calling ICMPSendPing() 
 *						   will corrupt the other application's ping 
 *						   result.
 *
 * Side Effects:    None
 *
 * Overview:        Claims ownership of the ICMP module.
 *
 * Note:            None
 ********************************************************************/
BOOL ICMPBeginUsage(void)
{
	if(ICMPFlags.bICMPInUse)
		return FALSE;

	ICMPFlags.bICMPInUse = TRUE;
	return TRUE;
}


/*********************************************************************
 * Function:        void ICMPEndUsage(void)
 *
 * PreCondition:    ICMPBeginUsage() was called by you and it 
 *					returned TRUE.
 *
 * Input:           None
 *
 * Output:          Your ownership of the ICMP module is released.  
 *					You can no longer use ICMPSendPing().
 *
 * Side Effects:    None
 *
 * Overview:        Gives up ownership of the ICMP module.
 *
 * Note:            None
 ********************************************************************/
void ICMPEndUsage(void)
{
	ICMPFlags.bICMPInUse = FALSE;
}

#endif //#if defined(STACK_USE_ICMP_CLIENT)

#endif //#if defined(STACK_USE_ICMP_SERVER) || defined(STACK_USE_ICMP_CLIENT)
