/*********************************************************************
 *
 *	Transmission Control Protocol (TCP) Communications Layer
 *  Module for Microchip TCP/IP Stack
 *	 -Provides reliable, handshaked transport of application stream 
 *    oriented data with flow control
 *	 -Reference: RFC 793
 *
 *********************************************************************
 * FileName:        TCP.c
 * Dependencies:    IP, Tick, Ethernet/WiFi (ENC28J60.c, ETH97J60.c, 
 *					ENCX24J600.c, or WFMac.c), ARP (optional), 
 *					DNS (optional)
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
 * Howard Schlunder		12/11/06	Changed almost everything to 
 *									better meet RFC 793.
 ********************************************************************/
#define __TCP_C

#include "TCPIP Stack/TCPIP.h"

#if defined(STACK_USE_TCP)

/****************************************************************************
  Section:
	Configuration Parameters
  ***************************************************************************/

// Starting port for client sockets
#define LOCAL_PORT_START_NUMBER (1024u)
// End port for client sockets
#define LOCAL_PORT_END_NUMBER   (5000u)

// For debugging only.  Normal applications should never enable these
//#define DEBUG_GENERATE_TX_LOSS		62257
//#define DEBUG_GENERATE_RX_LOSS		64225

// A lot of pointer dereference code can be removed if you 
// locally copy TCBStubs to an absolute memory location.
// If you define TCP_OPTIMIZE_FOR_SIZE, local caching will 
// occur and will substantially decrease the entire TCP ROM 
// footprint (up to 35%).  If you leave TCP_OPTIMIZE_FOR_SIZE 
// undefined, the local caching will be disabled.  On PIC18 
// products, this will improve TCP performance/throughput by 
// approximately 15%.
#define TCP_OPTIMIZE_FOR_SIZE

// For smallest size and best throughput, TCP_OPTIMIZE_FOR_SIZE 
// should always be enabled on PIC24/dsPIC products.  On PIC32 
// products there is very little difference and depnds on compiler 
// optimization level
#if defined(__C30__) && !defined(TCP_OPTIMIZE_FOR_SIZE)
	#define TCP_OPTIMIZE_FOR_SIZE
#elif defined(__C32__) && defined(TCP_OPTIMIZE_FOR_SIZE)
	#undef TCP_OPTIMIZE_FOR_SIZE
#endif

// TCP Maximum Segment Size for TX.  The TX maximum segment size is actually 
// govered by the remote node's MSS option advirtised during connection 
// establishment.  However, if the remote node specifies an unhandlably large 
// MSS (ex: > Ethernet MTU), this define sets a hard limit so that we don't 
// cause any TX buffer overflows.  If the remote node does not advirtise a MSS 
// option, all TX segments are fixed at 536 bytes maximum.
#define TCP_MAX_SEG_SIZE_TX			(1460u)

// TCP Maximum Segment Size for RX.  This value is advirtised during connection 
// establishment and the remote node should obey it.  This should be set to 536 
// to avoid IP layer fragmentation from causing packet loss.  However, raising 
// its value can enhance performance at the (small) risk of introducing 
// incompatibility with certain special remote nodes (ex: ones connected via a 
// slow dial up modem).
#define TCP_MAX_SEG_SIZE_RX			(536u)

// TCP Timeout and retransmit numbers
#define TCP_START_TIMEOUT_VAL   	((DWORD)TICK_SECOND*1)	// Timeout to retransmit unacked data
#define TCP_DELAYED_ACK_TIMEOUT		((DWORD)TICK_SECOND/10)	// Timeout for delayed-acknowledgement algorithm
#define TCP_FIN_WAIT_2_TIMEOUT		((DWORD)TICK_SECOND*5)	// Timeout for FIN WAIT 2 state
#define TCP_KEEP_ALIVE_TIMEOUT		((DWORD)TICK_SECOND*10)	// Timeout for keep-alive messages when no traffic is sent
#define TCP_CLOSE_WAIT_TIMEOUT		((DWORD)TICK_SECOND/5)	// Timeout for the CLOSE_WAIT state
#define TCP_MAX_RETRIES			    (5u)					// Maximum number of retransmission attempts
#define TCP_MAX_UNACKED_KEEP_ALIVES	(6u)					// Maximum number of keep-alive messages that can be sent without receiving a response before automatically closing the connection
#define TCP_MAX_SYN_RETRIES			(2u)	// Smaller than all other retries to reduce SYN flood DoS duration

#define TCP_AUTO_TRANSMIT_TIMEOUT_VAL	(TICK_SECOND/25ull)	// Timeout before automatically transmitting unflushed data
#define TCP_WINDOW_UPDATE_TIMEOUT_VAL	(TICK_SECOND/5ull)	// Timeout before automatically transmitting a window update due to a TCPGet() or TCPGetArray() function call

#define TCP_SYN_QUEUE_MAX_ENTRIES	(3u) 					// Number of TCP RX SYN packets to save if they cannot be serviced immediately
#define TCP_SYN_QUEUE_TIMEOUT		((DWORD)TICK_SECOND*3)	// Timeout for when SYN queue entries are deleted if unserviceable

/****************************************************************************
  Section:
	TCP Header Data Types
  ***************************************************************************/

#define FIN     (0x01)		// FIN Flag as defined in RFC
#define SYN     (0x02)		// SYN Flag as defined in RFC
#define RST     (0x04)		// Reset Flag as defined in RFC
#define PSH     (0x08)		// Push Flag as defined in RFC
#define ACK     (0x10)		// Acknowledge Flag as defined in RFC
#define URG     (0x20)		// Urgent Flag as defined in RFC

// TCP Header Data Structure
typedef struct
{
	WORD    SourcePort;		// Local port number
	WORD    DestPort;		// Remote port number
	DWORD   SeqNumber;		// Local sequence number
	DWORD   AckNumber;		// Acknowledging remote sequence number

	struct
	{
		unsigned char Reserved3      : 4;
		unsigned char Val            : 4;
	} DataOffset;			// Data offset flags nibble

	union
	{
		struct
		{
			unsigned char flagFIN    : 1;
			unsigned char flagSYN    : 1;
			unsigned char flagRST    : 1;
			unsigned char flagPSH    : 1;
			unsigned char flagACK    : 1;
			unsigned char flagURG    : 1;
			unsigned char Reserved2  : 2;
		} bits;
		BYTE byte;
	} Flags;				// TCP Flags as defined in RFC

	WORD    Window;			// Local free RX buffer window
	WORD    Checksum;		// Data payload checksum
	WORD    UrgentPointer;	// Urgent pointer
} TCP_HEADER;

#define TCP_OPTIONS_END_OF_LIST     (0x00u)		// End of List TCP Option Flag
#define TCP_OPTIONS_NO_OP           (0x01u)		// No Op TCP Option
#define TCP_OPTIONS_MAX_SEG_SIZE    (0x02u)		// Maximum segment size TCP flag
typedef struct
{
	BYTE        Kind;							// Type of option
	BYTE        Length;							// Length
	WORD_VAL    MaxSegSize;						// Maximum segment size
} TCP_OPTIONS;									// TCP Options data structure							

// Structure containing all the important elements of an incomming 
// SYN packet in order to establish a connection at a future time 
// if all sockets on the listening port are already connected to 
// someone
typedef struct 
{
	NODE_INFO	niSourceAddress;// Remote IP address and MAC address
	WORD		wSourcePort;	// Remote TCP port number that the response SYN needs to be sent to
	DWORD		dwSourceSEQ;	// Remote TCP SEQuence number that must be ACKnowledged when we send our response SYN
	WORD		wDestPort;		// Local TCP port which the original SYN was destined for
	WORD		wTimestamp;		// Timer to expire old SYN packets that can't be serviced at all
} TCP_SYN_QUEUE;


#if defined(STACK_CLIENT_MODE)
static WORD NextPort __attribute__((persistent));	// Tracking variable for next local client port number
#endif

/****************************************************************************
  Section:
	TCB Definitions
  ***************************************************************************/

// Determines the number of defined TCP sockets
#define TCP_SOCKET_COUNT	(sizeof(TCPSocketInitializer)/sizeof(TCPSocketInitializer[0]))


#if defined(HI_TECH_C)
	// The initializer forces this large array out of the bss section 
	// so we can link correctly.
	#pragma psect bigdata=TCB_uRAM_BIG
	#pragma psect data=TCB_uRAM
	static TCB_STUB TCBStubs[TCP_SOCKET_COUNT] = {'\0'};	
	#pragma psect data=ordinary_data_sect
	#pragma psect bigdata=ordinary_data_sect_big
#else
	// The TCB array is very large.  With the C18 compiler, one must 
	// modify the linker script to make an array that spans more than 
	// one memory bank.  To do this, make the necessary changes to your 
	// processor's linker script (.lkr).  Here is an example showing 
	// gpr11 and 128 bytes of gpr12 being combined into one 384 byte 
	// block used exclusively by the TCB_uRAM data section:
	// ...
	// //DATABANK   NAME=gpr11      START=0xB00          END=0xBFF
	// //DATABANK   NAME=gpr12      START=0xC00          END=0xCFF
	// DATABANK   NAME=gpr11b     START=0xB00          END=0xC7F           PROTECTED
	// DATABANK   NAME=gpr12      START=0xC80          END=0xCFF
	// ...
	// SECTION    NAME=TCB_uRAM    RAM=gpr11b
	// ...
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata TCB_uRAM
	#endif
	static TCB_STUB TCBStubs[TCP_SOCKET_COUNT];
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata					// Return to any other RAM section
	#endif
#endif

static TCB MyTCB;									// Currently loaded TCB
static TCP_SOCKET hCurrentTCP = INVALID_SOCKET;		// Current TCP socket
#if TCP_SYN_QUEUE_MAX_ENTRIES
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata SYN_QUEUE_RAM_SECT
	#endif
	static TCP_SYN_QUEUE SYNQueue[TCP_SYN_QUEUE_MAX_ENTRIES];	// Array of saved incoming SYN requests that need to be serviced later
	#if defined(__18CXX) && !defined(HI_TECH_C)	
		#pragma udata
	#endif
#endif

/****************************************************************************
  Section:
	Function Prototypes
  ***************************************************************************/

static void TCPRAMCopy(PTR_BASE wDest, BYTE vDestType, PTR_BASE wSource, BYTE vSourceType, WORD wLength);

#if defined(__18CXX)
	static void TCPRAMCopyROM(PTR_BASE wDest, BYTE wDestType, ROM BYTE* wSource, WORD wLength);
#else
	#define TCPRAMCopyROM(a,b,c,d)	TCPRAMCopy(a,b,c,TCP_PIC_RAM,d)
#endif

static void SendTCP(BYTE vTCPFlags, BYTE vSendFlags);
static void HandleTCPSeg(TCP_HEADER* h, WORD len);
static BOOL FindMatchingSocket(TCP_HEADER* h, NODE_INFO* remote);
static void SwapTCPHeader(TCP_HEADER* header);
static void CloseSocket(void);
static void SyncTCB(void);

#if defined(WF_CS_TRIS)
UINT16 WFGetTCBSize(void);
#endif

// Indicates if this packet is a retransmission (no reset) or a new packet (reset required)
#define SENDTCP_RESET_TIMERS	0x01
// Instead of transmitting normal data, a garbage octet is transmitted according to RFC 1122 section 4.2.3.6
#define SENDTCP_KEEP_ALIVE		0x02


/****************************************************************************
  Section:
	TCB Optimization Configuration
  ***************************************************************************/

#if defined(TCP_OPTIMIZE_FOR_SIZE)
	static TCB_STUB MyTCBStub;
	
	// Flushes MyTCBStub cache and loads up the specified TCB_STUB.
	// Does nothing on cache hit.
	static void SyncTCBStub(TCP_SOCKET hTCP)
	{
		if(hCurrentTCP == hTCP)
			return;
	
		if(hCurrentTCP != INVALID_SOCKET)
		{
			// Save the current TCB stub
			memcpy((void*)&TCBStubs[hCurrentTCP], (void*)&MyTCBStub, sizeof(MyTCBStub));
		}
	
		hCurrentTCP = hTCP;
	
		if(hTCP == INVALID_SOCKET)
			return;
	
		// Load up the new TCB stub
		memcpy((void*)&MyTCBStub, (void*)&TCBStubs[hTCP], sizeof(MyTCBStub));
	}
#else
	// Flushes MyTCBStub cache and loads up the specified TCB_STUB.
	// Does nothing on cache hit.
	#define SyncTCBStub(a)	hCurrentTCP = (a)
	// Alias to current TCP stub.
	#define MyTCBStub		TCBStubs[hCurrentTCP]
#endif



// Flushes MyTCB cache and loads up the specified TCB.
// Does nothing on cache hit.
static void SyncTCB(void)
{
	static TCP_SOCKET hLastTCB = INVALID_SOCKET;
	
	if(hLastTCB == hCurrentTCP)
		return;

	if(hLastTCB != INVALID_SOCKET)
	{
		// Save the current TCB
		TCPRAMCopy(TCBStubs[hLastTCB].bufferTxStart - sizeof(MyTCB), TCBStubs[hLastTCB].vMemoryMedium, (PTR_BASE)&MyTCB, TCP_PIC_RAM, sizeof(MyTCB));
	}

	// Load up the new TCB
	hLastTCB = hCurrentTCP;
	TCPRAMCopy((PTR_BASE)&MyTCB, TCP_PIC_RAM, MyTCBStub.bufferTxStart - sizeof(MyTCB), MyTCBStub.vMemoryMedium, sizeof(MyTCB));
}


/*****************************************************************************
  Function:
	void TCPInit(void)

  Summary:
	Initializes the TCP module.

  Description:
	Initializes the TCP module.  This function sets up the TCP buffers
	in memory and initializes each socket to the CLOSED state.  If
	insufficient memory was allocated for the TCP sockets, the function
	will hang here to be captured by the debugger.

  Precondition:
	None

  Parameters:
	None

  Returns:
  	None
  	
  Remarks:
	This function is called only one during lifetime of the application.
  ***************************************************************************/
void TCPInit(void)
{
	BYTE i;
	BYTE vSocketsAllocated;
	WORD wTXSize, wRXSize;
	PTR_BASE ptrBaseAddress;
	BYTE vMedium;
	#if TCP_ETH_RAM_SIZE > 0
	WORD wCurrentETHAddress = TCP_ETH_RAM_BASE_ADDRESS;
	#endif
	#if TCP_PIC_RAM_SIZE > 0
	PTR_BASE ptrCurrentPICAddress = TCP_PIC_RAM_BASE_ADDRESS;
	#endif
	#if TCP_SPI_RAM_SIZE > 0
	WORD wCurrentSPIAddress = TCP_SPI_RAM_BASE_ADDRESS;
	#endif

	#if defined(STACK_CLIENT_MODE)
		// Initialize NextPort to a random value if it is zero (such as after 
		// reset on a PIC32 or PIC18 when the static memory initializer is 
		// used).  By starting with a random number, we decrease the risk of 
		// reusing a port number that was previously used if the user power 
		// cycles the device.
		if(NextPort == 0u)
			NextPort = (((WORD)GenerateRandomDWORD()) & 0x07FFu) + LOCAL_PORT_START_NUMBER;
	#endif


	// Mark all SYN Queue entries as invalid by zeroing the memory
	#if TCP_SYN_QUEUE_MAX_ENTRIES
		memset((void*)SYNQueue, 0x00, sizeof(SYNQueue));
	#endif
	
	// Allocate all socket FIFO addresses
	vSocketsAllocated = 0;
	for(i = 0; i < TCP_SOCKET_COUNT; i++)
	{
		// Generate all needed sockets of each type (TCP_PURPOSE_*)
		SyncTCBStub(i);
	
		vMedium = TCPSocketInitializer[i].vMemoryMedium;
		wTXSize = TCPSocketInitializer[i].wTXBufferSize;
		wRXSize = TCPSocketInitializer[i].wRXBufferSize;
	
		switch(vMedium)
		{
			#if TCP_ETH_RAM_SIZE > 0
			case TCP_ETH_RAM:
				ptrBaseAddress = wCurrentETHAddress;
				wCurrentETHAddress += sizeof(TCB) + wTXSize+1 + wRXSize+1;
				// Do a sanity check to ensure that we aren't going to use memory that hasn't been allocated to us.
				// If your code locks up right here, it means you've incorrectly allocated your TCP socket buffers in TCPIPConfig.h.  See the TCP memory allocation section.  More RAM needs to be allocated to the base memory mediums, or the individual sockets TX and RX FIFOS and socket quantiy needs to be shrunken.
#if defined(WF_CS_TRIS)
				while(wCurrentETHAddress > TCP_ETH_RAM_BASE_ADDRESS + WFGetTCBSize()/*TCP_ETH_RAM_SIZE*/);
#else
				while(wCurrentETHAddress > TCP_ETH_RAM_BASE_ADDRESS + TCP_ETH_RAM_SIZE);
#endif
				break;
			#endif
				
			#if TCP_PIC_RAM_SIZE > 0
			case TCP_PIC_RAM:
				ptrBaseAddress = ptrCurrentPICAddress;
				ptrCurrentPICAddress += sizeof(TCB) + wTXSize+1 + wRXSize+1;
				// Do a sanity check to ensure that we aren't going to use memory that hasn't been allocated to us.
				// If your code locks up right here, it means you've incorrectly allocated your TCP socket buffers in TCPIPConfig.h.  See the TCP memory allocation section.  More RAM needs to be allocated to the base memory mediums, or the individual sockets TX and RX FIFOS and socket quantiy needs to be shrunken.
				while(ptrCurrentPICAddress > TCP_PIC_RAM_BASE_ADDRESS + TCP_PIC_RAM_SIZE);
				break;
			#endif
				
			#if TCP_SPI_RAM_SIZE > 0
			case TCP_SPI_RAM:
				ptrBaseAddress = wCurrentSPIAddress;
				wCurrentSPIAddress += sizeof(TCB) + wTXSize+1 + wRXSize+1;
				// Do a sanity check to ensure that we aren't going to use memory that hasn't been allocated to us.
				// If your code locks up right here, it means you've incorrectly allocated your TCP socket buffers in TCPIPConfig.h.  See the TCP memory allocation section.  More RAM needs to be allocated to the base memory mediums, or the individual sockets TX and RX FIFOS and socket quantiy needs to be shrunken.
				while(wCurrentSPIAddress > TCP_SPI_RAM_BASE_ADDRESS + TCP_SPI_RAM_SIZE);
				break;
			#endif
			
			default:
				while(1); // Undefined allocation medium.  Go fix your TCPIPConfig.h TCP memory allocations.
		}
	
		MyTCBStub.vMemoryMedium = vMedium;
		MyTCBStub.bufferTxStart	= ptrBaseAddress + sizeof(TCB);
		MyTCBStub.bufferRxStart	= MyTCBStub.bufferTxStart + wTXSize + 1;
		MyTCBStub.bufferEnd		= MyTCBStub.bufferRxStart + wRXSize;
		MyTCBStub.smState		= TCP_CLOSED;
		MyTCBStub.Flags.bServer	= FALSE;
		#if defined(STACK_USE_SSL)
		MyTCBStub.sslStubID = SSL_INVALID_ID;
		#endif		

		SyncTCB();
		MyTCB.vSocketPurpose = TCPSocketInitializer[i].vSocketPurpose;
		CloseSocket();
	}
}

/****************************************************************************
  Section:
	Connection Management Functions
  ***************************************************************************/


/*****************************************************************************
  Function:
	TCP_SOCKET TCPOpen(DWORD dwRemoteHost, BYTE vRemoteHostType, WORD wPort, BYTE vSocketPurpose)
    
  Summary:
    Opens a TCP socket for listening or as a client.

  Description:
    Provides a unified method for opening TCP sockets. This function can
    open both client and server sockets. For client sockets, it can accept
    a host name string to query in DNS, an IP address as a string, an IP
    address in binary form, or a previously resolved NODE_INFO structure
    containing the remote IP address and associated MAC address. When a
    host name or IP address only is provided, the TCP module will
    internally perform the necessary DNS and/or ARP resolution steps before
    reporting that the TCP socket is connected (via a call to
    TCPISConnected returning TRUE). Server sockets ignore this destination
    parameter and listen only on the indicated port.
    
    The vSocketPurpose field allows sockets to be opened with varying
    buffer size parameters and memory storage mediums. This field
    corresponds to pre-defined sockets allocated in the
    TCPSocketInitializer[] array in TCPIPConfig.h. The TCPIPConfig.h file
    can be edited using the TCP/IP Configuration Wizard.
    
    Sockets are statically allocated on boot, but can be claimed with this
    \function and freed using TCPDisconnect or TCPClose (for client
    sockets). Server sockets can be freed using TCPClose only (calls to
    TCPDisconnect will return server sockets to the listening state,
    allowing reuse).

  Conditions:
    TCP is initialized.

  Input:
    dwRemoteHost -     For client sockets only. Provide a pointer to a
                       null\-terminated string of the remote host name (ex\:
                       "www.microchip.com" or "192.168.1.123"), a literal
                       destination IP address (ex\: 0x7B01A8C0 or an IP_ADDR
                       data type), or a pointer to a NODE_INFO structure
                       with the remote IP address and remote node or gateway
                       MAC address specified. If a string is provided, note
                       that it must be statically allocated in memory and
                       cannot be modified or deallocated until
                       TCPIsConnected returns TRUE.<p />This parameter is
                       ignored for server sockets.
    vRemoteHostType -  Any one of the following flags to identify the
                       meaning of the dwRemoteHost parameter\:
                       * TCP_OPEN_SERVER &#45; Open a server socket and
                         ignore the dwRemoteHost parameter.
                       * TCP_OPEN_RAM_HOST &#45; Open a client socket and
                         connect it to a remote host who's name is stored as a
                         null terminated string in a RAM array. Ex\:
                         "www.microchip.com" or "192.168.0.123" (BYTE&#42;
                         type)
                       * TCP_OPEN_ROM_HOST &#45; Open a client socket and
                         connect it to a remote host who's name is stored as a
                         null terminated string in a literal string or ROM
                         array. Ex\: "www.microchip.com" or "192.168.0.123"
                         (ROM BYTE&#42; type)
                       * TCP_OPEN_IP_ADDRESS &#45; Open a client socket and
                         connect it to a remote IP address. Ex\: 0x7B01A8C0
                         for 192.168.1.123 (DWORD type). Note that the byte
                         ordering is big endian.
                       * TCP_OPEN_NODE_INFO &#45; Open a client socket and
                         connect it to a remote IP and MAC addresses pair
                         stored in a NODE_INFO structure. dwRemoteHost must be
                         a pointer to the NODE_INFO structure. This option is
                         provided for backwards compatibility with
                         applications built against prior stack versions that
                         only implemented the TCPConnect() function. It can
                         also be used to skip DNS and ARP resolution steps if
                         connecting to a remote node which you've already
                         connected to and have cached addresses for.
    wPort -            TCP port to listen on or connect to\:
                       * Client sockets &#45; the remote TCP port to which a
                         connection should be made. The local port for client
                         sockets will be automatically picked by the TCP
                         module.
                       * Server sockets &#45; the local TCP port on which to
                         listen for connections.
    vSocketPurpose -   Any of the TCP_PURPOSE_* constants defined in
                       TCPIPConfig.h or the TCPIPConfig utility (see
                       TCPSocketInitializer[] array).

  Return Values:
    INVALID_SOCKET -  No sockets of the specified type were available to be
                      opened.
    Otherwise -       A TCP_SOCKET handle. Save this handle and use it when
                      calling all other TCP APIs.

  Remarks:
    This function replaces the old TCPConnect and TCPListen functions.
    
    If TCP_OPEN_RAM_HOST or TCP_OPEN_ROM_HOST are used for the destination
    type, the DNS client module must also be enabled (STACK_USE_DNS must be
    defined in TCPIPConfig.h).

  Example:
    \ \ 
    <code>
    // Open a server socket
    skt = TCPOpen(NULL, TCP_OPEN_SERVER, HTTP_PORT, TCP_PURPOSE_HTTP_SERVER);
    
    // Open a client socket to www.microchip.com
    // The double cast here prevents compiler warnings
    skt = TCPOpen((DWORD)(PTR_BASE)"www.microchip.com",
                    TCP_OPEN_ROM_HOST, 80, TCP_PURPOSE_DEFAULT);
    
    // Reopen a client socket without repeating DNS or ARP
    SOCKET_INFO cache = TCPGetSocketInfo(skt);  // Call with the old socket
    skt = TCPOpen((DWORD)(PTR_BASE)&amp;cache.remote, TCP_OPEN_NODE_INFO,
                    cache.remotePort.Val, TCP_PURPOSE_DEFAULT);
    </code>                                                    
  *****************************************************************************/
