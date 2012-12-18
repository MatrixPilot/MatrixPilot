/*********************************************************************
 *
 *     Berekely Socket Distribution API Source File
 *
 *********************************************************************
 * FileName:        BerkeleyAPI.c
 * Description:     Berkeley Socket Distribution(BSD) APIs for Microchip TCPIP Stack
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
 * Author               Date    	Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Aseem Swalah         4/3/08  	Original
 ********************************************************************/

#include "TCPIPConfig.h"

#if defined(STACK_USE_BERKELEY_API)

#include "TCPIP Stack/TCPIP.h"

static BOOL HandlePossibleTCPDisconnection(SOCKET s);


#if defined(__18CXX) && !defined(HI_TECH_C)	
	#pragma udata BSD_uRAM
#endif
// Array of BSDSocket elements; used to track all socket state and connection information.
static struct BSDSocket  BSDSocketArray[BSD_SOCKET_COUNT];
#if defined(__18CXX) && !defined(HI_TECH_C)	
	#pragma udata
#endif

// Contains the next local port number to associate with a socket.
static WORD gAutoPortNumber = 1024;


/*****************************************************************************
  Function:
	void BerkeleySocketInit(void)

  Summary:
	Initializes the Berkeley socket structure array.

  Description:
	This function initializes the Berkeley socket array. This function should
	be called before any BSD socket call.

  Precondition:
	None.

  Parameters:
	None.

  Returns:
	None

  Remarks:
	None.
  ***************************************************************************/
void BerkeleySocketInit(void)
{
	unsigned int s;
	struct BSDSocket *socket;

	for ( s = 0; s < BSD_SOCKET_COUNT; s++ )
	{
		socket             = (struct BSDSocket *)&BSDSocketArray[s];
		socket->bsdState   = SKT_CLOSED;
		socket->SocketID = INVALID_UDP_SOCKET;
	}
}

/*****************************************************************************
  Function:
	SOCKET socket( int af, int type, int protocol )

  Summary:
	This function creates a new Berkeley socket.

  Description:
	This function creates a new BSD socket for the microchip
	TCPIP stack. The return socket descriptor is used for the subsequent
	BSD operations.

  Precondition:
	BerkeleySocketInit function should be called.

  Parameters:
	af - address family - AF_INET.
	type - socket type SOCK_DGRAM or SOCK_STREAM.
	protocol - IP protocol IPPROTO_UDP or IPPROTO_TCP.

  Returns:
	New socket descriptor. INVALID_SOCKET in case of error.

  Remarks:
	None.
  ***************************************************************************/
SOCKET socket( int af, int type, int protocol )
{
	struct BSDSocket *socket = BSDSocketArray;
	SOCKET s;

	if( af != AF_INET )
		return INVALID_SOCKET;

	if(protocol == IPPROTO_IP)		
	{
		switch(type)
		{
			case SOCK_DGRAM:
				protocol = IPPROTO_UDP;
			break;

			case SOCK_STREAM:
				protocol = IPPROTO_TCP;
			break;
			
			default:
			break;
		}
	}

	for( s = 0; s < BSD_SOCKET_COUNT; s++,socket++ )
	{
		if( socket->bsdState != SKT_CLOSED ) //socket in use
			continue;

		socket->SocketType = type;

		if( type == SOCK_DGRAM && protocol == IPPROTO_UDP )
		{
			socket->bsdState = SKT_CREATED;
			return s;
		}
		else if( type == SOCK_STREAM && protocol == IPPROTO_TCP )
		{
			socket->bsdState = SKT_CREATED;
			return s;
		}
		else
			return INVALID_SOCKET;
	}

	return INVALID_SOCKET;
}

