/*********************************************************************
 *
 * Dynamic DNS Client Module
 * Reference: DNS Update API Version 2.0.3 (www.dyndns.com)
 *
 *********************************************************************
 * FileName:        DynDNS.c
 * Dependencies:    TCP, Tick
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
 * Amit Shirbhate   	3/24/08 Original
 ********************************************************************/
#ifndef __DYNDNSCLIENT_C
#define __DYNDNSCLIENT_C

#include "TCPIPConfig.h"

#if defined STACK_USE_DYNAMICDNS_CLIENT

#include "TCPIP Stack/TCPIP.h"

// Delimiter to locate IP address from CheckIP server
static ROM BYTE _checkIpSrvrResponse[] = "Address:";

// Response codes from DynDNS Update Server
static ROM char* _updateIpSrvrResponse[] =
{
	/* 0 */  "good",		// Update was successful
	/* 1 */  "nochg",		// No change was made; request is considered abusive
	/* 2 */  "abuse",		// Account has been blocked for abuse
	/* 3 */  "badsys",		// System is not supported
	/* 4 */  "badagent",	// Agent has been blocked for abuse
	/* 5 */  "badauth", 	// Authentication failed
	/* 6 */  "!donator",	// A paid account feature was requested on a free account
	/* 7 */  "notfqdn",		// Hostname was not a fully-qualified domain name
	/* 8 */  "nohost",		// Host name was not found in the system
	/* 9 */  "!yours",		// The specified hostname does not belong to this account
	/* 10 */ "numhost", 	// Number of hosts does not match / serious error
	/* 11 */ "dnserr",		// System error was encountered, try again soon
	/* 12 */ "911",			// System error was encountered, try again later
};

/****************************************************************************
  Section:
	Dynamic DNS Services
	These services must support the DynDNS API, and correspond to
	DDNS_SERVICES enumeration
  ***************************************************************************/

	// Host names for various Dynamic DNS services
	ROM char * ROM ddnsServiceHosts[] =
	{
		"members.dyndns.org",		// DYNDNS_ORG
		"dynupdate.no-ip.com",		// NO_IP_COM
		"updates.dnsomatic.com",	// DNSOMATIC_COM
	};
	
	// Port numbers for various Dynamic DNS services
	static ROM WORD ddnsServicePorts[] =
	{
		80,							// DYNDNS_ORG
		80,							// NO_IP_COM
		80,							// DNSOMATIC_COM
	};

/****************************************************************************
  Section:
	Global Variables
  ***************************************************************************/
static IP_ADDR lastKnownIP;		// Last known IP address of this device
static DDNS_STATUS lastStatus;	// Status response from last update

DDNS_POINTERS DDNSClient;		// Configuration parameters for the module

static DWORD dwUpdateAt;		// Indicates when the next CheckIP should be done
static BOOL bForceUpdate;		// Indicates that the update should be done regardless
								// of whether or not the IP changed.  Use this flag 
								// when the user/pass/hostname have changed.

/****************************************************************************
  Function:
    void DDNSInit(void)

  Summary:
    Initializes the Dynamic DNS module.

  Description:
  	This function initializes the Dynamic DNS client.  It clears the 
  	DDNSClient pointers structure, and tells the module to attempt the 
  	first update after 15 seconds have elapsed (so as to allow the DHCP
  	configuration to stabalize).

  Precondition:
	None

  Parameters:
	None

  Returns:
	None
    
  Remarks:
	This function is called only one during lifetime of the application.
  ***************************************************************************/
void DDNSInit(void)
{
	// Clear the Dynamic DNS Client to start
	memset((void*)&DDNSClient, 0x00, sizeof(DDNSClient));
	
	// Use the default Check IP server
	DDNSClient.ROMPointers.CheckIPServer = 1;
	DDNSClient.CheckIPServer.szROM = DDNS_CHECKIP_SERVER;
	DDNSClient.CheckIPPort = DDNS_DEFAULT_PORT;

	// First update is 15 seconds after boot, allowing DHCP to stabilize
	dwUpdateAt = TickGet() + 15*TICK_SECOND;
	bForceUpdate = TRUE;
	lastStatus = DDNS_STATUS_UNKNOWN;
}

