/*********************************************************************
 *
 *	User Datagram Protocol (UDP) Communications Layer
 *  Module for Microchip TCP/IP Stack
 *	 -Provides unreliable, minimum latency transport of application 
 *    datagram (packet) oriented data
 *	 -Reference: RFC 768
 *
 *********************************************************************
 * FileName:        UDP.c
 * Dependencies:    IP, Ethernet (ENC28J60.c or ETH97J60.c)
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
 * Nilesh Rajbharti     3/19/01  Original        (Rev 1.0)
 * Nilesh Rajbharti     2/26/03  Fixed UDPGet and UDPProcess bugs
 *                               as discovered and fixed by John Owen
 *                               of Powerwave.
 *                               1. UDPGet would return FALSE on last good byte
 *                               2. UDPProcess was incorrectly calculating length.
 * Nilesh Rajbharti     5/19/03  Added bFirstRead flag similar to TCP
 *                               to detect very first UDPGet and
 *                               reset MAC Rx pointer to begining of
 *                               UDP data area.  This would ensure that
 *                               if UDP socket has pending Rx data and
 *                               another module resets MAC Rx pointer,
 *                               next UDP socket Get would get correct
 *                               data.
 * Robert Sloan (RSS)    5/29/03 Improved FindMatchingSocket()
 * Nilesh Rajbharti     12/2/03  Added UDPChecksum logic in UDPProcess()
 * Nilesh Rajbharti     12/5/03  Modified UDPProcess() and FindMatchingSocket()
 *                               to include localIP as new parameter.
 *                               This corrects pseudo header checksum
 *                               logic in UDPProcess().
 *                               It also corrects broadcast packet
 *                               matching correct in FindMatchingSocket().
 * Howard Schlunder		1/16/06	 Fixed an imporbable RX checksum bug 
 *								 when using a Microchip Ethernet controller)
 * Howard Schlunder		6/02/06	 Fixed a bug where all RXed UDP packets 
 *								 without a checksum (0x0000) were thrown
 *								 away.  No checksum is legal in UDP.
 * Howard Schlunder		8/10/06	 Fixed a bug where UDP sockets would 
 *								 unintentionally keep the remote MAC 
 *								 address cached, even after calling 
 *								 UDPInit(), UDPClose(), or reseting 
 *								 the part without clearing all the 
 *								 PIC memory.
 ********************************************************************/
#define __UDP_C

#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_UDP)

/****************************************************************************
  Section:
	Configuration Parameters
  ***************************************************************************/

// First port number for randomized local port number selection
#define LOCAL_UDP_PORT_START_NUMBER (4096u)

// Last port number for randomized local port number selection
#define LOCAL_UDP_PORT_END_NUMBER   (8192u)

/****************************************************************************
  Section:
	UDP Global Variables
  ***************************************************************************/

// Stores an array of information pertaining to each UDP socket
UDP_SOCKET_INFO		UDPSocketInfo[MAX_UDP_SOCKETS];

// Indicates which UDP socket is currently active
UDP_SOCKET			activeUDPSocket;

WORD UDPTxCount;	// Number of bytes written to this UDP segment
WORD UDPRxCount;	// Number of bytes read from this UDP segment
static UDP_SOCKET	LastPutSocket = INVALID_UDP_SOCKET;	// Indicates the last socket to which data was written
static WORD wPutOffset;		// Offset from beginning of payload where data is to be written.
static WORD wGetOffset;		// Offset from beginning of payload from where data is to be read.

// Stores various flags for the UDP module
static struct
{
	unsigned char bFirstRead : 1;		// No data has been read from this segment yet
	unsigned char bWasDiscarded : 1;	// The data in this segment has been discarded
} Flags;

// Indicates which socket has currently received data for this loop
static UDP_SOCKET SocketWithRxData = INVALID_UDP_SOCKET;

/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/

static UDP_SOCKET FindMatchingSocket(UDP_HEADER *h, NODE_INFO *remoteNode,
                                    IP_ADDR *localIP);

/****************************************************************************
  Section:
	Connection Management Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	void UDPInit(void)

  Summary:
	Initializes the UDP module.

  Description:
	Initializes the UDP module.  This function initializes all the UDP 
	sockets to the closed state.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function is called only one during lifetime of the application.
  ***************************************************************************/
void UDPInit(void)
{
    UDP_SOCKET s;

    for ( s = 0; s < MAX_UDP_SOCKETS; s++ )
    {
		UDPClose(s);
    }
	Flags.bWasDiscarded = 1;
}


