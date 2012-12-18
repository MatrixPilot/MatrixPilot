/*********************************************************************
 *
 *                  TCP Module Defs for Microchip TCP/IP Stack
 *
 *********************************************************************
 * FileName:        TCP.h
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
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/8/01  	Original        (Rev 1.0)
 * Howard Schlunder		11/30/06	See "TCPIP Stack Version.txt" file
 ********************************************************************/
#ifndef __TCP_HITECH_WORKAROUND_H
#define __TCP_HITECH_WORKAROUND_H

/****************************************************************************
  Section:
	Type Definitions
  ***************************************************************************/

// A TCP_SOCKET is stored as a single BYTE
typedef BYTE TCP_SOCKET;

#define INVALID_SOCKET      (0xFE)	// The socket is invalid or could not be opened
#define UNKNOWN_SOCKET      (0xFF)	// The socket is not known

/****************************************************************************
  Section:
	State Machine Variables
  ***************************************************************************/

// TCP States as defined by RFC 793
typedef enum
{
	TCP_GET_DNS_MODULE,		// Special state for TCP client mode sockets
	TCP_DNS_RESOLVE,		// Special state for TCP client mode sockets
	TCP_GATEWAY_SEND_ARP,	// Special state for TCP client mode sockets
	TCP_GATEWAY_GET_ARP,	// Special state for TCP client mode sockets

    TCP_LISTEN,				// Socket is listening for connections
    TCP_SYN_SENT,			// A SYN has been sent, awaiting an SYN+ACK
    TCP_SYN_RECEIVED,		// A SYN has been received, awaiting an ACK
    TCP_ESTABLISHED,		// Socket is connected and connection is established
    TCP_FIN_WAIT_1,			// FIN WAIT state 1
    TCP_FIN_WAIT_2,			// FIN WAIT state 2
    TCP_CLOSING,			// Socket is closing
//	TCP_TIME_WAIT, state is not implemented
	TCP_CLOSE_WAIT,			// Waiting to close the socket
    TCP_LAST_ACK,			// The final ACK has been sent
    TCP_CLOSED,				// Socket is idle and unallocated

    TCP_CLOSED_BUT_RESERVED	// Special state for TCP client mode sockets.  Socket is idle, but still allocated pending application closure of the handle.
} TCP_STATE;

typedef enum
{
	SSL_NONE = 0,			// No security is enabled
	SSL_HANDSHAKING,		// Handshake is progressing (no application data allowed)
	SSL_ESTABLISHED,		// Connection is established and secured
	SSL_CLOSED				// Connection has been closed (no applicaiton data is allowed)
} SSL_STATE;

/****************************************************************************
  Section:
	TCB Definitions
  ***************************************************************************/

// TCP Control Block (TCB) stub data storage.  Stubs are stored in local PIC RAM for speed.
// Current size is 34 bytes (PIC18), 36 bytes (PIC24/dsPIC), or 56 (PIC32)
typedef struct
{
	PTR_BASE bufferTxStart;		// First byte of TX buffer
	PTR_BASE bufferRxStart;		// First byte of RX buffer.  TX buffer ends 1 byte prior
	PTR_BASE bufferEnd;			// Last byte of RX buffer
	PTR_BASE txHead;			// Head pointer for TX
	PTR_BASE txTail;			// Tail pointer for TX
	PTR_BASE rxHead;			// Head pointer for RX
	PTR_BASE rxTail;			// Tail pointer for RX
    DWORD eventTime;			// Packet retransmissions, state changes
	WORD eventTime2;			// Window updates, automatic transmission
	union
	{
		WORD delayedACKTime;	// Delayed Acknowledgement timer
		WORD closeWaitTime;		// TCP_CLOSE_WAIT timeout timer
	} OverlappedTimers;
    TCP_STATE smState;			// State of this socket
    struct
    {
	    unsigned char vUnackedKeepalives : 3;		// Count of how many keepalives have been sent with no response
        unsigned char bServer : 1;					// Socket should return to listening state when closed
		unsigned char bTimerEnabled	: 1;			// Timer is enabled
		unsigned char bTimer2Enabled : 1;			// Second timer is enabled
		unsigned char bDelayedACKTimerEnabled : 1;	// DelayedACK timer is enabled
		unsigned char bOneSegmentReceived : 1;		// A segment has been received
		unsigned char bHalfFullFlush : 1;			// Flush is for being half full
		unsigned char bTXASAP : 1;					// Transmit as soon as possible (for Flush)
		unsigned char bTXASAPWithoutTimerReset : 1;	// Transmit as soon as possible (for Flush), but do not reset retransmission timers
		unsigned char bTXFIN : 1;					// FIN needs to be transmitted
		unsigned char bSocketReset : 1;				// Socket has been reset (self-clearing semaphore)
		unsigned char bSSLHandshaking : 1;			// Socket is in an SSL handshake
		unsigned char filler : 2;					// Future expansion
    } Flags;
	WORD_VAL remoteHash;	// Consists of remoteIP, remotePort, localPort for connected sockets.  It is a localPort number only for listening server sockets.

    #if defined(STACK_USE_SSL)
    PTR_BASE sslTxHead;		// Position of data being written in next SSL application record
    						//   Also serves as cache of localSSLPort when smState = TCP_LISTENING
    PTR_BASE sslRxHead;		// Position of incoming data not yet handled by SSL
    BYTE sslStubID;			// Which sslStub is associated with this connection
    BYTE sslReqMessage;		// Currently requested SSL message
    #endif

	BYTE vMemoryMedium;		// Which memory medium the TCB is actually stored
	
} TCB_STUB;