TCP_SOCKET TCPOpen(DWORD dwRemoteHost, BYTE vRemoteHostType, WORD wPort, BYTE vSocketPurpose)
{
	TCP_SOCKET hTCP;

	// Find an available socket that matches the specified socket type
	for(hTCP = 0; hTCP < TCP_SOCKET_COUNT; hTCP++)
	{
		SyncTCBStub(hTCP);

		// Sockets that are in use will be in a non-closed state
		if(MyTCBStub.smState != TCP_CLOSED)
			continue;

		SyncTCB();

		// See if this socket matches the desired type
		if(MyTCB.vSocketPurpose != vSocketPurpose)
			continue;

		// Start out assuming worst case Maximum Segment Size (changes when MSS 
		// option is received from remote node)
		MyTCB.wRemoteMSS = 536;

		// See if this is a server socket
		if(vRemoteHostType == TCP_OPEN_SERVER)
		{
			MyTCB.localPort.Val = wPort;
			MyTCBStub.Flags.bServer = TRUE;
			MyTCBStub.smState = TCP_LISTEN;
			MyTCBStub.remoteHash.Val = wPort;
			#if defined(STACK_USE_SSL_SERVER)
			MyTCB.localSSLPort.Val = 0;
			#endif
		}
		// Handle all the client mode socket types
		else
		{
			#if defined(STACK_CLIENT_MODE)
			{
				// Each new socket that is opened by this node, gets the 
				// next sequential local port number.
				if(NextPort < LOCAL_PORT_START_NUMBER || NextPort > LOCAL_PORT_END_NUMBER)
					NextPort = LOCAL_PORT_START_NUMBER;
				
				// Set the non-zero TCB fields
				MyTCB.localPort.Val = NextPort++;
				MyTCB.remotePort.Val = wPort;
	
				// Flag to start the DNS, ARP, SYN processes
				MyTCBStub.eventTime = TickGet();
				MyTCBStub.Flags.bTimerEnabled = 1;
	
				switch(vRemoteHostType)
				{
					#if defined(STACK_USE_DNS)
					case TCP_OPEN_RAM_HOST:
					case TCP_OPEN_ROM_HOST:
						MyTCB.remote.dwRemoteHost = dwRemoteHost;
						MyTCB.flags.bRemoteHostIsROM = (vRemoteHostType == TCP_OPEN_ROM_HOST);
						MyTCBStub.smState = TCP_GET_DNS_MODULE;
						break;
					#endif
		
					case TCP_OPEN_IP_ADDRESS:
						// dwRemoteHost is a literal IP address.  This 
						// doesn't need DNS and can skip directly to the 
						// Gateway ARPing step.
						MyTCBStub.remoteHash.Val = (((DWORD_VAL*)&dwRemoteHost)->w[1]+((DWORD_VAL*)&dwRemoteHost)->w[0] + wPort) ^ MyTCB.localPort.Val;
						MyTCB.remote.niRemoteMACIP.IPAddr.Val = dwRemoteHost;
						MyTCB.retryCount = 0;
						MyTCB.retryInterval = (TICK_SECOND/4)/256;
						MyTCBStub.smState = TCP_GATEWAY_SEND_ARP;
						break;
		
					case TCP_OPEN_NODE_INFO:
						MyTCBStub.remoteHash.Val = (((NODE_INFO*)(PTR_BASE)dwRemoteHost)->IPAddr.w[1]+((NODE_INFO*)(PTR_BASE)dwRemoteHost)->IPAddr.w[0] + wPort) ^ MyTCB.localPort.Val;
						memcpy((void*)(BYTE*)&MyTCB.remote, (void*)(BYTE*)(PTR_BASE)dwRemoteHost, sizeof(NODE_INFO));
						MyTCBStub.smState = TCP_SYN_SENT;
						SendTCP(SYN, SENDTCP_RESET_TIMERS);
						break;
				}
			}		
			#else
			{
				return INVALID_SOCKET;
			}	
			#endif
		}
		
		return hTCP;		
	}

	// If there is no socket available, return error.
	return INVALID_SOCKET;
}


/*****************************************************************************
  Function:
	BOOL TCPWasReset(TCP_SOCKET hTCP)

  Summary:
	Self-clearing semaphore inidicating socket reset.

  Description:
	This function is a self-clearing semaphore indicating whether or not
	a socket has been disconnected since the previous call.  This function
	works for all possible disconnections: a call to TCPDisconnect, a FIN 
	from the remote node, or an acknowledgement timeout caused by the loss
	of a network link.  It also returns TRUE after the first call to TCPInit.
	Applications should use this function to reset their state machines.
	
	This function was added due to the possibility of an error when relying
	on TCPIsConnected returing FALSE to check for a condition requiring a
	state machine reset.  If a socket is closed (due to a FIN ACK) and then
	immediately reopened (due to a the arrival of a new SYN) in the same
	cycle of the stack, calls to TCPIsConnected by the application will 
	never return FALSE even though the socket has been disconnected.  This 
	can cause errors for protocols such as HTTP in which a client will 
	immediately open a new connection upon closing of a prior one.  Relying
	on this function instead allows applications to trap those conditions 
	and properly reset their internal state for the new connection.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to check.

  Return Values:
  	TRUE - The socket has been disconnected since the previous call.
  	FALSE - The socket has not been disconnected since the previous call.
  ***************************************************************************/
BOOL TCPWasReset(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return TRUE;
    }
    
	SyncTCBStub(hTCP);
	
	if(MyTCBStub.Flags.bSocketReset)
	{
		MyTCBStub.Flags.bSocketReset = 0;
		return TRUE;
	}	
	
	return FALSE;
}


/*****************************************************************************
  Function:
	BOOL TCPIsConnected(TCP_SOCKET hTCP)

  Summary:
	Determines if a socket has an established connection.

  Description:
	This function determines if a socket has an established connection to 
	a remote node.  Call this function after calling TCPOpen to determine 
	when the connection is set up and ready for use.  This function was 
	historically used to check for disconnections, but TCPWasReset is now a
	more appropriate solution. 

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to check.

  Return Values:
  	TRUE - The socket has an established connection to a remote node.
  	FALSE - The socket is not currently connected.

  Remarks:
	A socket is said to be connected only if it is in the TCP_ESTABLISHED
	state.  Sockets in the process of opening or closing will return FALSE.
  ***************************************************************************/
BOOL TCPIsConnected(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	return (MyTCBStub.smState == TCP_ESTABLISHED);
}


/*****************************************************************************
  Function:
	void TCPDisconnect(TCP_SOCKET hTCP)

  Summary:
	Disconnects an open socket.

  Description:
	This function closes a connection to a remote node by sending a FIN (if 
	currently connected).
	
	The function can be called a second time to force a socket closed by 
	sending a RST packet.  This is useful when the application knows that 
	the remote node will not send an ACK (if it has crashed or lost its link),
	or when the application needs to reuse the socket immediately regardless
	of whether or not the remote node would like to transmit more data before
	closing.
	
	For client mode sockets, upon return, the hTCP handle is relinquished to 
	the TCP/IP stack and must no longer be used by the application (except for 
	an immediate subsequent call to TCPDisconnect() to force a RST 
	transmission, if needed).  
	
	For server mode sockets, upon return, the hTCP handle is NOT relinquished 
	to the TCP/IP stack.  After closing, the socket returns to the listening 
	state allowing future connection requests to be serviced.  This leaves the 
	hTCP handle in a valid state and must be retained for future operations on 
	the socket.  If you want to close the server and relinquish the socket back 
	to the TCP/IP stack, call the TCPClose() API instead of TCPDisconnect().

  Precondition:
	None

  Parameters:
	hTCP - Handle of the socket to disconnect.

  Returns:
	None

  Remarks:
	If the socket is using SSL, a CLOSE_NOTIFY record will be transmitted
	first to allow the SSL session to be resumed at a later time.
  ***************************************************************************/
void TCPDisconnect(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	SyncTCBStub(hTCP);

	// Delete all data in the RX FIFO
	// In this stack's API, the application TCP handle is 
	// immediately invalid after calling this function, so there 
	// is no longer any way to receive data from the TCP RX FIFO, 
	// even though the data is still there.  Leaving the data there 
	// could interfere with the remote node sending us a FIN if our
	// RX window is zero
	MyTCBStub.rxTail = MyTCBStub.rxHead;

	switch(MyTCBStub.smState)
	{
		#if defined(STACK_CLIENT_MODE) && defined(STACK_USE_DNS)
		case TCP_DNS_RESOLVE:
			DNSEndUsage();	// Release the DNS module, since the user is aborting
			CloseSocket();
			break;
		#endif

		case TCP_GET_DNS_MODULE:
		case TCP_GATEWAY_SEND_ARP:
		case TCP_GATEWAY_GET_ARP:
		case TCP_SYN_SENT:
			CloseSocket();
			break;

		case TCP_SYN_RECEIVED:
		case TCP_ESTABLISHED:
			#if defined(STACK_USE_SSL)
			// When disconnecting SSL sockets, send a close_notify so we can resume later
			if(MyTCBStub.sslStubID != SSL_INVALID_ID)
			{
				// Flush pending data and send close_notify
				SSLTxRecord(hTCP, MyTCBStub.sslStubID, SSL_APPLICATION);
				SSLTxMessage(hTCP, MyTCBStub.sslStubID, SSL_ALERT_CLOSE_NOTIFY);
			}
			#endif

			// Send the FIN.  This is done in a loop to ensure that if we have 
			// more data wating in the TX FIFO than can be sent in a single 
			// packet (due to the remote Max Segment Size packet size limit), 
			// we will keep generating more packets until either all data gets 
			// transmitted or the remote node's receive window fills up.
			do
			{
				SendTCP(FIN | ACK, SENDTCP_RESET_TIMERS);
				if(MyTCB.remoteWindow == 0u)
					break;
			} while(MyTCBStub.txHead != MyTCB.txUnackedTail);
			
			MyTCBStub.smState = TCP_FIN_WAIT_1;
			break;

		case TCP_CLOSE_WAIT:
			// Send the FIN.  This is done in a loop to ensure that if we have 
			// more data wating in the TX FIFO than can be sent in a single 
			// packet (due to the remote Max Segment Size packet size limit), 
			// we will keep generating more packets until either all data gets 
			// transmitted or the remote node's receive window fills up.
			do
			{
				SendTCP(FIN | ACK, SENDTCP_RESET_TIMERS);
				if(MyTCB.remoteWindow == 0u)
					break;
			} while(MyTCBStub.txHead != MyTCB.txUnackedTail);

			MyTCBStub.smState = TCP_LAST_ACK;
			break;
			
		// These states are all already closed or don't need explicit disconnecting -- they will disconnect by themselves after a while
		//case TCP_CLOSED:
		//case TCP_LISTEN:
		//case TCP_CLOSING:
		//case TCP_TIME_WAIT:
		//	return;

		case TCP_CLOSED_BUT_RESERVED:
			MyTCBStub.smState = TCP_CLOSED;
			break;

		// These states will close themselves after some delay, however, 
		// this is handled so that the user can call TCPDisconnect() 
		// twice to immediately close a socket (using an RST) without 
		// having to get an ACK back from the remote node.  This is 
		// great for instance when the application determines that 
		// the remote node has been physically disconnected and 
		// already knows that no ACK will be returned.  Alternatively, 
		// if the application needs to immediately reuse the socket 
		// regardless of what the other node's state is in (half open).
		case TCP_FIN_WAIT_1:
		case TCP_FIN_WAIT_2:
		case TCP_LAST_ACK:
		default:
			SendTCP(RST | ACK, 0);
			CloseSocket();
			break;
	}
}


/*****************************************************************************
  Function:
	void TCPClose(TCP_SOCKET hTCP)

  Summary:
	Disconnects an open socket and destroys the socket handle, including server 
	mode socket handles.

  Description:
	Disconnects an open socket and destroys the socket handle, including server 
	mode socket handles.  This function performs identically to the 
	TCPDisconnect() function, except that both client and server mode socket 
	handles are relinquished to the TCP/IP stack upon return.

  Precondition:
	None

  Parameters:
	hTCP - Handle to the socket to disconnect and close.

  Returns:
	None
  ***************************************************************************/
void TCPClose(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	SyncTCBStub(hTCP);
	MyTCBStub.Flags.bServer = FALSE;
	TCPDisconnect(hTCP);
}


/*****************************************************************************
  Function:
	SOCKET_INFO* TCPGetRemoteInfo(TCP_SOCKET hTCP)

  Summary:
	Obtains information about a currently open socket.

  Description:
	Returns the SOCKET_INFO structure associated with this socket.  This 
	contains the NODE_INFO structure with IP and MAC address (or gateway
	MAC) and the remote port.

  Precondition:
	TCP is initialized and the socket is connected.

  Parameters:
	hTCP - The socket to check.

  Returns:
	The SOCKET_INFO structure associated with this socket.  This structure is 
	allocated statically by the function and is valid only until the next 
	time TCPGetRemoteInfo() is called.
  ***************************************************************************/
SOCKET_INFO* TCPGetRemoteInfo(TCP_SOCKET hTCP)
{
	static SOCKET_INFO	RemoteInfo;

	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);
	SyncTCB();
	memcpy((void*)&RemoteInfo.remote, (void*)&MyTCB.remote, sizeof(NODE_INFO));
	RemoteInfo.remotePort.Val = MyTCB.remotePort.Val;

	return &RemoteInfo;
}



/****************************************************************************
  Section:
	Transmit Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	void TCPFlush(TCP_SOCKET hTCP)

  Summary:
	Immediately transmits all pending TX data.

  Description:
	This function immediately transmits all pending TX data with a PSH 
	flag.  If this function is not called, data will automatically be sent
	when either a) the TX buffer is half full or b) the 
	TCP_AUTO_TRANSMIT_TIMEOUT_VAL (default: 40ms) has elapsed.

  Precondition:
	TCP is initialized and the socket is connected.

  Parameters:
	hTCP - The socket whose data is to be transmitted.

  Returns:
	None

  Remarks:
	SSL application data is automatically flushed, so this function has 
	no effect for SSL sockets.
  ***************************************************************************/
void TCPFlush(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	SyncTCBStub(hTCP);
	SyncTCB();

	// NOTE: Pending SSL data will NOT be transferred here

	if(MyTCBStub.txHead != MyTCB.txUnackedTail)
	{
		// Send the TCP segment with all unacked bytes
		SendTCP(ACK, SENDTCP_RESET_TIMERS);
	}
}


/*****************************************************************************
  Function:
	WORD TCPIsPutReady(TCP_SOCKET hTCP)

  Summary:
	Determines how much free space is available in the TCP TX buffer.

  Description:
	Call this function to determine how many bytes can be written to the 
	TCP TX buffer.  If this function returns zero, the application must 
	return to the main stack loop before continuing in order to transmit
	more data.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to check.

  Returns:
	The number of bytes available to be written in the TCP TX buffer.
  ***************************************************************************/
WORD TCPIsPutReady(TCP_SOCKET hTCP)
{
	BYTE i;

	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	i = MyTCBStub.smState;

	// Unconnected sockets shouldn't be transmitting anything.
	if(!( (i == (BYTE)TCP_ESTABLISHED) || (i == (BYTE)TCP_CLOSE_WAIT) ))
		return 0;

	// Calculate the free space in this socket's TX FIFO
	#if defined(STACK_USE_SSL)
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
	{// Use sslTxHead as the head pointer when SSL is active
		WORD rem;
		
		// Find out raw free space
		if(MyTCBStub.sslTxHead >= MyTCBStub.txTail)
			rem = (MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart - 1) - (MyTCBStub.sslTxHead - MyTCBStub.txTail);
		else
			rem = MyTCBStub.txTail - MyTCBStub.sslTxHead - 1;
			
		// Reserve space for a new MAC and header
		if(rem > 22u)
			return rem - 22;
		else
			return 0;
	}
	#endif
	
	if(MyTCBStub.txHead >= MyTCBStub.txTail)
		return (MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart - 1) - (MyTCBStub.txHead - MyTCBStub.txTail);
	else
		return MyTCBStub.txTail - MyTCBStub.txHead - 1;
}


/*****************************************************************************
  Function:
	BOOL TCPPut(TCP_SOCKET hTCP, BYTE byte)

  Description:
	Writes a single byte to a TCP socket.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to which data is to be written.
	byte - The byte to write.

  Return Values:
	TRUE - The byte was written to the transmit buffer.
	FALSE - The transmit buffer was full, or the socket is not connected.
  ***************************************************************************/
BOOL TCPPut(TCP_SOCKET hTCP, BYTE byte)
{
	WORD wFreeTXSpace;

	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	wFreeTXSpace = TCPIsPutReady(hTCP);
	if(wFreeTXSpace == 0u)
		return FALSE;
	else if(wFreeTXSpace == 1u) // About to run out of space, lets transmit so the remote node might send an ACK back faster
		TCPFlush(hTCP);	

	// Send all current bytes if we are crossing half full
	// This is required to improve performance with the delayed 
	// acknowledgement algorithm
	if((!MyTCBStub.Flags.bHalfFullFlush) && (wFreeTXSpace <= ((MyTCBStub.bufferRxStart-MyTCBStub.bufferTxStart)>>1)))
	{
		TCPFlush(hTCP);	
		MyTCBStub.Flags.bHalfFullFlush = TRUE;
	}

	#if defined(STACK_USE_SSL)
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
	{
		TCPRAMCopy(MyTCBStub.sslTxHead, MyTCBStub.vMemoryMedium, (PTR_BASE)&byte, TCP_PIC_RAM, sizeof(byte));
		if(++MyTCBStub.sslTxHead >= MyTCBStub.bufferRxStart)
			MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
	}
	else
	{
		TCPRAMCopy(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, (PTR_BASE)&byte, TCP_PIC_RAM, sizeof(byte));
		if(++MyTCBStub.txHead >= MyTCBStub.bufferRxStart)
			MyTCBStub.txHead = MyTCBStub.bufferTxStart;
	}
	#else
	TCPRAMCopy(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, (PTR_BASE)&byte, TCP_PIC_RAM, sizeof(byte));
	if(++MyTCBStub.txHead >= MyTCBStub.bufferRxStart)
		MyTCBStub.txHead = MyTCBStub.bufferTxStart;
	#endif
	

	// Send the last byte as a separate packet (likely will make the remote node send back ACK faster)
	if(wFreeTXSpace == 1u)
	{
		TCPFlush(hTCP);
	}
	// If not already enabled, start a timer so this data will 
	// eventually get sent even if the application doens't call
	// TCPFlush()
	else if(!MyTCBStub.Flags.bTimer2Enabled)
	{
		MyTCBStub.Flags.bTimer2Enabled = TRUE;
		MyTCBStub.eventTime2 = (WORD)TickGetDiv256() + TCP_AUTO_TRANSMIT_TIMEOUT_VAL/256ull;
	}

	return TRUE;
}

/*****************************************************************************
  Function:
	WORD TCPPutArray(TCP_SOCKET hTCP, BYTE* data, WORD len)

  Description:
	Writes an array from RAM to a TCP socket.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to which data is to be written.
	data - Pointer to the array to be written.
	len  - Number of bytes to be written.

  Returns:
	The number of bytes written to the socket.  If less than len, the
	buffer became full or the socket is not conected.
  ***************************************************************************/
WORD TCPPutArray(TCP_SOCKET hTCP, BYTE* data, WORD len)
{
	WORD wActualLen;
	WORD wFreeTXSpace;
	WORD wRightLen = 0;

	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	wFreeTXSpace = TCPIsPutReady(hTCP);
	if(wFreeTXSpace == 0u)
	{
		TCPFlush(hTCP);
		return 0;
	}

	wActualLen = wFreeTXSpace;
	if(wFreeTXSpace > len)
		wActualLen = len;

	// Send all current bytes if we are crossing half full
	// This is required to improve performance with the delayed 
	// acknowledgement algorithm
	if((!MyTCBStub.Flags.bHalfFullFlush) && (wFreeTXSpace <= ((MyTCBStub.bufferRxStart-MyTCBStub.bufferTxStart)>>1)))
	{
		TCPFlush(hTCP);	
		MyTCBStub.Flags.bHalfFullFlush = TRUE;
	}
	
	#if defined(STACK_USE_SSL)
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
	{
		// See if we need a two part put
		if(MyTCBStub.sslTxHead + wActualLen >= MyTCBStub.bufferRxStart)
		{
			wRightLen = MyTCBStub.bufferRxStart-MyTCBStub.sslTxHead;
			TCPRAMCopy(MyTCBStub.sslTxHead, MyTCBStub.vMemoryMedium, (PTR_BASE)data, TCP_PIC_RAM, wRightLen);
			data += wRightLen;
			wActualLen -= wRightLen;
			MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
		}
	
		TCPRAMCopy(MyTCBStub.sslTxHead, MyTCBStub.vMemoryMedium, (PTR_BASE)data, TCP_PIC_RAM, wActualLen);
		MyTCBStub.sslTxHead += wActualLen;
	}
	else
	{
		// See if we need a two part put
		if(MyTCBStub.txHead + wActualLen >= MyTCBStub.bufferRxStart)
		{
			wRightLen = MyTCBStub.bufferRxStart-MyTCBStub.txHead;
			TCPRAMCopy(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, (PTR_BASE)data, TCP_PIC_RAM, wRightLen);
			data += wRightLen;
			wActualLen -= wRightLen;
			MyTCBStub.txHead = MyTCBStub.bufferTxStart;
		}
	
		TCPRAMCopy(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, (PTR_BASE)data, TCP_PIC_RAM, wActualLen);
		MyTCBStub.txHead += wActualLen;
	}
	#else
	// See if we need a two part put
	if(MyTCBStub.txHead + wActualLen >= MyTCBStub.bufferRxStart)
	{
		wRightLen = MyTCBStub.bufferRxStart-MyTCBStub.txHead;
		TCPRAMCopy(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, (PTR_BASE)data, TCP_PIC_RAM, wRightLen);
		data += wRightLen;
		wActualLen -= wRightLen;
		MyTCBStub.txHead = MyTCBStub.bufferTxStart;
	}

	TCPRAMCopy(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, (PTR_BASE)data, TCP_PIC_RAM, wActualLen);
	MyTCBStub.txHead += wActualLen;
	#endif

	// Send these bytes right now if we are out of TX buffer space
	if(wFreeTXSpace <= len)
	{
		TCPFlush(hTCP);
	}
	// If not already enabled, start a timer so this data will 
	// eventually get sent even if the application doens't call
	// TCPFlush()
	else if(!MyTCBStub.Flags.bTimer2Enabled)
	{
		MyTCBStub.Flags.bTimer2Enabled = TRUE;
		MyTCBStub.eventTime2 = (WORD)TickGetDiv256() + TCP_AUTO_TRANSMIT_TIMEOUT_VAL/256ull;
	}

	return wActualLen + wRightLen;
}

