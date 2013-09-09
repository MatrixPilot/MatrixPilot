/*********************************************************************
 *
 *  Internet Protocol (IP) Version 4 Communications Layer
 *  Module for Microchip TCP/IP Stack
 *   -Provides a transport for TCP, UDP, and ICMP messages
 *	 -Reference: RFC 791
 *
 *********************************************************************
 * FileName:        IP.c
 * Dependencies:    Network Layer interface (ENC28J60.c, ETH97J60.c, 
 *					ENCX24J600.c or WFMac.c)
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
 * Nilesh Rajbharti     4/27/01 Original        (Rev 1.0)
 * Nilesh Rajbharti     2/9/02  Cleanup
 * Nilesh Rajbharti     5/22/02 Rev 2.0 (See version.log for detail)
 * Howard Schlunder		8/31/04	Beta Rev 0.9 (See version.log for detail)
 * Howard Schlunder		1/5/06	Improved DMA checksum efficiency
 * Darren Rook			9/21/06	Corrected IPHeaderLen not being 
 * 								initialized when NON_MCHP_MAC defined.
 ********************************************************************/
#define __IP_C

#include "TCPIP Stack/TCPIP.h"

// This is left shifted by 4.  Actual value is 0x04.
#define IPv4                (0x40u)
#define IP_VERSION          IPv4

// IHL (Internet Header Length) is # of DWORDs in a header.
// Since, we do not support options, our IP header length will be
// minimum i.e. 20 bytes : IHL = 20 / 4 = 5.
#define IP_IHL              (0x05)

#define IP_SERVICE_NW_CTRL  (0x07)
#define IP_SERVICE_IN_CTRL  (0x06)
#define IP_SERVICE_ECP      (0x05)
#define IP_SERVICE_OVR      (0x04)
#define IP_SERVICE_FLASH    (0x03)
#define IP_SERVICE_IMM      (0x02)
#define IP_SERVICE_PRIOR    (0x01)
#define IP_SERVICE_ROUTINE  (0x00)

#define IP_SERVICE_N_DELAY  (0x00)
#define IP_SERCICE_L_DELAY  (0x08)
#define IP_SERVICE_N_THRPT  (0x00)
#define IP_SERVICE_H_THRPT  (0x10)
#define IP_SERVICE_N_RELIB  (0x00)
#define IP_SERVICE_H_RELIB  (0x20)

#define IP_SERVICE          (IP_SERVICE_ROUTINE | IP_SERVICE_N_DELAY)

#if defined(STACK_USE_ZEROCONF_MDNS_SD)
  #define MY_IP_TTL           (255)  // Time-To-Live in hops 
  // IP TTL is set to 255 for Multicast DNS compatibility. See mDNS-draft-08, section 4.
#else
  #define MY_IP_TTL           (100)  // Time-To-Live in hops
#endif



static WORD _Identifier = 0;
static BYTE IPHeaderLen;


static void SwapIPHeader(IP_HEADER* h);




/*********************************************************************
 * Function:        BOOL IPGetHeader( IP_ADDR    *localIP,
 *                                    NODE_INFO  *remote,
 *                                    BYTE        *Protocol,
 *                                    WORD        *len)
 *
 * PreCondition:    MACGetHeader() == TRUE
 *
 * Input:           localIP     - Local node IP Address as received
 *                                in current IP header.
 *                                If this information is not required
 *                                caller may pass NULL value.
 *                  remote      - Remote node info
 *                  Protocol    - Current packet protocol
 *                  len         - Current packet data length
 *
 * Output:          TRUE, if valid packet was received
 *                  FALSE otherwise
 *
 * Side Effects:    None
 *
 * Note:            Only one IP message can be received.
 *                  Caller may not transmit and receive a message
 *                  at the same time.
 *
 ********************************************************************/