/*****************************************************************************
Function:
	UDP_SOCKET UDPOpenEx(DWORD remoteHost, BYTE remoteHostType, UDP_PORT localPort,
	UDP_PORT remotePort)

 Summary:
    Opens a UDP socket for a client.

 Description:
    Provides a unified method for opening UDP sockets. This function can open both client and 
    server   sockets. For client sockets, it can accept a host name string to query in DNS, an IP
    address as a string, an IP address in binary form, or a previously resolved NODE_INFO 
    structure containing the remote IP address and associated MAC address. When a host name
    or IP address only is provided, UDP module will internally perform the necessary DNSResolve
    and/or ARP resolution steps before reporting that the UDP socket is connected (via a call to
    UDPISOpen returning TRUE). Server sockets ignore this destination parameter and listen 
    only on the indicated port.	Sockets are statically allocated on boot, but can be claimed with
    this function and freed using UDPClose .

Conditions:
UDPInit should be called.

Input:
    remoteHost -     Pointer to remote node info (MAC and IP address) for this connection.  
    If this is a     server socket (receives the first packet) or the destination is the broadcast 
    address, then this parameter should be NULL. For client sockets only. Provide a pointer to
    a null-terminated string of the remote host name (ex\:"www.microchip.com" or 
    "192.168.1.123"), a literal destination IP address (ex\: 0x7B01A8C0 or an IP_ADDR data type),
    or a pointer to a NODE_INFO structure with the remote IP address and remote node or gateway
    MAC address specified, If a string is provided.
    
    remoteHostType - Any one of the following flags to identify the meaning of the remoteHost parameter:
    	- UDP_OPEN_SERVER   = Open a server socket and ignore the remoteHost parameter. 
    		(e.g. - SNMP agent, DHCP server, Announce)
    	- UDP_OPEN_IP_ADDRESS = Open a client socket and connect it to a remote IP address.
    		Ex: 0x7B01A8C0 for 192.168.1.123 (DWORD type). Note that the byte ordering is big endian.
    	- UDP_OPEN_NODE_INFO = Open a client socket and connect it to a remote IP and MAC 
    		addresses pair stored in a NODE_INFO structure.
     	- UDP_OPEN_RAM_HOST = Open a client socket and connect it to a remote host who's 
     		name is stored as a null terminated string in a RAM array. Ex:"www.microchip.com" or
     		"192.168.0.123"
     	- UDP_OPEN_ROM_HOST = Open a client socket and connect it to a remote host who's
     		name is stored as a null terminated string in a literal string or ROM array. Ex: 
     		"www.microchip.com" or "192.168.0.123" 

    localPort - UDP port number to listen on.  If 0, stack will dynamically assign a unique port 
    number to use.

    remotePort - For client sockets, the remote port number.

Return Values:
  	Success - 
		A UDP socket handle that can be used for subsequent UDP API calls.
	Failure -
		INVALID_UDP_SOCKET.  This function fails when no more UDP socket
		handles are available.  Increase MAX_UDP_SOCKETS to make more sockets 	available.
Remarks:
	When finished using the UDP socket handle, call the UDPClose() function to free the 
	socket and delete the handle.

*****************************************************************************/
UDP_SOCKET UDPOpenEx(DWORD remoteHost, BYTE remoteHostType, UDP_PORT localPort,
		UDP_PORT remotePort)
{
	UDP_SOCKET s;
	UDP_SOCKET_INFO *p;

	// Local temp port numbers.
	static WORD NextPort __attribute__((persistent));


	p = UDPSocketInfo;
	for ( s = 0; s < MAX_UDP_SOCKETS; s++ )
	{
		if(p->localPort == INVALID_UDP_PORT)
		{
		 	p->localPort = localPort;  
			if(localPort == 0x0000u)
		   	{
			   if(NextPort > LOCAL_UDP_PORT_END_NUMBER || NextPort < LOCAL_UDP_PORT_START_NUMBER)
				   NextPort = LOCAL_UDP_PORT_START_NUMBER;

			   p->localPort    = NextPort++;
		   	}
			if((remoteHostType == UDP_OPEN_SERVER) || (remoteHost == 0))
			{
				  //Set remote node as 0xFF ( broadcast address)
				  // else Set broadcast address
				  memset((void*)&p->remote.remoteNode, 0xFF, sizeof(p->remote));
				  p->smState = UDP_OPENED;
			}
			else
			{
				switch(remoteHostType)
				{
#if defined (STACK_CLIENT_MODE)
#if defined (STACK_USE_DNS)
					case UDP_OPEN_RAM_HOST:
					case UDP_OPEN_ROM_HOST:
						//set the UDP state to UDP_GET_DNS_MODULE
						p->remote.remoteHost = remoteHost;
						p->flags.bRemoteHostIsROM = (remoteHostType == UDP_OPEN_ROM_HOST);
						p->smState = UDP_DNS_RESOLVE;
					break;
#endif
					case UDP_OPEN_IP_ADDRESS:
					// remoteHost is a literal IP address.	This doesn't need DNS and can skip directly to the Gateway ARPing step. 	
					//Next UDP state UDP_GATEWAY_SEND_ARP;
					p->remote.remoteNode.IPAddr.Val = remoteHost;
					p->retryCount = 0;
					p->retryInterval = (TICK_SECOND/4)/256;
					p->smState = UDP_GATEWAY_SEND_ARP;
					break;
#endif						
					case UDP_OPEN_NODE_INFO:
					//skip DNS and ARP resolution steps if connecting to a remote node which we've already
						memcpy((void*)(BYTE*)&p->remote,(void*)(BYTE*)(PTR_BASE)remoteHost,sizeof(p->remote));
						p->smState = UDP_OPENED;
					// CALL UDPFlushto transmit incluind peding data.
					break;
					default:
						break;
				}
			}
			p->remotePort   = remotePort;

			// Mark this socket as active.
			// Once an active socket is set, subsequent operation can be
			// done without explicitely supply socket identifier.
			activeUDPSocket = s;
			return s;
		}
		p++;
	}

	return (UDP_SOCKET)INVALID_UDP_SOCKET;

}