/****************************************************************************
  Function:
    void DDNSTask(void)

  Summary:
    Dynamic DNS client task/state machine.

  Description:
  	This function performs the background tasks of the Dynamic DNS Client.
  	Once the DDNSPointers structure is configured, this task attempt to 
  	update the Dynamic DNS hostname on a periodic schedule.
  	
  	The task first accesses the CheckIP server to determine the device's
  	current external IP address.  If the IP address has changed, it 
  	issues an update command to the dynamic DNS service to propagate the
  	change.  This sequence executes whenever dwUpdateAt elapses, which by
  	default is every 10 minutes, or when an update is forced.
    
  Precondition:
    DDNSInit() has been called.

  Parameters:
	None
	
  Returns:
    None

  Remarks:
	This function acts as a task (similar to one in an RTOS).  It
	performs its task in a co-operative manner, and the main application
	must call this function periodically to ensure that its tasks get 
	executed in a timely fashion.
  ***************************************************************************/
void DDNSTask(void)
{
	BYTE 				i;
	static DWORD		Timer;
	static TCP_SOCKET	MySocket = INVALID_SOCKET;
	static char ROM * 	ROMStrPtr;
	static char * 		RAMStrPtr;

	static BYTE vBuffer[16];
	WORD wPos;
	static IP_ADDR ipParsed;
	
	static enum
	{
		SM_IDLE = 0u,
		SM_BEGIN_CHECKIP,				//0x1
		SM_CHECKIP_SKT_OBTAINED,		//0x2
		SM_CHECKIP_FIND_DELIMITER,		//0x3
		SM_CHECKIP_FIND_ADDRESS,		//0x4
		SM_CHECKIP_DISCONNECT,			//0x5
		SM_IP_UPDATE_HOME,				//0x6
		SM_IP_UPDATE_SKT_OBTAINED,		//0x7

		/*  
			HTTP request msg is divided into 6 parts 
			SM_IP_UPDATE_REQ_A,B,C,D,E,F as the tcp ip tx
			buffer is only able to carry 200 bytes at a time.
		*/
		
		SM_IP_UPDATE_REQ_A,				//0x8
		SM_IP_UPDATE_REQ_B,				//0x9
		SM_IP_UPDATE_REQ_C,				//0xa	
		SM_IP_UPDATE_REQ_D,				//0xb
		SM_IP_UPDATE_REQ_E,				//0xc
		SM_IP_UPDATE_REQ_F,				//0xd

		SM_IPUPDATE_FIND_RESPONSE,		//0xe
		SM_IPUPDATE_PARSE_RESPONSE,		//0xf
		SM_IPUDATE_DISCONNECT,			//0x10
		SM_DONE,						// Done, try again in 10 minutes
		SM_SOFT_ERROR,					// Soft error, try again in 30 seconds
		SM_SYSTEM_ERROR 				// System error, try again in 30 minutes
	} smDDNS = SM_IDLE;

	switch(smDDNS)
	{
		case SM_IDLE:

			// Wait for timeout to begin IP check
			if((LONG)(TickGet() - dwUpdateAt) < 0)
				break;
			
			// Otherwise, continue to next state
			smDDNS = SM_BEGIN_CHECKIP;
				
		case SM_BEGIN_CHECKIP:
			
			// If a fatal error has occurred, abort to the SM_DONE state and keep
			// the error message.
			if(lastStatus >= DDNS_STATUS_ABUSE && lastStatus <= DDNS_STATUS_911)
			{
				smDDNS = SM_DONE;
				break;
			}

			// If DDNSClient is not properly configured, abort
			if( 
				// Verify that each pointer is not null, and is not empty
				(DDNSClient.ROMPointers.Host && (!DDNSClient.Host.szROM || *DDNSClient.Host.szROM == '\0') ) ||
				(!DDNSClient.ROMPointers.Host && (!DDNSClient.Host.szRAM || *DDNSClient.Host.szRAM == '\0') ) ||
				(DDNSClient.ROMPointers.Username && (!DDNSClient.Username.szROM || *DDNSClient.Username.szROM == '\0') ) ||
				(!DDNSClient.ROMPointers.Username && (!DDNSClient.Username.szRAM || *DDNSClient.Username.szRAM == '\0') ) ||
				(DDNSClient.ROMPointers.Password && (!DDNSClient.Password.szROM || *DDNSClient.Password.szROM == '\0') ) ||
				(!DDNSClient.ROMPointers.Password && (!DDNSClient.Password.szRAM || *DDNSClient.Password.szRAM == '\0') ) ||
				(DDNSClient.ROMPointers.CheckIPServer && (!DDNSClient.CheckIPServer.szROM || *DDNSClient.CheckIPServer.szROM == '\0') ) ||
				(!DDNSClient.ROMPointers.CheckIPServer && (!DDNSClient.CheckIPServer.szRAM || *DDNSClient.CheckIPServer.szRAM == '\0') ) ||
				(DDNSClient.ROMPointers.UpdateServer && (!DDNSClient.UpdateServer.szROM || *DDNSClient.UpdateServer.szROM == '\0') ) ||
				(!DDNSClient.ROMPointers.UpdateServer && (!DDNSClient.UpdateServer.szRAM || *DDNSClient.UpdateServer.szRAM == '\0') )
			)
			{
				smDDNS = SM_SOFT_ERROR;
				lastStatus = DDNS_STATUS_INVALID;
				break;
			}
			
			// Start with an invalidated IP String
			vBuffer[0] = '\0';
	
			// Connect a socket to the remote server
			if(DDNSClient.ROMPointers.CheckIPServer)
			{	
				MySocket = TCPOpen((DWORD)(ROM_PTR_BASE)DDNSClient.CheckIPServer.szROM, TCP_OPEN_ROM_HOST,
					DDNSClient.CheckIPPort, TCP_PURPOSE_DEFAULT);
			}
			else
			{
				MySocket = TCPOpen((DWORD)(PTR_BASE)DDNSClient.CheckIPServer.szRAM, TCP_OPEN_RAM_HOST,
					DDNSClient.CheckIPPort, TCP_PURPOSE_DEFAULT);						
			}
			
			// If no socket available, try again on next loop
			if(MySocket == INVALID_SOCKET)
				break;

			smDDNS++;
			Timer = TickGet();
			break;

		case SM_CHECKIP_SKT_OBTAINED:

			// Wait for the remote server to accept our connection request
			if(!TCPIsConnected(MySocket))
			{
				// Time out if too much time is spent in this state
				if(TickGet()-Timer > 6*TICK_SECOND)
				{
					// Close the socket so it can be used by other modules
					// We will retry soon
					TCPDisconnect(MySocket);
					MySocket = INVALID_SOCKET;
					lastStatus = DDNS_STATUS_CHECKIP_ERROR;
					smDDNS = SM_SOFT_ERROR;
				}
				break;
			}

			Timer = TickGet();

			// Make certain the socket can be written to
			if(TCPIsPutReady(MySocket) < 125u)//125 = size of TCP Tx buffer
				break;
			
			// Transmit the request to the server
			TCPPutROMString(MySocket, (ROM BYTE*)"GET / HTTP/1.0\r\nHost: ");

			if(DDNSClient.ROMPointers.CheckIPServer)
			{
				TCPPutROMString(MySocket, DDNSClient.CheckIPServer.szROM);
			}
			else
			{
				TCPPutString(MySocket, DDNSClient.CheckIPServer.szRAM);
			}

			TCPPutROMString(MySocket, (ROM BYTE*)"\r\nConnection: close\r\n\r\n");

			// Send the packet
			TCPFlush(MySocket);
			smDDNS++;
			break;

		case SM_CHECKIP_FIND_DELIMITER:

			// Check if remote node is still connected.  If not, force to the disconnect state,
			// but don't break because data may still be waiting.
			if(!TCPIsConnected(MySocket) || TickGet() - Timer > 6*TICK_SECOND)
				smDDNS = SM_CHECKIP_DISCONNECT;

			// Search out the "Address: " delimiter in the response
			wPos = TCPFindROMArray(MySocket, (ROM BYTE*)"Address: ", 9, 0, FALSE);
			
			// If not yet found, clear as much as possible and break
			if(wPos == 0xffff)
			{
				wPos = TCPIsGetReady(MySocket);
				if(wPos > 9u)
					TCPGetArray(MySocket, NULL, wPos - 9);
				break;
			}
				
			// Clear up to and past that string
			TCPGetArray(MySocket, NULL, wPos + 9);
		
			// Continue on to read the IP
			Timer = TickGet();
			smDDNS++;
		
		case SM_CHECKIP_FIND_ADDRESS:
			
			// Check if remote node is still connected.  If not, force to the disconnect state,
			// but don't break because data may still be waiting.
			if(!TCPIsConnected(MySocket) || TickGet() - Timer > 6*TICK_SECOND)
				smDDNS = SM_CHECKIP_DISCONNECT;

			// Search out the "</body>" delimiter in the response
			wPos = TCPFindROMArray(MySocket, (ROM BYTE*)"</body>", 7, 0, FALSE);
			
			// If not yet found, break
			if(wPos == 0xffff)
				break;
				
			// Read and terminate that string as the IP address (preventing buffer overflows)
			if(wPos > 15u)
				wPos = 15;
			TCPGetArray(MySocket, vBuffer, wPos);
			vBuffer[wPos] = '\0';
			
			// Parse the IP address that was read, invalidating on failure
			if(!StringToIPAddress(vBuffer, &ipParsed))
				vBuffer[0] = '\0';

			// Continue on to close the socket			
			
		case SM_CHECKIP_DISCONNECT:

			// Close the socket
			TCPDisconnect(MySocket);
			MySocket = INVALID_SOCKET;

			// Determine if an update is necessary
			if(vBuffer[0] == '\0')
			{// CheckIP Failed
				lastStatus = DDNS_STATUS_CHECKIP_ERROR;
				smDDNS = SM_SOFT_ERROR;
				break;
			}

			if( (ipParsed.Val ==lastKnownIP.Val) && (!bForceUpdate))
			{
				// IP address has not changed and no update is forced
				lastStatus = DDNS_STATUS_UNCHANGED;
				smDDNS = SM_DONE;
				break;
			}
			
			// Need to perform an update
			lastKnownIP = ipParsed;
			bForceUpdate = FALSE;
			smDDNS++;
			break;
			 
		case SM_IP_UPDATE_HOME:

			// Connect a socket to the remote server
			if(DDNSClient.ROMPointers.UpdateServer)
			{
				MySocket = TCPOpen((DWORD)(ROM_PTR_BASE)DDNSClient.UpdateServer.szROM, TCP_OPEN_ROM_HOST, 
					DDNSClient.UpdatePort, TCP_PURPOSE_DEFAULT);
			}
			else
			{
				MySocket = TCPOpen((DWORD)(PTR_BASE)DDNSClient.UpdateServer.szRAM, TCP_OPEN_RAM_HOST,
					DDNSClient.UpdatePort, TCP_PURPOSE_DEFAULT);
			}
	
			// If no socket is available, try again on the next loop
			if(MySocket == INVALID_SOCKET)
				break;
			
			// Move on to the next state
			smDDNS++;
			Timer = TickGet();
			break;

		case SM_IP_UPDATE_SKT_OBTAINED:
		
			// Wait for the remote server to accept our connection request
			if(!TCPIsConnected(MySocket))
			{
				// Time out if too much time is spent in this state
				if(TickGet() - Timer > 6*TICK_SECOND)
				{
					// Close the socket so it can be used by other modules
					// We will try again immediately
					TCPDisconnect(MySocket);
					MySocket = INVALID_SOCKET;
					lastStatus = DDNS_STATUS_UPDATE_ERROR;
					smDDNS--;
				}
				break;
			}
			
			// Reset timer and begin sending the request
			Timer = TickGet();
			smDDNS++;
			// No break needed...try to send first bit immediately.

		case SM_IP_UPDATE_REQ_A:
	
			// Check for lost connections or timeouts
			if(!TCPIsConnected(MySocket) || (TickGet() - Timer > 10*TICK_SECOND))
			{
				lastStatus = DDNS_STATUS_UPDATE_ERROR;
				smDDNS = SM_IPUDATE_DISCONNECT;
				break;
			}
			
			if(TCPIsPutReady(MySocket) < 25u)  // 25 =~ 16+9
				break;

			TCPPutROMString(MySocket, (ROM BYTE*)"GET /nic/update?hostname=");
			smDDNS++;
			// No break needed...try to send next bit immediately.
			
		case SM_IP_UPDATE_REQ_B:

			// Check for lost connections or timeouts
			if(!TCPIsConnected(MySocket) || (TickGet() - Timer > 10*TICK_SECOND))
			{
				lastStatus = DDNS_STATUS_UPDATE_ERROR;
				smDDNS = SM_IPUDATE_DISCONNECT;
				break; 
			}

			// Try to write, verifying that space is available first
			if(DDNSClient.ROMPointers.Host)
			{
				if(TCPIsPutReady(MySocket) < strlenpgm((ROM char*)DDNSClient.Host.szROM))
					break;
				TCPPutROMString(MySocket,DDNSClient.Host.szROM);
			}
			else
			{
				if(TCPIsPutReady(MySocket) < strlen((char*)DDNSClient.Host.szRAM))
					break;
				TCPPutString(MySocket,DDNSClient.Host.szRAM);
			}

			smDDNS++;
			// No break needed...try to send next bit immediately.
			
		case SM_IP_UPDATE_REQ_C:

			// Check for lost connections or timeouts
			if(!TCPIsConnected(MySocket) || TickGet() - Timer > 10*TICK_SECOND)
			{
				lastStatus = DDNS_STATUS_UPDATE_ERROR;
				smDDNS = SM_IPUDATE_DISCONNECT;
				break; 
			}

			if(TCPIsPutReady(MySocket) < 70u)
				break;
	
			TCPPutROMString(MySocket, (ROM BYTE*)"&myip=");
			TCPPutString(MySocket, vBuffer);
			TCPPutROMString(MySocket, (ROM BYTE*)"&wildcard=NOCHG&mx=NOCHG&backmx=NOCHG HTTP/1.0");	

			TCPFlush(MySocket);
			smDDNS++;
			// No break needed...try to send next bit immediately.

		case SM_IP_UPDATE_REQ_D:

			// Check for lost connections or timeouts
			if(!TCPIsConnected(MySocket) || TickGet() - Timer > 10*TICK_SECOND)
			{
				lastStatus = DDNS_STATUS_UPDATE_ERROR;
				smDDNS = SM_IPUDATE_DISCONNECT;
				break; 
			}
			
			if(TCPIsPutReady(MySocket) < 131u) // 131 =~ 8+23 + dynamic dns server hostname
				break;

			TCPPutROMString(MySocket, (ROM BYTE*)"\r\nHost: ");//8
			
			if(DDNSClient.ROMPointers.UpdateServer)
				TCPPutROMString(MySocket,DDNSClient.UpdateServer.szROM);
			else
				TCPPutString(MySocket,DDNSClient.UpdateServer.szRAM);
			
			TCPPutROMString(MySocket, (ROM BYTE*)"\r\nAuthorization: Basic ");//23

			TCPFlush(MySocket);
			smDDNS++;
			// No break needed...try to send the next bit immediately.

		case SM_IP_UPDATE_REQ_E:

			// Check for lost connections or timeouts
			if(!TCPIsConnected(MySocket) || TickGet() - Timer > 6*TICK_SECOND)
			{
				lastStatus = DDNS_STATUS_UPDATE_ERROR;
				smDDNS = SM_IPUDATE_DISCONNECT;
				break; 
			}
			
			// User name and passwords for DynDNS.org can each be up to 24 characters
			// Base64 encoded data is always at least 25% bigger than the original
			if(TCPIsPutReady(MySocket) < 100u)
				break;	

			if(DDNSClient.ROMPointers.Username)
			{
				ROMStrPtr = (ROM char*)DDNSClient.Username.szROM;
				wPos = strlenpgm(ROMStrPtr);
			}
			else
			{
				RAMStrPtr = (char*)DDNSClient.Username.szRAM;
				wPos = strlen((char*)RAMStrPtr);
			}

			i = 0;
			while(wPos)
			{
				while(i < wPos && i < 3u)
				{
					if(DDNSClient.ROMPointers.Username)
						vBuffer[i] = *ROMStrPtr++;
					else
						vBuffer[i] = *RAMStrPtr++;
					i++;
				}
				wPos -= i; 				
										
				if(i == 3u)
				{
					Base64Encode(vBuffer, i, vBuffer, 4);
					TCPPutArray(MySocket, vBuffer, 4);
					i = 0;
				}
			}

			if(DDNSClient.ROMPointers.Password)
			{		
				ROMStrPtr = (ROM char*)DDNSClient.Password.szROM;
				wPos = strlenpgm(ROMStrPtr);
			}
			else
			{
				RAMStrPtr = (char*)DDNSClient.Password.szRAM;
				wPos = strlen((char*)RAMStrPtr);
			}

		  	// Increment for the ':' separator and i for bytes left in username
		  	wPos += i + 1;
			
			vBuffer[i++] = ':';

			while(wPos)
			{
				while(i < wPos && i < 3u)
				{
					if(DDNSClient.ROMPointers.Password)
						vBuffer[i] = *ROMStrPtr++;
					else
						vBuffer[i] = *RAMStrPtr++;
					i++;
				}
				wPos -= i; 				
				Base64Encode(vBuffer, i, vBuffer, 4);
				TCPPutArray(MySocket, vBuffer, 4);
				i = 0;
			}
			
			TCPFlush(MySocket);
			smDDNS++;
			break;

			
		case SM_IP_UPDATE_REQ_F:

			// Check for lost connections or timeouts
			if(!TCPIsConnected(MySocket) || TickGet() - Timer > 10*TICK_SECOND)
			{
				lastStatus = DDNS_STATUS_UPDATE_ERROR;
				smDDNS = SM_IPUDATE_DISCONNECT;
				break; 
			}
			
			if(TCPIsPutReady(MySocket) < 50u)
				break;
			
			TCPPutROMString(MySocket, (ROM BYTE*)"\r\nUser-Agent: Microchip - TCPIPSTACK - "TCPIP_STACK_VERSION"\r\n\r\n");
			TCPFlush(MySocket);
			smDDNS++;
			
			// Reset the timer to wait for a response
			Timer = TickGet();
			break;
								
		case SM_IPUPDATE_FIND_RESPONSE:
			// Locate the response string

			// Wait up to 10 seconds for a response
			if(TickGet() - Timer > 10*TICK_SECOND)
			{
				lastStatus = DDNS_STATUS_UPDATE_ERROR;
				smDDNS = SM_IPUDATE_DISCONNECT;
				break;
			}
		
			// According to HTTP, the response will start after the two CRLFs
			wPos = TCPFindROMArray(MySocket, (ROM BYTE*)"\r\n\r\n", 4, 0, FALSE);

			// If not yet found, eliminate everything up to
			if(wPos == 0xffff)
			{
				wPos = TCPIsGetReady(MySocket);
				if(wPos > 4u)
					TCPGetArray(MySocket, NULL, wPos - 4);
				break;
			}
				
			TCPGetArray(MySocket, NULL, wPos+4);
			smDDNS++;
			// No break...continue to next state immediately
			
		case SM_IPUPDATE_PARSE_RESPONSE:
			// Try to parse the response text
			
			// Wait up to 10 seconds for the remote server to disconnect
			// so we know all data has been received
			if(TCPIsConnected(MySocket) && TickGet() - Timer < 10*TICK_SECOND)
				break;
			
			// Read the response code
		 	wPos = TCPIsGetReady(MySocket);
		 	if(wPos > sizeof(vBuffer) - 1)
		 		wPos = sizeof(vBuffer) - 1;

			wPos = TCPGetArray(MySocket, vBuffer, wPos);
			vBuffer[wPos] = '\0';
			for(i = 0; i < sizeof(vBuffer); i++)
				if(vBuffer[i] == ' ')
					vBuffer[i] = '\0';

			for(lastStatus = 0; lastStatus <= DDNS_STATUS_UPDATE_ERROR; lastStatus++)
				if(!strcmppgm2ram((char*)vBuffer, (ROM char*)_updateIpSrvrResponse[lastStatus]))
					break;
		
			smDDNS++;
			// No break...continue to finalization

		case SM_IPUDATE_DISCONNECT:
			// Close the socket so it can be used by other modules.
			if(MySocket != INVALID_SOCKET)
			{
				TCPDisconnect(MySocket);
				MySocket = INVALID_SOCKET;
			}
			
			// Determine what to do based on status
			if(lastStatus <= DDNS_STATUS_NUMHOST || lastStatus == DDNS_STATUS_UNCHANGED)
				smDDNS = SM_DONE;
			else if(lastStatus == DDNS_STATUS_911 || lastStatus == DDNS_STATUS_DNSERR)
				smDDNS = SM_SYSTEM_ERROR;
			else
				smDDNS = SM_SOFT_ERROR;
			
			smDDNS++;
			break;
			
		case SM_DONE:
			dwUpdateAt = TickGet() + 10*60*TICK_SECOND;	// 10 minutes
			smDDNS = SM_IDLE;
			break;
			
		case SM_SOFT_ERROR:
			dwUpdateAt = TickGet() + 30*TICK_SECOND; 		// 30 seconds
			smDDNS = SM_IDLE;
			break;
					
		case SM_SYSTEM_ERROR:
			dwUpdateAt = TickGet() + 30*60*TICK_SECOND;		// 30 minutes
			smDDNS = SM_IDLE;
			break;
	}
}