/*****************************************************************************
  Function:
	WORD TCPPutROMArray(TCP_SOCKET hTCP, ROM BYTE* data, WORD len)

  Description:
	Writes an array from ROM to a TCP socket.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to which data is to be written.
	data - Pointer to the array to be written.
	len  - Number of bytes to be written.

  Returns:
	The number of bytes written to the socket.  If less than len, the
	buffer became full or the socket is not conected.

  Remarks:
	This function is aliased to TCPPutArray on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
WORD TCPPutROMArray(TCP_SOCKET hTCP, ROM BYTE* data, WORD len)
{
	WORD wActualLen;
	WORD wFreeTXSpace;
	WORD wRightLen = 0;

	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	wFreeTXSpace = TCPIsPutReady(hTCP);
	if(wFreeTXSpace == 0u)
	{
		TCPFlush(hTCP);
		return 0;
	}

	// Send all current bytes if we are crossing half full
	// This is required to improve performance with the delayed 
	// acknowledgement algorithm
	if((!MyTCBStub.Flags.bHalfFullFlush) && (wFreeTXSpace <= ((MyTCBStub.bufferRxStart-MyTCBStub.bufferTxStart)>>1)))
	{
		TCPFlush(hTCP);	
		MyTCBStub.Flags.bHalfFullFlush = TRUE;
	}
	
	wActualLen = wFreeTXSpace;
	if(wFreeTXSpace > len)
		wActualLen = len;
	
	#if defined(STACK_USE_SSL)
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
	{
		// See if we need a two part put
		if(MyTCBStub.sslTxHead + wActualLen >= MyTCBStub.bufferRxStart)
		{
			wRightLen = MyTCBStub.bufferRxStart-MyTCBStub.sslTxHead;
			TCPRAMCopyROM(MyTCBStub.sslTxHead, MyTCBStub.vMemoryMedium, data, wRightLen);
			data += wRightLen;
			wActualLen -= wRightLen;
			MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
		}
	
		TCPRAMCopyROM(MyTCBStub.sslTxHead, MyTCBStub.vMemoryMedium, data, wActualLen);
		MyTCBStub.sslTxHead += wActualLen;
	}
	else
	{
		// See if we need a two part put
		if(MyTCBStub.txHead + wActualLen >= MyTCBStub.bufferRxStart)
		{
			wRightLen = MyTCBStub.bufferRxStart-MyTCBStub.txHead;
			TCPRAMCopyROM(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, data, wRightLen);
			data += wRightLen;
			wActualLen -= wRightLen;
			MyTCBStub.txHead = MyTCBStub.bufferTxStart;
		}
	
		TCPRAMCopyROM(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, data, wActualLen);
		MyTCBStub.txHead += wActualLen;
	}
	#else
	// See if we need a two part put
	if(MyTCBStub.txHead + wActualLen >= MyTCBStub.bufferRxStart)
	{
		wRightLen = MyTCBStub.bufferRxStart-MyTCBStub.txHead;
		TCPRAMCopyROM(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, data, wRightLen);
		data += wRightLen;
		wActualLen -= wRightLen;
		MyTCBStub.txHead = MyTCBStub.bufferTxStart;
	}

	TCPRAMCopyROM(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, data, wActualLen);
	MyTCBStub.txHead += wActualLen;
	#endif

	// Send these bytes right now if we are out of TX buffer space
	if(wFreeTXSpace <= len)
	{
		TCPFlush(hTCP);
	}
	// If not already enabled, start a timer so this data will 
	// eventually get sent even if the application doens't call
	// TCPFlush()
	else if(!MyTCBStub.Flags.bTimer2Enabled)
	{
		MyTCBStub.Flags.bTimer2Enabled = TRUE;
		MyTCBStub.eventTime2 = (WORD)TickGetDiv256() + TCP_AUTO_TRANSMIT_TIMEOUT_VAL/256ull;
	}

	return wActualLen + wRightLen;
}
#endif

/*****************************************************************************
  Function:
	BYTE* TCPPutString(TCP_SOCKET hTCP, BYTE* data)

  Description:
	Writes a null-terminated string from RAM to a TCP socket.  The 
	null-terminator is not copied to the socket.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to which data is to be written.
	data - Pointer to the string to be written.

  Returns:
	Pointer to the byte following the last byte written to the socket.  If
	this pointer does not dereference to a NUL byte, the buffer became full
	or the socket is not connected.

  Remarks:
	The return value of this function differs from that of TCPPutArray.  To
	write long strings in a single state, initialize the *data pointer to the
	first byte, then call this function repeatedly (breaking to the main 
	stack loop after each call) until the return value dereferences to a NUL
	byte.  Save the return value as the new starting *data pointer otherwise.
  ***************************************************************************/
BYTE* TCPPutString(TCP_SOCKET hTCP, BYTE* data)
{
	return data + TCPPutArray(hTCP, data, strlen((char*)data));
}

/*****************************************************************************
  Function:
	BYTE* TCPPutROMString(TCP_SOCKET hTCP, ROM BYTE* data)

  Description:
	Writes a null-terminated string from ROM to a TCP socket.  The 
	null-terminator is not copied to the socket.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to which data is to be written.
	data - Pointer to the string to be written.

  Returns:
	Pointer to the byte following the last byte written to the socket.  If
	this pointer does not dereference to a NUL byte, the buffer became full
	or the socket is not connected.

  Remarks:
	The return value of this function differs from that of TCPPutArray.  To
	write long strings in a single state, initialize the *data pointer to the
	first byte, then call this function repeatedly (breaking to the main 
	stack loop after each call) until the return value dereferences to a NUL
	byte.  Save the return value as the new starting *data pointer otherwise.
	
	This function is aliased to TCPPutString on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
ROM BYTE* TCPPutROMString(TCP_SOCKET hTCP, ROM BYTE* data)
{
	return data + TCPPutROMArray(hTCP, data, strlenpgm((ROM char*)data));
}
#endif

/*****************************************************************************
  Function:
	WORD TCPGetTxFIFOFull(TCP_SOCKET hTCP)

  Description:
	Determines how many bytes are pending in the TCP TX FIFO.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to check.

  Returns:
	Number of bytes pending to be flushed in the TCP TX FIFO.
  ***************************************************************************/
WORD TCPGetTxFIFOFull(TCP_SOCKET hTCP)
{
	WORD wDataLen;
	WORD wFIFOSize;

	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	// Calculate total usable FIFO size
	wFIFOSize = MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart - 1;

	// Find out how many data bytes are free in the TX FIFO
	wDataLen = TCPIsPutReady(hTCP);

	return wFIFOSize - wDataLen;
}



/****************************************************************************
  Section:
	Receive Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	void TCPDiscard(TCP_SOCKET hTCP)

  Description:
	Discards any pending data in the TCP RX FIFO.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket whose RX FIFO is to be cleared.

  Returns:
	None
  ***************************************************************************/
void TCPDiscard(TCP_SOCKET hTCP)
{
	if(TCPIsGetReady(hTCP))
	{
		SyncTCBStub(hTCP);
	
		// Delete all data in the RX buffer
		MyTCBStub.rxTail = MyTCBStub.rxHead;
	
		// Send a Window update message to the remote node
		SendTCP(ACK, SENDTCP_RESET_TIMERS);
	}
}


/*****************************************************************************
  Function:
	void WORD TCPIsGetReady(TCP_SOCKET hTCP)

  Summary:
	Determines how many bytes can be read from the TCP RX buffer.

  Description:
	Call this function to determine how many bytes can be read from the 
	TCP RX buffer.  If this function returns zero, the application must 
	return to the main stack loop before continuing in order to wait for
	more data to arrive.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to check.

  Returns:
	The number of bytes available to be read from the TCP RX buffer.
  ***************************************************************************/
WORD TCPIsGetReady(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);
		
	if(MyTCBStub.rxHead >= MyTCBStub.rxTail)
		return MyTCBStub.rxHead - MyTCBStub.rxTail;
	else
		return (MyTCBStub.bufferEnd - MyTCBStub.rxTail + 1) + (MyTCBStub.rxHead - MyTCBStub.bufferRxStart);
}


/*****************************************************************************
  Function:
	BOOL TCPGet(TCP_SOCKET hTCP, BYTE* byte)

  Description:
	Retrieves a single byte to a TCP socket.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket from which to read.
	byte - Pointer to location in which the read byte should be stored.

  Return Values:
	TRUE - A byte was read from the buffer.
	FALSE - The buffer was empty, or the socket is not connected.
  ***************************************************************************/
BOOL TCPGet(TCP_SOCKET hTCP, BYTE* byte)
{
	WORD wGetReadyCount;

	// See if there is any data which can be read
	wGetReadyCount = TCPIsGetReady(hTCP);
	if(wGetReadyCount == 0u)
		return FALSE;

	SyncTCBStub(hTCP);
	
	if(byte)
		TCPRAMCopy((PTR_BASE)byte, TCP_PIC_RAM, MyTCBStub.rxTail, MyTCBStub.vMemoryMedium, 1);
	if(++MyTCBStub.rxTail > MyTCBStub.bufferEnd)
		MyTCBStub.rxTail = MyTCBStub.bufferRxStart;

	// Send a window update if we've run out of data
	if(wGetReadyCount == 1u)
	{
		MyTCBStub.Flags.bTXASAPWithoutTimerReset = 1;
	}
	// If not already enabled, start a timer so a window 
	// update will get sent to the remote node at some point
	else if(!MyTCBStub.Flags.bTimer2Enabled)
	{
		MyTCBStub.Flags.bTimer2Enabled = TRUE;
		MyTCBStub.eventTime2 = (WORD)TickGetDiv256() + TCP_WINDOW_UPDATE_TIMEOUT_VAL/256ull;
	}


	return TRUE;
}


/*****************************************************************************
  Function:
	WORD TCPGetArray(TCP_SOCKET hTCP, BYTE* buffer, WORD len)

  Description:
	Reads an array of data bytes from a TCP socket's receive FIFO.  The data 
	is removed from the FIFO in the process.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket from which data is to be read.
	buffer - Pointer to the array to store data that was read.
	len  - Number of bytes to be read.

  Returns:
	The number of bytes read from the socket.  If less than len, the
	RX FIFO buffer became empty or the socket is not conected.
  ***************************************************************************/
WORD TCPGetArray(TCP_SOCKET hTCP, BYTE* buffer, WORD len)
{
	WORD wGetReadyCount;
	WORD RightLen = 0;

	// See if there is any data which can be read
	wGetReadyCount = TCPIsGetReady(hTCP);
	if(wGetReadyCount == 0u)
		return 0x0000u;

	SyncTCBStub(hTCP);

	// Make sure we don't try to read more data than is available
	if(len > wGetReadyCount)
		len = wGetReadyCount;

	// See if we need a two part get
	if(MyTCBStub.rxTail + len > MyTCBStub.bufferEnd)
	{
		RightLen = MyTCBStub.bufferEnd - MyTCBStub.rxTail + 1;
		if(buffer)
		{
			TCPRAMCopy((PTR_BASE)buffer, TCP_PIC_RAM, MyTCBStub.rxTail, MyTCBStub.vMemoryMedium, RightLen);
			buffer += RightLen;
		}
		len -= RightLen;
		MyTCBStub.rxTail = MyTCBStub.bufferRxStart;
	}

	if(buffer)
		TCPRAMCopy((PTR_BASE)buffer, TCP_PIC_RAM, MyTCBStub.rxTail, MyTCBStub.vMemoryMedium, len);
	MyTCBStub.rxTail += len;
	len += RightLen;

	// Send a window update if we've run low on data
	if(wGetReadyCount - len <= len)
	{
		MyTCBStub.Flags.bTXASAPWithoutTimerReset = 1;
	}
	else if(!MyTCBStub.Flags.bTimer2Enabled)
	// If not already enabled, start a timer so a window 
	// update will get sent to the remote node at some point
	{
		MyTCBStub.Flags.bTimer2Enabled = TRUE;
		MyTCBStub.eventTime2 = (WORD)TickGetDiv256() + TCP_WINDOW_UPDATE_TIMEOUT_VAL/256ull;
	}

	return len;
}


/*****************************************************************************
  Function:
	WORD TCPGetRxFIFOFree(TCP_SOCKET hTCP)

  Description:
	Determines how many bytes are free in the RX FIFO.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to check.

  Returns:
	The number of bytes free in the TCP RX FIFO.  If zero, no additional 
	data can be received until the application removes some data using one
	of the TCPGet family functions.
  ***************************************************************************/
WORD TCPGetRxFIFOFree(TCP_SOCKET hTCP)
{
	WORD wDataLen;
	WORD wFIFOSize;
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);
	
	// Calculate total usable FIFO size
	wFIFOSize = MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart;

	#if defined(STACK_USE_SSL)
	{
		PTR_BASE SSLtemp = MyTCBStub.rxHead;

		// Move SSL pointer to determine full buffer size
		if(MyTCBStub.sslStubID != SSL_INVALID_ID)
			MyTCBStub.rxHead = MyTCBStub.sslRxHead;

		// Find out how many data bytes are actually in the RX FIFO
		wDataLen = TCPIsGetReady(hTCP);
		
		// Move SSL pointer back to proper location (if we changed it)
		MyTCBStub.rxHead = SSLtemp;
	}
	#else
	{
		// Find out how many data bytes are actually in the RX FIFO
		wDataLen = TCPIsGetReady(hTCP);
	}
	#endif
	
	// Perform the calculation	
	return wFIFOSize - wDataLen;
}

/*****************************************************************************
  Function:
	WORD TCPPeekArray(TCP_SOCKET hTCP, BYTE *vBuffer, WORD wLen, WORD wStart)

  Summary:
  	Reads a specified number of data bytes from the TCP RX FIFO without 
  	removing them from the buffer.

  Description:
	Reads a specified number of data bytes from the TCP RX FIFO without 
  	removing them from the buffer.  No TCP control actions are taken as a 
  	result of this function (ex: no window update is sent to the remote node).
  	
  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to peak from (read without removing from stream).
	vBuffer - Destination to write the peeked data bytes.
	wLen - Length of bytes to peak from the RX FIFO and copy to vBuffer.
	wStart - Zero-indexed starting position within the FIFO to start peeking 
		from.

  Return Values:
	Number of bytes actually peeked from the stream and copied to vBuffer.  
	This value can be less than wLen if wStart + wLen is greater than the 
	deepest possible character in the RX FIFO.

  Remarks:
  	None
  ***************************************************************************/
WORD TCPPeekArray(TCP_SOCKET hTCP, BYTE *vBuffer, WORD wLen, WORD wStart)
{
	PTR_BASE ptrRead;
	WORD w;
	WORD wBytesUntilWrap;

	if(hTCP >= TCP_SOCKET_COUNT || wLen == 0)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	// Find out how many bytes are in the RX FIFO and decrease read length 
	// if the start offset + read length is beyond the end of the FIFO
	w = TCPIsGetReady(hTCP);
	if(wStart + wLen > w)
		wLen = w - wStart;

	// Find the read start location
	ptrRead = MyTCBStub.rxTail + wStart;
	if(ptrRead > MyTCBStub.bufferEnd)
		ptrRead -= MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;

	// Calculate how many bytes can be read in a single go
	wBytesUntilWrap = MyTCBStub.bufferEnd - ptrRead + 1;
	if(wLen <= wBytesUntilWrap)
	{
		// Read all at once
		TCPRAMCopy((PTR_BASE)vBuffer, TCP_PIC_RAM, ptrRead, MyTCBStub.vMemoryMedium, wLen);
	}
	else
	{
		// Read all bytes up to the wrap position and then read remaining bytes 
		// at the start of the buffer
		TCPRAMCopy((PTR_BASE)vBuffer, TCP_PIC_RAM, ptrRead, MyTCBStub.vMemoryMedium, wBytesUntilWrap);
		TCPRAMCopy((PTR_BASE)vBuffer+wBytesUntilWrap, TCP_PIC_RAM, MyTCBStub.bufferRxStart, MyTCBStub.vMemoryMedium, wLen - wBytesUntilWrap);
	}
	
	return wLen;
}

/*****************************************************************************
  Function:
	BYTE TCPPeek(TCP_SOCKET hTCP, WORD wStart)

  Summary:
  	Peaks at one byte in the TCP RX FIFO without removing it from the buffer.

  Description:
	Peaks at one byte in the TCP RX FIFO without removing it from the buffer.
  	
  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to peak from (read without removing from stream).
	wStart - Zero-indexed starting position within the FIFO to peek from.

  Return Values:
	Byte peeked from the RX FIFO.  If there is no data in the buffer or an 
	illegal wStart starting offset is given, then an indeterminate value is 
	returned.  The caller must ensure that valid parameters are passed to avoid 
	(i.e ensure that TCPIsGetReady() returns a number that is less than wStart 
	before calling TCPPeek()).

  Remarks:
  	Use the TCPPeekArray() function to read more than one byte.  It will 
  	perform better than calling TCPPeek() in a loop.
  ***************************************************************************/
BYTE TCPPeek(TCP_SOCKET hTCP, WORD wStart)
{
	BYTE i;
	
	TCPPeekArray(hTCP, &i, 1, wStart);
	return i;
}


/****************************************************************************
  Section:
	Search Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	WORD TCPFindArrayEx(TCP_SOCKET hTCP, BYTE* cFindArray, WORD wLen, 
						WORD wStart, WORD wSearchLen, BOOL bTextCompare)

  Summary:
  	Searches for a string in the TCP RX buffer.

  Description:
	This function finds the first occurrance of an array of bytes in the
	TCP RX buffer.  It can be used by an application to abstract searches 
	out of their own application code.  For increased efficiency, the 
	function is capable of limiting the scope of search to a specific
	range of bytes.  It can also perform a case-insensitive search if
	required.
	
	For example, if the buffer contains "I love PIC MCUs!" and the search
	array is "love" with a length of 4, a value of 2 will be returned.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to search within.
	cFindArray - The array of bytes to find in the buffer.
	wLen - Length of cFindArray.
	wStart - Zero-indexed starting position within the buffer.
	wSearchLen - Length from wStart to search in the buffer.
	bTextCompare - TRUE for case-insensitive text search, FALSE for binary search

  Return Values:
	0xFFFF - Search array not found
	Otherwise - Zero-indexed position of the first occurrance

  Remarks:
	Since this function usually must transfer data from external storage
	to internal RAM for comparison, its performance degrades significantly
	when the buffer is full and the array is not found.  For better 
	performance, try to search for characters that are expected to exist or
	limit the scope of the search as much as possible.  The HTTP2 module, 
	for example, uses this function to parse headers.  However, it searches 
	for newlines, then the separating colon, then reads the header name to 
	RAM for final comparison.  This has proven to be significantly faster  
	than searching for full header name strings outright.
  ***************************************************************************/
WORD TCPFindArrayEx(TCP_SOCKET hTCP, BYTE* cFindArray, WORD wLen, WORD wStart, WORD wSearchLen, BOOL bTextCompare)
{
	PTR_BASE ptrRead;
	WORD wDataLen;
	WORD wBytesUntilWrap;
	PTR_BASE ptrLocation;
	WORD wLenStart;
	BYTE *cFindArrayStart;
	BYTE i, j, k;
	BOOL isFinding;
	BYTE buffer[32];

	if(hTCP >= TCP_SOCKET_COUNT || wLen == 0)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	// Find out how many bytes are in the RX FIFO and return 
	// immediately if we won't possibly find a match
	wDataLen = TCPIsGetReady(hTCP) - wStart;
	if(wDataLen < wLen)
		return 0xFFFFu;
	if(wSearchLen && (wDataLen > wSearchLen))
		wDataLen = wSearchLen;

	ptrLocation = MyTCBStub.rxTail + wStart;
	if(ptrLocation > MyTCBStub.bufferEnd)
		ptrLocation -= MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;
	ptrRead = ptrLocation;
	wBytesUntilWrap = MyTCBStub.bufferEnd - ptrLocation + 1;
	ptrLocation = wStart;
	wLenStart = wLen;
	cFindArrayStart = cFindArray;
	j = *cFindArray++;
	isFinding = FALSE;
	if(bTextCompare)
	{
		if(j >= 'a' && j <= 'z')
			j += 'A'-'a';
	}

	// Search for the array
	while(1)
	{
		// Figure out how big of a chunk to read
		k = sizeof(buffer);
		if(k > wBytesUntilWrap)
			k = wBytesUntilWrap;
		if((WORD)k > wDataLen)
			k = wDataLen;

		// Read a chunk of data into the buffer
		TCPRAMCopy((PTR_BASE)buffer, TCP_PIC_RAM, ptrRead, MyTCBStub.vMemoryMedium, (WORD)k);
		ptrRead += k;
		wBytesUntilWrap -= k;

		if(wBytesUntilWrap == 0u)
		{
			ptrRead = MyTCBStub.bufferRxStart;
			wBytesUntilWrap = 0xFFFFu;
		}

		// Convert everything to uppercase
		if(bTextCompare)
		{
			for(i = 0; i < k; i++)
			{
				if(buffer[i] >= 'a' && buffer[i] <= 'z')
					buffer[i] += 'A'-'a';

				if(j == buffer[i])
				{
					if(--wLen == 0u)
						return ptrLocation-wLenStart + i + 1;
					j = *cFindArray++;
					isFinding = TRUE;
					if(j >= 'a' && j <= 'z')
						j += 'A'-'a';
				}
				else
				{
					wLen = wLenStart;
					if(isFinding)
					{
						cFindArray = cFindArrayStart;
						j = *cFindArray++;
						if(j >= 'a' && j <= 'z')
							j += 'A'-'a';
						isFinding = FALSE;
					}
				}
			}
		}
		else	// Compare as is
		{
			for(i = 0; i < k; i++)
			{
				if(j == buffer[i])
				{
					if(--wLen == 0u)
						return ptrLocation-wLenStart + i + 1;
					j = *cFindArray++;
					isFinding = TRUE;
				}
				else
				{
					wLen = wLenStart;
					if(isFinding)
					{
						cFindArray = cFindArrayStart;
						j = *cFindArray++;
						isFinding = FALSE;
					}
				}
			}
		}

		// Check to see if it is impossible to find a match
		wDataLen -= k;
		if(wDataLen < wLen)
			return 0xFFFFu;

		ptrLocation += k;
	}
}