/******************************************************************************
Function:
	void UDPTask(void)

  Summary:
  	Performs periodic UDP tasks.

  Description:
	This function performs any required periodic UDP tasks.  Each socket's state machine is 
	checked, and any elapsed timeout periods are handled.

  Precondition:
	UDP is initialized.

  Parameters:
	None

  Returns:
	None

******************************************************************************/
void UDPTask(void)
{
	UDP_SOCKET ss;
	
	for ( ss = 0; ss < MAX_UDP_SOCKETS; ss++ )
	{

		// need to put Extra check if UDP has opened or NOT

		if((UDPSocketInfo[ss].smState == UDP_OPENED) ||
			(UDPSocketInfo[ss].smState == UDP_CLOSED))
			continue;
		// A timeout has occured.  Respond to this timeout condition
		// depending on what state this socket is in.
		switch(UDPSocketInfo[ss].smState)
		{
			#if defined(STACK_CLIENT_MODE)
			#if defined(STACK_USE_DNS)
			case UDP_DNS_RESOLVE:
			if(DNSBeginUsage())
			{
				// call DNS Resolve function and move to UDP next State machine
				UDPSocketInfo[ss].smState = UDP_DNS_IS_RESOLVED;
				if(UDPSocketInfo[ss].flags.bRemoteHostIsROM)
					DNSResolveROM((ROM BYTE*)(ROM_PTR_BASE)UDPSocketInfo[ss].remote.remoteHost, DNS_TYPE_A);
				else
					DNSResolve((BYTE*)(PTR_BASE)UDPSocketInfo[ss].remote.remoteHost, DNS_TYPE_A);
			}
			break;				
			case UDP_DNS_IS_RESOLVED:
			{
				IP_ADDR ipResolvedDNSIP;
				// See if DNS resolution has finished.	Note that if the DNS 
				// fails, the &ipResolvedDNSIP will be written with 0x00000000. 
				// MyTCB.remote.dwRemoteHost is unioned with 
				// MyTCB.remote.niRemoteMACIP.IPAddr, so we can't directly write 
				// the DNS result into MyTCB.remote.niRemoteMACIP.IPAddr.  We 
				// must copy it over only if the DNS is resolution step was 
				// successful.
				
				if(DNSIsResolved(&ipResolvedDNSIP))
				{
					if(DNSEndUsage())
					{
						UDPSocketInfo[ss].remote.remoteNode.IPAddr.Val = ipResolvedDNSIP.Val;
						UDPSocketInfo[ss].smState = UDP_GATEWAY_SEND_ARP;
						UDPSocketInfo[ss].retryCount = 0;
						UDPSocketInfo[ss].retryInterval = (TICK_SECOND/4)/256;
					}
					else
					{
						UDPSocketInfo[ss].smState = UDP_DNS_RESOLVE;
					}
				}			
			}
			break;
			#endif // #if defined(STACK_USE_DNS)

			case UDP_GATEWAY_SEND_ARP:
				// Obtain the MAC address associated with the server's IP address 
				//(either direct MAC address on same subnet, or the MAC address of the Gateway machine)
				UDPSocketInfo[ss].eventTime = (WORD)TickGetDiv256();
				ARPResolve(&UDPSocketInfo[ss].remote.remoteNode.IPAddr);
				UDPSocketInfo[ss].smState = UDP_GATEWAY_GET_ARP;
				break;

			case UDP_GATEWAY_GET_ARP:
			if(!ARPIsResolved(&UDPSocketInfo[ss].remote.remoteNode.IPAddr, 
								&UDPSocketInfo[ss].remote.remoteNode.MACAddr))
			{
				// Time out if too much time is spent in this state
				// Note that this will continuously send out ARP 
				// requests for an infinite time if the Gateway 
				// never responds
				if((WORD)TickGetDiv256() - UDPSocketInfo[ss].eventTime> (WORD)UDPSocketInfo[ss].retryInterval)
				{
					// Exponentially increase timeout until we reach 6 attempts then stay constant
					if(UDPSocketInfo[ss].retryCount < 6u)
					{
						UDPSocketInfo[ss].retryCount++;
						UDPSocketInfo[ss].retryInterval <<= 1;
					}
					// Retransmit ARP request
					UDPSocketInfo[ss].smState = UDP_GATEWAY_SEND_ARP;
				}				
			}
			else
			{
				UDPSocketInfo[ss].smState = UDP_OPENED;
			}
			break;
			default:
			case UDP_OPENED:
			case UDP_CLOSED:
			// not used
			break;
#endif // #if defined(STACK_CLIENT_MODE)
		}
	}
} 

/******************************************************************************
 
  Function:
	  BOOL UDPISOpened(UDP_SOCKET socket)
  
 Summary:
	  Determines if a socket has an established connection.

 Description:
	This function determines if a socket has an established connection to a remote node .  
	Call this function after calling UDPOpen to determine when the connection is set up 
	and ready for use.  

 Precondition:
	UDP is initialized.

 Parameters:
	socket - The socket to check.

 Return Values:
	TRUE - The socket has been opened and ARP has been resolved.
	FALSE - The socket is not currently connected.

 Remarks:
	None
 
 *****************************************************************************/