// Remainder of TCP Control Block data.
// The rest of the TCB is stored in Ethernet buffer RAM or elsewhere as defined by vMemoryMedium.
// Current size is 41 (PIC18), 42 (PIC24/dsPIC), or 48 bytes (PIC32)
typedef struct
{
	DWORD		retryInterval;			// How long to wait before retrying transmission
	DWORD		MySEQ;					// Local sequence number
	DWORD		RemoteSEQ;				// Remote sequence number
	PTR_BASE	txUnackedTail;			// TX tail pointer for data that is not yet acked
    WORD_VAL	remotePort;				// Remote port number
    WORD_VAL	localPort;				// Local port number
	WORD		remoteWindow;			// Remote window size
	WORD		wFutureDataSize;		// How much out-of-order data has been received
	union
	{
		NODE_INFO	niRemoteMACIP;		// 10 bytes for MAC and IP address
		DWORD		dwRemoteHost;		// RAM or ROM pointer to a hostname string (ex: "www.microchip.com")
	} remote;
	SHORT		sHoleSize;				// Size of the hole, or -1 for none exists.  (0 indicates hole has just been filled)
    struct
    {
        unsigned char bFINSent : 1;		// A FIN has been sent
		unsigned char bSYNSent : 1;		// A SYN has been sent
		unsigned char bRemoteHostIsROM : 1;	// Remote host is stored in ROM
		unsigned char bRXNoneACKed1 : 1;	// A duplicate ACK was likely received
		unsigned char bRXNoneACKed2 : 1;	// A second duplicate ACK was likely received
		unsigned char filler : 3;		// future use
    } flags;
	WORD		wRemoteMSS;				// Maximum Segment Size option advirtised by the remote node during initial handshaking
    #if defined(STACK_USE_SSL)
    WORD_VAL	localSSLPort;			// Local SSL port number (for listening sockets)
    #endif
	BYTE		retryCount;				// Counter for transmission retries
	BYTE		vSocketPurpose;			// Purpose of socket (as defined in TCPIPConfig.h)
} TCB;

// Information about a socket
typedef struct
{
	NODE_INFO remote;		// NODE_INFO structure for remote node
	WORD_VAL remotePort;	// Port number associated with remote node
} SOCKET_INFO;

/****************************************************************************
  Section:
	Function Declarations
  ***************************************************************************/

void TCPInit(void);
SOCKET_INFO* TCPGetRemoteInfo(TCP_SOCKET hTCP);
BOOL TCPWasReset(TCP_SOCKET hTCP);
BOOL TCPIsConnected(TCP_SOCKET hTCP);
void TCPDisconnect(TCP_SOCKET hTCP);
void TCPClose(TCP_SOCKET hTCP);
WORD TCPIsPutReady(TCP_SOCKET hTCP);
BOOL TCPPut(TCP_SOCKET hTCP, BYTE byte);
WORD TCPPutArray(TCP_SOCKET hTCP, BYTE* Data, WORD Len);
BYTE* TCPPutString(TCP_SOCKET hTCP, BYTE* Data);
WORD TCPIsGetReady(TCP_SOCKET hTCP);
WORD TCPGetRxFIFOFree(TCP_SOCKET hTCP);
BOOL TCPGet(TCP_SOCKET hTCP, BYTE* byte);
WORD TCPGetArray(TCP_SOCKET hTCP, BYTE* buffer, WORD count);
BYTE TCPPeek(TCP_SOCKET hTCP, WORD wStart);
WORD TCPPeekArray(TCP_SOCKET hTCP, BYTE *vBuffer, WORD wLen, WORD wStart);
WORD TCPFindEx(TCP_SOCKET hTCP, BYTE cFind, WORD wStart, WORD wSearchLen, BOOL bTextCompare);
WORD TCPFindArrayEx(TCP_SOCKET hTCP, BYTE* cFindArray, WORD wLen, WORD wStart, WORD wSearchLen, BOOL bTextCompare);
void TCPDiscard(TCP_SOCKET hTCP);
BOOL TCPProcess(NODE_INFO* remote, IP_ADDR* localIP, WORD len);
void TCPTick(void);
void TCPFlush(TCP_SOCKET hTCP);