/*****************************************************************************
  Function:
	int bind( SOCKET s, const struct sockaddr* name, int namelen )

  Summary:
	This function assigns a name to the socket descriptor.

  Description:
	The bind function assigns a name to an unnamed socket. The
    name represents the local address of the communication
    endpoint. For sockets of type SOCK_STREAM, the name of the
    remote endpoint is assigned when a connect or accept function
    is executed.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	name - pointer to the sockaddr structure containing the
	local address of the socket.
	namelen - length of the sockaddr structure.

  Returns:
	If bind is successful, a value of 0 is returned. A return
    value of SOCKET_ERROR indicates an error.

  Remarks:
	None.
  ***************************************************************************/
int bind( SOCKET s, const struct sockaddr* name, int namelen )
{
	struct BSDSocket *socket;
	struct sockaddr_in *local_addr;
	WORD lPort;

	if( s >= BSD_SOCKET_COUNT )
		return SOCKET_ERROR;

	socket = &BSDSocketArray[s];

	if( socket->bsdState != SKT_CREATED ) //only work with recently created socket
		return SOCKET_ERROR;

	if( (unsigned int)namelen < sizeof(struct sockaddr_in) )
		return SOCKET_ERROR;

	local_addr = (struct sockaddr_in *)name;

	lPort = local_addr->sin_port;
    if( lPort == 0u ) //pick a port
	{
		lPort = gAutoPortNumber++;
        if(gAutoPortNumber > 5000u) // reset the port numbers
			gAutoPortNumber = 1024;
	}

	if(socket->SocketType == SOCK_DGRAM)
	{
		socket->SocketID = UDPOpenEx(0,UDP_OPEN_SERVER,lPort, 0);
		if(socket->SocketID == INVALID_UDP_SOCKET)
			return SOCKET_ERROR;
	}

	socket->localPort = lPort;
	socket->bsdState  = SKT_BOUND;
	return 0; //success
}

/*****************************************************************************
  Function:
	int listen( SOCKET s, int backlog )

  Summary:
	The listen function sets the specified socket in a listen mode

  Description:
	This function sets the specified socket in a listen
	mode. Calling the listen function indicates that the
	application is ready to accept connection requests arriving
	at a socket of type SOCK_STREAM. The connection request is
	queued (if possible) until accepted with an accept function.
	The backlog parameter defines the maximum number of pending
	connections that may be queued.

  Precondition:
	bind() must have been called on the s socket first.

  Parameters:
	s - Socket identifier returned from a prior socket() call.
	backlog - Maximum number of connection requests that can be queued.  Note 
		that each backlog requires a TCP_PURPOSE_BERKELEY_SERVER type TCP 
		socket to be allocated in the TCPSocketInitializer[] in TCPIPConfig.h.
		Also, ensure that BSD_SOCKET_COUNT (also in TCPIPConfig.h) is greater 
		than the backlog by at least 1 (more if you have other BSD sockets in 
		use).
	
  Returns:
	Returns 0 on success, else return SOCKET_ERROR.

  Remarks:
	None
  ***************************************************************************/
int listen( SOCKET s, int backlog )
{
	struct BSDSocket *ps;
	SOCKET clientSockID;
	unsigned int socketcount;
	unsigned char assigned;

	if( s >= BSD_SOCKET_COUNT )
		return SOCKET_ERROR;

	ps = &BSDSocketArray[s];

	if(ps->SocketType != SOCK_STREAM)
		return SOCKET_ERROR;

	if(ps->bsdState == SKT_BSD_LISTEN)
		backlog = ps->backlog;

	if((ps->bsdState != SKT_BOUND) && (ps->bsdState != SKT_BSD_LISTEN))
		return SOCKET_ERROR;

	while(backlog--)
	{
		assigned = 0;
		for(socketcount = 0; socketcount < BSD_SOCKET_COUNT; socketcount++)
		{
			if(BSDSocketArray[socketcount].bsdState != SKT_CLOSED)
				continue;

			clientSockID = TCPOpen(0, TCP_OPEN_SERVER, ps->localPort, TCP_PURPOSE_BERKELEY_SERVER);
			if(clientSockID == INVALID_SOCKET)
				return SOCKET_ERROR;
				
			// Clear the first reset flag
			TCPWasReset(clientSockID);

			assigned = 1;
			ps->bsdState = SKT_BSD_LISTEN;
			ps->backlog = backlog;

			BSDSocketArray[socketcount].SocketID = clientSockID;
			BSDSocketArray[socketcount].bsdState = SKT_LISTEN;
			BSDSocketArray[socketcount].isServer = TRUE;
			BSDSocketArray[socketcount].localPort = ps->localPort;
			BSDSocketArray[socketcount].SocketType = SOCK_STREAM;
			break;
		}
		if(!assigned)
			return SOCKET_ERROR;
	}
	return 0; //Success
}