BOOL UDPIsOpened(UDP_SOCKET socket)
{
	return (UDPSocketInfo[socket].smState == UDP_OPENED);
}


#if 0
/*****************************************************************************
  Function:
	void UDPTask(void)

  Summary:
	Performs state management and housekeeping for UDP.
	
  Description:
	Performs state management and housekeeping for UDP.  This is an internal
	function meant to be called by StackTask() (not a user API).

  Precondition:
	None

  Parameters:
	None

  Return Values:
  	None
  	
  Remarks:
	UDPTask() is called once per StackTask() iteration to ensure that calls 
	to UDPIsPutReady() always update the Ethernet Write pointer location 
	between StackTask() iterations.
  ***************************************************************************/
void UDPTask(void)
{
	LastPutSocket = INVALID_UDP_SOCKET;
}


/*****************************************************************************
  Function:
	UDP_SOCKET UDPOpen(UDP_PORT localPort, NODE_INFO* remoteNode, 
						UDP_PORT remotePort)

  Summary:
	Creates a UDP socket handle for transmiting or receiving UDP packets.
	
  Description:
	Creates a UDP socket handle for transmiting or receiving UDP packets.  
	Call this function to obtain a handle required by other UDP function.

  Precondition:
	UDPInit() must have been previously called.

  Parameters:
	localPort - UDP port number to listen on.  If 0, stack will dynamically 
		assign a unique port number to use.
	remoteNode - Pointer to remote node info (MAC and IP address) for this
		connection.  If this is a server socket (receives the first packet) 
		or the destination is the broadcast address, then this parameter
		should be NULL.
	remotePort - For client sockets, the remote port number.

  Return Values:
  	Success -
		A UDP socket handle that can be used for subsequent UDP API calls.
	Failure -
		INVALID_UDP_SOCKET.  This function fails when no more UDP socket 
		handles are available.  Increase MAX_UDP_SOCKETS to make more sockets 
		available.
  	
  Remarks:
	When finished using the UDP socket handle, call the UDPClose() function 
	to free the socket and delete the handle.
  ***************************************************************************/
UDP_SOCKET UDPOpen(UDP_PORT localPort,
                   NODE_INFO *remoteNode,
                   UDP_PORT remotePort)
{
    UDP_SOCKET s;
    UDP_SOCKET_INFO *p;

	// Local temp port numbers.
	static WORD NextPort __attribute__((persistent));


    p = UDPSocketInfo;
    for ( s = 0; s < MAX_UDP_SOCKETS; s++ )
    {
        if(p->localPort == INVALID_UDP_PORT)
        {
			p->localPort = localPort;	

			if(localPort == 0x0000u)
			{
				if(NextPort > LOCAL_UDP_PORT_END_NUMBER || NextPort < LOCAL_UDP_PORT_START_NUMBER)
					NextPort = LOCAL_UDP_PORT_START_NUMBER;
	
	            p->localPort    = NextPort++;
			}

            // If remoteNode is supplied, remember it.
            if(remoteNode)
            {
                memcpy((void*)&p->remoteNode,
                        (const void*)remoteNode,
                        sizeof(p->remoteNode));
            }
            else
			{
				// else Set broadcast address
				memset((void*)&p->remoteNode, 0xFF, sizeof(p->remoteNode));
			}

            p->remotePort   = remotePort;

            // Mark this socket as active.
            // Once an active socket is set, subsequent operation can be
            // done without explicitely supply socket identifier.
            activeUDPSocket = s;
            return s;
        }
        p++;
    }

    return (UDP_SOCKET)INVALID_UDP_SOCKET;
}

#endif


/*****************************************************************************
  Function:
	void UDPClose(UDP_SOCKET s)

  Summary:
	Closes a UDP socket and frees the handle.
	
  Description:
	Closes a UDP socket and frees the handle.  Call this function to release
	a socket and return it to the pool for use by future communications.

  Precondition:
	UDPInit() must have been previously called.

  Parameters:
	s - The socket handle to be released.  If an illegal handle value is 
		provided, the function safely does nothing.

  Returns:
  	None
  	
  Remarks:
	This function does not affect the previously designated active socket.
  ***************************************************************************/
void UDPClose(UDP_SOCKET s)
{
	if(s >= MAX_UDP_SOCKETS)
		return;

	UDPSocketInfo[s].localPort = INVALID_UDP_PORT;
	UDPSocketInfo[s].remote.remoteNode.IPAddr.Val = 0x00000000;
	UDPSocketInfo[s].smState = UDP_CLOSED;
}


/*****************************************************************************
  Function:
	void UDPSetTxBuffer(WORD wOffset)

  Summary:
	Moves the pointer within the TX buffer.
	
  Description:
	This function allows the write location within the TX buffer to be 
	specified.  Future calls to UDPPut, UDPPutArray, UDPPutString, etc will
	write data from the indicated location.

  Precondition:
	UDPInit() must have been previously called and a socket is currently 
	active.

  Parameters:
	wOffset - Offset from beginning of UDP packet data payload to place the
		write pointer.

  Returns:
  	None
  ***************************************************************************/