// Create a server socket and ignore dwRemoteHost.
#define TCP_OPEN_SERVER		0u
#if defined(STACK_CLIENT_MODE)
	#if defined(STACK_USE_DNS)
		// Create a client socket and use dwRemoteHost as a RAM pointer to a hostname string.
		#define TCP_OPEN_RAM_HOST	1u
		// Create a client socket and use dwRemoteHost as a ROM pointer to a hostname string.
		#define TCP_OPEN_ROM_HOST	2u
	#else
		// Emit an undeclared identifier diagnostic if code tries to use TCP_OPEN_RAM_HOST while the DNS client module is not enabled. 
		#define TCP_OPEN_RAM_HOST	You_need_to_enable_STACK_USE_DNS_to_use_TCP_OPEN_RAM_HOST
		// Emit an undeclared identifier diagnostic if code tries to use TCP_OPEN_ROM_HOST while the DNS client module is not enabled. 
		#define TCP_OPEN_ROM_HOST	You_need_to_enable_STACK_USE_DNS_to_use_TCP_OPEN_ROM_HOST
	#endif
	// Create a client socket and use dwRemoteHost as a literal IP address.
	#define TCP_OPEN_IP_ADDRESS	3u
	// Create a client socket and use dwRemoteHost as a pointer to a NODE_INFO structure containing the exact remote IP address and MAC address to use.
	#define TCP_OPEN_NODE_INFO	4u
#else
	// Emit an undeclared identifier diagnostic if code tries to use TCP_OPEN_RAM_HOST while STACK_CLIENT_MODE feature is not enabled. 
	#define TCP_OPEN_RAM_HOST	You_need_to_enable_STACK_CLIENT_MODE_to_use_TCP_OPEN_RAM_HOST
	// Emit an undeclared identifier diagnostic if code tries to use TCP_OPEN_ROM_HOST while STACK_CLIENT_MODE feature is not enabled. 
	#define TCP_OPEN_ROM_HOST	You_need_to_enable_STACK_CLIENT_MODE_to_use_TCP_OPEN_ROM_HOST
	// Emit an undeclared identifier diagnostic if code tries to use TCP_OPEN_IP_ADDRESS while STACK_CLIENT_MODE feature is not enabled. 
	#define TCP_OPEN_IP_ADDRESS	You_need_to_enable_STACK_CLIENT_MODE_to_use_TCP_OPEN_IP_ADDRESS
	// Emit an undeclared identifier diagnostic if code tries to use TCP_OPEN_NODE_INFO while STACK_CLIENT_MODE feature is not enabled. 
	#define TCP_OPEN_NODE_INFO	You_need_to_enable_STACK_CLIENT_MODE_to_use_TCP_OPEN_NODE_INFO
#endif
TCP_SOCKET TCPOpen(DWORD dwRemoteHost, BYTE vRemoteHostType, WORD wPort, BYTE vSocketPurpose);

#if defined(__18CXX)
	WORD TCPFindROMArrayEx(TCP_SOCKET hTCP, ROM BYTE* cFindArray, WORD wLen, WORD wStart, WORD wSearchLen, BOOL bTextCompare);

	/*****************************************************************************
	  Summary:
		Alias to TCPFindROMArrayEx with no length parameter.
	
	  Description:
		This function is an alias to TCPFindROMArrayEx with no length parameter.  
		It is provided for backwards compatibility with an older API.
	  ***************************************************************************/
	#define TCPFindROMArray(a,b,c,d,e)		TCPFindROMArrayEx(a,b,c,d,0,e)
	
	WORD TCPPutROMArray(TCP_SOCKET hTCP, ROM BYTE* Data, WORD Len);
	ROM BYTE* TCPPutROMString(TCP_SOCKET hTCP, ROM BYTE* Data);