/*****************************************************************************
  Function:
	SOCKET accept(SOCKET s, struct sockaddr* addr, int* addrlen)

  Summary:
	This function accepts connection requests queued for a listening socket.

  Description:
	The accept function is used to accept connection requests
	queued for a listening socket. If a connection request is
	pending, accept removes the request from the queue, and a new
	socket is created for the connection. The original listening
	socket remains open and continues to queue new connection
	requests. The socket must be a SOCK_STREAM type socket.

  Precondition:
	listen function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to
	socket. must be bound to a local name and in listening mode.
	addr - Optional pointer to a buffer that receives the address
	of the connecting entity.
	addrlen - Optional pointer to an integer that contains the
	length of the address addr

  Returns:
	If the accept function succeeds, it returns a non-negative
	integer that is a descriptor for the accepted socket.
	Otherwise, the value INVALID_SOCKET is returned.

  Remarks:
	None.
  ***************************************************************************/
SOCKET accept(SOCKET s, struct sockaddr* addr, int* addrlen)
{
	struct BSDSocket *pListenSock;
	SOCKET_INFO *remoteSockInfo;
	struct sockaddr_in *addrRemote;
	unsigned int sockCount;
	TCP_SOCKET hTCP;

	if( s >= BSD_SOCKET_COUNT )
		return INVALID_SOCKET;

	pListenSock = &BSDSocketArray[s]; /* Get the pointer to listening server socket */

	if ( pListenSock->bsdState != SKT_BSD_LISTEN )
		return INVALID_SOCKET;
	if ( pListenSock->SocketType != SOCK_STREAM )
		return INVALID_SOCKET;

	for(sockCount = 0; sockCount < BSD_SOCKET_COUNT; sockCount++)
	{
		if(BSDSocketArray[sockCount].bsdState != SKT_LISTEN)
			continue;

		if(BSDSocketArray[sockCount].localPort != pListenSock->localPort)
			continue;

		hTCP = BSDSocketArray[sockCount].SocketID;
		
		// We don't care about connections and disconnections before we can 
		// process them, so clear the reset flag
		TCPWasReset(hTCP);	
		
		if(TCPIsConnected(hTCP))
		{
			remoteSockInfo = TCPGetRemoteInfo(hTCP);
			if(addr)
			{
				if(addrlen)
				{
					if((unsigned int)*addrlen < sizeof(struct sockaddr_in))
						return INVALID_SOCKET;
					addrRemote = (struct sockaddr_in *)addr;
					addrRemote->sin_addr.S_un.S_addr = remoteSockInfo->remote.IPAddr.Val;
					addrRemote->sin_port = remoteSockInfo->remotePort.Val;
					*addrlen = sizeof(struct sockaddr_in);
				}
			}
			BSDSocketArray[sockCount].remotePort = remoteSockInfo->remotePort.Val;
			BSDSocketArray[sockCount].remoteIP   = remoteSockInfo->remote.IPAddr.Val;
			BSDSocketArray[sockCount].bsdState = SKT_EST;
			return sockCount;
		}
	}

	return INVALID_SOCKET;
}