/*****************************************************************************
  Function:
	WORD TCPFindROMArrayEx(TCP_SOCKET hTCP, BYTE* cFindArray, WORD wLen, 
						WORD wStart, WORD wSearchLen, BOOL bTextCompare)

  Summary:
  	Searches for a ROM string in the TCP RX buffer.

  Description:
	This function finds the first occurrance of an array of bytes in the
	TCP RX buffer.  It can be used by an application to abstract searches 
	out of their own application code.  For increased efficiency, the 
	function is capable of limiting the scope of search to a specific
	range of bytes.  It can also perform a case-insensitive search if
	required.
	
	For example, if the buffer contains "I love PIC MCUs!" and the search
	array is "love" with a length of 4, a value of 2 will be returned.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to search within.
	cFindArray - The array of bytes to find in the buffer.
	wLen - Length of cFindArray.
	wStart - Zero-indexed starting position within the buffer.
	wSearchLen - Length from wStart to search in the buffer.
	bTextCompare - TRUE for case-insensitive text search, FALSE for binary search

  Return Values:
	0xFFFF - Search array not found
	Otherwise - Zero-indexed position of the first occurrance

  Remarks:
	Since this function usually must transfer data from external storage
	to internal RAM for comparison, its performance degrades significantly
	when the buffer is full and the array is not found.  For better 
	performance, try to search for characters that are expected to exist or
	limit the scope of the search as much as possible.  The HTTP2 module, 
	for example, uses this function to parse headers.  However, it searches 
	for newlines, then the separating colon, then reads the header name to 
	RAM for final comparison.  This has proven to be significantly faster  
	than searching for full header name strings outright.
	
	This function is aliased to TCPFindArrayEx on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
WORD TCPFindROMArrayEx(TCP_SOCKET hTCP, ROM BYTE* cFindArray, WORD wLen, WORD wStart, WORD wSearchLen, BOOL bTextCompare)
{
	PTR_BASE ptrRead;
	WORD wDataLen;
	WORD wBytesUntilWrap;
	PTR_BASE ptrLocation;
	WORD wLenStart;
	ROM BYTE *cFindArrayStart;
	BYTE i, j, k;
	BOOL isFinding;
	BYTE buffer[32];

	if(hTCP >= TCP_SOCKET_COUNT || wLen == 0)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	// Find out how many bytes are in the RX FIFO and return 
	// immediately if we won't possibly find a match
	wDataLen = TCPIsGetReady(hTCP) - wStart;
	if(wDataLen < wLen)
		return 0xFFFFu;
	if(wSearchLen && (wDataLen > wSearchLen))
		wDataLen = wSearchLen;

	ptrLocation = MyTCBStub.rxTail + wStart;
	if(ptrLocation > MyTCBStub.bufferEnd)
		ptrLocation -= MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;
	ptrRead = ptrLocation;
	wBytesUntilWrap = MyTCBStub.bufferEnd - ptrLocation + 1;
	ptrLocation = wStart;
	wLenStart = wLen;
	cFindArrayStart = cFindArray;
	j = *cFindArray++;
	isFinding = FALSE;
	if(bTextCompare)
	{
		if(j >= 'a' && j <= 'z')
			j += 'A'-'a';
	}

	// Search for the array
	while(1)
	{
		// Figure out how big of a chunk to read
		k = sizeof(buffer);
		if(k > wBytesUntilWrap)
			k = wBytesUntilWrap;
		if((WORD)k > wDataLen)
			k = wDataLen;

		// Read a chunk of data into the buffer
		TCPRAMCopy((PTR_BASE)buffer, TCP_PIC_RAM, ptrRead, MyTCBStub.vMemoryMedium, (WORD)k);
		ptrRead += k;
		wBytesUntilWrap -= k;

		if(wBytesUntilWrap == 0u)
		{
			ptrRead = MyTCBStub.bufferRxStart;
			wBytesUntilWrap = 0xFFFFu;
		}

		// Convert everything to uppercase
		if(bTextCompare)
		{
			for(i = 0; i < k; i++)
			{
				if(buffer[i] >= 'a' && buffer[i] <= 'z')
					buffer[i] += 'A'-'a';

				if(j == buffer[i])
				{
					if(--wLen == 0u)
						return ptrLocation-wLenStart + i + 1;
					j = *cFindArray++;
					isFinding = TRUE;
					if(j >= 'a' && j <= 'z')
						j += 'A'-'a';
				}
				else
				{
					wLen = wLenStart;
					if(isFinding)
					{
						cFindArray = cFindArrayStart;
						j = *cFindArray++;
						if(j >= 'a' && j <= 'z')
							j += 'A'-'a';
						isFinding = FALSE;
					}
				}
			}
		}
		else	// Compare as is
		{
			for(i = 0; i < k; i++)
			{
				if(j == buffer[i])
				{
					if(--wLen == 0u)
						return ptrLocation-wLenStart + i + 1;
					j = *cFindArray++;
					isFinding = TRUE;
				}
				else
				{
					wLen = wLenStart;
					if(isFinding)
					{
						cFindArray = cFindArrayStart;
						j = *cFindArray++;
						isFinding = FALSE;
					}
				}
			}
		}

		// Check to see if it is impossible to find a match
		wDataLen -= k;
		if(wDataLen < wLen)
			return 0xFFFFu;

		ptrLocation += k;
	}
}
#endif


/*****************************************************************************
  Function:
	WORD TCPFindEx(TCP_SOCKET hTCP, BYTE cFind,
						WORD wStart, WORD wSearchLen, BOOL bTextCompare)

  Summary:
  	Searches for a byte in the TCP RX buffer.

  Description:
	This function finds the first occurrance of a byte in the TCP RX
	buffer.  It can be used by an application to abstract searches 
	out of their own application code.  For increased efficiency, the 
	function is capable of limiting the scope of search to a specific
	range of bytes.  It can also perform a case-insensitive search if
	required.
	
	For example, if the buffer contains "I love PIC MCUs!" and the cFind
	byte is ' ', a value of 1 will be returned.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP - The socket to search within.
	cFind - The byte to find in the buffer.
	wStart - Zero-indexed starting position within the buffer.
	wSearchLen - Length from wStart to search in the buffer.
	bTextCompare - TRUE for case-insensitive text search, FALSE for binary search

  Return Values:
	0xFFFF - Search array not found
	Otherwise - Zero-indexed position of the first occurrance

  Remarks:
	Since this function usually must transfer data from external storage
	to internal RAM for comparison, its performance degrades significantly
	when the buffer is full and the array is not found.  For better 
	performance, try to search for characters that are expected to exist or
	limit the scope of the search as much as possible.  The HTTP2 module, 
	for example, uses this function to parse headers.  However, it searches 
	for newlines, then the separating colon, then reads the header name to 
	RAM for final comparison.  This has proven to be significantly faster  
	than searching for full header name strings outright.
  ***************************************************************************/
WORD TCPFindEx(TCP_SOCKET hTCP, BYTE cFind, WORD wStart, WORD wSearchLen, BOOL bTextCompare)
{
	return TCPFindArrayEx(hTCP, &cFind, sizeof(cFind), wStart, wSearchLen, bTextCompare);
}



/****************************************************************************
  Section:
	Data Processing Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	void TCPTick(void)

  Summary:
  	Performs periodic TCP tasks.

  Description:
	This function performs any required periodic TCP tasks.  Each 
	socket's state machine is checked, and any elapsed timeout periods
	are handled.

  Precondition:
	TCP is initialized.

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
void TCPTick(void)
{
	TCP_SOCKET hTCP;
	BOOL bRetransmit;
	BOOL bCloseSocket;
	BYTE vFlags;
	WORD w;

	// Periodically all "not closed" sockets must perform timed operations
	for(hTCP = 0; hTCP < TCP_SOCKET_COUNT; hTCP++)
	{
		SyncTCBStub(hTCP);
		
		// Handle any SSL Processing and Message Transmission
		#if defined(STACK_USE_SSL)
		if(MyTCBStub.sslStubID != SSL_INVALID_ID)
		{
			// Handle any periodic tasks, such as RSA operations
			SSLPeriodic(hTCP, MyTCBStub.sslStubID);
			
			// If unsent data is waiting, transmit it as an application record
			if(MyTCBStub.sslTxHead != MyTCBStub.txHead && TCPSSLGetPendingTxSize(hTCP) != 0u)
				SSLTxRecord(hTCP, MyTCBStub.sslStubID, SSL_APPLICATION);
			
			// If an SSL message is requested, send it now
			if(MyTCBStub.sslReqMessage != SSL_NO_MESSAGE)
				SSLTxMessage(hTCP, MyTCBStub.sslStubID, MyTCBStub.sslReqMessage);
		}
		#endif
		
		vFlags = 0x00;
		bRetransmit = FALSE;
		bCloseSocket = FALSE;

		// Transmit ASAP data if the medium is available
		if(MyTCBStub.Flags.bTXASAP || MyTCBStub.Flags.bTXASAPWithoutTimerReset)
		{
			if(MACIsTxReady())
			{
				vFlags = ACK;
				bRetransmit = MyTCBStub.Flags.bTXASAPWithoutTimerReset;
			}
		}

		// Perform any needed window updates and data transmissions
		if(MyTCBStub.Flags.bTimer2Enabled)
		{
			// See if the timeout has occured, and we need to send a new window update and pending data
			if((SHORT)(MyTCBStub.eventTime2 - (WORD)TickGetDiv256()) <= (SHORT)0)
				vFlags = ACK;
		}

		// Process Delayed ACKnowledgement timer
		if(MyTCBStub.Flags.bDelayedACKTimerEnabled)
		{
			// See if the timeout has occured and delayed ACK needs to be sent
			if((SHORT)(MyTCBStub.OverlappedTimers.delayedACKTime - (WORD)TickGetDiv256()) <= (SHORT)0)
				vFlags = ACK;
		}
		
		// Process TCP_CLOSE_WAIT timer
		if(MyTCBStub.smState == TCP_CLOSE_WAIT)
		{
			// Automatically close the socket on our end if the application 
			// fails to call TCPDisconnect() is a reasonable amount of time.
			if((SHORT)(MyTCBStub.OverlappedTimers.closeWaitTime - (WORD)TickGetDiv256()) <= (SHORT)0)
			{
				vFlags = FIN | ACK;
				MyTCBStub.smState = TCP_LAST_ACK;
			}
		}

		// Process listening server sockets that might have a SYN waiting in the SYNQueue[]
		#if TCP_SYN_QUEUE_MAX_ENTRIES
			if(MyTCBStub.smState == TCP_LISTEN)
			{
				for(w = 0; w < TCP_SYN_QUEUE_MAX_ENTRIES; w++)
				{
					// Abort search if there are no more valid records
					if(SYNQueue[w].wDestPort == 0u)
						break;
					
					// Stop searching if this SYN queue entry can be used by this socket
					#if defined(STACK_USE_SSL_SERVER)
					if(SYNQueue[w].wDestPort == MyTCBStub.remoteHash.Val || SYNQueue[w].wDestPort == MyTCBStub.sslTxHead)
					#else
					if(SYNQueue[w].wDestPort == MyTCBStub.remoteHash.Val)
					#endif
					{
						// Set up our socket and generate a reponse SYN+ACK packet
						SyncTCB();
						
						#if defined(STACK_USE_SSL_SERVER)
						// If this matches the SSL port, make sure that can be configured
						// before continuing.  If not, break and leave this in the queue
						if(SYNQueue[w].wDestPort == MyTCBStub.sslTxHead && !TCPStartSSLServer(hTCP))
							break;
						#endif
						
						memcpy((void*)&MyTCB.remote.niRemoteMACIP, (void*)&SYNQueue[w].niSourceAddress, sizeof(NODE_INFO));
						MyTCB.remotePort.Val = SYNQueue[w].wSourcePort;
						MyTCB.RemoteSEQ = SYNQueue[w].dwSourceSEQ + 1;
						MyTCBStub.remoteHash.Val = (MyTCB.remote.niRemoteMACIP.IPAddr.w[1] + MyTCB.remote.niRemoteMACIP.IPAddr.w[0] + MyTCB.remotePort.Val) ^ MyTCB.localPort.Val;
						vFlags = SYN | ACK;
						MyTCBStub.smState = TCP_SYN_RECEIVED;
						
						// Delete this SYN from the SYNQueue and compact the SYNQueue[] array
						TCPRAMCopy((PTR_BASE)&SYNQueue[w], TCP_PIC_RAM, (PTR_BASE)&SYNQueue[w+1], TCP_PIC_RAM, (TCP_SYN_QUEUE_MAX_ENTRIES-1u-w)*sizeof(TCP_SYN_QUEUE));
						SYNQueue[TCP_SYN_QUEUE_MAX_ENTRIES-1].wDestPort = 0u;
	
						break;
					}
				}
			}
		#endif

		if(vFlags)
			SendTCP(vFlags, bRetransmit ? 0 : SENDTCP_RESET_TIMERS);

		// The TCP_CLOSED, TCP_LISTEN, and sometimes the TCP_ESTABLISHED 
		// state don't need any timeout events, so see if the timer is enabled
		if(!MyTCBStub.Flags.bTimerEnabled)
		{
			#if defined(TCP_KEEP_ALIVE_TIMEOUT)
				// Only the established state has any use for keep-alives
				if(MyTCBStub.smState == TCP_ESTABLISHED)
				{
					// If timeout has not occured, do not do anything.
					if((LONG)(TickGet() - MyTCBStub.eventTime) < (LONG)0)
						continue;
		
					// If timeout has occured and the connection appears to be dead (no 
					// responses from remote node at all), close the connection so the 
					// application doesn't sit around indefinitely with a useless socket 
					// that it thinks is still open
					if(MyTCBStub.Flags.vUnackedKeepalives == TCP_MAX_UNACKED_KEEP_ALIVES)
					{
						vFlags = MyTCBStub.Flags.bServer;

						// Force an immediate FIN and RST transmission
						// Double calling TCPDisconnect() will also place us 
						// back in the listening state immediately if a server socket.
						TCPDisconnect(hTCP);
						TCPDisconnect(hTCP);
						
						// Prevent client mode sockets from getting reused by other applications.  
						// The application must call TCPDisconnect() with the handle to free this 
						// socket (and the handle associated with it)
						if(!vFlags)
							MyTCBStub.smState = TCP_CLOSED_BUT_RESERVED;
						
						continue;
					}
					
					// Otherwise, if a timeout occured, simply send a keep-alive packet
					SyncTCB();
					SendTCP(ACK, SENDTCP_KEEP_ALIVE);
					MyTCBStub.eventTime = TickGet() + TCP_KEEP_ALIVE_TIMEOUT;
				}
			#endif
			continue;
		}

		// If timeout has not occured, do not do anything.
		if((LONG)(TickGet() - MyTCBStub.eventTime) < (LONG)0)
			continue;

		// Load up extended TCB information
		SyncTCB();

		// A timeout has occured.  Respond to this timeout condition
		// depending on what state this socket is in.
		switch(MyTCBStub.smState)
		{
			#if defined(STACK_CLIENT_MODE)
			#if defined(STACK_USE_DNS)
			case TCP_GET_DNS_MODULE:
				if(DNSBeginUsage())
				{
					MyTCBStub.smState = TCP_DNS_RESOLVE;
					if(MyTCB.flags.bRemoteHostIsROM)
						DNSResolveROM((ROM BYTE*)(ROM_PTR_BASE)MyTCB.remote.dwRemoteHost, DNS_TYPE_A);
					else
						DNSResolve((BYTE*)(PTR_BASE)MyTCB.remote.dwRemoteHost, DNS_TYPE_A);
				}
				break;
				
			case TCP_DNS_RESOLVE:
			{
				IP_ADDR ipResolvedDNSIP;

				// See if DNS resolution has finished.  Note that if the DNS 
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
						MyTCB.remote.niRemoteMACIP.IPAddr.Val = ipResolvedDNSIP.Val;
						MyTCBStub.smState = TCP_GATEWAY_SEND_ARP;
						MyTCBStub.remoteHash.Val = (MyTCB.remote.niRemoteMACIP.IPAddr.w[1]+MyTCB.remote.niRemoteMACIP.IPAddr.w[0] + MyTCB.remotePort.Val) ^ MyTCB.localPort.Val;
						MyTCB.retryCount = 0;
						MyTCB.retryInterval = (TICK_SECOND/4)/256;
					}
					else
					{
						MyTCBStub.eventTime = TickGet() + 10*TICK_SECOND;
						MyTCBStub.smState = TCP_GET_DNS_MODULE;
					}
				}
				break;
			}
			#endif // #if defined(STACK_USE_DNS)
				
			case TCP_GATEWAY_SEND_ARP:
				// Obtain the MAC address associated with the server's IP address (either direct MAC address on same subnet, or the MAC address of the Gateway machine)
				MyTCBStub.eventTime2 = (WORD)TickGetDiv256();
				ARPResolve(&MyTCB.remote.niRemoteMACIP.IPAddr);
				MyTCBStub.smState = TCP_GATEWAY_GET_ARP;
				break;

			case TCP_GATEWAY_GET_ARP:
				// Wait for the MAC address to finish being obtained
				if(!ARPIsResolved(&MyTCB.remote.niRemoteMACIP.IPAddr, &MyTCB.remote.niRemoteMACIP.MACAddr))
				{
					// Time out if too much time is spent in this state
					// Note that this will continuously send out ARP 
					// requests for an infinite time if the Gateway 
					// never responds
					if((WORD)TickGetDiv256() - MyTCBStub.eventTime2 > (WORD)MyTCB.retryInterval)
					{
						// Exponentially increase timeout until we reach 6 attempts then stay constant
						if(MyTCB.retryCount < 6u)
						{
							MyTCB.retryCount++;
							MyTCB.retryInterval <<= 1;
						}

						// Retransmit ARP request
						MyTCBStub.smState = TCP_GATEWAY_SEND_ARP;
					}
					break;
				}
				
				// Send out SYN connection request to remote node
				// This automatically disables the Timer from 
				// continuously firing for this socket
				vFlags = SYN;
				bRetransmit = FALSE;
				MyTCBStub.smState = TCP_SYN_SENT;
				break;
			#endif // #if defined(STACK_CLIENT_MODE)
			
			case TCP_SYN_SENT:
				// Keep sending SYN until we hear from remote node.
				// This may be for infinite time, in that case
				// caller must detect it and do something.
				vFlags = SYN;
				bRetransmit = TRUE;

				// Exponentially increase timeout until we reach TCP_MAX_RETRIES attempts then stay constant
				if(MyTCB.retryCount >= (TCP_MAX_RETRIES - 1))
				{
					MyTCB.retryCount = TCP_MAX_RETRIES - 1;
					MyTCB.retryInterval = TCP_START_TIMEOUT_VAL<<(TCP_MAX_RETRIES-1);
				}
				break;
	
			case TCP_SYN_RECEIVED:
				// We must receive ACK before timeout expires.
				// If not, resend SYN+ACK.
				// Abort, if maximum attempts counts are reached.
				if(MyTCB.retryCount < TCP_MAX_SYN_RETRIES)
				{
					vFlags = SYN | ACK;
					bRetransmit = TRUE;
				}
				else
				{
					if(MyTCBStub.Flags.bServer)
					{
						vFlags = RST | ACK;
						bCloseSocket = TRUE;
					}
					else
					{
						vFlags = SYN;
					}
				}
				break;
	
			case TCP_ESTABLISHED:
			case TCP_CLOSE_WAIT:
				// Retransmit any unacknowledged data
				if(MyTCB.retryCount < TCP_MAX_RETRIES)
				{
					vFlags = ACK;
					bRetransmit = TRUE;
				}
				else
				{
					// No response back for too long, close connection
					// This could happen, for instance, if the communication 
					// medium was lost
					MyTCBStub.smState = TCP_FIN_WAIT_1;
					vFlags = FIN | ACK;
				}
				break;
	
			case TCP_FIN_WAIT_1:
				if(MyTCB.retryCount < TCP_MAX_RETRIES)
				{
					// Send another FIN
					vFlags = FIN | ACK;
					bRetransmit = TRUE;
				}
				else
				{
					// Close on our own, we can't seem to communicate 
					// with the remote node anymore
					vFlags = RST | ACK;
					bCloseSocket = TRUE;
				}
				break;
	
			case TCP_FIN_WAIT_2:
				// Close on our own, we can't seem to communicate 
				// with the remote node anymore
				vFlags = RST | ACK;
				bCloseSocket = TRUE;
				break;

			case TCP_CLOSING:
				if(MyTCB.retryCount < TCP_MAX_RETRIES)
				{
					// Send another ACK+FIN (the FIN is retransmitted 
					// automatically since it hasn't been acknowledged by 
					// the remote node yet)
					vFlags = ACK;
					bRetransmit = TRUE;
				}
				else
				{
					// Close on our own, we can't seem to communicate 
					// with the remote node anymore
					vFlags = RST | ACK;
					bCloseSocket = TRUE;
				}
				break;
	
//			case TCP_TIME_WAIT:
//				// Wait around for a while (2MSL) and then goto closed state
//				bCloseSocket = TRUE;
//				break;
//			

			case TCP_LAST_ACK:
				// Send some more FINs or close anyway
				if(MyTCB.retryCount < TCP_MAX_RETRIES)
				{
					vFlags = FIN | ACK;
					bRetransmit = TRUE;
				}
				else
				{
					vFlags = RST | ACK;
					bCloseSocket = TRUE;
				}
				break;
			
			default:
				break;
		}

		if(vFlags)
		{
			// Transmit all unacknowledged data over again
			if(bRetransmit)
			{
				// Set the appropriate retry time
				MyTCB.retryCount++;
				MyTCB.retryInterval <<= 1;
		
				// Calculate how many bytes we have to roll back and retransmit
				w = MyTCB.txUnackedTail - MyTCBStub.txTail;
				if(MyTCB.txUnackedTail < MyTCBStub.txTail)
					w += MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart;
				
				// Perform roll back of local SEQuence counter, remote window 
				// adjustment, and cause all unacknowledged data to be 
				// retransmitted by moving the unacked tail pointer.
				MyTCB.MySEQ -= w;
				MyTCB.remoteWindow += w;
				MyTCB.txUnackedTail = MyTCBStub.txTail;		
				SendTCP(vFlags, 0);
			}
			else
				SendTCP(vFlags, SENDTCP_RESET_TIMERS);

		}
		
		if(bCloseSocket)
			CloseSocket();
	}
	
	
	#if TCP_SYN_QUEUE_MAX_ENTRIES
		// Process SYN Queue entry timeouts
		for(w = 0; w < TCP_SYN_QUEUE_MAX_ENTRIES; w++)
		{
			// Abort search if there are no more valid records
			if(SYNQueue[w].wDestPort == 0u)
				break;
			
			// See if this SYN has timed out
			if((WORD)TickGetDiv256() - SYNQueue[w].wTimestamp > (WORD)(TCP_SYN_QUEUE_TIMEOUT/256ull))
			{
				// Delete this SYN from the SYNQueue and compact the SYNQueue[] array
				TCPRAMCopy((PTR_BASE)&SYNQueue[w], TCP_PIC_RAM, (PTR_BASE)&SYNQueue[w+1], TCP_PIC_RAM, (TCP_SYN_QUEUE_MAX_ENTRIES-1u-w)*sizeof(TCP_SYN_QUEUE));
				SYNQueue[TCP_SYN_QUEUE_MAX_ENTRIES-1].wDestPort = 0u;
	
				// Since we deleted an entry, we need to roll back one 
				// index so next loop will process the correct record
				w--;	
			}
		}
	#endif
}


/*****************************************************************************
  Function:
	BOOL TCPProcess(NODE_INFO* remote, IP_ADDR* localIP, WORD len)

  Summary:
  	Handles incoming TCP segments.

  Description:
	This function handles incoming TCP segments.  When a segment arrives, it
	is compared to open sockets using a hash of the remote port and IP.  
	On a match, the data is passed to HandleTCPSeg for further processing.

  Precondition:
	TCP is initialized and a TCP segment is ready in the MAC buffer.

  Parameters:
	remote - Remote NODE_INFO structure
	localIP - This stack's IP address (for header checking)
	len - Total length of the waiting TCP segment

  Return Values:
	TRUE - the segment was properly handled.
	FALSE - otherwise
  ***************************************************************************/