void UDPSetTxBuffer(WORD wOffset)
{
	IPSetTxBuffer(wOffset+sizeof(UDP_HEADER));
	wPutOffset = wOffset;
}


/*****************************************************************************
  Function:
	void UDPSetRxBuffer(WORD wOffset)

  Summary:
	Moves the pointer within the RX buffer.
	
  Description:
	This function allows the read location within the RX buffer to be 
	specified.  Future calls to UDPGet and UDPGetArray will read data from
	the indicated location forward.

  Precondition:
	UDPInit() must have been previously called and a socket is currently 
	active.

  Parameters:
	wOffset - Offset from beginning of UDP packet data payload to place the
		read pointer.

  Returns:
  	None
  ***************************************************************************/
void UDPSetRxBuffer(WORD wOffset)
{
	IPSetRxBuffer(wOffset+sizeof(UDP_HEADER));
	wGetOffset = wOffset;
}



/****************************************************************************
  Section:
	Transmit Functions
  ***************************************************************************/
  
/*****************************************************************************
  Function:
	WORD UDPIsPutReady(UDP_SOCKET s)

  Summary:
	Determines how many bytes can be written to the UDP socket.
	
  Description:
	This function determines if bytes can be written to the specified UDP
	socket.  It also prepares the UDP module for writing by setting the 
	indicated socket as the currently active connection.

  Precondition:
	UDPInit() must have been previously called.

  Parameters:
	s - The socket to be made active

  Returns:
  	The number of bytes that can be written to this socket.
  ***************************************************************************/
WORD UDPIsPutReady(UDP_SOCKET s)
{
	if(!MACIsTxReady())
		return 0;

	if(LastPutSocket != s)
	{
		LastPutSocket = s;
		UDPTxCount = 0;
		UDPSetTxBuffer(0);
	}

	activeUDPSocket = s;

	return MAC_TX_BUFFER_SIZE - sizeof(IP_HEADER) - sizeof(UDP_HEADER) - UDPTxCount;
}

/*****************************************************************************
  Function:
	BOOL UDPPut(BYTE v)

  Summary:
	Writes a byte to the currently active socket.
	
  Description:
	This function writes a single byte to the currently active UDP socket, 
	while incrementing the buffer length.  UDPIsPutReady should be used 
	before calling this function to specify the currently active socket.

  Precondition:
	UDPIsPutReady() was previously called to specify the current socket.

  Parameters:
	v - The byte to be loaded into the transmit buffer.

  Return Values:
  	TRUE - The byte was successfully written to the socket.
  	FALSE - The transmit buffer is already full and so the write failed.
  ***************************************************************************/
BOOL UDPPut(BYTE v)
{
	// See if we are out of transmit space.
	if(wPutOffset >= (MAC_TX_BUFFER_SIZE - sizeof(IP_HEADER) - sizeof(UDP_HEADER)))
	{
		return FALSE;
	}

    // Load application data byte
    MACPut(v);
	wPutOffset++;
	if(wPutOffset > UDPTxCount)
		UDPTxCount = wPutOffset;

    return TRUE;
}

/*****************************************************************************
  Function:
	WORD UDPPutArray(BYTE *cData, WORD wDataLen)

  Summary:
	Writes an array of bytes to the currently active socket.
	
  Description:
	This function writes an array of bytes to the currently active UDP socket, 
	while incrementing the buffer length.  UDPIsPutReady should be used 
	before calling this function to specify the currently active socket.

  Precondition:
	UDPIsPutReady() was previously called to specify the current socket.

  Parameters:
	cData - The array to write to the socket.
	wDateLen - Number of bytes from cData to be written.
	
  Returns:
  	The number of bytes successfully placed in the UDP transmit buffer.  If
  	this value is less than wDataLen, then the buffer became full and the
  	input was truncated.
  ***************************************************************************/
WORD UDPPutArray(BYTE *cData, WORD wDataLen)
{
	WORD wTemp;

	wTemp = (MAC_TX_BUFFER_SIZE - sizeof(IP_HEADER) - sizeof(UDP_HEADER)) - wPutOffset;
	if(wTemp < wDataLen)
		wDataLen = wTemp;

	wPutOffset += wDataLen;
	if(wPutOffset > UDPTxCount)
		UDPTxCount = wPutOffset;

    // Load application data bytes
    MACPutArray(cData, wDataLen);

    return wDataLen;
}

