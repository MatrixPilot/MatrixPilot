/*********************************************************************
 *
 *	Announce Client and Server
 *  Module for Microchip TCP/IP Stack
 *	 -Provides device hostname and IP address discovery on a local 
 *    Ethernet subnet (same broadcast domain)
 *	 -Reference: None.  Hopefully AN833 in the future.
 *
 *********************************************************************
 * FileName:        Announce.c
 * Dependencies:    UDP
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
 * Howard Schlunder     10/7/04	Original
 * Howard Schlunder		2/9/05	Simplified MAC address to text 
 *								conversion logic
 * Howard Schlunder		2/14/05	Fixed subnet broadcast calculation
 * Howard Schlunder		10/3/06	Fixed a remoteNode saving bug
 ********************************************************************/
#define __ANNOUNCE_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_ANNOUNCE)

#include "TCPIP Stack/TCPIP.h"

// The announce port
#define ANNOUNCE_PORT	30303



extern NODE_INFO remoteNode;

/****************************************************************************************************
  Function:
            void AnnounceIP(void)
    
  Summary:
    Transmits an Announce packet.
  Conditions:
    Stack is initialized()
  Return:
    None
  Side Effects:
    None
  Description:
    AnnounceIP opens a UDP socket and transmits a broadcast packet to port
    \30303. If a computer is on the same subnet and a utility is looking
    for packets on the UDP port, it will receive the broadcast. For this
    application, it is used to announce the change of this board's IP
    address. The messages can be viewed with the TCP/IP Discoverer
    software tool.
  Remarks:
    A UDP socket must be available before this function is called. It is
    freed at the end of the function. MAX_UDP_SOCKETS may need to be
    increased if other modules use UDP sockets.                                                      
  ****************************************************************************************************/
void AnnounceIP(void)
{
	UDP_SOCKET	MySocket;
	BYTE 		i;

	if(!MACIsLinked())  // Check for link before blindly opening and transmitting (similar to DHCP case)
		return;

	// Open a UDP socket for outbound broadcast transmission
	//MySocket = UDPOpen(2860, NULL, ANNOUNCE_PORT);
	MySocket = UDPOpenEx(0,UDP_OPEN_SERVER,2860, ANNOUNCE_PORT);
	// Abort operation if no UDP sockets are available
	// If this ever happens, incrementing MAX_UDP_SOCKETS in 
	// StackTsk.h may help (at the expense of more global memory 
	// resources).
	if(MySocket == INVALID_UDP_SOCKET)
		return;
	
	// Make certain the socket can be written to
	while(!UDPIsPutReady(MySocket));
	
	// Begin sending our MAC address in human readable form.
	// The MAC address theoretically could be obtained from the 
	// packet header when the computer receives our UDP packet, 
	// however, in practice, the OS will abstract away the useful
	// information and it would be difficult to obtain.  It also 
	// would be lost if this broadcast packet were forwarded by a
	// router to a different portion of the network (note that 
	// broadcasts are normally not forwarded by routers).
	UDPPutArray((BYTE*)AppConfig.NetBIOSName, sizeof(AppConfig.NetBIOSName)-1);
	UDPPut('\r');
	UDPPut('\n');

	// Convert the MAC address bytes to hex (text) and then send it
	i = 0;
	while(1)
	{
		UDPPut(btohexa_high(AppConfig.MyMACAddr.v[i]));
	    UDPPut(btohexa_low(AppConfig.MyMACAddr.v[i]));
	    if(++i == 6u)
	    	break;
	    UDPPut('-');
	}

	// Send some other human readable information.
	UDPPutROMString((ROM BYTE*)"\r\nDHCP/Power event occurred");

	// Send the packet
	UDPFlush();
	
	// Close the socket so it can be used by other modules
	UDPClose(MySocket);
}

/*********************************************************************
 * Function:        void DiscoveryTask(void)
 *
 * Summary:         Announce callback task.
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Recurring task used to listen for Discovery
 *                  messages on the specified ANNOUNCE_PORT.  These
 *                  messages can be sent using the Microchip Device
 *                  Discoverer tool. If one is received, this
 *                  function will transmit a reply.
 *
 * Note:            A UDP socket must be available before this 
 *					function is called.  It is freed at the end of 
 *					the function.  MAX_UDP_SOCKETS may need to be 
 *					increased if other modules use UDP sockets.
 ********************************************************************/
void DiscoveryTask(void)
{
	static enum {
		DISCOVERY_HOME = 0,
		DISCOVERY_LISTEN,
		DISCOVERY_REQUEST_RECEIVED,
		DISCOVERY_DISABLED
	} DiscoverySM = DISCOVERY_HOME;

	static UDP_SOCKET	MySocket;
	BYTE 				i;
	
	switch(DiscoverySM)
	{
		case DISCOVERY_HOME:
			// Open a UDP socket for inbound and outbound transmission
			// Since we expect to only receive broadcast packets and 
			// only send unicast packets directly to the node we last 
			// received from, the remote NodeInfo parameter can be anything
			//MySocket = UDPOpen(ANNOUNCE_PORT, NULL, ANNOUNCE_PORT);
			MySocket = UDPOpenEx(0,UDP_OPEN_SERVER,ANNOUNCE_PORT, ANNOUNCE_PORT);

			if(MySocket == INVALID_UDP_SOCKET)
				return;
			else
				DiscoverySM++;
			break;

		case DISCOVERY_LISTEN:
			// Do nothing if no data is waiting
			if(!UDPIsGetReady(MySocket))
				return;
			
			// See if this is a discovery query or reply
			UDPGet(&i);
			UDPDiscard();
			if(i != 'D')
				return;

			// We received a discovery request, reply when we can
			DiscoverySM++;

			// Change the destination to the unicast address of the last received packet
        	memcpy((void*)&UDPSocketInfo[MySocket].remote.remoteNode, (const void*)&remoteNode, sizeof(remoteNode));
			
			// No break needed.  If we get down here, we are now ready for the DISCOVERY_REQUEST_RECEIVED state

		case DISCOVERY_REQUEST_RECEIVED:
			if(!UDPIsPutReady(MySocket))
				return;

			// Begin sending our MAC address in human readable form.
			// The MAC address theoretically could be obtained from the 
			// packet header when the computer receives our UDP packet, 
			// however, in practice, the OS will abstract away the useful
			// information and it would be difficult to obtain.  It also 
			// would be lost if this broadcast packet were forwarded by a
			// router to a different portion of the network (note that 
			// broadcasts are normally not forwarded by routers).
			UDPPutArray((BYTE*)AppConfig.NetBIOSName, sizeof(AppConfig.NetBIOSName)-1);
			UDPPut('\r');
			UDPPut('\n');
		
			// Convert the MAC address bytes to hex (text) and then send it
			i = 0;
			while(1)
			{
				UDPPut(btohexa_high(AppConfig.MyMACAddr.v[i]));
			    UDPPut(btohexa_low(AppConfig.MyMACAddr.v[i]));
			    if(++i == 6u)
			    	break;
			    UDPPut('-');
			}
			UDPPut('\r');
			UDPPut('\n');

			// Send the packet
			UDPFlush();

			// Listen for other discovery requests
			DiscoverySM = DISCOVERY_LISTEN;
			break;

		case DISCOVERY_DISABLED:
			break;
	}	

}


#endif //#if defined(STACK_USE_ANNOUNCE)