/*****************************************************************************
  Function:
	int connect( SOCKET s, struct sockaddr* name, int namelen )

  Summary:
	This function connects to the peer communications end point.

  Description:
	The connect function assigns the address of the peer
	communications endpoint. For stream sockets, connection is
	established between the endpoints. For datagram sockets, an
	address filter is established between the endpoints until
	changed with another connect() function.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	name - pointer to the sockaddr structure containing the
	peer address and port number.
	namelen - length of the sockaddr structure.

  Returns:
	If the connect() function succeeds, it returns 0. Otherwise,
	the value SOCKET_ERROR is returned to indicate an error
	condition. For stream based socket, if the connection is not
	established yet, connect returns SOCKET_CNXN_IN_PROGRESS.

  Remarks:
	None.
  ***************************************************************************/
int connect( SOCKET s, struct sockaddr* name, int namelen )
{
	struct BSDSocket *socket;
	struct sockaddr_in *addr;
	DWORD remoteIP;
	WORD remotePort;
	WORD localPort;

	if( s >= BSD_SOCKET_COUNT )
		return SOCKET_ERROR;

	socket = &BSDSocketArray[s];

	if( socket->bsdState < SKT_CREATED )
		return SOCKET_ERROR;

	if( (unsigned int)namelen < sizeof(struct sockaddr_in))
		return SOCKET_ERROR;

	addr = (struct sockaddr_in *)name;
	remotePort 	= addr->sin_port;
	remoteIP 	= addr->sin_addr.S_un.S_addr;

	if( remoteIP == 0u || remotePort == 0u )
		return SOCKET_ERROR;

	if( socket->SocketType == SOCK_STREAM )
	{
		switch(socket->bsdState)
		{
		case SKT_EST:
			return 0; // already established

		case SKT_IN_PROGRESS:
			if(HandlePossibleTCPDisconnection(s))
				return SOCKET_ERROR;

			if(!TCPIsConnected(socket->SocketID))
				return SOCKET_CNXN_IN_PROGRESS;

			socket->bsdState = SKT_EST;
			return 0; //success

		case SKT_CREATED:
		case SKT_BOUND:
			socket->SocketID = TCPOpen(remoteIP, TCP_OPEN_IP_ADDRESS, remotePort, TCP_PURPOSE_BERKELEY_CLIENT);
			if(socket->SocketID == INVALID_SOCKET)
				return SOCKET_ERROR;

			// Clear the first reset flag
			TCPWasReset(socket->SocketID);

			socket->isServer = FALSE;
			socket->bsdState = SKT_IN_PROGRESS;
			return SOCKET_CNXN_IN_PROGRESS;

		default:
			return SOCKET_ERROR;
		}
	}
	else
	{
		// If not explicitly bound to a local port, implicitly do the binding
		if(socket->bsdState == SKT_CREATED)
		{
			localPort = gAutoPortNumber++;
	        if(gAutoPortNumber > 5000u) // reset the port numbers
				gAutoPortNumber = 1024;

			socket->SocketID = UDPOpenEx(0,UDP_OPEN_SERVER,localPort, remotePort);
			if(socket->SocketID == INVALID_UDP_SOCKET)
				return SOCKET_ERROR;
			socket->bsdState = SKT_BOUND;
		}
		if(socket->bsdState != SKT_BOUND)
			return SOCKET_ERROR;

		// UDP: remote port is used as a filter. Need to call connect when using
		// send/recv calls. No need to call 'connect' if using sendto/recvfrom 
		// calls.
		socket->remotePort = remotePort;
		socket->remoteIP = remoteIP;
		return 0; //success
	}
	return SOCKET_ERROR;
}

/*****************************************************************************
  Function:
	int send( SOCKET s, const char* buf, int len, int flags )
	
  Summary:
	The send function is used to send outgoing data on an already
	connected socket.

  Description:
	The send function is used to send outgoing data on an already
	connected socket. This function is used to send a reliable,
	ordered stream of data bytes on a socket of type SOCK_STREAM
	but can also be used to send datagrams on a socket of type SOCK_DGRAM.

  Precondition:
	connect function should be called for TCP and UDP sockets.
	Server side, accept function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data buffer containing data to transmit.
	len - length of data in bytes.
	flags - message flags. Currently this field is not supported.

  Returns:
	On success, send returns number of bytes sent. In case of
	error, returns SOCKET_ERROR. a zero indicates no data send.

  Remarks:
	None.
  ***************************************************************************/