/*****************************************************************************
  Function:
	WORD UDPPutROMArray(ROM BYTE *cData, WORD wDataLen)

  Summary:
	Writes an array of bytes from ROM to the currently active socket.
	
  Description:
	This function writes an array of bytes from ROM to the currently active 
	UDP socket, while incrementing the buffer length.  UDPIsPutReady should 
	be used before calling this function to specify the currently active 
	socket.

  Precondition:
	UDPIsPutReady() was previously called to specify the current socket.

  Parameters:
	cData - The array to write to the socket.
	wDateLen - Number of bytes from cData to be written.
	
  Returns:
  	The number of bytes successfully placed in the UDP transmit buffer.  If
  	this value is less than wDataLen, then the buffer became full and the
  	input was truncated.
  	
  Remarks:
	This function is aliased to UDPPutArray on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
WORD UDPPutROMArray(ROM BYTE *cData, WORD wDataLen)
{
	WORD wTemp;

	wTemp = (MAC_TX_BUFFER_SIZE - sizeof(IP_HEADER) - sizeof(UDP_HEADER)) - wPutOffset;
	if(wTemp < wDataLen)
		wDataLen = wTemp;

	wPutOffset += wDataLen;
	if(wPutOffset > UDPTxCount)
		UDPTxCount = wPutOffset;

    // Load application data bytes
    MACPutROMArray(cData, wDataLen);

    return wDataLen;
}
#endif

/*****************************************************************************
  Function:
	BYTE* UDPPutString(BYTE *strData)

  Summary:
	Writes null-terminated string to the currently active socket.
	
  Description:
	This function writes a null-terminated string to the currently active 
	UDP socket, while incrementing the buffer length.  UDPIsPutReady should 
	be used before calling this function to specify the currently active
	socket.

  Precondition:
	UDPIsPutReady() was previously called to specify the current socket.

  Parameters:
	cData - Pointer to the string to be written to the socket.
	
  Returns:
  	A pointer to the byte following the last byte written.  Note that this
  	is different than the UDPPutArray functions.  If this pointer does not
  	dereference to a NULL byte, then the buffer became full and the input
  	data was truncated.
  ***************************************************************************/
BYTE* UDPPutString(BYTE *strData)
{
	return strData + UDPPutArray(strData, strlen((char*)strData));
}

/*****************************************************************************
  Function:
	BYTE* UDPPutString(BYTE *strData)

  Summary:
	Writes null-terminated string from ROM to the currently active socket.
	
  Description:
	This function writes a null-terminated string from ROM to the currently
	active UDP socket, while incrementing the buffer length.  UDPIsPutReady
	should be used before calling this function to specify the currently
	active socket.

  Precondition:
	UDPIsPutReady() was previously called to specify the current socket.

  Parameters:
	cData - Pointer to the string to be written to the socket.
	
  Returns:
  	A pointer to the byte following the last byte written.  Note that this
  	is different than the UDPPutArray functions.  If this pointer does not
  	dereference to a NULL byte, then the buffer became full and the input
  	data was truncated.
  
  Remarks:
	This function is aliased to UDPPutString on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
ROM BYTE* UDPPutROMString(ROM BYTE *strData)
{
	return strData + UDPPutROMArray(strData, strlenpgm((ROM char*)strData));
}
#endif

/*****************************************************************************
  Function:
	void UDPFlush(void)

  Summary:
	Transmits all pending data in a UDP socket.
	
  Description:
	This function builds a UDP packet with the pending TX data and marks it 
	for transmission over the network interface.  Since UDP is a frame-based
	protocol, this function must be called before returning to the main
	stack loop whenever any data is written.

  Precondition:
	UDPIsPutReady() was previously called to specify the current socket, and
	data has been written to the socket using the UDPPut family of functions.

  Parameters:
	None
	
  Returns:
  	None

  Remarks:
	Note that unlike TCPFlush, UDPFlush must be called before returning to 
	the main stack loop.  There is no auto transmit for UDP segments.
  ***************************************************************************/
void UDPFlush(void)
{
    UDP_HEADER      h;
    UDP_SOCKET_INFO *p;
    WORD			wUDPLength;

    p = &UDPSocketInfo[activeUDPSocket];

	wUDPLength = UDPTxCount + sizeof(UDP_HEADER);

	// Generate the correct UDP header
    h.SourcePort        = swaps(p->localPort);
    h.DestinationPort   = swaps(p->remotePort);
    h.Length            = swaps(wUDPLength);
	h.Checksum 			= 0x0000;
    
	// Calculate IP pseudoheader checksum if we are going to enable 
	// the checksum field
	#if defined(UDP_USE_TX_CHECKSUM)
	{
		PSEUDO_HEADER   pseudoHeader;
		
		pseudoHeader.SourceAddress	= AppConfig.MyIPAddr;
		pseudoHeader.DestAddress    = p->remote.remoteNode.IPAddr;
		pseudoHeader.Zero           = 0x0;
		pseudoHeader.Protocol       = IP_PROT_UDP;
		pseudoHeader.Length			= wUDPLength;
		SwapPseudoHeader(pseudoHeader);
		h.Checksum = ~CalcIPChecksum((BYTE*)&pseudoHeader, sizeof(pseudoHeader));
	}
	#endif

	// Position the hardware write pointer where we will need to 
	// begin writing the IP header
	MACSetWritePtr(BASE_TX_ADDR + sizeof(ETHER_HEADER));
	
	// Write IP header to packet
	IPPutHeader(&p->remote.remoteNode, IP_PROT_UDP, wUDPLength);

    // Write UDP header to packet
    MACPutArray((BYTE*)&h, sizeof(h));
    
	// Calculate the final UDP checksum and write it in, if enabled
	#if defined(UDP_USE_TX_CHECKSUM)
	{
        PTR_BASE	wReadPtrSave;
        WORD		wChecksum;

		wReadPtrSave = MACSetReadPtr(BASE_TX_ADDR + sizeof(ETHER_HEADER) + sizeof(IP_HEADER));
		wChecksum = CalcIPBufferChecksum(wUDPLength);
		if(wChecksum == 0x0000u)
			wChecksum = 0xFFFF;
		MACSetReadPtr(wReadPtrSave);
		MACSetWritePtr(BASE_TX_ADDR + sizeof(ETHER_HEADER) + sizeof(IP_HEADER) + 6);	// 6 is the offset to the Checksum field in UDP_HEADER
		MACPutArray((BYTE*)&wChecksum, sizeof(wChecksum));
	}
	#endif
    
	// Transmit the packet
    MACFlush();

	// Reset packet size counter for the next TX operation
    UDPTxCount = 0;
	LastPutSocket = INVALID_UDP_SOCKET;
}



