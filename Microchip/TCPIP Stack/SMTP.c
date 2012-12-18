/*********************************************************************
 *
 *	Simple Mail Transfer Protocol (SMTP) Client
 *	Module for Microchip TCP/IP Stack
 *   -Provides ability to send Emails
 *	 -Reference: RFC 2821
 *
 *********************************************************************
 * FileName:        SMTP.c
 * Dependencies:    TCP, ARP, DNS, Tick
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
 * Howard Schlunder     3/03/06	Original
 * Howard Schlunder		11/2/06	Vastly improved for release
 ********************************************************************/
#define __SMTP_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_SMTP_CLIENT)

#include "TCPIP Stack/TCPIP.h"


/****************************************************************************
  Section:
	SMTP Client Configuration Parameters
  ***************************************************************************/
#define SMTP_PORT					25					// Default port to use when unspecified
#define SMTP_SERVER_REPLY_TIMEOUT	(TICK_SECOND*8)		// How long to wait before assuming the connection has been dropped (default 8 seconds)


/****************************************************************************
  Section:
	SMTP Client Public Variables
  ***************************************************************************/
// The global set of SMTP_POINTERS.
// Set these parameters after calling SMTPBeginUsage successfully.
SMTP_POINTERS SMTPClient;	

/****************************************************************************
  Section:
	SMTP Client Internal Variables
  ***************************************************************************/
static IP_ADDR SMTPServer;						// IP address of the remote SMTP server
static TCP_SOCKET MySocket = INVALID_SOCKET;	// Socket currently in use by the SMTP client

// State machine for the CR LF Period replacement
// Used by SMTPPut to transparently replace "\r\n." with "\r\n.."
static union
{
	BYTE *Pos;
	enum
	{
		CR_PERIOD_SEEK_CR = 0,		// Idle state, waiting for '\r'
		CR_PERIOD_SEEK_LF,			// "\r" has been written, so check next byte for '\n'
		CR_PERIOD_SEEK_PERIOD,		// "\r\n" has been written, so check next byte for '.'
		CR_PERIOD_NEED_INSERTION	// "\r\n." has been written, so an additional '.'
									//   must be written before the next byte.
	} State;
} CRPeriod;

// State of the transport for the SMTP Client
static enum
{
	TRANSPORT_HOME = 0,			// Idle state
	TRANSPORT_BEGIN,			// Preparing to make connection
	TRANSPORT_NAME_RESOLVE,		// Resolving the SMTP server address
	TRANSPORT_OBTAIN_SOCKET,	// Obtaining a socket for the SMTP connection
	#if defined(STACK_USE_SSL_CLIENT)
	TRANSPORT_SECURING_SOCKET,	// Securing the socket for the SMTP over SSL connection
	#endif
	TRANSPORT_SOCKET_OBTAINED,	// SMTP connection successful
	TRANSPORT_CLOSE				// STMP socket is closed
} TransportState = TRANSPORT_HOME;

// Message state machine for the SMTP Client
static enum
{
	SMTP_HOME = 0,				// Idle start state for SMTP client (application is preparing message)
	SMTP_HELO,					// HELO is being sent to server
	SMTP_HELO_ACK,				// Received an ACK for the HELO
	SMTP_AUTH_LOGIN,			// Requesting to log in
	SMTP_AUTH_LOGIN_ACK,		// Log in request accepted
	SMTP_AUTH_USERNAME,			// Sending user name
	SMTP_AUTH_USERNAME_ACK,		// User name accepted
	SMTP_AUTH_PASSWORD,			// Sending password
	SMTP_AUTH_PASSWORD_ACK,		// Password was accepted
	SMTP_MAILFROM,				// Sending inital MAIL FROM command
	SMTP_MAILFROM_ACK,			// MAIL FROM was accepted
	SMTP_RCPTTO_INIT,			// Preparing to send RCPT TO
	SMTP_RCPTTO,				// Sending RCPT TO command
	SMTP_RCPTTO_ACK,			// RCPT TO was accepted
	SMTP_RCPTTO_ISDONE,			// Done sending RCPT TO commands
	SMTP_RCPTTOCC_INIT,			// Preparing to send RCPT TO CC commands
	SMTP_RCPTTOCC,				// Sending RCPT TO CC commands
	SMTP_RCPTTOCC_ACK,			// RCPT TO CC was accepted
	SMTP_RCPTTOCC_ISDONE,		// Done sending RCPT TO CC
	SMTP_RCPTTOBCC_INIT,		// Preparing to send RCPT TO BCC commands
	SMTP_RCPTTOBCC,				// Sending RCPT TO BCC commands
	SMTP_RCPTTOBCC_ACK,			// RCPT TO BCC was accepted
	SMTP_RCPTTOBCC_ISDONE,		// Done sending RCPT TO BCC
	SMTP_DATA,					// Sending DATA command
	SMTP_DATA_ACK,				// DATA command accpted
	SMTP_DATA_HEADER,			// Sending message headers
	SMTP_DATA_BODY_INIT,		// Preparing for message body
	SMTP_DATA_BODY,				// Sending message body
	SMTP_DATA_BODY_ACK,			// Message body accepted
	SMTP_QUIT_INIT,				// Sending QUIT command
	SMTP_QUIT					// QUIT accepted, connection closing
} SMTPState;

// State machine for writing the SMTP message headers
static enum
{
	PUTHEADERS_FROM_INIT = 0,	// Preparing to send From header
	PUTHEADERS_FROM,			// Sending From header
	PUTHEADERS_TO_INIT,			// Preparing to send To header
	PUTHEADERS_TO,				// Sending To header
	PUTHEADERS_CC_INIT,			// Preparing to send CC header
	PUTHEADERS_CC,				// Sending CC header
	PUTHEADERS_SUBJECT_INIT,	// Preparing to send Subject header
	PUTHEADERS_SUBJECT,			// Sending Subject header
	PUTHEADERS_OTHER_INIT,		// Preparing to send additional headers
	PUTHEADERS_OTHER,			// Sending additional headers
	PUTHEADERS_DONE				// Done writing all headers
} PutHeadersState;