/*****************************************************************************
  Function:
	void DDNSForceUpdate(void)

  Summary:
	Forces an immediate DDNS update

  Description:
	This function forces the DDNS Client to execute a full update
	immediately.  Any error message is cleared, and the update will be
	executed whether the IP address has changed or not.  Call this 
	function every time the DDNSClient parameters have been modified.

  Precondition:
	DDNSInit must have been called.

  Parameters:
	None

  Returns:
  	None
  ***************************************************************************/
void DDNSForceUpdate(void)
{
	// Force update on next DDNSClient call
	dwUpdateAt = TickGet();
	bForceUpdate = TRUE;
	lastStatus = DDNS_STATUS_UNKNOWN;
}

/*****************************************************************************
  Function:
	void DDNSSetService(DDNS_SERVICES svc)

  Summary:
	Selects a pre-configured Dynamic DNS service

  Description:
	This function selects a Dynamic DNS service based on parameters 
	configured in ddnsServiceHosts and ddnsServicePorts.  These arrays
	must match the DDNS_SERVICES enumeration.

  Precondition:
	None

  Parameters:
	svc - one of the DDNS_SERVICES elements to indicate the selected service

  Returns:
	None
  ***************************************************************************/
void DDNSSetService(DDNS_SERVICES svc)
{
	DDNSClient.ROMPointers.UpdateServer = 1;
	DDNSClient.UpdateServer.szROM = (ROM BYTE*)ddnsServiceHosts[svc];
	DDNSClient.UpdatePort = ddnsServicePorts[svc];
}


/*****************************************************************************
  Function:
	IP_ADDR DDNSGetLastIP(void)

  Summary:
	Returns the last known external IP address of the device.

  Description:
	This function returns the last known external IP address of the device.

 Precondition:
	None

  Parameters:
	None

  Returns:
	The last known external IP address of the device.
  ***************************************************************************/
IP_ADDR DDNSGetLastIP(void)
{
	return lastKnownIP;
}



/*****************************************************************************
  Function:
	DDNS_STATUS DDNSGetLastStatus(void)

  Summary:
	Returns the status of the most recent update.

  Description:
	This function returns the status of the most recent update.  See the
	DDNS_STATUS enumeration for possible codes.

  Precondition:
	None

  Parameters:
	None

  Returns:
	DDNS_STATUS indicating the status code for the most recent update.	
  ***************************************************************************/

DDNS_STATUS DDNSGetLastStatus(void)
{
	return lastStatus;	
}

#endif //STACK_USE_DYNAMICDNS_CLIENT
#endif //__DYNDNSCLIENT_C