/****************************************************************************
  Section:
	Receive Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	WORD UDPIsGetReady(UDP_SOCKET s)

  Summary:
	Determines how many bytes can be read from the UDP socket.
	
  Description:
	This function determines if bytes can be read from the specified UDP
	socket.  It also prepares the UDP module for reading by setting the 
	indicated socket as the currently active connection.

  Precondition:
	UDPInit() must have been previously called.

  Parameters:
	s - The socket to be made active (which has already been opened or is
		listening)

  Returns:
  	The number of bytes that can be read from this socket.
  ***************************************************************************/
WORD UDPIsGetReady(UDP_SOCKET s)
{
    activeUDPSocket = s;
	if(SocketWithRxData != s)
		return 0;

    // If this is the very first time we are accessing this packet, 
    // move the read point to the begining of the packet.
    if(Flags.bFirstRead)
    {
        Flags.bFirstRead = 0;
        UDPSetRxBuffer(0);
    }

    return UDPRxCount - wGetOffset;
}

/*****************************************************************************
  Function:
	BOOL UDPGet(BYTE *v)

  Summary:
	Reads a byte from the currently active socket.
	
  Description:
	This function reads a single byte from the currently active UDP socket, 
	while decrementing the remaining buffer length.  UDPIsGetReady should be 
	used before calling this function to specify the currently active socket.

  Precondition:
	UDPIsGetReady() was previously called to specify the current socket.

  Parameters:
	v - The buffer to receive the data being read.

  Return Values:
  	TRUE - A byte was successfully read
  	FALSE - No data remained in the read buffer
  ***************************************************************************/
BOOL UDPGet(BYTE *v)
{
	// Make sure that there is data to return
    if((wGetOffset >= UDPRxCount) || (SocketWithRxData != activeUDPSocket))
        return FALSE;

    *v = MACGet();
    wGetOffset++;

    return TRUE;
}


/*****************************************************************************
  Function:
	WORD UDPGetArray(BYTE *cData, WORD wDataLen)

  Summary:
	Reads an array of bytes from the currently active socket.
	
  Description:
	This function reads an array of bytes from the currently active UDP socket, 
	while decrementing the remaining bytes available. UDPIsGetReady should be 
	used before calling this function to specify the currently active socket.

  Precondition:
	UDPIsGetReady() was previously called to specify the current socket.

  Parameters:
	cData - The buffer to receive the bytes being read.  If NULL, the bytes are 
			simply discarded without being written anywhere (effectively skips 
			over the bytes in the RX buffer, although if you need to skip a lot 
			of data, seeking using the UDPSetRxBuffer() will be more efficient).
	wDateLen - Number of bytes to be read from the socket.
	
  Returns:
  	The number of bytes successfully read from the UDP buffer.  If this
  	value is less than wDataLen, then the buffer was emptied and no more 
  	data is available.
  ***************************************************************************/
WORD UDPGetArray(BYTE *cData, WORD wDataLen)
{
	WORD wBytesAvailable;
	
	// Make sure that there is data to return
    if((wGetOffset >= UDPRxCount) || (SocketWithRxData != activeUDPSocket))
		return 0;

	// Make sure we don't try to read more data than exists
	wBytesAvailable = UDPRxCount - wGetOffset;
	if(wBytesAvailable < wDataLen)
		wDataLen = wBytesAvailable;

	wDataLen = MACGetArray(cData, wDataLen);
    wGetOffset += wDataLen;

    return wDataLen;
}

/*****************************************************************************
  Function:
	void UDPDiscard(void)

  Summary:
	Discards any remaining RX data from a UDP socket.
	
  Description:
	This function discards any remaining received data in the currently 
	active UDP socket.

  Precondition:
	UDPIsGetReady() was previously called to select the currently active
	socket.

  Parameters:
	None
	
  Returns:
  	None

  Remarks:
	It is safe to call this function more than is necessary.  If no data is
	available, this function does nothing.
  ***************************************************************************/
void UDPDiscard(void)
{
	if(!Flags.bWasDiscarded)
	{
		MACDiscardRx();
		UDPRxCount = 0;
		SocketWithRxData = INVALID_UDP_SOCKET;
		Flags.bWasDiscarded = 1;
	}
}



/****************************************************************************
  Section:
	Data Processing Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	BOOL UDPProcess(NODE_INFO *remoteNode, IP_ADDR *localIP, WORD len)

  Summary:
	Handles an incoming UDP segment.
	
  Description:
	This function handles an incoming UDP segment to determine if it is 
	acceptable and should be handed to one of the stack applications for
	processing.

  Precondition:
	UDPInit() has been called an a UDP segment is ready in the MAC buffer.

  Parameters:
	remoteNode - The remote node that sent this segment.
	localIP - The destination IP address for this segment.
	len - Total length of the UDP segment.
	
  Return Values:
  	TRUE - A valid packet is waiting and the stack applications should be
  		called to handle it.
  	FALSE - The packet was discarded.
  ***************************************************************************/