BOOL TCPProcess(NODE_INFO* remote, IP_ADDR* localIP, WORD len)
{
	TCP_HEADER      TCPHeader;
	PSEUDO_HEADER   pseudoHeader;
	WORD_VAL        checksum1;
	WORD_VAL        checksum2;
	BYTE            optionsSize;

	// Calculate IP pseudoheader checksum.
	pseudoHeader.SourceAddress      = remote->IPAddr;
	pseudoHeader.DestAddress        = *localIP;
	pseudoHeader.Zero               = 0x0;
	pseudoHeader.Protocol           = IP_PROT_TCP;
	pseudoHeader.Length          	= len;

	SwapPseudoHeader(pseudoHeader);

	checksum1.Val = ~CalcIPChecksum((BYTE*)&pseudoHeader,
		sizeof(pseudoHeader));

	// Now calculate TCP packet checksum in NIC RAM - should match
	// pesudo header checksum
	checksum2.Val = CalcIPBufferChecksum(len);

	// Compare checksums.
	if(checksum1.Val != checksum2.Val)
	{
		MACDiscardRx();
		return TRUE;
	}

#if defined(DEBUG_GENERATE_RX_LOSS)
	// Throw RX packets away randomly
	if(LFSRRand() > DEBUG_GENERATE_RX_LOSS)
	{
		MACDiscardRx();
		return TRUE;
	}
#endif

	// Retrieve TCP header.
	IPSetRxBuffer(0);
	MACGetArray((BYTE*)&TCPHeader, sizeof(TCPHeader));
	SwapTCPHeader(&TCPHeader);


	// Skip over options to retrieve data bytes
	optionsSize = (BYTE)((TCPHeader.DataOffset.Val << 2)-
		sizeof(TCPHeader));
	len = len - optionsSize - sizeof(TCPHeader);

	// Find matching socket.
	if(FindMatchingSocket(&TCPHeader, remote))
	{
		#if defined(STACK_USE_SSL)
		PTR_BASE prevRxHead;
		// For SSL connections, show HandleTCPSeg() the full data buffer
		prevRxHead = MyTCBStub.rxHead;
		if(MyTCBStub.sslStubID != SSL_INVALID_ID)
			MyTCBStub.rxHead = MyTCBStub.sslRxHead;
		#endif
		
		HandleTCPSeg(&TCPHeader, len);
		
		#if defined(STACK_USE_SSL)
		if(MyTCBStub.sslStubID != SSL_INVALID_ID)
		{
			// Restore the buffer state
			MyTCBStub.sslRxHead = MyTCBStub.rxHead;
			MyTCBStub.rxHead = prevRxHead;

			// Process the new SSL data, using the currently loaded stub
			TCPSSLHandleIncoming(hCurrentTCP);
		}
		#endif
	}
//	else
//	{
//		// NOTE: RFC 793 specifies that if the socket is closed and a segment 
//		// arrives, we should send back a RST if the RST bit in the incoming 
//		// packet is not set.  Instead, we will just silently ignore such a 
//		// packet since this is what firewalls do on purpose to enhance 
//		// security.
//		//if(!TCPHeader.Flags.bits.flagRST)
//		//	SendTCP(RST, SENDTCP_RESET_TIMERS);
//	}

	// Finished with this packet, discard it and free the Ethernet RAM for new packets
	MACDiscardRx();

	return TRUE;
}


/*****************************************************************************
  Function:
	static void SendTCP(BYTE vTCPFlags, BYTE vSendFlags)

  Summary:
	Transmits a TPC segment.

  Description:
	This function assembles and transmits a TCP segment, including any 
	pending data.  It also supports retransmissions, keep-alives, and 
	other packet types.

  Precondition:
	TCP is initialized.

  Parameters:
	vTCPFlags - Additional TCP flags to include
	vSendFlags - Any combinations of SENDTCP_* constants to modify the
				 transmit behavior or contents.

  Returns:
	None
  ***************************************************************************/
static void SendTCP(BYTE vTCPFlags, BYTE vSendFlags)
{
	WORD_VAL        wVal;
	TCP_HEADER      header;
	TCP_OPTIONS     options;
	PSEUDO_HEADER   pseudoHeader;
	WORD 			len;
	
	SyncTCB();

	// FINs must be handled specially
	if(vTCPFlags & FIN)
	{
		MyTCBStub.Flags.bTXFIN = 1;
		vTCPFlags &= ~FIN;
	}

	// Status will now be synched, disable automatic future 
	// status transmissions
	MyTCBStub.Flags.bTimer2Enabled = 0;
	MyTCBStub.Flags.bDelayedACKTimerEnabled = 0;
	MyTCBStub.Flags.bOneSegmentReceived = 0;
	MyTCBStub.Flags.bTXASAP = 0;
	MyTCBStub.Flags.bTXASAPWithoutTimerReset = 0;
	MyTCBStub.Flags.bHalfFullFlush = 0;

	//  Make sure that we can write to the MAC transmit area
	while(!IPIsTxReady());

	// Put all socket application data in the TX space
	if(vTCPFlags & (SYN | RST))
	{
		// Don't put any data in SYN and RST messages
		len = 0;
	}
	else
	{
		// Begin copying any application data over to the TX space
		if(MyTCBStub.txHead == MyTCB.txUnackedTail)
		{
			// All caught up on data TX, no real data for this packet
			len = 0;
		}
		else if(MyTCBStub.txHead > MyTCB.txUnackedTail)
		{
			len = MyTCBStub.txHead - MyTCB.txUnackedTail;

			if(len > MyTCB.remoteWindow)
				len = MyTCB.remoteWindow;

			if(len > MyTCB.wRemoteMSS)
			{
				len = MyTCB.wRemoteMSS;
				MyTCBStub.Flags.bTXASAPWithoutTimerReset = 1;
			}

			// Copy application data into the raw TX buffer
			TCPRAMCopy(BASE_TX_ADDR+sizeof(ETHER_HEADER)+sizeof(IP_HEADER)+sizeof(TCP_HEADER), TCP_ETH_RAM, MyTCB.txUnackedTail, MyTCBStub.vMemoryMedium, len);
			MyTCB.txUnackedTail += len;
		}
		else
		{
			pseudoHeader.Length = MyTCBStub.bufferRxStart - MyTCB.txUnackedTail;
			len = pseudoHeader.Length + MyTCBStub.txHead - MyTCBStub.bufferTxStart;

			if(len > MyTCB.remoteWindow)
				len = MyTCB.remoteWindow;

			if(len > MyTCB.wRemoteMSS)
			{
				len = MyTCB.wRemoteMSS;
				MyTCBStub.Flags.bTXASAPWithoutTimerReset = 1;
			}

			if(pseudoHeader.Length > len)
				pseudoHeader.Length = len;

			// Copy application data into the raw TX buffer
			TCPRAMCopy(BASE_TX_ADDR+sizeof(ETHER_HEADER)+sizeof(IP_HEADER)+sizeof(TCP_HEADER), TCP_ETH_RAM, MyTCB.txUnackedTail, MyTCBStub.vMemoryMedium, pseudoHeader.Length);
			pseudoHeader.Length = len - pseudoHeader.Length;
	
			// Copy any left over chunks of application data over
			if(pseudoHeader.Length)
			{
				TCPRAMCopy(BASE_TX_ADDR+sizeof(ETHER_HEADER)+sizeof(IP_HEADER)+sizeof(TCP_HEADER)+(MyTCBStub.bufferRxStart-MyTCB.txUnackedTail), TCP_ETH_RAM, MyTCBStub.bufferTxStart, MyTCBStub.vMemoryMedium, pseudoHeader.Length);
			}

			MyTCB.txUnackedTail += len;
			if(MyTCB.txUnackedTail >= MyTCBStub.bufferRxStart)
				MyTCB.txUnackedTail -= MyTCBStub.bufferRxStart-MyTCBStub.bufferTxStart;
		}

		// If we are to transmit a FIN, make sure we can put one in this packet
		if(MyTCBStub.Flags.bTXFIN)
		{
			if((len != MyTCB.remoteWindow) && (len != MyTCB.wRemoteMSS))
				vTCPFlags |= FIN;
		}
	}

	// Ensure that all packets with data of some kind are 
	// retransmitted by TCPTick() until acknowledged
	// Pure ACK packets with no data are not ACKed back in TCP
	if(len || (vTCPFlags & (SYN | FIN)))
	{
		// Transmitting data, update remote window variable to reflect smaller 
		// window.
		MyTCB.remoteWindow -= len;

		// Push (PSH) all data for enhanced responsiveness on 
		// the remote end, especially with GUIs
		if(len)
			vTCPFlags |= PSH;

		if(vSendFlags & SENDTCP_RESET_TIMERS)
		{
			MyTCB.retryCount = 0;
			MyTCB.retryInterval = TCP_START_TIMEOUT_VAL;
		}	

		MyTCBStub.eventTime = TickGet() + MyTCB.retryInterval;
		MyTCBStub.Flags.bTimerEnabled = 1;
	}
	else if(vSendFlags & SENDTCP_KEEP_ALIVE)
	{
		// Increment Keep Alive TX counter to handle disconnection if not response is returned
		MyTCBStub.Flags.vUnackedKeepalives++;
		
		// Generate a dummy byte
		MyTCB.MySEQ -= 1;
		len = 1;
	}
	else if(MyTCBStub.Flags.bTimerEnabled) 
	{
		// If we have data to transmit, but the remote RX window is zero, 
		// so we aren't transmitting any right now then make sure to not 
		// extend the retry counter or timer.  This will stall our TX 
		// with a periodic ACK sent to the remote node.
		if(!(vSendFlags & SENDTCP_RESET_TIMERS))
		{
			// Roll back retry counters since we can't send anything, 
			// but only if we incremented it in the first place
			if(MyTCB.retryCount)
			{
				MyTCB.retryCount--;
				MyTCB.retryInterval >>= 1;
			}
		}
	
		MyTCBStub.eventTime = TickGet() + MyTCB.retryInterval;
	}
	

	header.SourcePort			= MyTCB.localPort.Val;
	header.DestPort				= MyTCB.remotePort.Val;
	header.SeqNumber			= MyTCB.MySEQ;
	header.AckNumber			= MyTCB.RemoteSEQ;
	header.Flags.bits.Reserved2	= 0;
	header.DataOffset.Reserved3	= 0;
	header.Flags.byte			= vTCPFlags;
	header.UrgentPointer        = 0;

	// Update our send sequence number and ensure retransmissions 
	// of SYNs and FINs use the right sequence number
	MyTCB.MySEQ += (DWORD)len;
	if(vTCPFlags & SYN)
	{
		// SEG.ACK needs to be zero for the first SYN packet for compatibility 
		// with certain paranoid TCP/IP stacks, even though the ACK flag isn't 
		// set (indicating that the AckNumber field is unused).
		if(!(vTCPFlags & ACK))
			header.AckNumber = 0x00000000;

		if(MyTCB.flags.bSYNSent)
			header.SeqNumber--;
		else
		{
			MyTCB.MySEQ++;
			MyTCB.flags.bSYNSent = 1;
		}
	}
	if(vTCPFlags & FIN)
	{
        MyTCB.flags.bFINSent = 1;   // do not advance the seq no for FIN!
	}

	// Calculate the amount of free space in the RX buffer area of this socket
	if(MyTCBStub.rxHead >= MyTCBStub.rxTail)
		header.Window = (MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart) - (MyTCBStub.rxHead - MyTCBStub.rxTail);
	else
		header.Window = MyTCBStub.rxTail - MyTCBStub.rxHead - 1;

	// Calculate the amount of free space in the MAC RX buffer area and adjust window if needed
	wVal.Val = MACGetFreeRxSize();
	if(wVal.Val < 64)
    {
		wVal.Val = 0;
    }
    else
    {
		wVal.Val -= 64;
    }
	// Force the remote node to throttle back if we are running low on general RX buffer space
	if(header.Window > wVal.Val)
		header.Window = wVal.Val;

	SwapTCPHeader(&header);


	len += sizeof(header);
	header.DataOffset.Val   = sizeof(header) >> 2;

	// Insert the MSS (Maximum Segment Size) TCP option if this is SYN packet
	if(vTCPFlags & SYN)
	{
		len += sizeof(options);
		options.Kind = TCP_OPTIONS_MAX_SEG_SIZE;
		options.Length = 0x04;

		// Load MSS and swap to big endian
		options.MaxSegSize.Val = (((TCP_MAX_SEG_SIZE_RX)&0x00FF)<<8) | (((TCP_MAX_SEG_SIZE_RX)&0xFF00)>>8);

		header.DataOffset.Val   += sizeof(options) >> 2;
	}

	// Calculate IP pseudoheader checksum.
	pseudoHeader.SourceAddress	= AppConfig.MyIPAddr;
	pseudoHeader.DestAddress    = MyTCB.remote.niRemoteMACIP.IPAddr;
	pseudoHeader.Zero           = 0x0;
	pseudoHeader.Protocol       = IP_PROT_TCP;
	pseudoHeader.Length			= len;
	SwapPseudoHeader(pseudoHeader);
	header.Checksum = ~CalcIPChecksum((BYTE*)&pseudoHeader, sizeof(pseudoHeader));

	// Write IP header
	MACSetWritePtr(BASE_TX_ADDR + sizeof(ETHER_HEADER));
	IPPutHeader(&MyTCB.remote.niRemoteMACIP, IP_PROT_TCP, len);
	MACPutArray((BYTE*)&header, sizeof(header));
	if(vTCPFlags & SYN)
		MACPutArray((BYTE*)&options, sizeof(options));

	// Update the TCP checksum
	MACSetReadPtr(BASE_TX_ADDR + sizeof(ETHER_HEADER) + sizeof(IP_HEADER));
	wVal.Val = CalcIPBufferChecksum(len);
#if defined(DEBUG_GENERATE_TX_LOSS)
	// Damage TCP checksums on TX packets randomly
	if(LFSRRand() > DEBUG_GENERATE_TX_LOSS)
	{
		wVal.Val++;
	}
#endif
	MACSetWritePtr(BASE_TX_ADDR + sizeof(ETHER_HEADER) + sizeof(IP_HEADER) + 16);
	MACPutArray((BYTE*)&wVal, sizeof(WORD));

	// Physically start the packet transmission over the network
	MACFlush();
}

/*****************************************************************************
  Function:
	static BOOL FindMatchingSocket(TCP_HEADER* h, NODE_INFO* remote)

  Summary:
	Finds a suitable socket for a TCP segment.

  Description:
	This function searches through the sockets and attempts to match one with
	a given TCP header and NODE_INFO structure.  If a socket is found, its 
	index is saved in hCurrentTCP and the associated MyTCBStub and MyTCB are
	loaded. Otherwise, INVALID_SOCKET is placed in hCurrentTCP.
	
  Precondition:
	TCP is initialized.

  Parameters:
	h - TCP header to be matched against
	remote - The remote node who sent this header

  Return Values:
	TRUE - A match was found and is loaded in hCurrentTCP
	FALSE - No suitable socket was found and hCurrentTCP is INVALID_SOCKET
  ***************************************************************************/
static BOOL FindMatchingSocket(TCP_HEADER* h, NODE_INFO* remote)
{
	TCP_SOCKET hTCP;
	TCP_SOCKET partialMatch;
	WORD hash;

	// Prevent connections on invalid port 0
	if(h->DestPort == 0u)
		return FALSE;

	partialMatch = INVALID_SOCKET;
	hash = (remote->IPAddr.w[1]+remote->IPAddr.w[0] + h->SourcePort) ^ h->DestPort;

	// Loop through all sockets looking for a socket that is expecting this 
	// packet or can handle it.
	for(hTCP = 0; hTCP < TCP_SOCKET_COUNT; hTCP++ )
	{
		SyncTCBStub(hTCP);

		if(MyTCBStub.smState == TCP_CLOSED)
		{
			continue;
		}
		else if(MyTCBStub.smState == TCP_LISTEN)
		{// For listening ports, check if this is the correct port
			if(MyTCBStub.remoteHash.Val == h->DestPort)
				partialMatch = hTCP;
			
			#if defined(STACK_USE_SSL_SERVER)
			// Check the SSL port as well for SSL Servers
			// 0 is defined as an invalid port number
			if(MyTCBStub.sslTxHead == h->DestPort)
				partialMatch = hTCP;
			#endif
			
			continue;
		}
		else if(MyTCBStub.remoteHash.Val != hash)
		{// Ignore if the hash doesn't match
			continue;
		}

		SyncTCB();
		if(	h->DestPort == MyTCB.localPort.Val &&
			h->SourcePort == MyTCB.remotePort.Val &&
			remote->IPAddr.Val == MyTCB.remote.niRemoteMACIP.IPAddr.Val)
		{
			return TRUE;
		}
	}


	// If there is a partial match, then a listening socket is currently 
	// available.  Set up the extended TCB with the info needed 
	// to establish a connection and return this socket to the 
	// caller.
	if(partialMatch != INVALID_SOCKET)
	{
		SyncTCBStub(partialMatch);
		SyncTCB();
	
		// For SSL ports, begin the SSL Handshake
		#if defined(STACK_USE_SSL_SERVER)
		if(MyTCBStub.sslTxHead == h->DestPort)
		{
			// Try to start an SSL session.  If no stubs are available,
			// we can't service this request right now, so ignore it.
			if(!TCPStartSSLServer(partialMatch))
				partialMatch = INVALID_SOCKET;
		}
		#endif
	
		// Make sure the above check didn't fail (this is unfortunately 
		// redundant for non-SSL sockets).  Otherwise, fall out to below
		// and add to the SYN queue.
		if(partialMatch != INVALID_SOCKET)
		{
			MyTCBStub.remoteHash.Val = hash;
		
			memcpy((void*)&MyTCB.remote, (void*)remote, sizeof(NODE_INFO));
			MyTCB.remotePort.Val = h->SourcePort;
			MyTCB.localPort.Val = h->DestPort;
			MyTCB.txUnackedTail	= MyTCBStub.bufferTxStart;
		
			// All done, and we have a match
			return TRUE;
		}
	}

	// No available sockets are listening on this port.  (Or, for
	// SSL requests, perhaps no SSL sessions were available.  However,
	// there may be a server socket which is currently busy but 
	// could handle this packet, so we should check.
	#if TCP_SYN_QUEUE_MAX_ENTRIES
	{
		WORD wQueueInsertPos;
		
		// See if this is a SYN packet
		if(!h->Flags.bits.flagSYN)
			return FALSE;

		// See if there is space in our SYN queue
		if(SYNQueue[TCP_SYN_QUEUE_MAX_ENTRIES-1].wDestPort)
			return FALSE;
		
		// See if we have this SYN already in our SYN queue.
		// If not already in the queue, find out where we 
		// should insert this SYN to the queue
		for(wQueueInsertPos = 0; wQueueInsertPos < TCP_SYN_QUEUE_MAX_ENTRIES; wQueueInsertPos++)
		{
			// Exit loop if we found a free record
			if(SYNQueue[wQueueInsertPos].wDestPort == 0u)
				break;

			// Check if this SYN packet is already in the SYN queue
			if(SYNQueue[wQueueInsertPos].wDestPort != h->DestPort)
				continue;
			if(SYNQueue[wQueueInsertPos].wSourcePort != h->SourcePort)
				continue;
			if(SYNQueue[wQueueInsertPos].niSourceAddress.IPAddr.Val != remote->IPAddr.Val)
				continue;

			// SYN matches SYN queue entry.  Update timestamp and do nothing.
			SYNQueue[wQueueInsertPos].wTimestamp = TickGetDiv256();
			return FALSE;
		}
		
		// Check to see if we have any server sockets which 
		// are currently connected, but could handle this SYN 
		// request at a later time if the client disconnects.
		for(hTCP = 0; hTCP < TCP_SOCKET_COUNT; hTCP++)
		{
			SyncTCBStub(hTCP);
			if(!MyTCBStub.Flags.bServer)
				continue;

			SyncTCB();
			#if defined(STACK_USE_SSL_SERVER)
			if((MyTCB.localPort.Val != h->DestPort) && (MyTCB.localSSLPort.Val != h->DestPort))
			#else
			if(MyTCB.localPort.Val != h->DestPort)
			#endif
				continue;

			// Generate the SYN queue entry
			memcpy((void*)&SYNQueue[wQueueInsertPos].niSourceAddress, (void*)remote, sizeof(NODE_INFO));
			SYNQueue[wQueueInsertPos].wSourcePort = h->SourcePort;
			SYNQueue[wQueueInsertPos].dwSourceSEQ = h->SeqNumber;
			SYNQueue[wQueueInsertPos].wDestPort = h->DestPort;
			SYNQueue[wQueueInsertPos].wTimestamp = TickGetDiv256();

			return FALSE;
		}
	}
	#endif
		
	return FALSE;

}



/*****************************************************************************
  Function:
	static void SwapTCPHeader(TCP_HEADER* header)

  Summary:
	Swaps endian-ness of a TCP header.

  Description:
	This function swaps the endian-ness of a given TCP header for comparison.

  Precondition:
	None

  Parameters:
	header - The TCP header that is to be swapped

  Returns:
	None
  ***************************************************************************/
static void SwapTCPHeader(TCP_HEADER* header)
{
	header->SourcePort      = swaps(header->SourcePort);
	header->DestPort        = swaps(header->DestPort);
	header->SeqNumber       = swapl(header->SeqNumber);
	header->AckNumber       = swapl(header->AckNumber);
	header->Window          = swaps(header->Window);
	header->Checksum        = swaps(header->Checksum);
	header->UrgentPointer   = swaps(header->UrgentPointer);
}



/*****************************************************************************
  Function:
	static void CloseSocket(void)

  Summary:
	Closes a TCP socket.

  Description:
	This function closes a TCP socket.  All socket state information is 
	reset, and any buffered bytes are discarded.  The socket is no longer
	accessible by the application after this point.

  Precondition:
	The TCPStub corresponding to the socket to be closed is synced.

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
static void CloseSocket(void)
{
	SyncTCB();

	MyTCBStub.remoteHash.Val = MyTCB.localPort.Val;
	MyTCBStub.txHead = MyTCBStub.bufferTxStart;
	MyTCBStub.txTail = MyTCBStub.bufferTxStart;
	MyTCBStub.rxHead = MyTCBStub.bufferRxStart;
	MyTCBStub.rxTail = MyTCBStub.bufferRxStart;
	MyTCBStub.smState = MyTCBStub.Flags.bServer ? TCP_LISTEN : TCP_CLOSED;
	MyTCBStub.Flags.vUnackedKeepalives = 0;
	MyTCBStub.Flags.bTimerEnabled = 0;
	MyTCBStub.Flags.bTimer2Enabled = 0;
	MyTCBStub.Flags.bDelayedACKTimerEnabled = 0;
	MyTCBStub.Flags.bOneSegmentReceived = 0;
	MyTCBStub.Flags.bHalfFullFlush = 0;
	MyTCBStub.Flags.bTXASAP = 0;
	MyTCBStub.Flags.bTXASAPWithoutTimerReset = 0;
	MyTCBStub.Flags.bTXFIN = 0;
	MyTCBStub.Flags.bSocketReset = 1;

	#if defined(STACK_USE_SSL)
	// If SSL is active, then we need to close it
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
	{
		SSLTerminate(MyTCBStub.sslStubID);
		MyTCBStub.sslStubID = SSL_INVALID_ID;

		// Swap the SSL port and local port back to proper values
		MyTCBStub.remoteHash.Val = MyTCB.localSSLPort.Val;
		MyTCB.localSSLPort.Val = MyTCB.localPort.Val;
		MyTCB.localPort.Val = MyTCBStub.remoteHash.Val;
	}

	// Reset the SSL buffer pointers
	MyTCBStub.sslRxHead = MyTCBStub.bufferRxStart;
	MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
	#endif
	
	#if defined(STACK_USE_SSL_SERVER)
	MyTCBStub.sslTxHead = MyTCB.localSSLPort.Val;
	#endif

	MyTCB.flags.bFINSent = 0;
	MyTCB.flags.bSYNSent = 0;
	MyTCB.flags.bRXNoneACKed1 = 0;
	MyTCB.flags.bRXNoneACKed2 = 0;
	MyTCB.txUnackedTail = MyTCBStub.bufferTxStart;
	((DWORD_VAL*)(&MyTCB.MySEQ))->w[0] = LFSRRand();
	((DWORD_VAL*)(&MyTCB.MySEQ))->w[1] = LFSRRand();
	MyTCB.sHoleSize = -1;
	MyTCB.remoteWindow = 1;
}


/*****************************************************************************
  Function:
	static WORD GetMaxSegSizeOption(void)

  Summary:
	Obtains the Maximum Segment Size (MSS) TCP Option out of the TCP header 
	for the current socket.

  Description:
	Parses the current TCP packet header and extracts the Maximum Segment Size 
	option.  

  Precondition:
	Must be called while a TCP packet is present and being processed via 
	HandleTCPSeg() and only if the the TCP SYN flag is set.

  Parameters:
	None

  Returns:
	Maximum segment size option value.  If illegal or not present, a failsafe 
	value of 536 is returned.  If the option is larger than the 
	TCP_MAX_SEG_SIZE_TX upper limit, then TCP_MAX_SEG_SIZE_TX is returned.

  Remarks:
	The internal MAC Read Pointer is moved but not restored.
  ***************************************************************************/