#else
	#define TCPFindROMArray(a,b,c,d,e) 		TCPFindArray(a,(BYTE*)b,c,d,e)
	#define TCPFindROMArrayEx(a,b,c,d,e,f) 	TCPFindArrayEx(a,(BYTE*)b,c,d,e,f)
	#define TCPPutROMArray(a,b,c)			TCPPutArray(a,(BYTE*)b,c)
	#define TCPPutROMString(a,b)			TCPPutString(a,(BYTE*)b)
#endif

WORD TCPGetTxFIFOFull(TCP_SOCKET hTCP);
// Alias to TCPIsGetReady provided for API completeness
#define TCPGetRxFIFOFull(a)					TCPIsGetReady(a)
// Alias to TCPIsPutReady provided for API completeness
#define TCPGetTxFIFOFree(a) 				TCPIsPutReady(a)

#define TCP_ADJUST_GIVE_REST_TO_RX	0x01u	// Resize flag: extra bytes go to RX 
#define TCP_ADJUST_GIVE_REST_TO_TX	0x02u	// Resize flag: extra bytes go to TX
#define TCP_ADJUST_PRESERVE_RX		0x04u	// Resize flag: attempt to preserve RX buffer
#define TCP_ADJUST_PRESERVE_TX		0x08u	// Resize flag: attempt to preserve TX buffer
BOOL TCPAdjustFIFOSize(TCP_SOCKET hTCP, WORD wMinRXSize, WORD wMinTXSize, BYTE vFlags);

#if defined(STACK_USE_SSL)
BOOL TCPStartSSLClient(TCP_SOCKET hTCP, BYTE* host);
BOOL TCPStartSSLClientEx(TCP_SOCKET hTCP, BYTE* host, void * buffer, BYTE suppDataType);
BOOL TCPStartSSLServer(TCP_SOCKET hTCP);
BOOL TCPAddSSLListener(TCP_SOCKET hTCP, WORD port);
BOOL TCPRequestSSLMessage(TCP_SOCKET hTCP, BYTE msg);
BOOL TCPSSLIsHandshaking(TCP_SOCKET hTCP);
BOOL TCPIsSSL(TCP_SOCKET hTCP);
void TCPSSLHandshakeComplete(TCP_SOCKET hTCP);
void TCPSSLDecryptMAC(TCP_SOCKET hTCP, ARCFOUR_CTX* ctx, WORD len);
void TCPSSLInPlaceMACEncrypt(TCP_SOCKET hTCP, ARCFOUR_CTX* ctx, BYTE* MACSecret, WORD len);
void TCPSSLPutRecordHeader(TCP_SOCKET hTCP, BYTE* hdr, BOOL recDone);
WORD TCPSSLGetPendingTxSize(TCP_SOCKET hTCP);
void TCPSSLHandleIncoming(TCP_SOCKET hTCP);
#endif

/*****************************************************************************
  Summary:
	Alias to TCPFindEx with no length parameter.

  Description:
	This function is an alias to TCPFindEx with no length parameter.  It is
	provided for backwards compatibility with an older API.
  ***************************************************************************/
#define TCPFind(a,b,c,d)					TCPFindEx(a,b,c,0,d)


/*****************************************************************************
  Summary:
	Alias to TCPFindArrayEx with no length parameter.

  Description:
	This function is an alias to TCPFindArrayEx with no length parameter.  It is
	provided for backwards compatibility with an older API.
  ***************************************************************************/
#define TCPFindArray(a,b,c,d,e)				TCPFindArrayEx(a,b,c,d,0,e)

/*****************************************************************************
  Summary:
	Alias to TCPOpen as a server.

  Description:
	This function is an alias to TCPOpen for server sockets.  It is provided
	for backwards compatibility with older versions of the stack.  New
	applications should use the TCPOpen API instead.
  ***************************************************************************/
#define TCPListen(port)			TCPOpen(0, TCP_OPEN_SERVER, port, TCP_PURPOSE_DEFAULT)

/*****************************************************************************
  Summary:
	Alias to TCPOpen as a client.

  Description:
	This function is an alias to TCPOpen for client sockets.  It is provided
	for backwards compatibility with older versions of the stack.  New
	applications should use the TCPOpen API instead.
  ***************************************************************************/
#define TCPConnect(remote,port)	TCPOpen((DWORD)remote, TCP_OPEN_NODE_INFO, port, TCP_PURPOSE_DEFAULT)


#endif