BOOL UDPProcess(NODE_INFO *remoteNode, IP_ADDR *localIP, WORD len)
{
    UDP_HEADER		h;
    UDP_SOCKET		s;
    PSEUDO_HEADER	pseudoHeader;
    DWORD_VAL		checksums;

	UDPRxCount = 0;

    // Retrieve UDP header.
    MACGetArray((BYTE*)&h, sizeof(h));

    h.SourcePort        = swaps(h.SourcePort);
    h.DestinationPort   = swaps(h.DestinationPort);
    h.Length            = swaps(h.Length) - sizeof(UDP_HEADER);

	// See if we need to validate the checksum field (0x0000 is disabled)
	if(h.Checksum)
	{
	    // Calculate IP pseudoheader checksum.
	    pseudoHeader.SourceAddress		= remoteNode->IPAddr;
	    pseudoHeader.DestAddress.Val	= localIP->Val;
	    pseudoHeader.Zero				= 0x0;
	    pseudoHeader.Protocol			= IP_PROT_UDP;
	    pseudoHeader.Length				= len;

	    SwapPseudoHeader(pseudoHeader);
	
	    checksums.w[0] = ~CalcIPChecksum((BYTE*)&pseudoHeader,
	                                    sizeof(pseudoHeader));
	
	
	    // Now calculate UDP packet checksum in NIC RAM -- should match pseudoHeader
	    IPSetRxBuffer(0);
	    checksums.w[1] = CalcIPBufferChecksum(len);
	
	    if(checksums.w[0] != checksums.w[1])
	    {
	        MACDiscardRx();
	        return FALSE;
	    }
	}

    s = FindMatchingSocket(&h, remoteNode, localIP);
    if(s == INVALID_UDP_SOCKET)
    {
        // If there is no matching socket, There is no one to handle
        // this data.  Discard it.
        MACDiscardRx();
		return FALSE;
    }
    else
    {
		SocketWithRxData = s;
        UDPRxCount = h.Length;
        Flags.bFirstRead = 1;
		Flags.bWasDiscarded = 0;
    }


    return TRUE;
}

/*****************************************************************************
  Function:
	static UDP_SOCKET FindMatchingSocket(UDP_HEADER *h, NODE_INFO *remoteNode,
                                     		IP_ADDR *localIP)

  Summary:
	Matches an incoming UDP segment to a currently active socket.
	
  Description:
	This function attempts to match an incoming UDP segment to a currently
	active socket for processing.

  Precondition:
	UDP segment header and IP header have both been retrieved.

  Parameters:
	h - The UDP header that was received.
	remoteNode - IP and MAC of the remote node that sent this segment.
	localIP - IP address that this segment was destined for.
	
  Returns:
  	A UDP_SOCKET handle of a matching socket, or INVALID_UDP_SOCKET when no
  	match could be made.
  ***************************************************************************/
static UDP_SOCKET FindMatchingSocket(UDP_HEADER *h,
                                     NODE_INFO *remoteNode,
                                     IP_ADDR *localIP)
{
	UDP_SOCKET s;
	UDP_SOCKET partialMatch;
	UDP_SOCKET_INFO *p;

// This is commented out because most applications don't need this type of filtering.  It comes at a performance cost.
//	// Filter out unicast packets that aren't for our IP address, but accept 
//	// all multicast and broadcast traffic
//	if(!((localIP->Val == AppConfig.MyIPAddr.Val) || (localIP->v[0] & 0x80) || (localIP->Val == (AppConfig.MyIPAddr.Val | (~AppConfig.MyMask.Val)))))
//		return INVALID_UDP_SOCKET;

	// Discard any packets received that were generated by ourself.  In 
	// structured Wi-Fi networks, the Access Point rebroadcasts our broadcast 
	// and multicast packets, causing self-reception to occur unless filtered 
	// out.
	if(remoteNode->IPAddr.Val == AppConfig.MyIPAddr.Val)
		return INVALID_UDP_SOCKET;

	partialMatch = INVALID_UDP_SOCKET;

	p = UDPSocketInfo;
    for(s = 0; s < MAX_UDP_SOCKETS; s++)
	{
		// This packet is said to be matching with current socket:
		// 1. If its destination port matches with our local port and
		// 2. Packet source IP address matches with previously saved socket remote IP address and
		// 3. Packet source port number matches with previously saved socket remote port number
		if(p->localPort == h->DestinationPort)
		{
			if(p->remotePort == h->SourcePort)
			{
				if(p->remote.remoteNode.IPAddr.Val == remoteNode->IPAddr.Val)
				{
					return s;
				}
			}

			partialMatch = s;
		}
		p++;
	}

	if(partialMatch != INVALID_UDP_SOCKET)
	{
		p = &UDPSocketInfo[partialMatch];

		memcpy((void*)&p->remote.remoteNode,
				(const void*)remoteNode, sizeof(p->remote.remoteNode) );

		p->remotePort = h->SourcePort;
	}

	return partialMatch;
}


#endif //#if defined(STACK_USE_UDP)