static WORD GetMaxSegSizeOption(void)
{
	BYTE vOptionsBytes;
	BYTE vOption;
	WORD wMSS;

	// Find out how many options bytes are in this packet.
	IPSetRxBuffer(2+2+4+4);	// Seek to data offset field, skipping Source port (2), Destination port (2), Sequence number (4), and Acknowledgement number (4)
	vOptionsBytes = MACGet();
	vOptionsBytes = ((vOptionsBytes&0xF0)>>2) - sizeof(TCP_HEADER);

	// Return minimum Maximum Segment Size value of 536 bytes if none are 
	// present
	if(vOptionsBytes == 0u)
		return 536;
		
	// Seek to beginning of options
	MACGetArray(NULL, 7);

	// Search for the Maximum Segment Size option	
	while(vOptionsBytes--)
	{
		vOption = MACGet();
		
		if(vOption == 0u)	// End of Options list
			break;
		
		if(vOption == 1u)	// NOP option
			continue;
			
		if(vOption == 2u)	// Maximum Segment Size option
		{
			if(vOptionsBytes < 3u)
				break;

			wMSS = 0;
				
			// Get option length
			vOption = MACGet();
			if(vOption == 4u)
			{// Retrieve MSS and swap value to little endian
				((BYTE*)&wMSS)[1] = MACGet();
				((BYTE*)&wMSS)[0] = MACGet();
			}
			
			if(wMSS < 536u)
				break;
			if(wMSS > TCP_MAX_SEG_SIZE_TX)
				return TCP_MAX_SEG_SIZE_TX;
			else 
				return wMSS;
		}
		else
		{ // Assume this is a multi byte option and throw it way
			if(vOptionsBytes < 2u)
				break;
			vOption = MACGet();
			if(vOptionsBytes < vOption)
				break;
			MACGetArray(NULL, vOption);
			vOptionsBytes -= vOption;
		}
		
	}
	
	// Did not find MSS option, return worst case default
	return 536;
}

/*****************************************************************************
  Function:
	static void HandleTCPSeg(TCP_HEADER* h, WORD len)

  Summary:
	Processes an incoming TCP segment.

  Description:
	Once an incoming segment has been matched to a socket, this function
	performs the necessary processing with the data.  Depending on the 
	segment and the state, this may include copying data to the TCP buffer,
	re-assembling out-of order packets, continuing an initialization or 
	closing handshake, or closing the socket altogether.

  Precondition:
	TCP is initialized and the current TCP stub is already synced.

  Parameters:
	h - The TCP header for this packet
	len - The total buffer length of this segment

  Returns:
	None
  ***************************************************************************/
static void HandleTCPSeg(TCP_HEADER* h, WORD len)
{
	DWORD dwTemp;
	PTR_BASE wTemp;
	LONG lMissingBytes;
	WORD wMissingBytes;
	WORD wFreeSpace;
	BYTE localHeaderFlags;
	DWORD localAckNumber;
	DWORD localSeqNumber;
	WORD wSegmentLength;
	BOOL bSegmentAcceptable;
	WORD wNewWindow;


	// Cache a few variables in local RAM.  
	// PIC18s take a fair amount of code and execution time to 
	// dereference pointers frequently.
	localHeaderFlags = h->Flags.byte;
	localAckNumber = h->AckNumber;
	localSeqNumber = h->SeqNumber;

	// We received a packet, reset the keep alive timer and count
	#if defined(TCP_KEEP_ALIVE_TIMEOUT)
		MyTCBStub.Flags.vUnackedKeepalives = 0;
		if(!MyTCBStub.Flags.bTimerEnabled)
			MyTCBStub.eventTime = TickGet() + TCP_KEEP_ALIVE_TIMEOUT;
	#endif

	// Handle TCP_LISTEN and TCP_SYN_SENT states
	// Both of these states will return, so code following this 
	// state machine need not check explicitly for these two 
	// states.
	switch(MyTCBStub.smState)
	{
		case TCP_LISTEN:
			// First: check RST flag
			if(localHeaderFlags & RST)
			{
				CloseSocket();	// Unbind remote IP address/port info
				return;
			}

			// Second: check ACK flag, which would be invalid
			if(localHeaderFlags & ACK)
			{
				// Use a believable sequence number and reset the remote node
				MyTCB.MySEQ = localAckNumber;
				SendTCP(RST, 0);
				CloseSocket();	// Unbind remote IP address/port info
				return;
			}

			// Third: check for SYN flag, which is what we're looking for
			if(localHeaderFlags & SYN)
			{
				// We now have a sequence number for the remote node
				MyTCB.RemoteSEQ = localSeqNumber + 1;

				// Get MSS option
				MyTCB.wRemoteMSS = GetMaxSegSizeOption();

				// Set Initial Send Sequence (ISS) number
				// Nothing to do on this step... ISS already set in CloseSocket()
				
				// Respond with SYN + ACK
				SendTCP(SYN | ACK, SENDTCP_RESET_TIMERS);
				MyTCBStub.smState = TCP_SYN_RECEIVED;
			}
			else
			{
				CloseSocket();	// Unbind remote IP address/port info
			}

			// Fourth: check for other text and control
			// Nothing to do since we don't support this
			return;

		case TCP_SYN_SENT:
			// Second: check the RST bit
			// This is out of order because this stack has no API for 
			// notifying the application that the connection seems to 
			// be failing.  Instead, the application must time out and 
			// the stack will just keep trying in the mean time.
			if(localHeaderFlags & RST)
				return;

			// First: check ACK bit
			if(localHeaderFlags & ACK)
			{
				if(localAckNumber != MyTCB.MySEQ)
				{
					// Send a RST packet with SEQ = SEG.ACK, but retain our SEQ 
					// number for arivial of any other SYN+ACK packets
					localSeqNumber = MyTCB.MySEQ;	// Save our original SEQ number
					MyTCB.MySEQ = localAckNumber;	// Set SEQ = SEG.ACK
					SendTCP(RST, SENDTCP_RESET_TIMERS);		// Send the RST
					MyTCB.MySEQ = localSeqNumber;	// Restore original SEQ number
					return;
				}
			}

			// Third: check the security and precedence
			// No such feature in this stack.  We want to accept all connections.

			// Fourth: check the SYN bit
			if(localHeaderFlags & SYN)
			{
				// We now have an initial sequence number and window size
				MyTCB.RemoteSEQ = localSeqNumber + 1;
				MyTCB.remoteWindow = h->Window;

				// Get MSS option
				MyTCB.wRemoteMSS = GetMaxSegSizeOption();

				if(localHeaderFlags & ACK)
				{
					SendTCP(ACK, SENDTCP_RESET_TIMERS);
					MyTCBStub.smState = TCP_ESTABLISHED;
					// Set up keep-alive timer
					#if defined(TCP_KEEP_ALIVE_TIMEOUT)
						MyTCBStub.eventTime = TickGet() + TCP_KEEP_ALIVE_TIMEOUT;
					#endif
					MyTCBStub.Flags.bTimerEnabled = 0;
				}
				else
				{
					SendTCP(SYN | ACK, SENDTCP_RESET_TIMERS);
					MyTCBStub.smState = TCP_SYN_RECEIVED;
				}
			}

			// Fifth: drop the segment if neither SYN or RST is set
			return;

		default:
			break;
	}

	//
	// First: check the sequence number
	//
	wSegmentLength = len;
	if(localHeaderFlags & FIN)
		wSegmentLength++;
	if(localHeaderFlags & SYN)
		wSegmentLength++;

	// Calculate the RX FIFO space
	if(MyTCBStub.rxHead >= MyTCBStub.rxTail)
		wFreeSpace = (MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart) - (MyTCBStub.rxHead - MyTCBStub.rxTail);
	else
		wFreeSpace = MyTCBStub.rxTail - MyTCBStub.rxHead - 1;

	// Calculate the number of bytes ahead of our head pointer this segment skips
	lMissingBytes = localSeqNumber - MyTCB.RemoteSEQ;
	wMissingBytes = (WORD)lMissingBytes;
	
	// Run TCP acceptability tests to verify that this packet has a valid sequence number
	bSegmentAcceptable = FALSE;
	if(wSegmentLength)
	{
		// Check to see if we have free space, and if so, if any of the data falls within the freespace
		if(wFreeSpace)
		{
			// RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
			if((lMissingBytes >= (LONG)0) && (wFreeSpace > (DWORD)lMissingBytes))
				bSegmentAcceptable = TRUE;
			else
			{
				// RCV.NXT =< SEG.SEQ+SEG.LEN-1 < RCV.NXT+RCV.WND
				if((lMissingBytes + (LONG)wSegmentLength > (LONG)0) && (lMissingBytes <= (LONG)(SHORT)(wFreeSpace - wSegmentLength)))
					bSegmentAcceptable = TRUE;
			}
			
			if((lMissingBytes < (LONG)wFreeSpace) && ((SHORT)wMissingBytes + (SHORT)wSegmentLength > (SHORT)0))
				bSegmentAcceptable = TRUE;
		}
		// Segments with data are not acceptable if we have no free space
	}
	else
	{
		// Zero length packets are acceptable if they fall within our free space window
		// SEG.SEQ = RCV.NXT
		if(lMissingBytes == 0)
		{
			bSegmentAcceptable = TRUE;
		}
		else
		{
			// RCV.NXT =< SEG.SEQ < RCV.NXT+RCV.WND
			if((lMissingBytes >= (LONG)0) && (wFreeSpace > (DWORD)lMissingBytes))
				bSegmentAcceptable = TRUE;
		}
	}
	
	if(!bSegmentAcceptable)
	{
		// Unacceptable segment, drop it and respond appropriately
		if(!(localHeaderFlags & RST)) 
			SendTCP(ACK, SENDTCP_RESET_TIMERS);
		return;
	}


	//
	// Second: check the RST bit
	//
	//
	// Fourth: check the SYN bit
	//
	// Note, that since the third step is not implemented, we can 
	// combine this second and fourth step into a single operation.
	if(localHeaderFlags & (RST | SYN))
	{
		CloseSocket();
		return;
	}

	//
	// Third: check the security and precedence
	//
	// Feature not supported.  Let's process this segment.

	//
	// Fifth: check the ACK bit
	//
	if(!(localHeaderFlags & ACK))
		return;

	switch(MyTCBStub.smState)
	{
		case TCP_SYN_RECEIVED:
			if(localAckNumber != MyTCB.MySEQ)
			{
				// Send a RST packet with SEQ = SEG.ACK, but retain our SEQ 
				// number for arivial of any other correct packets
				localSeqNumber = MyTCB.MySEQ;	// Save our original SEQ number
				MyTCB.MySEQ = localAckNumber;	// Set SEQ = SEG.ACK
				SendTCP(RST, SENDTCP_RESET_TIMERS);		// Send the RST
				MyTCB.MySEQ = localSeqNumber;	// Restore original SEQ number
				return;
			}
			MyTCBStub.smState = TCP_ESTABLISHED;
			// No break

		case TCP_ESTABLISHED:
		case TCP_FIN_WAIT_1:
		case TCP_FIN_WAIT_2:
		case TCP_CLOSE_WAIT:
		case TCP_CLOSING:
			// Calculate what the highest possible SEQ number in our TX FIFO is
			wTemp = MyTCBStub.txHead - MyTCB.txUnackedTail;
			if((SHORT)wTemp < (SHORT)0)
				wTemp += MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart;
			dwTemp = MyTCB.MySEQ + (DWORD)wTemp;

			// Drop the packet if it ACKs something we haven't sent
            dwTemp = (LONG)localAckNumber - (LONG)dwTemp;
            if((LONG)dwTemp > 0)
            {   // acknowledged more than we've sent??
                if(!MyTCB.flags.bFINSent || dwTemp != 1)
                {
                    SendTCP(ACK, 0);
                    return;
                }
                else
                {
                    localAckNumber--;   // since we don't count the FIN anyway
                }
            }

			// Throw away all ACKnowledged TX data:
			// Calculate what the last acknowledged sequence number was (ignoring any FINs we sent)
			dwTemp = MyTCB.MySEQ - (LONG)(SHORT)(MyTCB.txUnackedTail - MyTCBStub.txTail);
			if(MyTCB.txUnackedTail < MyTCBStub.txTail)
				dwTemp -= MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart;
	
			// Calcluate how many bytes were ACKed with this packet
			dwTemp = localAckNumber - dwTemp;
			if(((LONG)(dwTemp) > (LONG)0) && (dwTemp <= MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart))
			{
				MyTCB.flags.bRXNoneACKed1 = 0;
				MyTCB.flags.bRXNoneACKed2 = 0;
				MyTCBStub.Flags.bHalfFullFlush = FALSE;
	
				// Bytes ACKed, free up the TX FIFO space
				wTemp = MyTCBStub.txTail;
				MyTCBStub.txTail += dwTemp;
				if(MyTCB.txUnackedTail >= wTemp)
				{
					if(MyTCB.txUnackedTail < MyTCBStub.txTail)
					{
						MyTCB.MySEQ += MyTCBStub.txTail - MyTCB.txUnackedTail;
						MyTCB.txUnackedTail = MyTCBStub.txTail;
					}
				}
				else
				{
					wTemp = MyTCB.txUnackedTail + (MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart);
					if(wTemp < MyTCBStub.txTail)
					{
						MyTCB.MySEQ += MyTCBStub.txTail - wTemp;
						MyTCB.txUnackedTail = MyTCBStub.txTail;
					}
				}
				if(MyTCBStub.txTail >= MyTCBStub.bufferRxStart)
					MyTCBStub.txTail -= MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart;
				if(MyTCB.txUnackedTail >= MyTCBStub.bufferRxStart)
					MyTCB.txUnackedTail -= MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart;
			}
			else
			{
				// See if we have outstanding TX data that is waiting for an ACK
				if(MyTCBStub.txTail != MyTCB.txUnackedTail)
				{
					if(MyTCB.flags.bRXNoneACKed1)
					{
						if(MyTCB.flags.bRXNoneACKed2)
						{
							// Set up to perform a fast retransmission
							// Roll back unacknowledged TX tail pointer to cause retransmit to occur
							MyTCB.MySEQ -= (LONG)(SHORT)(MyTCB.txUnackedTail - MyTCBStub.txTail);
							if(MyTCB.txUnackedTail < MyTCBStub.txTail)
								MyTCB.MySEQ -= (LONG)(SHORT)(MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart);
							MyTCB.txUnackedTail = MyTCBStub.txTail;
							MyTCBStub.Flags.bTXASAPWithoutTimerReset = 1;
						}
						MyTCB.flags.bRXNoneACKed2 = 1;
					}
					MyTCB.flags.bRXNoneACKed1 = 1;
				}
			}

			// No need to keep our retransmit timer going if we have nothing that needs ACKing anymore
			if(MyTCBStub.txTail == MyTCBStub.txHead)
			{
				// Make sure there isn't a "FIN byte in our TX FIFO"
				if(MyTCBStub.Flags.bTXFIN == 0u)
				{
					// Convert retransmission timer to keep-alive timer
					#if defined(TCP_KEEP_ALIVE_TIMEOUT)
						MyTCBStub.eventTime = TickGet() + TCP_KEEP_ALIVE_TIMEOUT;
					#endif
					MyTCBStub.Flags.bTimerEnabled = 0;
				}
				else
				{
					// "Throw away" FIN byte from our TX FIFO if it has been ACKed
					if((MyTCB.MySEQ == localAckNumber) && MyTCB.flags.bFINSent)
					{
						MyTCBStub.Flags.bTimerEnabled = 0;
						MyTCBStub.Flags.bTXFIN = 0;
					}
				}
			}

			// The window size advirtised in this packet is adjusted to account 
			// for any bytes that we have transmitted but haven't been ACKed yet 
			// by this segment.
			wNewWindow = h->Window - ((WORD)(MyTCB.MySEQ - localAckNumber));

			// Update the local stored copy of the RemoteWindow.
			// If previously we had a zero window, and now we don't, then 
			// immediately send whatever was pending.
			if((MyTCB.remoteWindow == 0u) && wNewWindow)
				MyTCBStub.Flags.bTXASAP = 1;
			MyTCB.remoteWindow = wNewWindow;

			// A couple of states must do all of the TCP_ESTABLISHED stuff, but also a little more
			if(MyTCBStub.smState == TCP_FIN_WAIT_1)
			{
				// Check to see if our FIN has been ACKnowledged
				if((MyTCB.MySEQ == localAckNumber) && MyTCB.flags.bFINSent)
				{
					// Reset our timer for forced closure if the remote node 
					// doesn't send us a FIN in a timely manner.
					MyTCBStub.eventTime = TickGet() + TCP_FIN_WAIT_2_TIMEOUT;
					MyTCBStub.Flags.bTimerEnabled = 1;
					MyTCBStub.smState = TCP_FIN_WAIT_2;
				}
			}
			else if(MyTCBStub.smState == TCP_FIN_WAIT_2)
			{
				// RFC noncompliance:
				// The remote node should not keep sending us data 
				// indefinitely after we send a FIN to it.  
				// However, some bad stacks may still keep sending 
				// us data indefinitely after ACKing our FIN.  To 
				// prevent this from locking up our socket, let's 
				// send a RST right now and close forcefully on 
				// our side.
				if(!(localHeaderFlags & FIN))
				{
					MyTCB.MySEQ = localAckNumber;	// Set SEQ = SEG.ACK
					SendTCP(RST | ACK, 0);
					CloseSocket();
					return;
				}
			}
			else if(MyTCBStub.smState == TCP_CLOSING)
			{
				// Check to see if our FIN has been ACKnowledged
				if(MyTCB.MySEQ == localAckNumber)
				{
					// RFC not recommended: We should be going to 
					// the TCP_TIME_WAIT state right here and 
					// starting a 2MSL timer, but since we have so 
					// few precious sockets, we can't afford to 
					// leave a socket waiting around doing nothing 
					// for a long time.  If the remote node does 
					// not recieve this ACK, it'll have to figure 
					// out on it's own that the connection is now 
					// closed.
					CloseSocket();
				}

				return;
			}

			break;

		case TCP_LAST_ACK:
			// Check to see if our FIN has been ACKnowledged
			if(MyTCB.MySEQ + 1 == localAckNumber)
				CloseSocket();
			return;

//		case TCP_TIME_WAIT:
//			// Nothing is supposed to arrive here.  If it does, reset the quiet timer.
//			SendTCP(ACK, SENDTCP_RESET_TIMERS);
//			return;

		default:
			break;
	}

	//
	// Sixth: Check the URG bit
	//
	// Urgent packets are not supported in this stack, so we
	// will throw them away instead
	if(localHeaderFlags & URG)
		return;

	//
	// Seventh: Process the segment text
	//
	// Throw data away if in a state that doesn't accept data
	if(MyTCBStub.smState == TCP_CLOSE_WAIT)
		return;
	if(MyTCBStub.smState == TCP_CLOSING)
		return;
	if(MyTCBStub.smState == TCP_LAST_ACK)
		return;
//	if(MyTCBStub.smState == TCP_TIME_WAIT)
//		return;

	// Copy any valid segment data into our RX FIFO, if any
	if(len)
	{
		// See if there are bytes we must skip
		if((SHORT)wMissingBytes <= 0)
		{
			// Position packet read pointer to start of useful data area.
			IPSetRxBuffer((h->DataOffset.Val << 2) - wMissingBytes);
			len += wMissingBytes;		
	
			// Truncate packets that would overflow our TCP RX FIFO
			// and request a retransmit by sending a duplicate ACK
			if(len > wFreeSpace)
				len = wFreeSpace;
	
			MyTCB.RemoteSEQ += (DWORD)len;
		
			// Copy the application data from the packet into the socket RX FIFO
			// See if we need a two part copy (spans bufferEnd->bufferRxStart)
			if(MyTCBStub.rxHead + len > MyTCBStub.bufferEnd)
			{
				wTemp = MyTCBStub.bufferEnd - MyTCBStub.rxHead + 1;
				TCPRAMCopy(MyTCBStub.rxHead, MyTCBStub.vMemoryMedium, (PTR_BASE)-1, TCP_ETH_RAM, wTemp);
				TCPRAMCopy(MyTCBStub.bufferRxStart, MyTCBStub.vMemoryMedium, (PTR_BASE)-1, TCP_ETH_RAM, len - wTemp);
				MyTCBStub.rxHead = MyTCBStub.bufferRxStart + (len - wTemp);
			}
			else
			{
				TCPRAMCopy(MyTCBStub.rxHead, MyTCBStub.vMemoryMedium, (PTR_BASE)-1, TCP_ETH_RAM, len);
				MyTCBStub.rxHead += len;
			}
		
			// See if we have a hole and other data waiting already in the RX FIFO
			if(MyTCB.sHoleSize != -1)
			{
				MyTCB.sHoleSize -= len;
				wTemp = MyTCB.wFutureDataSize + MyTCB.sHoleSize;
		
				// See if we just closed up a hole, and if so, advance head pointer
				if((SHORT)wTemp < (SHORT)0)
				{
					MyTCB.sHoleSize = -1;
				}
				else if(MyTCB.sHoleSize <= 0)
				{
					MyTCB.RemoteSEQ += wTemp;
					MyTCBStub.rxHead += wTemp;
					if(MyTCBStub.rxHead > MyTCBStub.bufferEnd)
						MyTCBStub.rxHead -= MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;							
					MyTCB.sHoleSize = -1;
				}
			}
		} // This packet is out of order or we lost a packet, see if we can generate a hole to accomodate it
		else if((SHORT)wMissingBytes > 0)
		{
			// Truncate packets that would overflow our TCP RX FIFO
			if(len + wMissingBytes > wFreeSpace)
				len = wFreeSpace - wMissingBytes;
		
			// Position packet read pointer to start of useful data area.
			IPSetRxBuffer(h->DataOffset.Val << 2);
	
			// See if we need a two part copy (spans bufferEnd->bufferRxStart)
			if(MyTCBStub.rxHead + wMissingBytes + len > MyTCBStub.bufferEnd)
			{
				// Calculate number of data bytes to copy before wraparound
				wTemp = MyTCBStub.bufferEnd - MyTCBStub.rxHead + 1 - wMissingBytes;
				if((SHORT)wTemp >= 0)
				{
					TCPRAMCopy(MyTCBStub.rxHead + wMissingBytes, MyTCBStub.vMemoryMedium, (PTR_BASE)-1, TCP_ETH_RAM, wTemp);
					TCPRAMCopy(MyTCBStub.bufferRxStart, MyTCBStub.vMemoryMedium, (PTR_BASE)-1, TCP_ETH_RAM, len - wTemp);
				}
				else
				{
					TCPRAMCopy(MyTCBStub.rxHead + wMissingBytes - (MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1), MyTCBStub.vMemoryMedium, (PTR_BASE)-1, TCP_ETH_RAM, len);
				}
			}
			else
			{
				TCPRAMCopy(MyTCBStub.rxHead + wMissingBytes, MyTCBStub.vMemoryMedium, (PTR_BASE)-1, TCP_ETH_RAM, len);
			}
		
			// Record the hole is here
			if(MyTCB.sHoleSize == -1)
			{
				MyTCB.sHoleSize = wMissingBytes;
				MyTCB.wFutureDataSize = len;
			}
			else
			{
				// We already have a hole, see if we can shrink the hole 
				// or extend the future data size
				if(wMissingBytes < (WORD)MyTCB.sHoleSize)
				{
					if((wMissingBytes + len > (WORD)MyTCB.sHoleSize + MyTCB.wFutureDataSize) || (wMissingBytes + len < (WORD)MyTCB.sHoleSize))
						MyTCB.wFutureDataSize = len;
					else
						MyTCB.wFutureDataSize = (WORD)MyTCB.sHoleSize + MyTCB.wFutureDataSize - wMissingBytes;
					MyTCB.sHoleSize = wMissingBytes;
				}
				else if(wMissingBytes + len > (WORD)MyTCB.sHoleSize + MyTCB.wFutureDataSize)
				{
					// Make sure that there isn't a second hole between 
					// our future data and this TCP segment's future data
					if(wMissingBytes <= (WORD)MyTCB.sHoleSize + MyTCB.wFutureDataSize)
						MyTCB.wFutureDataSize += wMissingBytes + len - (WORD)MyTCB.sHoleSize - MyTCB.wFutureDataSize;
				}
				
			}
		}
	}

	// Send back an ACK of the data (+SYN | FIN) we just received, 
	// if any.  To minimize bandwidth waste, we are implementing 
	// the delayed acknowledgement algorithm here, only sending 
	// back an immediate ACK if this is the second segment received.  
	// Otherwise, a 200ms timer will cause the ACK to be transmitted.
	if(wSegmentLength)
	{
		// For non-established sockets, let's delete all data in 
		// the RX buffer immediately after receiving it.  This is 
		// not really how TCP was intended to operate since a 
		// socket cannot receive any response after it sends a FIN,
		// but our TCP application API doesn't readily accomodate
		// receiving data after calling TCPDisconnect(), which 
		// invalidates the application TCP handle.  By deleting all 
		// data, we'll ensure that the RX window is nonzero and 
		// the remote node will be able to send us a FIN response, 
		// which needs an RX window of at least 1.
		if(MyTCBStub.smState != TCP_ESTABLISHED)
			MyTCBStub.rxTail = MyTCBStub.rxHead;

		if(MyTCBStub.Flags.bOneSegmentReceived)
		{
			SendTCP(ACK, SENDTCP_RESET_TIMERS);
			SyncTCB();
			// bOneSegmentReceived is cleared in SendTCP(), so no need here
		}
		else
		{
			MyTCBStub.Flags.bOneSegmentReceived = TRUE;	
		
			// Do not send an ACK immediately back.  Instead, we will 
			// perform delayed acknowledgements.  To do this, we will 
			// just start a timer
			if(!MyTCBStub.Flags.bDelayedACKTimerEnabled)
			{
				MyTCBStub.Flags.bDelayedACKTimerEnabled = 1;
				MyTCBStub.OverlappedTimers.delayedACKTime = (WORD)TickGetDiv256() + (WORD)((TCP_DELAYED_ACK_TIMEOUT)>>8);
			}
		}
	}

	//
	// Eighth: check the FIN bit
	//
	if(localHeaderFlags & FIN)
	{
		// Note: Since we don't have a good means of storing "FIN bytes" 
		// in our TCP RX FIFO, we must ensure that FINs are processed 
		// in-order.
		if(MyTCB.RemoteSEQ + 1 == localSeqNumber + (DWORD)wSegmentLength)
		{
			// FINs are treated as one byte of data for ACK sequencing
			MyTCB.RemoteSEQ++;
			
			switch(MyTCBStub.smState)
			{
				case TCP_SYN_RECEIVED:
					// RFC in exact: Our API has no need for the user 
					// to explicitly close a socket that never really 
					// got opened fully in the first place, so just 
					// transmit a FIN automatically and jump to 
					// TCP_LAST_ACK
					MyTCBStub.smState = TCP_LAST_ACK;
					SendTCP(FIN | ACK, SENDTCP_RESET_TIMERS);
					return;

				case TCP_ESTABLISHED:
					// Go to TCP_CLOSE_WAIT state
					MyTCBStub.smState = TCP_CLOSE_WAIT;
					
					// For legacy applications that don't call 
					// TCPDisconnect() as needed and expect the TCP/IP 
					// Stack to automatically close sockets when the 
					// remote node sends a FIN, let's start a timer so 
					// that we will eventually close the socket automatically
					MyTCBStub.OverlappedTimers.closeWaitTime = (WORD)TickGetDiv256() + (WORD)((TCP_CLOSE_WAIT_TIMEOUT)>>8);
					break;
	
				case TCP_FIN_WAIT_1:
					if(MyTCB.MySEQ == localAckNumber)
					{
						// RFC not recommended: We should be going to 
						// the TCP_TIME_WAIT state right here and 
						// starting a 2MSL timer, but since we have so 
						// few precious sockets, we can't afford to 
						// leave a socket waiting around doing nothing 
						// for a long time.  If the remote node does 
						// not recieve this ACK, it'll have to figure 
						// out on it's own that the connection is now 
						// closed.
						SendTCP(ACK, 0);
						CloseSocket();
						return;
					}
					else
					{
						MyTCBStub.smState = TCP_CLOSING;
					}
					break;
	
				case TCP_FIN_WAIT_2:
					// RFC not recommended: We should be going to 
					// the TCP_TIME_WAIT state right here and 
					// starting a 2MSL timer, but since we have so 
					// few precious sockets, we can't afford to 
					// leave a socket waiting around doing nothing 
					// for a long time.  If the remote node does 
					// not recieve this ACK, it'll have to figure 
					// out on it's own that the connection is now 
					// closed.
					SendTCP(ACK, 0);
					CloseSocket();
					return;

				default:
					break;
			}

			// Acknowledge receipt of FIN
			SendTCP(ACK, SENDTCP_RESET_TIMERS);
		}
	}
}