int send( SOCKET s, const char* buf, int len, int flags )
{
	return sendto(s, buf, len, flags, NULL, 0);
}

/*****************************************************************************
  Function:
	int sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)

  Summary:
	This function used to send the data for both connection oriented and connection-less
	sockets.

  Description:
	The sendto function is used to send outgoing data on a socket.
	The destination address is given by to and tolen. Both 
	Datagram and stream sockets are supported.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data buffer containing data to transmit.
	len - length of data in bytes.
	flags - message flags. Currently this field is not supported.
	to - Optional pointer to the the sockaddr structure containing the
		destination address.  If NULL, the currently bound remote port and IP 
		address are used as the destination.
	tolen - length of the sockaddr structure.

  Returns:
	On success, sendto returns number of bytes sent. In case of
	error returns SOCKET_ERROR

  Remarks:
	None.
  ***************************************************************************/
int sendto( SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen )
{
	struct BSDSocket *socket;
	int size = SOCKET_ERROR;
	NODE_INFO remoteInfo;
	static DWORD startTick;		// NOTE: startTick really should be a per socket BSDSocket structure member since other BSD calls can interfere with the ARP cycles
	WORD wRemotePort;
	struct sockaddr_in local;

	if( s >= BSD_SOCKET_COUNT )
		return SOCKET_ERROR;

	socket = &BSDSocketArray[s];

	if(socket->bsdState == SKT_CLOSED)
		return SOCKET_ERROR;

	if(socket->SocketType == SOCK_DGRAM) //UDP
	{
		// Decide the destination IP address and port
		remoteInfo.IPAddr.Val = socket->remoteIP;
		wRemotePort = socket->remotePort;
		if(to)
		{
			if((unsigned int)tolen != sizeof(struct sockaddr_in))
				return SOCKET_ERROR;
			wRemotePort = ((struct sockaddr_in*)to)->sin_port;
			remoteInfo.IPAddr.Val = ((struct sockaddr_in*)to)->sin_addr.s_addr;
			
			// Implicitly bind the socket if it isn't already
			if(socket->bsdState == SKT_CREATED)
			{
				memset(&local, 0x00, sizeof(local));
				if(bind(s, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
					return SOCKET_ERROR;
			}
		}
		if(remoteInfo.IPAddr.Val == IP_ADDR_ANY)
			remoteInfo.IPAddr.Val = 0xFFFFFFFFu;

		// Set the remote IP and MAC address if it is different from what we already have stored in the UDP socket
		if(UDPSocketInfo[socket->SocketID].remote.remoteNode.IPAddr.Val != remoteInfo.IPAddr.Val)
		{
			if(ARPIsResolved(&remoteInfo.IPAddr, &remoteInfo.MACAddr))
			{
				memcpy((void*)&UDPSocketInfo[socket->SocketID].remote.remoteNode, (void*)&remoteInfo, sizeof(remoteInfo));
			}
			else
			{
				if(TickGet() - startTick > 1*TICK_SECOND)
				{
					ARPResolve(&remoteInfo.IPAddr);
					startTick = TickGet();
				}
				return SOCKET_ERROR;
			}
		}
		
		// Select the UDP socket and see if we can write to it
		if(UDPIsPutReady(socket->SocketID))
		{
			// Set the proper remote port
			UDPSocketInfo[socket->SocketID].remotePort = wRemotePort;

			// Write data and send UDP datagram
			size = UDPPutArray((BYTE*)buf, len);
			UDPFlush();
			return size;
		}
	}
	else if(socket->SocketType == SOCK_STREAM) //TCP will only send to the already established socket.
	{
		if(socket->bsdState != SKT_EST)
			return SOCKET_ERROR;

		if(HandlePossibleTCPDisconnection(s))
			return SOCKET_ERROR;
			
		// Handle special case were 0 return value is okay
		if(len == 0)
			return 0;

		// Write data to the socket. If one or more bytes were written, then 
		// return this value.  Otherwise, fail and return SOCKET_ERROR.
		size = TCPPutArray(socket->SocketID, (BYTE*)buf, len);
		if(size)
			return size;
	}
	return SOCKET_ERROR;
}

/*****************************************************************************
  Function:
	int recv( SOCKET s, char* buf, int len, int flags )

  Summary:
	The recv() function is used to receive incoming data that has
	been queued for a socket.

  Description:
	The recv() function is used to receive incoming data that has
	been queued for a socket. This function can be used with both 
	datagram and stream socket. If the available data
	is too large to fit in the supplied application buffer buf,
	excess bytes are discarded in case of SOCK_DGRAM type
	sockets.  For SOCK_STREAM types, the data is buffered
	internally so the application can retreive all data by
	multiple calls of recvfrom.

  Precondition:
	connect function should be called for TCP and UDP sockets.
	Server side, accept function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data receive buffer.
	len - buffer length in bytes.
	flags - no significance in this implementation

  Returns:
	If recv is successful, the number of bytes copied to
	application buffer buf is returned. A value of zero indicates
	no data available. A return value of SOCKET_ERROR (-1)
	indicates an error condition. A return value of SOCKET_DISCONNECTED
	indicates the connection no longer exists.

  Remarks:
	None.
  ***************************************************************************/
int recv( SOCKET s, char* buf, int len, int flags )
{
	struct BSDSocket *socket;

	if( s >= BSD_SOCKET_COUNT )
		return SOCKET_ERROR;

	socket = &BSDSocketArray[s];

	if(socket->SocketType == SOCK_STREAM) //TCP
	{
		if(socket->bsdState != SKT_EST)
			return SOCKET_ERROR;

		if(HandlePossibleTCPDisconnection(s))
			return SOCKET_ERROR;

		return TCPGetArray(socket->SocketID, (BYTE*)buf, len);
	}
	else if(socket->SocketType == SOCK_DGRAM) //UDP
	{
		if(socket->bsdState != SKT_BOUND)
			return SOCKET_ERROR;

		if(UDPIsGetReady(socket->SocketID))
			return UDPGetArray((BYTE*)buf, len);
	}

	return 0;
}

/*****************************************************************************
  Function:
	int recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)

  Summary:
	The recvfrom() function is used to receive incoming data that
	has been queued for a socket.

  Description:
	The recvfrom() function is used to receive incoming data that
	has been queued for a socket. This function can be used with
	both datagram and stream type sockets. If the available data
	is too large to fit in the supplied application buffer buf,
	excess bytes are discarded in case of SOCK_DGRAM type
	sockets. For SOCK_STREAM types, the data is buffered
	internally so the application can retreive all data by
	multiple calls of recvfrom.

  Precondition:
	socket function should be called.

  Parameters:
	s - Socket descriptor returned from a previous call to socket.
	buf - application data receive buffer.
	len - buffer length in bytes.
	flags - message flags. Currently this is not supported.
	from - pointer to the sockaddr structure that will be
	filled in with the destination address.
	fromlen - size of buffer pointed by from.

  Returns:
	If recvfrom is successful, the number of bytes copied to
	application buffer buf is returned. A value of zero indicates
	no data available. A return value of SOCKET_ERROR (-1)
	indicates an error condition.

  Remarks:
	None.
  ***************************************************************************/
int recvfrom( SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen )
{
	struct BSDSocket *socket;
	struct sockaddr_in *rem_addr;
	SOCKET_INFO *remoteSockInfo;

	socket = &BSDSocketArray[s];
	rem_addr = (struct sockaddr_in *)from;

	if(socket->SocketType == SOCK_DGRAM) //UDP
	{
		// If this BSD socket doesn't have a Microchip UDP socket associated 
		// with it yet, then no data can be received and we must not use the 
		// socket->SocketID parameter, which isn't set yet.
		if(socket->bsdState != SKT_BOUND)
			return 0;
		
		if(UDPIsGetReady(socket->SocketID))
		{
			// Capture sender information (can change packet to packet)
			if(from && fromlen)
			{
				if((unsigned int)*fromlen >= sizeof(struct sockaddr_in))
				{
					rem_addr->sin_addr.S_un.S_addr = UDPSocketInfo[socket->SocketID].remote.remoteNode.IPAddr.Val;
					rem_addr->sin_port = UDPSocketInfo[socket->SocketID].remotePort;
					*fromlen = sizeof(struct sockaddr_in);
				}
			}

			return UDPGetArray((BYTE*)buf, len);
		}
	}
	else  //TCP recieve from already connected socket.
	{
		if(from && fromlen)
		{
			// Capture sender information (will always match socket connection information)
			if((unsigned int)*fromlen >= sizeof(struct sockaddr_in))
			{
				remoteSockInfo = TCPGetRemoteInfo(socket->SocketID);
				rem_addr->sin_addr.S_un.S_addr = remoteSockInfo->remote.IPAddr.Val;
				rem_addr->sin_port = remoteSockInfo->remotePort.Val;
				*fromlen = sizeof(struct sockaddr_in);
			}
		}
		return recv(s, buf, len, 0);
	}
	return 0;
}

/*****************************************************************************
  Function:
	int gethostname(char* name, int namelen )

  Summary:
	Returns the standard host name for the system.

  Description:
	This function returns the standard host name of the system which is 
	calling this function.	The returned name is null-terminated.

  Precondition:
	None.

  Parameters:
	name - Pointer to a buffer that receives the local host name.
	namelen - size of the name array.

  Returns:
	Success will return a value of 0. 
	If name is too short to hold the host name or any other error occurs, 
	SOCKET_ERROR (-1) will be returned.  On error, *name will be unmodified 
	and no null terminator will be generated.

  Remarks:
	None.
  ***************************************************************************/
int gethostname(char* name, int namelen)
{
	WORD wSourceLen;
	WORD w;
	BYTE v;

	wSourceLen = sizeof(AppConfig.NetBIOSName);
	for(w = 0; w < wSourceLen; w++)
	{
		v = AppConfig.NetBIOSName[w];
		if((v == ' ') || (v == 0u))
			break;
	}
	wSourceLen = w;
	if(namelen < (int)wSourceLen + 1)
		return SOCKET_ERROR;

	memcpy((void*)name, (void*)AppConfig.NetBIOSName, wSourceLen);
	name[wSourceLen] = 0;

	return 0;
}

/*****************************************************************************
  Function:
	int closesocket( SOCKET s )
	
  Summary:
	The closesocket function closes an existing socket.

  Description:
	The closesocket function closes an existing socket.  
	This function releases the socket descriptor s.  
	Any data buffered at the socket is discarded.  If the 
	socket s is no longer needed, closesocket() must be 
	called in order to release all resources associated with s.

  Precondition:
	None.

  Parameters:
	s - Socket descriptor returned from a previous call to socket

  Returns:
	If closesocket is successful, a value of 0 is returned. 
	A return value of SOCKET_ERROR (-1) indicates an error.

  Remarks:
	None.
  ***************************************************************************/
int closesocket( SOCKET s )
{	
	BYTE i;
	struct BSDSocket *socket;

	if( s >= BSD_SOCKET_COUNT )
		return SOCKET_ERROR;

	socket = &BSDSocketArray[s];

	if(socket->bsdState == SKT_CLOSED)
		return 0;	// Nothing to do, so return success

	if(socket->SocketType == SOCK_STREAM)
	{
		if(socket->bsdState == SKT_BSD_LISTEN)
		{
			// This is a listerner handle, so when we close it we also should 
			// close all TCP sockets that were opened for backlog processing 
			// but didn't actually get connected
			for(i = 0; i < sizeof(BSDSocketArray)/sizeof(BSDSocketArray[0]); i++)
			{
				if(BSDSocketArray[i].bsdState != SKT_LISTEN)
					continue;
				if(BSDSocketArray[i].localPort == socket->localPort)
				{
					TCPClose(BSDSocketArray[i].SocketID);
					BSDSocketArray[i].bsdState = SKT_CLOSED;
				}
			}
		}
		else if(socket->bsdState >= SKT_LISTEN)
		{
			// For server sockets, if the parent listening socket is still open, 
			// then return this socket to the queue for future backlog processing.
			if(socket->isServer)
			{
				for(i = 0; i < sizeof(BSDSocketArray)/sizeof(BSDSocketArray[0]); i++)
				{
					if(BSDSocketArray[i].bsdState != SKT_BSD_LISTEN)
						continue;
					if(BSDSocketArray[i].localPort == socket->localPort)
					{
						TCPDisconnect(socket->SocketID);
						
						// Listener socket is still open, so just return to the 
						// listening state so that the user must call accept() again to 
						// reuse this BSD socket
						socket->bsdState = SKT_LISTEN;
						return 0;
					}
				}
				// If we get down here, then the parent listener socket has 
				// apparently already been closed, so this socket can not be 
				// reused.  Close it complete.
				TCPClose(socket->SocketID);
			}
			else if(socket->bsdState != SKT_DISCONNECTED)	// this is a client socket that isn't already disconnected
			{
				TCPClose(socket->SocketID);
			}
		}
	}
	else //udp sockets
	{
		if(socket->bsdState == SKT_BOUND)
			UDPClose(socket->SocketID);
	}

	socket->bsdState = SKT_CLOSED;
	return 0; //success
}


/*****************************************************************************
  Function:
	static BOOL HandlePossibleTCPDisconnection(SOCKET s)
	
  Summary:
	Internal function that checks for asynchronous TCP connection state 
	changes and resynchs the BSD socket descriptor state to match. 

  Description:
	Internal function that checks for asynchronous TCP connection state 
	changes and resynchs the BSD socket descriptor state to match. 

  Precondition:
	None

  Parameters:
	s - TCP type socket descriptor returned from a previous call to socket.  
	    This socket must be in the SKT_LISTEN, SKT_IN_PROGRESS, SKT_EST, or 
	    SKT_DISCONNECTED states.

  Returns:
	TRUE - Socket is disconnected
	FALSE - Socket is 

  ***************************************************************************/
static BOOL HandlePossibleTCPDisconnection(SOCKET s)
{
	struct BSDSocket *socket;
	BYTE i;
	BOOL bSocketWasReset;

	socket = &BSDSocketArray[s];

	// Nothing to do if disconnection has already been handled
	if(socket->bsdState == SKT_DISCONNECTED)
		return TRUE;	

	// Find out if a disconnect has occurred
	bSocketWasReset = TCPWasReset(socket->SocketID);

	// For server sockets, if the parent listening socket is still open, 
	// then return this socket to the queue for future backlog processing.
	if(socket->isServer)
	{
		for(i = 0; i < sizeof(BSDSocketArray)/sizeof(BSDSocketArray[0]); i++)
		{
			if(BSDSocketArray[i].bsdState != SKT_BSD_LISTEN)
				continue;
			if(BSDSocketArray[i].localPort == socket->localPort)
			{
				// Nothing to do if a disconnect has not occurred
				if(!bSocketWasReset)
					return FALSE;

				// Listener socket is still open, so just return to the 
				// listening state so that the user must call accept() again to 
				// reuse this BSD socket
				socket->bsdState = SKT_LISTEN;
				return TRUE;
			}
		}
	}
			
	// If we get down here and the socket was reset, then this socket 
	// should be closed so that no more clients can connect to it.  However, 
	// we can't go to the BSD SKT_CLOSED state directly since the user still 
	// has to call closesocket() with this s SOCKET descriptor first.
	if(bSocketWasReset)
	{
		TCPClose(socket->SocketID);
		socket->bsdState = SKT_DISCONNECTED;
		return TRUE;
	}
	
	return FALSE;
}

#endif //STACK_USE_BERKELEY_API