// State machine for parsing incoming responses
static enum
{
	RX_BYTE_0 = 0,
	RX_BYTE_1,
	RX_BYTE_2,
	RX_BYTE_3,
	RX_SEEK_CR,
	RX_SEEK_LF
} RXParserState;

// Internal flags used by the SMTP Client
static union
{
	BYTE Val;
	struct
	{
		unsigned char RXSkipResponse:1;
		unsigned char SMTPInUse:1;
		unsigned char SentSuccessfully:1;
		unsigned char ReadyToStart:1;
		unsigned char ReadyToFinish:1;
		unsigned char ConnectedOnce:1;
		unsigned char filler:2;
	} bits;
} SMTPFlags = {0x00};
	
// Response code from server when an error exists
static WORD ResponseCode;

/****************************************************************************
  Section:
	SMTP Client Internal Function Prototypes
  ***************************************************************************/
static BYTE *FindEmailAddress(BYTE *str, WORD *wLen);
static ROM BYTE *FindROMEmailAddress(ROM BYTE *str, WORD *wLen);

/****************************************************************************
  Section:
	SMTP Function Implementations
  ***************************************************************************/

/*****************************************************************************
  Function:
	BOOL SMTPBeginUsage(void)

  Summary:
	Requests control of the SMTP client module.

  Description:
	Call this function before calling any other SMTP Client APIs.  This 
	function obtains a lock on the SMTP Client, which can only be used by
	one stack application at a time.  Once the application is finished
	with the SMTP client, it must call SMTPEndUsage to release control
	of the module to any other waiting applications.
	
	This function initializes all the SMTP state machines and variables
	back to their default state.

  Precondition:
	None

  Parameters:
	None

  Return Values:
	TRUE - The application has successfully obtained control of the module
	FALSE - The SMTP module is in use by another application.  Call 
		SMTPBeginUsage again later, after returning to the main program loop
  ***************************************************************************/
BOOL SMTPBeginUsage(void)
{
	if(SMTPFlags.bits.SMTPInUse)
		return FALSE;

	SMTPFlags.Val = 0x00;
	SMTPFlags.bits.SMTPInUse = TRUE;
	TransportState = TRANSPORT_BEGIN;
	RXParserState = RX_BYTE_0;
	SMTPState = SMTP_HOME;
	memset((void*)&SMTPClient, 0x00, sizeof(SMTPClient));
	SMTPClient.ServerPort = SMTP_PORT;
		
	return TRUE;
}

/*****************************************************************************
  Function:
	WORD SMTPEndUsage(void)

  Summary:
	Releases control of the SMTP client module.

  Description:
	Call this function to release control of the SMTP client module once
	an application is finished using it.  This function releases the lock
	obtained by SMTPBeginUsage, and frees the SMTP client to be used by 
	another application.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	None

  Return Values:
	SMTP_SUCCESS - A message was successfully sent
	SMTP_RESOLVE_ERROR - The SMTP server could not be resolved
	SMTP_CONNECT_ERROR - The connection to the SMTP server failed or was
		prematurely terminated
	1-199 and 300-399 - The last SMTP server response code
  ***************************************************************************/
WORD SMTPEndUsage(void)
{
	if(!SMTPFlags.bits.SMTPInUse)
		return 0xFFFF;

	// Release the DNS module, if in use
	if(TransportState == TRANSPORT_NAME_RESOLVE)
		DNSEndUsage();
	
	// Release the TCP socket, if in use
	if(MySocket != INVALID_SOCKET)
	{
		TCPDisconnect(MySocket);
		MySocket = INVALID_SOCKET;
	}
	
	// Release the SMTP module
	SMTPFlags.bits.SMTPInUse = FALSE;
	TransportState = TRANSPORT_HOME;

	if(SMTPFlags.bits.SentSuccessfully)
	{
		return 0;
	}
	else
	{
		return ResponseCode;
	}
}

/*****************************************************************************
  Function:
	void SMTPTask(void)

  Summary:
	Performs any pending SMTP client tasks

  Description:
	This function handles periodic tasks associated with the SMTP client,
	such as processing initial connections and command sequences.

  Precondition:
	None

  Parameters:
	None

  Returns:
	None

  Remarks:
	This function acts as a task (similar to one in an RTOS).  It
	performs its task in a co-operative manner, and the main application
	must call this function repeatedly to ensure that all open or new
	connections are served in a timely fashion.
  ***************************************************************************/