/****************************************************************************
  Section:
	Buffer Management Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	BOOL TCPAdjustFIFOSize(TCP_SOCKET hTCP, WORD wMinRXSize, 
							WORD wMinTXSize, BYTE vFlags)

  Summary:
	Adjusts the relative sizes of the RX and TX buffers.

  Description:
	This function can be used to adjust the relative sizes of the RX and
	TX FIFO depending on the immediate needs of an application.  Since a 
	larger FIFO can allow more data to be sent in a given packet, adjusting 
	the relative sizes on the fly can allow for optimal transmission speed 
	for one-sided application protocols.  For example, HTTP typically 
	begins by receiving large amounts of data from the client, then switches
	to serving large amounts of data back.  Adjusting the FIFO at these 
	points can increase performance substantially.  Once the FIFO is
	adjusted, a window update is sent.
	
	If neither or both of TCP_ADJUST_GIVE_REST_TO_TX and 
	TCP_ADJUST_GIVE_REST_TO_RX are set, the function distributes the
	remaining space equally.
	
	Received data can be preserved as long as the buffer is expanding and 
	has not wrapped.

  Precondition:
	TCP is initialized.

  Parameters:
	hTCP		- The socket to be adjusted
	wMinRXSize	- Minimum number of byte for the RX FIFO
	wMinTXSize 	- Minimum number of bytes for the RX FIFO
	vFlags		- Any combination of TCP_ADJUST_GIVE_REST_TO_RX, 
				  TCP_ADJUST_GIVE_REST_TO_TX, TCP_ADJUST_PRESERVE_RX.
				  TCP_ADJUST_PRESERVE_TX is not currently supported.

  Return Values:
	TRUE - The FIFOs were adjusted successfully
	FALSE - Minimum RX, Minimum TX, or flags couldn't be accommodated and
			therefore the socket was left unchanged.

  Side Effects:
	Any unacknowledged or untransmitted data in the TX FIFO is always
	deleted.

  Remarks:
	At least one byte must always be allocated to the RX buffer so that
	a FIN can be received.  The function automatically corrects for this.
  ***************************************************************************/
BOOL TCPAdjustFIFOSize(TCP_SOCKET hTCP, WORD wMinRXSize, WORD wMinTXSize, BYTE vFlags)
{
	PTR_BASE ptrTemp, ptrHead;
	WORD wTXAllocation;
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	// Load up info on this socket
	SyncTCBStub(hTCP);

	// RX has to be at least 1 byte to receive SYN and FIN bytes 
	// from the remote node, even if they aren't stored in the RX FIFO
	if(wMinRXSize == 0u)
		wMinRXSize = 1;
		
	// SSL connections need to be able to send or receive at least 
	// a full Alert record, MAC, and FIN
	#if defined(STACK_USE_SSL)
	if(TCPIsSSL(hTCP) && wMinRXSize < 25u)
		wMinRXSize = 25;
	if(TCPIsSSL(hTCP) && wMinTXSize < 25u)
		wMinTXSize = 25;
	#endif
	
	// Make sure space is available for minimums
	ptrTemp = MyTCBStub.bufferEnd - MyTCBStub.bufferTxStart - 1;
	if(wMinRXSize + wMinTXSize > ptrTemp)
		return FALSE;

	SyncTCB();

	// Set both allocation flags if none set
	if(!(vFlags & (TCP_ADJUST_GIVE_REST_TO_TX | TCP_ADJUST_GIVE_REST_TO_RX)))
		vFlags |= TCP_ADJUST_GIVE_REST_TO_TX | TCP_ADJUST_GIVE_REST_TO_RX;
		

	// Allocate minimums
	wTXAllocation = wMinTXSize;
	ptrTemp -= wMinRXSize + wMinTXSize;

	// Allocate extra
	if(vFlags & TCP_ADJUST_GIVE_REST_TO_TX)
	{
		if(vFlags & TCP_ADJUST_GIVE_REST_TO_RX)
		{
			// Do a 50%/50% split with any odd byte always going to the RX FIFO
			wTXAllocation += ptrTemp>>1;
		}
		else
		{
			wTXAllocation += ptrTemp;
		}
	}

	// Calculate new bufferRxStart pointer
	ptrTemp = MyTCBStub.bufferTxStart + wTXAllocation + 1;

	// Find the head pointer to use
	ptrHead = MyTCBStub.rxHead;
	#if defined(STACK_USE_SSL)
	if(TCPIsSSL(hTCP))
		ptrHead = MyTCBStub.sslRxHead;
	#endif
	
	// If there's out-of-order data pending, adjust the head pointer to compensate
	if(MyTCB.sHoleSize != -1)
	{
		ptrHead += MyTCB.sHoleSize + MyTCB.wFutureDataSize;
		if(ptrHead > MyTCBStub.bufferEnd)
			ptrHead -= MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;
	}

	// Determine if resizing will lose any RX data
	if(MyTCBStub.rxTail < ptrHead)
	{
		if(ptrTemp > MyTCBStub.rxTail)
		{
			if(vFlags & TCP_ADJUST_PRESERVE_RX)
				return FALSE;
			else
			{
				MyTCBStub.rxTail = ptrTemp;
				MyTCBStub.rxHead = ptrTemp;

				#if defined(STACK_USE_SSL)
				MyTCBStub.sslRxHead = ptrTemp;
				#endif
			}
		}
	}
	else if(MyTCBStub.rxTail > ptrHead)
	{
		if(ptrTemp > MyTCBStub.bufferRxStart)
		{
			if(vFlags & TCP_ADJUST_PRESERVE_RX)
				return FALSE;
			else
			{
				MyTCBStub.rxTail = ptrTemp;
				MyTCBStub.rxHead = ptrTemp;
				
				#if defined(STACK_USE_SSL)
				MyTCBStub.sslRxHead = ptrTemp;
				#endif
			}
		}
	}
	else
	{
		// No data to preserve, but we may need to move 
		// the pointers to stay in the RX space
		MyTCBStub.rxTail = ptrTemp;
		MyTCBStub.rxHead = ptrTemp;
		
		#if defined(STACK_USE_SSL)
		MyTCBStub.sslRxHead = ptrTemp;
		#endif
	}
	
	// If we need to preserve data that wrapped in the ring, we must copy
	if(ptrHead < MyTCBStub.rxTail && (vFlags & TCP_ADJUST_PRESERVE_RX))
	{
		TCPRAMCopy(ptrTemp, MyTCBStub.vMemoryMedium, 
			MyTCBStub.bufferRxStart, MyTCBStub.vMemoryMedium,
			ptrHead - MyTCBStub.bufferRxStart);

		// Move the pointers if they were in front of the tail
		#if defined(STACK_USE_SSL)
		if(TCPIsSSL(hTCP) && MyTCBStub.sslRxHead < MyTCBStub.rxTail)
			MyTCBStub.sslRxHead -= MyTCBStub.bufferRxStart - ptrTemp;
		#endif
		if(MyTCBStub.rxHead < MyTCBStub.rxTail)
			MyTCBStub.rxHead -= MyTCBStub.bufferRxStart - ptrTemp;
	}
	
	// Move the RX buffer pointer - it's the one that divides the two
	MyTCBStub.bufferRxStart = ptrTemp;

	// Empty the TX buffer
	MyTCB.txUnackedTail = MyTCBStub.bufferTxStart;
	MyTCBStub.txTail = MyTCBStub.bufferTxStart;
	MyTCBStub.txHead = MyTCBStub.bufferTxStart;
	
	#if defined(STACK_USE_SSL)
	if(TCPIsSSL(hTCP))
		MyTCBStub.sslTxHead = MyTCBStub.txHead + 5;
	#endif
	
	// Send a window update to notify remote node of change
	if(MyTCBStub.smState == TCP_ESTABLISHED)
		SendTCP(ACK, SENDTCP_RESET_TIMERS);

	return TRUE;

}

/*****************************************************************************
  Function:
	static void TCPRAMCopy(PTR_BASE ptrDest, BYTE vDestType, PTR_BASE ptrSource, 
							BYTE vSourceType, WORD wLength)

  Summary:
	Copies data to/from various memory mediums.

  Description:
	This function copies data between memory mediums (PIC RAM, SPI
	RAM, and Ethernet buffer RAM).

  Precondition:
	TCP is initialized.

  Parameters:
	ptrDest		- Address to write to
	vDestType	- Destination meidum (TCP_PIC_RAM, TCP_ETH_RAM, TCP_SPI_RAM)
	ptrSource	- Address to copy from
	vSourceType - Source medium (TCP_PIC_RAM, TCP_ETH_RAM, or TCP_SPI_RAM)
	wLength		- Number of bytes to copy

  Returns:
	None

  Remarks:
	Copying to a destination region that overlaps with the source address 
	is supported only if the destination start address is at a lower memory 
	address (closer to 0x0000) than the source pointer.  However, if they do 
	overlap there must be at least 4 bytes of non-overlap to ensure correct 
	results due to hardware DMA requirements.
  ***************************************************************************/
static void TCPRAMCopy(PTR_BASE ptrDest, BYTE vDestType, PTR_BASE ptrSource, BYTE vSourceType, WORD wLength)
{
	#if defined(SPIRAM_CS_TRIS)
	BYTE vBuffer[16];
	WORD w;
	#endif
		
	switch(vSourceType)
	{
		case TCP_PIC_RAM:
			switch(vDestType)
			{
				case TCP_PIC_RAM:
					memcpy((void*)(BYTE*)ptrDest, (void*)(BYTE*)ptrSource, wLength);
					break;
	
				case TCP_ETH_RAM:
					if(ptrDest!=(PTR_BASE)-1)
						MACSetWritePtr(ptrDest);
					MACPutArray((BYTE*)ptrSource, wLength);
					break;
	
				#if defined(SPIRAM_CS_TRIS)
				case TCP_SPI_RAM:
					SPIRAMPutArray(ptrDest, (BYTE*)ptrSource, wLength);
					break;
				#endif
			}
			break;
	
		case TCP_ETH_RAM:
			switch(vDestType)
			{
				case TCP_PIC_RAM:
					if(ptrSource!=(PTR_BASE)-1)
						MACSetReadPtr(ptrSource);
					MACGetArray((BYTE*)ptrDest, wLength);
					break;
	
				case TCP_ETH_RAM:
					MACMemCopyAsync(ptrDest, ptrSource, wLength);
					while(!MACIsMemCopyDone());
					break;
	
				#if defined(SPIRAM_CS_TRIS)
				case TCP_SPI_RAM:
					if(ptrSource!=(PTR_BASE)-1)
						MACSetReadPtr(ptrSource);
					w = sizeof(vBuffer);
					while(wLength)
					{
						if(w > wLength)
							w = wLength;
						
						// Read and write a chunk	
						MACGetArray(vBuffer, w);
						SPIRAMPutArray(ptrDest, vBuffer, w);
						ptrDest += w;
						wLength -= w;
					}
					break;
				#endif
			}
			break;
	
		#if defined(SPIRAM_CS_TRIS)
		case TCP_SPI_RAM:
			switch(vDestType)
			{
				case TCP_PIC_RAM:
					SPIRAMGetArray(ptrSource, (BYTE*)ptrDest, wLength);
					break;
	
				case TCP_ETH_RAM:
					if(ptrDest!=(PTR_BASE)-1)
						MACSetWritePtr(ptrDest);
					w = sizeof(vBuffer);
					while(wLength)
					{
						if(w > wLength)
							w = wLength;
						
						// Read and write a chunk	
						SPIRAMGetArray(ptrSource, vBuffer, w);
						ptrSource += w;
						MACPutArray(vBuffer, w);
						wLength -= w;
					}
					break;
	
				case TCP_SPI_RAM:
					// Copy all of the data over in chunks
					w = sizeof(vBuffer);
					while(wLength)
					{
						if(w > wLength)
							w = wLength;
							
						SPIRAMGetArray(ptrSource, vBuffer, w);
						SPIRAMPutArray(ptrDest, vBuffer, w);
						ptrSource += w;
						ptrDest += w;
						wLength -= w;
					}
					break;
			}
			break;
		#endif			
	}
}

/*****************************************************************************
  Function:
	static void TCPRAMCopyROM(PTR_BASE wDest, BYTE wDestType, ROM BYTE* wSource, 
								WORD wLength)

  Summary:
	Copies data to/from various memory mediums.

  Description:
	This function copies data between memory mediums (PIC RAM, SPI
	RAM, and Ethernet buffer RAM).  This function is to be used when 
	copying from ROM.

  Precondition:
	TCP is initialized.

  Parameters:
	wDest		- Address to write to
	wDestType	- Destination meidum (TCP_PIC_RAM, TCP_ETH_RAM, TCP_SPI_RAM)
	wSource		- Address to copy from
	wLength		- Number of bytes to copy

  Returns:
	None

  Remarks:
	Copying to a destination region that overlaps with the source address 
	is supported only if the destination start address is at a lower memory 
	address (closer to 0x0000) than the source pointer.
	
	This function is aliased to TCPRAMCopy on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
static void TCPRAMCopyROM(PTR_BASE wDest, BYTE wDestType, ROM BYTE* wSource, WORD wLength)
{
	BYTE vBuffer[16];
	WORD w;
	
	switch(wDestType)
	{
		case TCP_PIC_RAM:
			memcpypgm2ram((void*)(BYTE*)wDest, (ROM void*)wSource, wLength);
			break;
	
		case TCP_ETH_RAM:
			if(wDest!=(PTR_BASE)-1)
				MACSetWritePtr(wDest);
			w = sizeof(vBuffer);
			while(wLength)
			{
				if(w > wLength)
					w = wLength;
				
				// Read and write a chunk	
				memcpypgm2ram(vBuffer, (ROM void*)wSource, w);
				MACPutArray(vBuffer, w);
				wSource += w;
				wLength -= w;
			}
			break;
	
		#if defined(SPIRAM_CS_TRIS)
		case TCP_SPI_RAM:
			w = sizeof(vBuffer);
			while(wLength)
			{
				if(w > wLength)
					w = wLength;
				
				// Read and write a chunk	
				memcpypgm2ram(vBuffer, (ROM void*)wSource, w);
				SPIRAMPutArray(wDest, vBuffer, w);
				wDest += w;
				wSource += w;
				wLength -= w;
			}
			break;
		#endif
	}
}
#endif

/****************************************************************************
  Section:
	SSL Functions
  ***************************************************************************/

/*****************************************************************************
  Function:
	BOOL TCPStartSSLClient(TCP_SOCKET hTCP, BYTE* host)

  Summary:
	Begins an SSL client session.

  Description:
	This function escalates the current connection to an SSL secured 
	connection by initiating an SSL client handshake.

  Precondition:
	TCP is initialized and hTCP is already connected.

  Parameters:
	hTCP		- TCP connection to secure
	host		- Expected host name on certificate (currently ignored)

  Return Values:
	TRUE 		- an SSL connection was initiated
	FALSE 		- Insufficient SSL resources (stubs) were available

  Remarks:
	The host parameter is currently ignored and is not validated.
  ***************************************************************************/
#if defined(STACK_USE_SSL_CLIENT)
BOOL TCPStartSSLClient(TCP_SOCKET hTCP, BYTE* host)
{
	BYTE i;
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	
	// Make sure SSL is not established already
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
		return FALSE;
	
	// Try to start the session
	MyTCBStub.sslStubID = SSLStartSession(hTCP, NULL, 0);
	
	// Make sure a session stub was obtained
	if(MyTCBStub.sslStubID == SSL_INVALID_ID)
		return FALSE;

	// Mark connection as handshaking and return
	MyTCBStub.sslReqMessage = SSL_CLIENT_HELLO;
	MyTCBStub.sslRxHead = MyTCBStub.rxHead;
	MyTCBStub.sslTxHead = MyTCBStub.txHead;
	MyTCBStub.Flags.bSSLHandshaking = 1;
	for(i = 0; i < 5u; i++)
	{// Skip first 5 bytes in TX for the record header
		if(++MyTCBStub.sslTxHead >= MyTCBStub.bufferRxStart)
			MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
	}
	return TRUE;
}
#endif // SSL Client

/*****************************************************************************
  Function:
	BOOL TCPStartSSLClientEx(TCP_SOCKET hTCP, BYTE* host, BYTE * buffer, BYTE suppDataType)

  Summary:
	Begins an SSL client session.

  Description:
	This function escalates the current connection to an SSL secured 
	connection by initiating an SSL client handshake.

  Precondition:
	TCP is initialized and hTCP is already connected.

  Parameters:
	hTCP			- TCP connection to secure
	host			- Expected host name on certificate (currently ignored)
	buffer      	- Buffer for supplementary data return
	suppDataType 	- Type of supplementary data to copy

  Return Values:
	TRUE 		- an SSL connection was initiated
	FALSE 		- Insufficient SSL resources (stubs) were available

  Remarks:
	The host parameter is currently ignored and is not validated.
  ***************************************************************************/