BOOL IPGetHeader(IP_ADDR *localIP,
                 NODE_INFO *remote,
                 BYTE *protocol,
                 WORD *len)
{
    WORD_VAL    CalcChecksum;
    IP_HEADER   header;

#if defined(NON_MCHP_MAC)
    WORD_VAL    ReceivedChecksum;
    WORD        checksums[2];
    BYTE        optionsLen;
	#define MAX_OPTIONS_LEN     (40u)            // As per RFC 791.
    BYTE        options[MAX_OPTIONS_LEN];
#endif

    // Read IP header.
    MACGetArray((BYTE*)&header, sizeof(header));

    // Make sure that this is an IPv4 packet.
    if((header.VersionIHL & 0xf0) != IP_VERSION)
    	return FALSE;

	// Throw this packet away if it is a fragment.  
	// We don't have enough RAM for IP fragment reconstruction.
	if(header.FragmentInfo & 0xFF1F)
		return FALSE;

	IPHeaderLen = (header.VersionIHL & 0x0f) << 2;

#if !defined(NON_MCHP_MAC)
	// Validate the IP header.  If it is correct, the checksum 
	// will come out to 0x0000 (because the header contains a 
	// precomputed checksum).  A corrupt header will have a 
	// nonzero checksum.
	CalcChecksum.Val = MACCalcRxChecksum(0, IPHeaderLen);

	// Seek to the end of the IP header
	MACSetReadPtrInRx(IPHeaderLen);

    if(CalcChecksum.Val)
#else
    // Calculate options length in this header, if there is any.
    // IHL is in terms of numbers of 32-bit DWORDs; i.e. actual
    // length is 4 times IHL.
    optionsLen = IPHeaderLen - sizeof(header);

    // If there is any option(s), read it so that we can include them
    // in checksum calculation.
    if ( optionsLen > MAX_OPTIONS_LEN )
        return FALSE;

    if ( optionsLen > 0u )
        MACGetArray(options, optionsLen);

    // Save header checksum; clear it and recalculate it ourselves.
    ReceivedChecksum.Val = header.HeaderChecksum;
    header.HeaderChecksum = 0;

    // Calculate checksum of header including options bytes.
    checksums[0] = ~CalcIPChecksum((BYTE*)&header, sizeof(header));

    // Calculate Options checksum too, if they are present.
    if ( optionsLen > 0u )
        checksums[1] = ~CalcIPChecksum((BYTE*)options, optionsLen);
    else
        checksums[1] = 0;

    CalcChecksum.Val  = CalcIPChecksum((BYTE*)checksums,
                                            2 * sizeof(WORD));

    // Make sure that checksum is correct
    if ( ReceivedChecksum.Val != CalcChecksum.Val )
#endif
    {
        // Bad packet. The function caller will be notified by means of the FALSE 
        // return value and it should discard the packet.
        return FALSE;
    }

    // Network to host conversion.
    SwapIPHeader(&header);

    // If caller is intrested, return destination IP address
    // as seen in this IP header.
    if ( localIP )
        localIP->Val    = header.DestAddress.Val;

    remote->IPAddr.Val  = header.SourceAddress.Val;
    *protocol           = header.Protocol;
    *len 				= header.TotalLength - IPHeaderLen;

    return TRUE;
}




/*********************************************************************
 * Function: WORD IPPutHeader(NODE_INFO *remote,
 *           				  BYTE protocol,
 *                			  WORD len)
 *
 * PreCondition:    IPIsTxReady() == TRUE
 *
 * Input:           *remote     - Destination node address
 *                  protocol    - Current packet protocol
 *                  len         - Current packet data length
 *
 * Output:          (WORD)0
 *
 * Side Effects:    None
 *
 * Note:            Only one IP message can be transmitted at any
 *                  time.
 ********************************************************************/
WORD IPPutHeader(NODE_INFO *remote,
                 BYTE protocol,
                 WORD len)
{
    IP_HEADER   header;
    
    IPHeaderLen = sizeof(IP_HEADER);

    header.VersionIHL       = IP_VERSION | IP_IHL;
    header.TypeOfService    = IP_SERVICE;
    header.TotalLength      = sizeof(header) + len;
    header.Identification   = ++_Identifier;
    header.FragmentInfo     = 0;
    header.TimeToLive       = MY_IP_TTL;
    header.Protocol         = protocol;
    header.HeaderChecksum   = 0;
	header.SourceAddress 	= AppConfig.MyIPAddr;

    header.DestAddress.Val = remote->IPAddr.Val;

    SwapIPHeader(&header);

    header.HeaderChecksum   = CalcIPChecksum((BYTE*)&header, sizeof(header));

    MACPutHeader(&remote->MACAddr, MAC_IP, (sizeof(header)+len));
    MACPutArray((BYTE*)&header, sizeof(header));

    return 0x0000;

}

/*********************************************************************
 * Function:        IPSetRxBuffer(WORD Offset)
 *
 * PreCondition:    IPHeaderLen must have been intialized by 
 *					IPGetHeader() or IPPutHeader()
 *
 * Input:           Offset from beginning of IP data field
 *
 * Output:          Next Read/Write access to receive buffer is
 *                  set to Offset 
 *
 * Side Effects:    None
 *
 * Note:            None
 *
 ********************************************************************/
void IPSetRxBuffer(WORD Offset) 
{
	MACSetReadPtrInRx(Offset+IPHeaderLen);
}



static void SwapIPHeader(IP_HEADER* h)
{
    h->TotalLength      = swaps(h->TotalLength);
    h->Identification   = swaps(h->Identification);
    h->HeaderChecksum   = swaps(h->HeaderChecksum);
}