void SMTPTask(void)
{
	BYTE			i;
	WORD			w;
	BYTE			vBase64Buffer[4];
	static DWORD	Timer;
	static BYTE		RXBuffer[4];
	static ROM BYTE *ROMStrPtr, *ROMStrPtr2;
	static BYTE 	*RAMStrPtr;
	static WORD		wAddressLength;

	switch(TransportState)
	{
		case TRANSPORT_HOME:
			// SMTPBeginUsage() is the only function which will kick 
			// the state machine into the next state
			break;

		case TRANSPORT_BEGIN:
			// Wait for the user to program all the pointers and then 
			// call SMTPSendMail()
			if(!SMTPFlags.bits.ReadyToStart)
				break;

			// Obtain ownership of the DNS resolution module
			if(!DNSBeginUsage())
				break;

			// Obtain the IP address associated with the SMTP mail server
			if(SMTPClient.Server.szRAM || SMTPClient.Server.szROM)
			{
				if(SMTPClient.ROMPointers.Server)
					DNSResolveROM(SMTPClient.Server.szROM, DNS_TYPE_A);
				else
					DNSResolve(SMTPClient.Server.szRAM, DNS_TYPE_A);
			}
			else
			{
				// If we don't have a mail server, try to send the mail 
				// directly to the destination SMTP server
				if(SMTPClient.To.szRAM && !SMTPClient.ROMPointers.To)
				{
					SMTPClient.Server.szRAM = (BYTE*)strchr((char*)SMTPClient.To.szRAM, '@');
					SMTPClient.ROMPointers.Server = 0;
				}
				else if(SMTPClient.To.szROM && SMTPClient.ROMPointers.To)
				{
					SMTPClient.Server.szROM = (ROM BYTE*)strchrpgm((ROM char*)SMTPClient.To.szROM, '@');
					SMTPClient.ROMPointers.Server = 1;
				}

				if(!(SMTPClient.Server.szRAM || SMTPClient.Server.szROM))
				{
					if(SMTPClient.CC.szRAM && !SMTPClient.ROMPointers.CC)
					{
						SMTPClient.Server.szRAM = (BYTE*)strchr((char*)SMTPClient.CC.szRAM, '@');
						SMTPClient.ROMPointers.Server = 0;
					}
					else if(SMTPClient.CC.szROM && SMTPClient.ROMPointers.CC)
					{
						SMTPClient.Server.szROM = (ROM BYTE*)strchrpgm((ROM char*)SMTPClient.CC.szROM, '@');
						SMTPClient.ROMPointers.Server = 1;
					}
				}

				if(!(SMTPClient.Server.szRAM || SMTPClient.Server.szROM))
				{
					if(SMTPClient.BCC.szRAM && !SMTPClient.ROMPointers.BCC)
					{
						SMTPClient.Server.szRAM = (BYTE*)strchr((char*)SMTPClient.BCC.szRAM, '@');
						SMTPClient.ROMPointers.Server = 0;
					}
					else if(SMTPClient.BCC.szROM && SMTPClient.ROMPointers.BCC)
					{
						SMTPClient.Server.szROM = (ROM BYTE*)strchrpgm((ROM char*)SMTPClient.BCC.szROM, '@');
						SMTPClient.ROMPointers.Server = 1;
					}
				}

				// See if we found a hostname anywhere which we could resolve
				if(!(SMTPClient.Server.szRAM || SMTPClient.Server.szROM))
				{
					DNSEndUsage();
					ResponseCode = SMTP_RESOLVE_ERROR;
					TransportState = TRANSPORT_HOME;
					break;
				}

				// Skip over the @ sign and resolve the host name
				if(SMTPClient.ROMPointers.Server)
				{
					SMTPClient.Server.szROM++;
					DNSResolveROM(SMTPClient.Server.szROM, DNS_TYPE_MX);
				}
				else
				{
					SMTPClient.Server.szRAM++;
					DNSResolve(SMTPClient.Server.szRAM, DNS_TYPE_MX);
				}
			}
			
			Timer = TickGet();
			TransportState++;
			break;

		case TRANSPORT_NAME_RESOLVE:
			// Wait for the DNS server to return the requested IP address
			if(!DNSIsResolved(&SMTPServer))
			{
				// Timeout after 6 seconds of unsuccessful DNS resolution
				if(TickGet() - Timer > 6*TICK_SECOND)
				{
					ResponseCode = SMTP_RESOLVE_ERROR;
					TransportState = TRANSPORT_HOME;
					DNSEndUsage();
				}
				break;
			}

			// Release the DNS module, we no longer need it
			if(!DNSEndUsage())
			{
				// An invalid IP address was returned from the DNS 
				// server.  Quit and fail permanantly if host is not valid.
				ResponseCode = SMTP_RESOLVE_ERROR;
				TransportState = TRANSPORT_HOME;
				break;
			}

			TransportState++;
			// No need to break here

		case TRANSPORT_OBTAIN_SOCKET:
			// Connect a TCP socket to the remote SMTP server
			MySocket = TCPOpen(SMTPServer.Val, TCP_OPEN_IP_ADDRESS, SMTPClient.ServerPort, TCP_PURPOSE_DEFAULT);
			
			// Abort operation if no TCP sockets are available
			// If this ever happens, add some more 
			// TCP_PURPOSE_DEFAULT sockets in TCPIPConfig.h
			if(MySocket == INVALID_SOCKET)
				break;

			TransportState++;
			Timer = TickGet();
			// No break; fall into TRANSPORT_SOCKET_OBTAINED
			
		#if defined(STACK_USE_SSL_CLIENT)
		case TRANSPORT_SECURING_SOCKET:		
			if(!TCPIsConnected(MySocket))
			{
				// Don't stick around in the wrong state if the
				// server was connected, but then disconnected us.
				// Also time out if we can't establish the connection 
				// to the SMTP server
				if((LONG)(TickGet()-Timer) > (LONG)(SMTP_SERVER_REPLY_TIMEOUT))
				{
					ResponseCode = SMTP_CONNECT_ERROR;
					TransportState = TRANSPORT_CLOSE;
				}

				break;
			}
			SMTPFlags.bits.ConnectedOnce = TRUE;
			
			// Start SSL if needed for this connection
			if(SMTPClient.UseSSL && !TCPStartSSLClient(MySocket,NULL))
				break;
			
			// Move on to main state
			Timer = TickGet();
			TransportState++;
			break;		
		#endif

		case TRANSPORT_SOCKET_OBTAINED:
			if(!TCPIsConnected(MySocket))
			{
				// Don't stick around in the wrong state if the
				// server was connected, but then disconnected us.
				// Also time out if we can't establish the connection 
				// to the SMTP server
				if(SMTPFlags.bits.ConnectedOnce || ((LONG)(TickGet()-Timer) > (LONG)(SMTP_SERVER_REPLY_TIMEOUT)))
				{
					ResponseCode = SMTP_CONNECT_ERROR;
					TransportState = TRANSPORT_CLOSE;
				}

				break;
			}
			SMTPFlags.bits.ConnectedOnce = TRUE;
			
			#if defined(STACK_USE_SSL_CLIENT)
			// Make sure the SSL handshake has completed
			if(SMTPClient.UseSSL && TCPSSLIsHandshaking(MySocket))
				break;
			#endif

			// See if the server sent us anything
			while(TCPIsGetReady(MySocket))
			{
				TCPGet(MySocket, &i);
				switch(RXParserState)
				{
					case RX_BYTE_0:
					case RX_BYTE_1:
					case RX_BYTE_2:
						RXBuffer[RXParserState] = i;
						RXParserState++;
						break;
	
					case RX_BYTE_3:
						switch(i)
						{
							case ' ':
								SMTPFlags.bits.RXSkipResponse = FALSE;
								RXParserState++;
								break;
							case '-':
								SMTPFlags.bits.RXSkipResponse = TRUE;
								RXParserState++;
								break;
							case '\r':
								RXParserState = RX_SEEK_LF;
								break;
						}
						break;
	
					case RX_SEEK_CR:
						if(i == '\r')
							RXParserState++;
						break;
	
					case RX_SEEK_LF:
						// If we received the whole command
						if(i == '\n')
						{
							RXParserState = RX_BYTE_0;

							if(!SMTPFlags.bits.RXSkipResponse)
							{
								// The server sent us a response code
								// Null terminate the ASCII reponse code so we can convert it to an integer
								RXBuffer[3] = 0;
								ResponseCode = atoi((char*)RXBuffer);

								// Handle the response
								switch(SMTPState)
								{
									case SMTP_HELO_ACK:
										if(ResponseCode >= 200u && ResponseCode <= 299u)
										{
											if(SMTPClient.Username.szRAM || SMTPClient.Username.szROM)
												SMTPState = SMTP_AUTH_LOGIN;
											else
												SMTPState = SMTP_MAILFROM;
										}
										else
											SMTPState = SMTP_QUIT_INIT;
										break;


									case SMTP_AUTH_LOGIN_ACK:
									case SMTP_AUTH_USERNAME_ACK:
										if(ResponseCode == 334u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;

									case SMTP_AUTH_PASSWORD_ACK:
										if(ResponseCode == 235u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;

									case SMTP_HOME:
									case SMTP_MAILFROM_ACK:
									case SMTP_RCPTTO_ACK:
									case SMTP_RCPTTOCC_ACK:
									case SMTP_RCPTTOBCC_ACK:
										if(ResponseCode >= 200u && ResponseCode <= 299u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;
							
									case SMTP_DATA_ACK:
										if(ResponseCode == 354u)
											SMTPState++;
										else
											SMTPState = SMTP_QUIT_INIT;
										break;
							
									case SMTP_DATA_BODY_ACK:
										if(ResponseCode >= 200u && ResponseCode <= 299u)
											SMTPFlags.bits.SentSuccessfully = TRUE;
							
										SMTPState = SMTP_QUIT_INIT;
										break;

									// Default case needed to supress compiler diagnostics
									default:
										break;
								}
							}
						}
						else if(i != '\r')
							RXParserState--;
	
						break;
				}
			}

			// Generate new data in the TX buffer, as needed, if possible
			if(TCPIsPutReady(MySocket) < 64u)
				break;

			switch(SMTPState)
			{
				case SMTP_HELO:
					if(SMTPClient.Username.szROM == NULL)
						TCPPutROMString(MySocket, (ROM BYTE*)"HELO MCHPBOARD\r\n");
					else
						TCPPutROMString(MySocket, (ROM BYTE*)"EHLO MCHPBOARD\r\n");
					TCPFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_AUTH_LOGIN:
					// Note: This state is only entered from SMTP_HELO_ACK if the application 
					// has specified a Username to use (either SMTPClient.Username.szROM or 
					// SMTPClient.Username.szRAM is non-NULL)
					TCPPutROMString(MySocket, (ROM BYTE*)"AUTH LOGIN\r\n");
					TCPFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_AUTH_USERNAME:
					// Base 64 encode and transmit the username.
					if(SMTPClient.ROMPointers.Username)
					{
						ROMStrPtr = SMTPClient.Username.szROM;
						w = strlenpgm((ROM char*)ROMStrPtr);
					}
					else
					{
						RAMStrPtr = SMTPClient.Username.szRAM;
						w = strlen((char*)RAMStrPtr);
					}

					while(w)
					{
						i = 0;
						while((i < w) && (i < sizeof(vBase64Buffer)*3/4))
						{
							if(SMTPClient.ROMPointers.Username)
								vBase64Buffer[i] = *ROMStrPtr++;
							else
								vBase64Buffer[i] = *RAMStrPtr++;
							i++;
						}
						w -= i;					
						Base64Encode(vBase64Buffer, i, vBase64Buffer, sizeof(vBase64Buffer));
						TCPPutArray(MySocket, vBase64Buffer, sizeof(vBase64Buffer));
					}
					TCPPutROMString(MySocket, (ROM BYTE*)"\r\n");
					TCPFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_AUTH_PASSWORD:
					// Base 64 encode and transmit the password
					if(SMTPClient.ROMPointers.Password)
					{
						ROMStrPtr = SMTPClient.Password.szROM;
						w = strlenpgm((ROM char*)ROMStrPtr);
					}
					else
					{
						RAMStrPtr = SMTPClient.Password.szRAM;
						w = strlen((char*)RAMStrPtr);
					}

					while(w)
					{
						i = 0;
						while((i < w) && (i < sizeof(vBase64Buffer)*3/4))
						{
							if(SMTPClient.ROMPointers.Password)
								vBase64Buffer[i] = *ROMStrPtr++;
							else
								vBase64Buffer[i] = *RAMStrPtr++;
							i++;
						}
						w -= i;					
						Base64Encode(vBase64Buffer, i, vBase64Buffer, sizeof(vBase64Buffer));
						TCPPutArray(MySocket, vBase64Buffer, sizeof(vBase64Buffer));
					}
					TCPPutROMString(MySocket, (ROM BYTE*)"\r\n");
					TCPFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_MAILFROM:
					// Send MAIL FROM header.  Note that this is for the SMTP server validation, 
					// not what actually will be displayed in the recipients mail client as a 
					// return address.
					TCPPutROMString(MySocket, (ROM BYTE*)"MAIL FROM:<");
					if(SMTPClient.ROMPointers.From)
					{
						ROMStrPtr = FindROMEmailAddress(SMTPClient.From.szROM, &wAddressLength);
						TCPPutROMArray(MySocket, ROMStrPtr, wAddressLength);
					}
					else
					{
						RAMStrPtr = FindEmailAddress(SMTPClient.From.szRAM, &wAddressLength);
						TCPPutArray(MySocket, RAMStrPtr, wAddressLength);
					}
					TCPPutROMString(MySocket, (ROM BYTE*)">\r\n");
					TCPFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_RCPTTO_INIT:
					// See if there are any (To) recipients to process
					if(SMTPClient.To.szRAM && !SMTPClient.ROMPointers.To)
					{
						RAMStrPtr = FindEmailAddress(SMTPClient.To.szRAM, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTO;
							break;
						}
					}
					if(SMTPClient.To.szROM && SMTPClient.ROMPointers.To)
					{
						ROMStrPtr = FindROMEmailAddress(SMTPClient.To.szROM, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTO;
							break;
						}
					}
					
					SMTPState = SMTP_RCPTTOCC_INIT;
					break;

				case SMTP_RCPTTO:
				case SMTP_RCPTTOCC:
				case SMTP_RCPTTOBCC:
					TCPPutROMString(MySocket, (ROM BYTE*)"RCPT TO:<");
					if(	(SMTPClient.ROMPointers.To  && (SMTPState == SMTP_RCPTTO)) || 
						(SMTPClient.ROMPointers.CC  && (SMTPState == SMTP_RCPTTOCC)) || 
						(SMTPClient.ROMPointers.BCC && (SMTPState == SMTP_RCPTTOBCC)) )
						TCPPutROMArray(MySocket, ROMStrPtr, wAddressLength);
					else
						TCPPutArray(MySocket, RAMStrPtr, wAddressLength);
					TCPPutROMString(MySocket, (ROM BYTE*)">\r\n");
					TCPFlush(MySocket);
					SMTPState++;
					break;

				case SMTP_RCPTTO_ISDONE:
					// See if we have any more (To) recipients to process
					// If we do, we must roll back a couple of states
					if(SMTPClient.ROMPointers.To)
						ROMStrPtr = FindROMEmailAddress(ROMStrPtr+wAddressLength, &wAddressLength);
					else
						RAMStrPtr = FindEmailAddress(RAMStrPtr+wAddressLength, &wAddressLength);
	
					if(wAddressLength)
					{
						SMTPState = SMTP_RCPTTO;
						break;
					}

					// All done with To field
					SMTPState++;
					//No break

				case SMTP_RCPTTOCC_INIT:
					// See if there are any Carbon Copy (CC) recipients to process
					if(SMTPClient.CC.szRAM && !SMTPClient.ROMPointers.CC)
					{
						RAMStrPtr = FindEmailAddress(SMTPClient.CC.szRAM, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTOCC;
							break;
						}
					}
					if(SMTPClient.CC.szROM && SMTPClient.ROMPointers.CC)
					{
						ROMStrPtr = FindROMEmailAddress(SMTPClient.CC.szROM, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTOCC;
							break;
						}
					}
					
					SMTPState = SMTP_RCPTTOBCC_INIT;
					break;

				case SMTP_RCPTTOCC_ISDONE:
					// See if we have any more Carbon Copy (CC) recipients to process
					// If we do, we must roll back a couple of states
					if(SMTPClient.ROMPointers.CC)
						ROMStrPtr = FindROMEmailAddress(ROMStrPtr+wAddressLength, &wAddressLength);
					else
						RAMStrPtr = FindEmailAddress(RAMStrPtr+wAddressLength, &wAddressLength);

					if(wAddressLength)
					{
						SMTPState = SMTP_RCPTTOCC;
						break;
					}

					// All done with CC field
					SMTPState++;
					//No break

				case SMTP_RCPTTOBCC_INIT:
					// See if there are any Blind Carbon Copy (BCC) recipients to process
					if(SMTPClient.BCC.szRAM && !SMTPClient.ROMPointers.BCC)
					{
						RAMStrPtr = FindEmailAddress(SMTPClient.BCC.szRAM, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTOBCC;
							break;
						}
					}
					if(SMTPClient.BCC.szROM && SMTPClient.ROMPointers.BCC)
					{
						ROMStrPtr = FindROMEmailAddress(SMTPClient.BCC.szROM, &wAddressLength);
						if(wAddressLength)
						{
							SMTPState = SMTP_RCPTTOBCC;
							break;
						}
					}

					// All done with BCC field
					SMTPState = SMTP_DATA;
					break;

				case SMTP_RCPTTOBCC_ISDONE:
					// See if we have any more Blind Carbon Copy (CC) recipients to process
					// If we do, we must roll back a couple of states
					if(SMTPClient.ROMPointers.BCC)
						ROMStrPtr = FindROMEmailAddress(ROMStrPtr+wAddressLength, &wAddressLength);
					else
						RAMStrPtr = FindEmailAddress(RAMStrPtr+wAddressLength, &wAddressLength);

					if(wAddressLength)
					{
						SMTPState = SMTP_RCPTTOBCC;
						break;
					}

					// All done with BCC field
					SMTPState++;
					//No break

				case SMTP_DATA:
					TCPPutROMString(MySocket, (ROM BYTE*)"DATA\r\n");
					SMTPState++;
					PutHeadersState = PUTHEADERS_FROM_INIT;
					TCPFlush(MySocket);
					break;

				case SMTP_DATA_HEADER:
					while((PutHeadersState != PUTHEADERS_DONE) && (TCPIsPutReady(MySocket) > 64u))
					{
						switch(PutHeadersState)
						{
							case PUTHEADERS_FROM_INIT:
								if(SMTPClient.From.szRAM || SMTPClient.From.szROM)
								{
									PutHeadersState = PUTHEADERS_FROM;
									TCPPutROMString(MySocket, (ROM BYTE*)"From: ");
								}
								else
								{
									PutHeadersState = PUTHEADERS_TO_INIT;
								}
								break;
								
							case PUTHEADERS_FROM:
								if(SMTPClient.ROMPointers.From)
								{
									SMTPClient.From.szROM = TCPPutROMString(MySocket, SMTPClient.From.szROM);
									if(*SMTPClient.From.szROM == 0u)
										PutHeadersState = PUTHEADERS_TO_INIT;
								}
								else
								{
									SMTPClient.From.szRAM = TCPPutString(MySocket, SMTPClient.From.szRAM);
									if(*SMTPClient.From.szRAM == 0u)
										PutHeadersState = PUTHEADERS_TO_INIT;
								}
								break;

							case PUTHEADERS_TO_INIT:
								if(SMTPClient.To.szRAM || SMTPClient.To.szROM)
								{
									PutHeadersState = PUTHEADERS_TO;
									TCPPutROMString(MySocket, (ROM BYTE*)"\r\nTo: ");
								}
								else
								{
									PutHeadersState = PUTHEADERS_CC_INIT;
								}
								break;
								
							case PUTHEADERS_TO:
								if(SMTPClient.ROMPointers.To)
								{
									SMTPClient.To.szROM = TCPPutROMString(MySocket, SMTPClient.To.szROM);
									if(*SMTPClient.To.szROM == 0u)
										PutHeadersState = PUTHEADERS_CC_INIT;
								}
								else
								{
									SMTPClient.To.szRAM = TCPPutString(MySocket, SMTPClient.To.szRAM);
									if(*SMTPClient.To.szRAM == 0u)
										PutHeadersState = PUTHEADERS_CC_INIT;
								}
								break;

							case PUTHEADERS_CC_INIT:
								if(SMTPClient.CC.szRAM || SMTPClient.CC.szROM)
								{
									PutHeadersState = PUTHEADERS_CC;
									TCPPutROMString(MySocket, (ROM BYTE*)"\r\nCC: ");
								}
								else
								{
									PutHeadersState = PUTHEADERS_SUBJECT_INIT;
								}
								break;
								
							case PUTHEADERS_CC:
								if(SMTPClient.ROMPointers.CC)
								{
									SMTPClient.CC.szROM = TCPPutROMString(MySocket, SMTPClient.CC.szROM);
									if(*SMTPClient.CC.szROM == 0u)
										PutHeadersState = PUTHEADERS_SUBJECT_INIT;
								}
								else
								{
									SMTPClient.CC.szRAM = TCPPutString(MySocket, SMTPClient.CC.szRAM);
									if(*SMTPClient.CC.szRAM == 0u)
										PutHeadersState = PUTHEADERS_SUBJECT_INIT;
								}
								break;

							case PUTHEADERS_SUBJECT_INIT:
								if(SMTPClient.Subject.szRAM || SMTPClient.Subject.szROM)
								{
									PutHeadersState = PUTHEADERS_SUBJECT;
									TCPPutROMString(MySocket, (ROM BYTE*)"\r\nSubject: ");
								}
								else
								{
									PutHeadersState = PUTHEADERS_OTHER_INIT;
								}
								break;
								
							case PUTHEADERS_SUBJECT:
								if(SMTPClient.ROMPointers.Subject)
								{
									SMTPClient.Subject.szROM = TCPPutROMString(MySocket, SMTPClient.Subject.szROM);
									if(*SMTPClient.Subject.szROM == 0u)
										PutHeadersState = PUTHEADERS_OTHER_INIT;
								}
								else
								{
									SMTPClient.Subject.szRAM = TCPPutString(MySocket, SMTPClient.Subject.szRAM);
									if(*SMTPClient.Subject.szRAM == 0u)
										PutHeadersState = PUTHEADERS_OTHER_INIT;
								}
								break;

							case PUTHEADERS_OTHER_INIT:
								TCPPutROMArray(MySocket, (ROM BYTE*)"\r\n", 2);
								if(SMTPClient.OtherHeaders.szRAM || SMTPClient.OtherHeaders.szROM)
								{
									PutHeadersState = PUTHEADERS_OTHER;
								}
								else
								{
									TCPPutROMArray(MySocket, (ROM BYTE*)"\r\n", 2);
									PutHeadersState = PUTHEADERS_DONE;
									SMTPState++;
								}
								break;
								
							case PUTHEADERS_OTHER:
								if(SMTPClient.ROMPointers.OtherHeaders)
								{
									SMTPClient.OtherHeaders.szROM = TCPPutROMString(MySocket, SMTPClient.OtherHeaders.szROM);
									if(*SMTPClient.OtherHeaders.szROM == 0u)
									{
										TCPPutROMArray(MySocket, (ROM BYTE*)"\r\n", 2);
										PutHeadersState = PUTHEADERS_DONE;
										SMTPState++;
									}
								}
								else
								{
									SMTPClient.OtherHeaders.szRAM = TCPPutString(MySocket, SMTPClient.OtherHeaders.szRAM);
									if(*SMTPClient.OtherHeaders.szRAM == 0u)
									{
										TCPPutROMArray(MySocket, (ROM BYTE*)"\r\n", 2);
										PutHeadersState = PUTHEADERS_DONE;
										SMTPState++;
									}
								}
								break;
							
							// Default case needed to supress compiler diagnostics
							default:
								break;
						}
					}
					TCPFlush(MySocket);
					break;
		
				case SMTP_DATA_BODY_INIT:
					SMTPState++;
					RAMStrPtr = SMTPClient.Body.szRAM;
					ROMStrPtr2 = (ROM BYTE*)"\r\n.\r\n";
					CRPeriod.Pos = NULL;
					if(RAMStrPtr)
						CRPeriod.Pos = (BYTE*)strstrrampgm((char*)RAMStrPtr, (ROM char*)"\r\n.");
					// No break here
		
				case SMTP_DATA_BODY:
					if(SMTPClient.Body.szRAM || SMTPClient.Body.szROM)
					{
						if(*ROMStrPtr2)
						{
							// Put the application data, doing the transparancy replacement of "\r\n." with "\r\n.."
							while(CRPeriod.Pos)
							{
								CRPeriod.Pos += 3;
								RAMStrPtr += TCPPutArray(MySocket, RAMStrPtr, CRPeriod.Pos-RAMStrPtr);
								if(RAMStrPtr == CRPeriod.Pos)
								{
									if(!TCPPut(MySocket, '.'))
									{
										CRPeriod.Pos -= 3;
										break;
									}
								}
								else
								{
									CRPeriod.Pos -= 3;
									break;
								}
								CRPeriod.Pos = (BYTE*)strstrrampgm((char*)RAMStrPtr, (ROM char*)"\r\n.");
							}
							
							// If we get down here, either all replacements have been made or there is no remaining space in the TCP output buffer
							RAMStrPtr = TCPPutString(MySocket, RAMStrPtr);
							ROMStrPtr2 = TCPPutROMString(MySocket, ROMStrPtr2);
							TCPFlush(MySocket);
						}
					}
					else
					{
						if(SMTPFlags.bits.ReadyToFinish)
						{
							if(*ROMStrPtr2)
							{
								ROMStrPtr2 = TCPPutROMString(MySocket, ROMStrPtr2);
								TCPFlush(MySocket);
							}
		
						}
					}

					if(*ROMStrPtr2 == 0u)
					{
						SMTPState++;
					}
					break;
		
				case SMTP_QUIT_INIT:
					SMTPState++;
					ROMStrPtr = (ROM BYTE*)"QUIT\r\n";
					// No break here

				case SMTP_QUIT:
					if(*ROMStrPtr)
					{
						ROMStrPtr = TCPPutROMString(MySocket, ROMStrPtr);
						TCPFlush(MySocket);
					}

					if(*ROMStrPtr == 0u)
					{
						TransportState = TRANSPORT_CLOSE;
					}
					break;
				
				// Default case needed to supress compiler diagnostics
				default:
					break;
			}
			break;

		case TRANSPORT_CLOSE:
			// Close the socket so it can be used by other modules
			TCPDisconnect(MySocket);
			MySocket = INVALID_SOCKET;

			// Go back to doing nothing
			TransportState = TRANSPORT_HOME;
			break;
	}
}

/*****************************************************************************
  Function:
	void SMTPSendMail(void)

  Summary:
	Initializes the message sending process.

  Description:
	This function starts the state machine that performs the actual
	transmission of the message.  Call this function after all the fields
	in SMTPClient have been set.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
void SMTPSendMail(void)
{
	SMTPFlags.bits.ReadyToStart = TRUE;
}

/*****************************************************************************
  Function:
	BOOL SMTPIsBusy(void)

  Summary:
	Determines if the SMTP client is busy.

  Description:
	Call this function to determine if the SMTP client is busy performing
	background tasks.  This function should be called after any call to 
	SMTPSendMail, SMTPPutDone to determine if the stack has finished
	performing its internal tasks.  It should also be called prior to any
	call to SMTPIsPutReady to verify that the SMTP client has not
	prematurely disconnected.  When this function returns FALSE, the next
	call should be to SMTPEndUsage to release the module and obtain the
	status code for the operation.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	None

  Return Values:
	TRUE - The SMTP Client is busy with internal tasks or sending an 
		on-the-fly message.
	FALSE - The SMTP Client is terminated and is ready to be released.
  ***************************************************************************/
BOOL SMTPIsBusy(void)
{
	return TransportState != TRANSPORT_HOME;
}

/*****************************************************************************
  Function:
	WORD SMTPIsPutReady(void)

  Summary:
	Determines how much data can be written to the SMTP client.

  Description:
	Use this function to determine how much data can be written to the SMTP 
	client when generating an on-the-fly message.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call, and an on-the-fly 
	message is being generated.  This requires that SMTPSendMail was called
	with SMTPClient.Body set to NULL.

  Parameters:
	None

  Returns:
	The number of free bytes the SMTP TX FIFO.

  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)
  ***************************************************************************/
WORD SMTPIsPutReady(void)
{
	if(SMTPState != SMTP_DATA_BODY)
		return 0;

	return TCPIsPutReady(MySocket);	
}

/*****************************************************************************
  Function:
	BOOL SMTPPut(BYTE c)

  Description:
	Writes a single byte to the SMTP client.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	c - The byte to be written

  Return Values:
	TRUE - The byte was successfully written
	FALSE - The byte was not written, most likely because the buffer was full

  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)
  ***************************************************************************/
BOOL SMTPPut(BYTE c)
{
	if(CRPeriod.State == CR_PERIOD_NEED_INSERTION)
	{
		if(TCPPut(MySocket, '.'))
			CRPeriod.State = CR_PERIOD_SEEK_CR;
		else
			return FALSE;
	}

	switch(CRPeriod.State)
	{
		case CR_PERIOD_SEEK_CR:
			if(c == '\r')
				CRPeriod.State++;
			break;

		case CR_PERIOD_SEEK_LF:
			if(c == '\n')
				CRPeriod.State++;
			else if(c != '\r')
				CRPeriod.State--;
			break;

		case CR_PERIOD_SEEK_PERIOD:
			if(c == '.')
				CRPeriod.State++;	// CR_PERIOD_NEED_INSERTION
			else if(c == '\r')
				CRPeriod.State--;
			else
				CRPeriod.State = CR_PERIOD_SEEK_CR;
			break;
		
		// Default case needed to supress compiler diagnostics 
		// (CR_PERIOD_NEED_INSERTION state already handled above)
		default:
			break;
	}

	if(!TCPPut(MySocket, c))
		return FALSE;

	return TRUE;
}

/*****************************************************************************
  Function:
	WORD SMTPPutArray(BYTE* Data, WORD Len)

  Description:
	Writes a series of bytes to the SMTP client.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	Data - The data to be written
	Len - How many bytes should be written

  Returns:
	The number of bytes written.  If less than Len, then the TX FIFO became
	full before all bytes could be written.
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)
	
  Internal:
	SMTPPut must be used instead of TCPPutArray because "\r\n." must be
	transparently replaced by "\r\n..".
  ***************************************************************************/
WORD SMTPPutArray(BYTE* Data, WORD Len)
{
	WORD result = 0;

	while(Len--)
	{
		if(SMTPPut(*Data++))
		{
			result++;
		}
		else
		{
			Data--;
			break;
		}
	}

	return result;
}

/*****************************************************************************
  Function:
	WORD SMTPPutROMArray(ROM BYTE* Data, WORD Len)

  Description:
	Writes a series of bytes from ROM to the SMTP client.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	Data - The data to be written
	Len - How many bytes should be written

  Returns:
	The number of bytes written.  If less than Len, then the TX FIFO became
	full before all bytes could be written.
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)
	
  	This function is aliased to SMTPPutArray on non-PIC18 platforms.
	
  Internal:
	SMTPPut must be used instead of TCPPutArray because "\r\n." must be
	transparently replaced by "\r\n..".
  ***************************************************************************/
#if defined(__18CXX)
WORD SMTPPutROMArray(ROM BYTE* Data, WORD Len)
{
	WORD result = 0;

	while(Len--)
	{
		if(SMTPPut(*Data++))
		{
			result++;
		}
		else
		{
			Data--;
			break;
		}
	}

	return result;
}
#endif

/*****************************************************************************
  Function:
	WORD SMTPPutString(BYTE* Data)

  Description:
	Writes a string to the SMTP client.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	Data - The data to be written

  Returns:
	The number of bytes written.  If less than the length of Data, then the 
	TX FIFO became full before all bytes could be written.
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)
	
  Internal:
	SMTPPut must be used instead of TCPPutString because "\r\n." must be
	transparently replaced by "\r\n..".
  ***************************************************************************/
WORD SMTPPutString(BYTE* Data)
{
	WORD result = 0;

	while(*Data)
	{
		if(SMTPPut(*Data++))
		{
			result++;
		}
		else
		{
			Data--;
			break;
		}
	}

	return result;
}

/*****************************************************************************
  Function:
	WORD SMTPPutROMString(ROM BYTE* Data)

  Description:
	Writes a string from ROM to the SMTP client.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	Data - The data to be written

  Returns:
	The number of bytes written.  If less than the length of Data, then the 
	TX FIFO became full before all bytes could be written.
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)
	
  	This function is aliased to SMTPPutString on non-PIC18 platforms.
	
  Internal:
	SMTPPut must be used instead of TCPPutString because "\r\n." must be
	transparently replaced by "\r\n..".
  ***************************************************************************/
#if defined(__18CXX)
WORD SMTPPutROMString(ROM BYTE* Data)
{
	WORD result = 0;

	while(*Data)
	{
		if(SMTPPut(*Data++))
		{
			result++;
		}
		else
		{
			Data--;
			break;
		}
	}

	return result;
}
#endif

/*****************************************************************************
  Function:
	void SMTPFlush(void)

  Description:
	Flushes the SMTP socket and forces all data to be sent.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	None

  Returns:
	None
	
  Remarks:
	This function should only be called externally when the SMTP client is
	generating an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)
  ***************************************************************************/
void SMTPFlush(void)
{
	TCPFlush(MySocket);
}

/*****************************************************************************
  Function:
	void SMTPPutDone(void)

  Description:
	Indicates that the on-the-fly message is complete.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call, and the SMTP client is
	generated an on-the-fly message.  (That is, SMTPSendMail was called
	with SMTPClient.Body set to NULL.)

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
void SMTPPutDone(void)
{
	SMTPFlags.bits.ReadyToFinish = TRUE;
}

/*****************************************************************************
  Function:
	static BYTE *FindEmailAddress(BYTE *str, WORD *wLen)

  Summary:
	Searches a string for an e-mail address.

  Description:
	This function locates an e-mail address in a string.  It is used 
	internally by the SMTP client to parse out the actual address from
	the From and To strings so that the MAIL FROM and RCPT TO commands
	can be sent to the SMTP server.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	str - The string in which to search for an e-mail address
	wLen - the length of str

  Returns:
	A pointer to the e-mail address
  ***************************************************************************/
static BYTE *FindEmailAddress(BYTE *str, WORD *wLen)
{
	BYTE *lpStart;
	BYTE c;
	union
	{
		BYTE Val;
		struct
		{
			BYTE FoundOpenBracket	: 1;
			BYTE FoundAt			: 1;
		} bits;
	} ParseStates;

	lpStart = str;
	*wLen = 0x0000;
	ParseStates.Val = 0x00;

	while((c = *str++))
	{	
		if(c == '<')
		{
			ParseStates.bits.FoundOpenBracket = 1;
			lpStart = str;
			*wLen = -1;
		}
		else if(c == '@')
			ParseStates.bits.FoundAt = 1;


		if(	!ParseStates.bits.FoundOpenBracket &&
			!ParseStates.bits.FoundAt &&
			(c == ' ' || c == ','))
		{
			lpStart = str;
			continue;
		}
		else if(c == ',')
			break;

		if(ParseStates.bits.FoundOpenBracket && ParseStates.bits.FoundAt)
		{
			if(c == '>')
				break;
		}
		
		// Advance to next character
		*wLen += 1;
	}

	if(!ParseStates.bits.FoundAt)
		*wLen = 0;

	return lpStart;
}

/*****************************************************************************
  Function:
	static ROM BYTE *FindROMEmailAddress(ROM BYTE *str, WORD *wLen)

  Summary:
	Searches a ROM string for an e-mail address.

  Description:
	This function locates an e-mail address in a string.  It is used 
	internally by the SMTP client to parse out the actual address from
	the From and To strings so that the MAIL FROM and RCPT TO commands
	can be sent to the SMTP server.

  Precondition:
	SMTPBeginUsage returned TRUE on a previous call.

  Parameters:
	str - The ROM string in which to search for an e-mail address
	wLen - the length of str

  Returns:
	A pointer to the e-mail address
  ***************************************************************************/
static ROM BYTE *FindROMEmailAddress(ROM BYTE *str, WORD *wLen)
{
	ROM BYTE *lpStart;
	BYTE c;
	union
	{
		BYTE Val;
		struct
		{
			BYTE FoundOpenBracket	: 1;
			BYTE FoundAt			: 1;
		} bits;
	} ParseStates;

	lpStart = str;
	*wLen = 0x0000;
	ParseStates.Val = 0x00;

	while((c = *str++))
	{	
		if(c == '<')
		{
			ParseStates.bits.FoundOpenBracket = 1;
			lpStart = str;
			*wLen = -1;
		}
		else if(c == '@')
			ParseStates.bits.FoundAt = 1;


		if(	!ParseStates.bits.FoundOpenBracket &&
			!ParseStates.bits.FoundAt &&
			(c == ' ' || c == ','))
		{
			lpStart = str;
			continue;
		}
		else if(c == ',')
			break;

		if(ParseStates.bits.FoundOpenBracket && ParseStates.bits.FoundAt)
		{
			if(c == '>')
				break;
		}
		
		// Advance to next character
		*wLen += 1;
	}

	if(!ParseStates.bits.FoundAt)
		*wLen = 0;

	return lpStart;
}

#endif //#if defined(STACK_USE_SMTP_CLIENT)