#if defined(STACK_USE_SSL_CLIENT)
BOOL TCPStartSSLClientEx(TCP_SOCKET hTCP, BYTE* host, void * buffer, BYTE suppDataType)
{
	BYTE i;
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	
	// Make sure SSL is not established already
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
		return FALSE;
	
	// Try to start the session
	MyTCBStub.sslStubID = SSLStartSession(hTCP, buffer, suppDataType);
	
	// Make sure a session stub was obtained
	if(MyTCBStub.sslStubID == SSL_INVALID_ID)
		return FALSE;

	// Mark connection as handshaking and return
	MyTCBStub.sslReqMessage = SSL_CLIENT_HELLO;
	MyTCBStub.sslRxHead = MyTCBStub.rxHead;
	MyTCBStub.sslTxHead = MyTCBStub.txHead;
	MyTCBStub.Flags.bSSLHandshaking = 1;
	for(i = 0; i < 5u; i++)
	{// Skip first 5 bytes in TX for the record header
		if(++MyTCBStub.sslTxHead >= MyTCBStub.bufferRxStart)
			MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
	}
	return TRUE;
}
#endif // SSL Client

/*****************************************************************************
  Function:
	BOOL TCPStartSSLServer(TCP_SOCKET hTCP)

  Summary:
	Begins an SSL server session.

  Description:
	This function sets up an SSL server session when a new connection is
	established on an SSL port.

  Precondition:
	TCP is initialized and hTCP is already connected.

  Parameters:
	hTCP		- TCP connection to secure

  Return Values:
	TRUE		- an SSL connection was initiated
	FALSE		- Insufficient SSL resources (stubs) were available
  ***************************************************************************/
#if defined(STACK_USE_SSL_SERVER)
BOOL TCPStartSSLServer(TCP_SOCKET hTCP)
{
	BYTE i;
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	SyncTCB();
	
	// Make sure SSL is not established already
	if(MyTCBStub.sslStubID != SSL_INVALID_ID)
		return TRUE;
	
	// Try to start the session
	MyTCBStub.sslStubID = SSLStartSession(hTCP, NULL, 0);
	
	// Make sure a session stub was obtained
	if(MyTCBStub.sslStubID == SSL_INVALID_ID)
		return FALSE;

	// Swap the localPort and localSSLPort
	MyTCBStub.remoteHash.Val = MyTCB.localPort.Val;
	MyTCB.localPort.Val = MyTCB.localSSLPort.Val;
	MyTCB.localSSLPort.Val = MyTCBStub.remoteHash.Val;	

	// Mark connection as handshaking and return
	MyTCBStub.sslReqMessage = SSL_NO_MESSAGE;
	MyTCBStub.sslRxHead = MyTCBStub.rxHead;
	MyTCBStub.sslTxHead = MyTCBStub.txHead;
	MyTCBStub.Flags.bSSLHandshaking = 1;
	for(i = 0; i < 5u; i++)
	{// Skip first 5 bytes in TX for the record header
		if(++MyTCBStub.sslTxHead >= MyTCBStub.bufferRxStart)
			MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
	}
	return TRUE;
}
#endif // SSL Client

/*****************************************************************************
  Function:
	BOOL TCPAddSSLListener(TCP_SOCKET hTCP, WORD port)

  Summary:
	Listens for SSL connection on a specific port.

  Description:
	This function adds an additional listening port to a TCP connection.  
	Connections made on this alternate port will be secured via SSL.

  Precondition:
	TCP is initialized and hTCP is listening.

  Parameters:
	hTCP		- TCP connection to secure
	port		- SSL port to listen on

  Return Values:
	TRUE		- SSL port was added.
	FALSE		- The socket was not a listening socket.
  ***************************************************************************/
#if defined(STACK_USE_SSL_SERVER)
BOOL TCPAddSSLListener(TCP_SOCKET hTCP, WORD port)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	
	if(MyTCBStub.smState != TCP_LISTEN)
		return FALSE;
	
	SyncTCB();
	
	MyTCB.localSSLPort.Val = port;
	MyTCBStub.sslTxHead = port;

	return TRUE;
}
#endif // SSL Server

/*****************************************************************************
  Function:
	BOOL TCPRequestSSLMessage(TCP_SOCKET hTCP, BYTE msg)

  Summary:
	Requests an SSL message to be transmitted.

  Description:
	This function is called to request that a specific SSL message be
	transmitted.  This message should only be called by the SSL module.
	
  Precondition:
	TCP is initialized.

  Parameters:
	hTCP		- TCP connection to use
	msg			- One of the SSL_MESSAGE types to transmit.

  Return Values:
	TRUE		- The message was requested.
	FALSE		- Another message is already pending transmission.
  ***************************************************************************/
#if defined(STACK_USE_SSL)
BOOL TCPRequestSSLMessage(TCP_SOCKET hTCP, BYTE msg)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	
	if(msg == SSL_NO_MESSAGE || MyTCBStub.sslReqMessage == SSL_NO_MESSAGE)
	{
		MyTCBStub.sslReqMessage = msg;
		return TRUE;
	}
	
	return FALSE;
}
#endif // SSL

/*****************************************************************************
  Function:
	BOOL TCPSSLIsHandshaking(TCP_SOCKET hTCP)

  Summary:
	Determines if an SSL session is still handshaking.

  Description:
	Call this function after calling TCPStartSSLClient until FALSE is
	returned.  Then your application may continue with its normal data
	transfer (which is now secured).
	
  Precondition:
	TCP is initialized and hTCP is connected.

  Parameters:
	hTCP		- TCP connection to check

  Return Values:
	TRUE		- SSL handshake is still progressing
	FALSE		- SSL handshake has completed
  ***************************************************************************/
#if defined(STACK_USE_SSL)
BOOL TCPSSLIsHandshaking(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	return MyTCBStub.Flags.bSSLHandshaking;	
}
#endif // SSL

/*****************************************************************************
  Function:
	BOOL TCPIsSSL(TCP_SOCKET hTCP)

  Summary:
	Determines if a TCP connection is secured with SSL.

  Description:
	Call this function to determine whether or not a TCP connection is 
	secured with SSL.
	
  Precondition:
	TCP is initialized and hTCP is connected.

  Parameters:
	hTCP		- TCP connection to check

  Return Values:
	TRUE		- Connection is secured via SSL
	FALSE		- Connection is not secured
  ***************************************************************************/
#if defined(STACK_USE_SSL)
BOOL TCPIsSSL(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return FALSE;
    }
    
	SyncTCBStub(hTCP);
	
	if(MyTCBStub.sslStubID == SSL_INVALID_ID)
		return FALSE;
	
	return TRUE;
}
#endif // SSL

/*****************************************************************************
  Function:
	void TCPSSLHandshakeComplete(TCP_SOCKET hTCP)

  Summary:
	Clears the SSL handshake flag.

  Description:
	This function clears the flag indicating that an SSL handshake is
	complete.
	
  Precondition:
	TCP is initialized and hTCP is connected.

  Parameters:
	hTCP		- TCP connection to set

  Returns:
	None

  Remarks:
	This function should never be called by an application.  It is used 
	only by the SSL module itself.
  ***************************************************************************/
#if defined(STACK_USE_SSL)
void TCPSSLHandshakeComplete(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	SyncTCBStub(hTCP);
	MyTCBStub.Flags.bSSLHandshaking = 0;
}
#endif // SSL

/*****************************************************************************
  Function:
	void TCPSSLDecryptMAC(TCP_SOCKET hTCP, ARCFOUR_CTX* ctx, WORD len)

  Summary:
	Decrypts and MACs data arriving via SSL.

  Description:
	This function decrypts data in the TCP buffer and calculates the MAC over
	the data.  All data is left in the exact same location in the TCP buffer.
	It is called to help process incoming SSL records.
	
  Precondition:
	TCP is initialized, hTCP is connected, and ctx's Sbox is loaded.

  Parameters:
	hTCP		- TCP connection to decrypt in
	ctx			- ARCFOUR encryption context to use
	len 		- Number of bytes to crypt
	inPlace		- TRUE to write back in place, FALSE to write at end of
					currently visible data.

  Returns:
	None

  Remarks:
	This function should never be called by an application.  It is used 
	only by the SSL module itself.
  ***************************************************************************/
#if defined(STACK_USE_SSL)
void TCPSSLDecryptMAC(TCP_SOCKET hTCP, ARCFOUR_CTX* ctx, WORD len)
{
	PTR_BASE wSrc, wDest, wBlockLen, wTemp;
	BYTE buffer[32];
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	// Set up the pointers
	SyncTCBStub(hTCP);
	wSrc = MyTCBStub.rxTail;
	wDest = wSrc;
	
	// Handle 32 bytes at a time
	while(len)
	{
		// Determine how many bytes we can read
		wBlockLen = sizeof(buffer);
		if(wBlockLen > len) // Don't do more than we should
			wBlockLen = len;
		
		// Read those bytes to a buffer
		if(wSrc + wBlockLen > MyTCBStub.bufferEnd)
		{// Two part read
			wTemp = MyTCBStub.bufferEnd - wSrc + 1;
			TCPRAMCopy((PTR_BASE)buffer, TCP_PIC_RAM, wSrc, MyTCBStub.vMemoryMedium, wTemp);
			TCPRAMCopy((PTR_BASE)buffer+wTemp, TCP_PIC_RAM, MyTCBStub.bufferRxStart, MyTCBStub.vMemoryMedium, wBlockLen - wTemp);
			wSrc = MyTCBStub.bufferRxStart + wBlockLen - wTemp;
		}
		else
		{
			TCPRAMCopy((PTR_BASE)buffer, TCP_PIC_RAM, wSrc, MyTCBStub.vMemoryMedium, wBlockLen);
			wSrc += wBlockLen;
		}
		
		// Decrypt and hash
		ARCFOURCrypt(ctx, buffer, wBlockLen);
		SSLMACAdd(buffer, wBlockLen);
		
		// Write decrypted bytes back
		if(wDest + wBlockLen > MyTCBStub.bufferEnd)
		{// Two part write
			wTemp = MyTCBStub.bufferEnd - wDest + 1;
			TCPRAMCopy(wDest, MyTCBStub.vMemoryMedium, (PTR_BASE)buffer, TCP_PIC_RAM, wTemp);
			TCPRAMCopy(MyTCBStub.bufferRxStart, MyTCBStub.vMemoryMedium, (PTR_BASE)buffer+wTemp, TCP_PIC_RAM, wBlockLen - wTemp);
			wDest = MyTCBStub.bufferRxStart + wBlockLen - wTemp;
		}
		else
		{
			TCPRAMCopy(wDest, MyTCBStub.vMemoryMedium, (PTR_BASE)buffer, TCP_PIC_RAM, wBlockLen);
			wDest += wBlockLen;
		}
		
		// Update the length remaining
		len -= wBlockLen;
	}
}	
#endif // SSL

/*****************************************************************************
  Function:
	void TCPSSLInPlaceMACEncrypt(TCP_SOCKET hTCP, ARCFOUR_CTX* ctx, 
									BYTE* MACSecret, WORD len)

  Summary:
	Encrypts and MACs data in place in the TCP TX buffer.

  Description:
	This function encrypts data in the TCP buffer while calcuating a MAC.  
	When encryption is finished, the MAC is appended to the buffer and 
	the record will be ready to transmit.
	
  Precondition:
	TCP is initialized, hTCP is connected, and ctx's Sbox is loaded.

  Parameters:
	hTCP		- TCP connection to encrypt in
	ctx			- ARCFOUR encryption context to use
	MACSecret	- MAC encryption secret to use
	len 		- Number of bytes to crypt

  Returns:
	None

  Remarks:
	This function should never be called by an application.  It is used 
	only by the SSL module itself.
  ***************************************************************************/
#if defined(STACK_USE_SSL)
void TCPSSLInPlaceMACEncrypt(TCP_SOCKET hTCP, ARCFOUR_CTX* ctx, BYTE* MACSecret, WORD len)
{
	PTR_BASE pos;
	WORD blockLen;
	BYTE buffer[32];
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	// Set up the pointers
	SyncTCBStub(hTCP);
	pos = MyTCBStub.txHead;
	for(blockLen = 0; blockLen < 5u; blockLen++)
	{// Skips first 5 bytes for the header
		if(++pos >= MyTCBStub.bufferRxStart)
			pos = MyTCBStub.bufferTxStart;
	}
	
	// Handle 32 bytes at a time
	while(len)
	{
		// Determine how many bytes we can read
		blockLen = sizeof(buffer);
		if(blockLen > len) // Don't do more than we should
			blockLen = len;
		if(blockLen > MyTCBStub.bufferRxStart - pos) // Don't pass the end
			blockLen = MyTCBStub.bufferRxStart - pos;
		
		// Read those bytes to a buffer
		TCPRAMCopy((PTR_BASE)buffer, TCP_PIC_RAM, pos, MyTCBStub.vMemoryMedium, blockLen);
		
		// Hash and encrypt
		SSLMACAdd(buffer, blockLen);
		ARCFOURCrypt(ctx, buffer, blockLen);
		
		// Put them back
		TCPRAMCopy(pos, MyTCBStub.vMemoryMedium, (PTR_BASE)buffer, TCP_PIC_RAM, blockLen);
		
		// Update the pointers
		pos += blockLen;
		len -= blockLen;
		if(pos >= MyTCBStub.bufferRxStart)
			pos = MyTCBStub.bufferTxStart;
	}
	
	// Calculate and add the MAC
	SSLMACCalc(MACSecret, buffer);
	ARCFOURCrypt(ctx, buffer, 16);

	// Write the MAC to the TX FIFO
	// Can't use TCPPutArray here because TCPIsPutReady() saves 16 bytes for the MAC
	// TCPPut* functions use this to prevent writing too much data.  Therefore, the
	// functionality is duplicated here.
	
	len = 16;
	blockLen = 0;
	// See if we need a two part put
	if(MyTCBStub.sslTxHead + len >= MyTCBStub.bufferRxStart)
	{
		blockLen = MyTCBStub.bufferRxStart-MyTCBStub.sslTxHead;
		TCPRAMCopy(MyTCBStub.sslTxHead, MyTCBStub.vMemoryMedium, (PTR_BASE)buffer, TCP_PIC_RAM, blockLen);
		len -= blockLen;
		MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
	}
	
	TCPRAMCopy(MyTCBStub.sslTxHead, MyTCBStub.vMemoryMedium, (PTR_BASE)&buffer[blockLen], TCP_PIC_RAM, len);
	MyTCBStub.sslTxHead += len;

}	
#endif // SSL

/*****************************************************************************
  Function:
	void TCPSSLPutRecordHeader(TCP_SOCKET hTCP, BYTE* hdr, BOOL recDone)

  Summary:
	Writes an SSL record header and sends an SSL record.

  Description:
	This function writes an SSL record header to the pending TCP SSL data, 
	then indicates that the data is ready to be sent by moving the txHead
	pointer.
	
	If the record is complete, set recDone to TRUE.  The sslTxHead 
	pointer will be moved forward 5 bytes to leave space for a future 
	record header.  If the record is only partially sent, use FALSE and
	to leave the pointer where it is so that more data can be added
	to the record.  Partial records can only be used for the 
	SERVER_CERTIFICATE handshake message.
	
  Precondition:
	TCP is initialized, and hTCP is connected with an active SSL session.

  Parameters:
	hTCP		- TCP connection to write the header and transmit with
	hdr			- Record header (5 bytes) to send or NULL to just 
				  move the pointerctx
	recDone		- TRUE if the record is done, FALSE otherwise

  Returns:
	None

  Remarks:
	This function should never be called by an application.  It is used 
	only by the SSL module itself.
  ***************************************************************************/
#if defined(STACK_USE_SSL)
void TCPSSLPutRecordHeader(TCP_SOCKET hTCP, BYTE* hdr, BOOL recDone)
{
	BYTE i;
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	// Set up the pointers
	SyncTCBStub(hTCP);
	
	// Write the header if needed
	if(hdr)
	{// This is a new record, so insert the header
		for(i = 0; i < 5u; i++)
		{
			TCPRAMCopy(MyTCBStub.txHead, MyTCBStub.vMemoryMedium, (PTR_BASE)hdr+i, TCP_PIC_RAM, sizeof(BYTE));
			if(++MyTCBStub.txHead >= MyTCBStub.bufferRxStart)
				MyTCBStub.txHead = MyTCBStub.bufferTxStart;
		}
	}
	
	// Move the txHead pointer to indicate what data is ready
	// Also, flush just the header, then all the data.  This shotguns two 
	// packets down the line, therefore causing immediate ACKs by the 
	// remote node.  Reconnect handshakes are as much as 60% faster now.
	TCPFlush(hTCP);
	MyTCBStub.txHead = MyTCBStub.sslTxHead;
	TCPFlush(hTCP);
	
	// If this record is done, move the sslTxHead forward
	// to accomodate the next record header
	if(recDone)
	{
		for(i = 0; i < 5u; i++)
		{// Skip first 5 bytes in TX for the record header
			if(++MyTCBStub.sslTxHead >= MyTCBStub.bufferRxStart)
				MyTCBStub.sslTxHead = MyTCBStub.bufferTxStart;
		}
	}
}	
#endif // SSL

/*****************************************************************************
  Function:
	WORD TCPSSLGetPendingTxSize(TCP_SOCKET hTCP)

  Summary:
	Determines how many bytes are pending for a future SSL record.

  Description:
	This function determines how many bytes are pending for a future SSL
	record.
	
  Precondition:
	TCP is initialized, and hTCP is connected with an active SSL connection.

  Parameters:
	hTCP		- TCP connection to check

  Returns:
	None
  ***************************************************************************/
#if defined(STACK_USE_SSL)
WORD TCPSSLGetPendingTxSize(TCP_SOCKET hTCP)
{
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return 0;
    }
    
	SyncTCBStub(hTCP);

	// Non-SSL connections have no pending SSL data
	//if(MyTCBStub.sslStubID == SSL_INVALID_ID)
	//	return 0;
			
	// Determine how many bytes are waiting to be written in this record
	if(MyTCBStub.sslTxHead > MyTCBStub.txHead)
		return MyTCBStub.sslTxHead - MyTCBStub.txHead - 5;
	else
		return (MyTCBStub.bufferRxStart - MyTCBStub.bufferTxStart - 1) - (MyTCBStub.txHead - MyTCBStub.sslTxHead - 1) - 5;
}
#endif


/*****************************************************************************
  Function:
	void TCPSSLHandleIncoming(TCP_SOCKET hTCP)

  Summary:
	Hands newly arrive TCP data to the SSL module for processing.

  Description:
	This function processes incoming TCP data as an SSL record and 
	performs any necessary repositioning and decrypting.
	
  Precondition:
	TCP is initialized, and hTCP is connected with an active SSL session.

  Parameters:
	hTCP		- TCP connection to handle incoming data on

  Returns:
	None

  Remarks:
	This function should never be called by an application.  It is used 
	only by the SSL module itself.
  ***************************************************************************/
#if defined(STACK_USE_SSL)
void TCPSSLHandleIncoming(TCP_SOCKET hTCP)
{
	PTR_BASE prevRxTail, nextRxHead, startRxTail, wSrc, wDest;
	WORD wToMove, wLen, wSSLBytesThatPoofed, wDecryptedBytes;
	
	if(hTCP >= TCP_SOCKET_COUNT)
    {
        return;
    }
    
	// Sync the stub
	SyncTCBStub(hTCP);

	// If new data is waiting
	if(MyTCBStub.sslRxHead != MyTCBStub.rxHead)
	{
		// Reconfigure pointers for SSL use
		prevRxTail = MyTCBStub.rxTail;
		nextRxHead = MyTCBStub.rxHead;
		MyTCBStub.rxTail = MyTCBStub.rxHead;
		MyTCBStub.rxHead = MyTCBStub.sslRxHead;
		
		do
		{
			startRxTail = MyTCBStub.rxTail;

			// Handle incoming data.  This function performs deframing of the 
			// SSL records, decryption, and MAC verification.
			wSSLBytesThatPoofed = TCPIsGetReady(hTCP);
			wDecryptedBytes = SSLRxRecord(hTCP, MyTCBStub.sslStubID);
			wSSLBytesThatPoofed -= TCPIsGetReady(hTCP);

			// Now need to move data to fill the SSL header/MAC/padding hole, 
			// if there is one
			if(wSSLBytesThatPoofed)
			{	
				// Sync the TCP so we can see if there is a TCP hole
				SyncTCB();

				// Calculate how big the SSL hole is
				if(MyTCB.sHoleSize == -1)
				{// Just need to move pending SSL data
					wToMove = TCPIsGetReady(hTCP);
				}
				else
				{// A TCP hole exists, so move all data
					wToMove = TCPIsGetReady(hTCP) + MyTCB.sHoleSize + MyTCB.wFutureDataSize;
				}
				
				// Start with the destination as the startRxTail and source as current rxTail
				wDest = startRxTail;
				wSrc = MyTCBStub.rxTail;
				
				// If data exists between the end of the buffer and 
				// the destination, then move it forward
				if(wSrc > wDest)
				{
					wLen = MyTCBStub.bufferEnd - wSrc + 1;
					if(wLen > wToMove)
						wLen = wToMove;
					TCPRAMCopy(wDest, MyTCBStub.vMemoryMedium, 
							   wSrc, MyTCBStub.vMemoryMedium, wLen);
					wDest += wLen;
					wSrc = MyTCBStub.bufferRxStart;
					wToMove -= wLen;
				}
				
				// If data remains to be moved, fill in to end of buffer
				if(wToMove)
				{
					wLen = MyTCBStub.bufferEnd - wDest + 1;
					if(wLen > wToMove)
						wLen = wToMove;
					TCPRAMCopy(wDest, MyTCBStub.vMemoryMedium, 
							   wSrc, MyTCBStub.vMemoryMedium, wLen);
					wDest = MyTCBStub.bufferRxStart;
					wSrc += wLen;
					wToMove -= wLen;
				}
				
				// If data still remains, copy from from front + len to front
				if(wToMove)
				{
					TCPRAMCopy(wDest, MyTCBStub.vMemoryMedium,
							   wSrc, MyTCBStub.vMemoryMedium, wToMove);
				}

				// Since bytes poofed, we need to move the head pointers 
				// backwards by an equal amount.
				MyTCBStub.rxHead -= wSSLBytesThatPoofed;
				if(MyTCBStub.rxHead < MyTCBStub.bufferRxStart)
					MyTCBStub.rxHead += MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;
				MyTCBStub.sslRxHead = MyTCBStub.rxHead;
			}
				
			// Move tail pointer forward by the number of decrypted bytes ready 
			// for the application (but not poofed bytes)
			MyTCBStub.rxTail = startRxTail + wDecryptedBytes;
			if(MyTCBStub.rxTail > MyTCBStub.bufferEnd)
				MyTCBStub.rxTail -= MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;
			nextRxHead += wDecryptedBytes;
			
			// Loop until SSLRxRecord() runs out of data and stops doing 
			// anything
		} while(wSSLBytesThatPoofed || (startRxTail != MyTCBStub.rxTail));

		// Restore TCP buffer pointers to point to the decrypted application data 
		// only
		if(nextRxHead > MyTCBStub.bufferEnd)
			nextRxHead -= MyTCBStub.bufferEnd - MyTCBStub.bufferRxStart + 1;
		MyTCBStub.rxTail = prevRxTail;
		MyTCBStub.rxHead = nextRxHead;
	}
}	
#endif


#endif //#if defined(STACK_USE_TCP)
