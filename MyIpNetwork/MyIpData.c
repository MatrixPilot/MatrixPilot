
#ifndef _MYIPDATA_C_
#define _MYIPDATA_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))

#include "defines.h"
#include "TCPIP Stack/TCPIP.h"
#include "MyIpData.h"

#if (NETWORK_USE_UART1 == 1)
#include "MyIpUART1.h"
#endif
#if (NETWORK_USE_UART2 == 1)
#include "MyIpUART2.h"
#endif
#if (NETWORK_USE_FLYBYWIRE == 1)
#include "MyIpFlyByWire.h"
#endif
#if (NETWORK_USE_MAVLINK == 1)
#include "MyIpMAVLink.h"
#endif
#if (NETWORK_USE_DEBUG == 1)
#include "MyIpDebug.h"
#endif




//////////////////////////
// Variables
MyIpDataType MyIpData[] =
{
	#if (NETWORK_USE_UART1 == 1)
	{ {},0,0,0,0,0,0,0,		eSourceUART1,eTCP, NULL, 24},				// TCP Server, listening on port 23, use this for telnet
	#endif
	#if (NETWORK_USE_UART2 == 1)
	//{ {},0,0,0,0,0,0,0,		eSourceUART2,eUDP, "192.168.11.100", 3001},	// UDP Client connecting to 192.168.11.104:23, use this for HK-GCS
	{ {},0,0,0,0,0,0,0,			eSourceUART2,eTCP, NULL, 23},			// TCP Server
	//{ {},0,0,0,0,0,0,0,		eSourceUART2,eTCP, "192.168.11.100", 3002},	// TCP Client connecting to 192.168.11.104:23, use this for HK-GCS
	#endif
	#if (NETWORK_USE_FLYBYWIRE == 1)
	//{ {},0,0,0,0,0,0,0,		eSourceFlyByWire,eTCP, "192.168.11.100", 3003},	// UDB FlyByWire
	{ {},0,0,0,0,0,0,0,		eSourceFlyByWire,eTCP, "192.168.11.100", 3003},	// UDB FlyByWire
	//{ {},0,0,0,0,0,0,0,		eSourceFlyByWire,eTCP, NULL, 3003},	// UDB FlyByWire
	#endif
	#if (NETWORK_USE_MAVLINK == 1)
	{ {},0,0,0,0,0,0,0,		eSourceMAVLink,eUDP, "192.168.11.100", 3004},	// UDB MAV Link stream
	#endif
	#if (NETWORK_USE_DEBUG == 1)
	{ {},0,0,0,0,0,0,0,		eSourceDebug,eTCP, NULL, 25},					// Telnet server
	#endif
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eTCP, NULL, 23},				// TCP Server, listening on port 23, use this for telnet
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eTCP, "192.168.11.100", 24},	// TCP Client connecting to 192.168.11.100:23, use this for HK-GCS
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eUDP, NULL,14550},				// UDP Server, listening on port 14550, Use this for qGroundControl or HK-GCS
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eUDP, "192.168.11.100", 14550},	// UDP Client connecting to 192.168.11.100:14550, Use this for HK-GCS
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eTCP, "username.DynDNS.com", 23}, // You can even use URLs!
};
// ** HUMAN ERROR WARNING!!!
// Check that there are enough TCP_PURPOSE_TELEMETRY slots in TCPIPConfig.h for as many eTCP sockets you have


//////////////////////////////////////
// Local Functions
int MyIpThreadSafeReadBufferHead(BYTE s);
BOOL MyIpThreadSafeEOLcheck(BYTE s, BOOL doClearFlag);
void SendAsyncTxData_Bulk(BYTE s);
void MyIpProcessRxData(BYTE s);
BYTE Get_TCP_PURPOSE(eSource src);
void MyIpOnConnect(BYTE s);


unsigned int NumSockets(void)
{
	// This constant is not able to be global, a worker function is needed.
	return NUM_SOCKETS;
}

BOOL MyIpIsConnectedSocket(BYTE s)
{
	//if (s >= NumSockets())
	//	return FALSE;

	return (eSM_CONNECTED == MyIpData[s].state);
}
BOOL MyIpIsConnectedSrc(eSource src)
{
	BYTE s;
	BOOL result = FALSE;
	
	for (s = 0; s < NumSockets(); s++)
	{
		if (src == MyIpData[s].source)
		{
			result |= MyIpIsConnectedSocket(s);
		}	
	}
	return result;
}


void LoadStringSocket(BYTE s, char* buf)
{
	//if (s >= NumSockets())
	//	return;
	while (*buf) { LoadNetworkAsyncTxBufferSocket(s, *buf++); }	
}
void LoadStringSrc(eSource src, char* buf)
{
	while (*buf) { LoadNetworkAsyncTxBufferSrc(src, *buf++); }	
}

void LoadPrintSrc(eSource src, unsigned long data, unsigned char spacing)
{
	BYTE s;
	for (s = 0; s < NumSockets(); s++)
	{
		if (src == MyIpData[s].source)
		{
			LoadPrintSocket(s, data, spacing);
		}
	} // for		
}
void LoadPrintSocket(BYTE s, unsigned long data, unsigned char spacing)
{
	// This looks ugly but it only consumes 278 bytes of ROM.
	// TODO: remove this function and convert whoever calls it to use vsnprintf()
	
	//if (s >= NumSockets())
	//	return;

	//32 bit
	if ((data > 0xFFFF) || (spacing >= 6))
	{
		if (data >= 1000000000)	LoadNetworkAsyncTxBufferSocket(s, 48+( data/1000000000));
		else if (spacing == 10) LoadNetworkAsyncTxBufferSocket(s, '0');
		if (data >= 100000000) LoadNetworkAsyncTxBufferSocket(s, 48+((data%1000000000)/100000000));
		else if (spacing >= 9) LoadNetworkAsyncTxBufferSocket(s, '0');
		if (data >= 10000000) LoadNetworkAsyncTxBufferSocket(s, 48+((data%100000000)/10000000));
		else if (spacing >= 8) LoadNetworkAsyncTxBufferSocket(s, '0');
		if (data >= 1000000) LoadNetworkAsyncTxBufferSocket(s, 48+((data%10000000)/1000000));
		else if (spacing >= 7) LoadNetworkAsyncTxBufferSocket(s, '0');
		if (data >= 100000) LoadNetworkAsyncTxBufferSocket(s, 48+((data%1000000)/100000));
		else if (spacing >= 6) LoadNetworkAsyncTxBufferSocket(s, '0');
	}
	
	//16 bit
	if ((data > 0xFF) || (spacing >= 4))
	{
		if (data >= 10000) LoadNetworkAsyncTxBufferSocket(s, 48+((data%100000)/10000));
		else if (spacing >= 5) LoadNetworkAsyncTxBufferSocket(s, '0');
		if (data >= 1000) LoadNetworkAsyncTxBufferSocket(s, 48+((data%10000)/1000));
		else if (spacing >= 4) LoadNetworkAsyncTxBufferSocket(s, '0');
	}	
	
	//8 bit
	if (data >= 100) LoadNetworkAsyncTxBufferSocket(s, 48+((data%1000)/100));
	else if (spacing >= 3) LoadNetworkAsyncTxBufferSocket(s, '0');
	if (data >= 10) LoadNetworkAsyncTxBufferSocket(s, 48+((data%100)/10));
	else if (spacing >= 2) LoadNetworkAsyncTxBufferSocket(s, '0');
	
	LoadNetworkAsyncTxBufferSocket(s, 48+(data%10));
	
}

// This is sometimes called from within an interrupt (i.e. _U2TXInterrupt) when sending data.
// In the case of UART2 it takes a copy of the outgoing byte and loads it into a circular buffer
// which will later be asynchonously read in the idle thread to transmit it
void LoadNetworkAsyncTxBufferSrc(eSource src, BYTE data)
{
	BYTE s;
	
	for (s = 0; s < NumSockets(); s++)
	{
		// selectively load the sockets with routed data instead of loading them all with the same data.
		if (src == MyIpData[s].source)
		{
			LoadNetworkAsyncTxBufferSocket(s, data);
		} // if 
	} // for s
}

void LoadNetworkAsyncTxBufferSocket(BYTE s, BYTE data)
{
	//if (s >= NumSockets())
	//	return;
		
	MyIpData[s].buffer_head++;
	if (MyIpData[s].buffer_head >= TX_BUFFER_SIZE)
	{
		MyIpData[s].buffer_head = 0;
	}
	MyIpData[s].buffer[MyIpData[s].buffer_head] = data;
}	

// init telemetry variables and states
void InitMyIpData(void)
{
	int i;
	BYTE s; // socket index
	DWORD tick = TickGet();
	
	for (s = 0; s < NumSockets(); s++)
	{
		if (MyIpData[s].type == eTCP)
			MyIpData[s].socket = INVALID_SOCKET;
		else
			MyIpData[s].socket = INVALID_UDP_SOCKET;
			
		MyIpData[s].foundEOL = FALSE;
		MyIpData[s].state = eSM_HOME;
		MyIpData[s].buffer_head = 0;
		MyIpData[s].buffer_tail = 0;
		MyIpData[s].connectTimer = tick;
		MyIpData[s].connectRetries = 0;
		for (i = 0; i < TX_BUFFER_SIZE; i++)
		{
			MyIpData[s].buffer[i] = 0;
		}
		
		// **** do not intialize these, they are done at the MyIpData[]
		//  struct declaration and only shown here for completeness
		//MyIpData[s].source = user set
		//MyIpData[s].port = user set
		//MyIpData[s].serverIP = user set
		//MyIpData[s].type = user set
	} // for s
	
	#if (NETWORK_USE_UART1 == 1)
		MyIpInit_UART1();
	#endif
	#if (NETWORK_USE_UART2 == 1)
		MyIpInit_UART2();
	#endif
	#if (NETWORK_USE_FLYBYWIRE == 1)
		MyIpInit_FlyByWire();
	#endif
	#if (NETWORK_USE_MAVLINK == 1)
		MyIpInit_MAVLink();
	#endif
	#if (NETWORK_USE_DEBUG == 1)
		MyIpInit_Debug();
	#endif
}

// Read the circular buffer head index (written to from _U2TXInterrupt) from
// the "idle thread" in a thread-safe manner.
int MyIpThreadSafeReadBufferHead(BYTE s)
{
	//if (s >= NumSockets())
	//	return 0;

	int head;
	
	switch (MyIpData[s].source)
	{
	#if (NETWORK_USE_UART1 == 1)
	case eSourceUART1:
		head = MyIpThreadSafeReadBufferHead_UART1(s);
		break;
	#endif
	
	#if (NETWORK_USE_UART2 == 1)
	case eSourceUART2:
		head = MyIpThreadSafeReadBufferHead_UART2(s);
		break;
	#endif
	
	#if (NETWORK_USE_FLYBYWIRE == 1)
	case eSourceFlyByWire:
		head = MyIpThreadSafeReadBufferHead_FlyByWire(s);
		break;
	#endif

	#if (NETWORK_USE_MAVLINK == 1)
	case eSourceMAVLink:
		head = MyIpThreadSafeReadBufferHead_MAVLink(s);
		break;
	#endif
	
	#if (NETWORK_USE_DEBUG == 1)
	case eSourceDebug:
		head = MyIpThreadSafeReadBufferHead_Debug(s);
		break;
	#endif
	
	default:
		head = 0;
		break;
	}	
	return head;
}

void MyIpSetEOLflagSrc(eSource src)
{
	BYTE s;
	
	for (s = 0; s < NumSockets(); s++)
	{
		if (src == MyIpData[s].source)
		{
			MyIpData[s].foundEOL = TRUE;
		}	
	} // for
}

void MyIpSetEOLflagSocket(BYTE s)
{
	//if (s >= NumSockets())
	//	return;
	MyIpData[s].foundEOL = TRUE;
}


// Read the End-Of-Line flag (set in _U2TXInterrupt) and clear it from
// the "idle thread" in a thread-safe manner
BOOL MyIpThreadSafeEOLcheck(BYTE s, BOOL doClearFlag)
{
	//if (s >= NumSockets())
	//	return FALSE;

	BOOL eolFound;
	
	switch (MyIpData[s].source)
	{
	#if (NETWORK_USE_UART1 == 1)
	case eSourceUART1:
		eolFound = MyIpThreadSafeEOLcheck_UART1(s, doClearFlag);
		break;
	#endif

	#if (NETWORK_USE_UART2 == 1)
	case eSourceUART2:
		eolFound = MyIpThreadSafeEOLcheck_UART2(s, doClearFlag);
		break;
	#endif

	#if (NETWORK_USE_FLYBYWIRE == 1)
	case eSourceFlyByWire:
		eolFound = MyIpThreadSafeEOLcheck_FlyByWire(s, doClearFlag);
		break;
	#endif

	#if (NETWORK_USE_MAVLINK == 1)
	case eSourceMAVLink:
		eolFound = MyIpThreadSafeEOLcheck_MAVLink(s, doClearFlag);
		break;
	#endif

	#if (NETWORK_USE_DEBUG == 1)
	case eSourceDebug:
		eolFound = MyIpThreadSafeEOLcheck_Debug(s, doClearFlag);
		break;
	#endif

	default:
		eolFound = TRUE; // default TRUE so we trigger a send of whatever it is
		break;
	} // switch src
	
	return eolFound;
}

void ServiceMyIpData(BYTE s)
{
	#if (NETWORK_USE_UART1 == 1)
	if (eSourceUART1 == MyIpData[s].source)
	{
		MyIpService_UART1(s);
	}	
	#endif

	#if (NETWORK_USE_UART2 == 1)
	if (eSourceUART2 == MyIpData[s].source)
	{
		MyIpService_UART2(s);
	}	
	#endif

	#if (NETWORK_USE_FLYBYWIRE == 1)
	if (eSourceFlyByWire == MyIpData[s].source)
	{
		MyIpService_FlyByWire(s);
	}	
	#endif

	#if (NETWORK_USE_MAVLINK == 1)
	if (eSourceMAVLink == MyIpData[s].source)
	{
		MyIpService_MAVLink(s);
	}	
	#endif

	#if (NETWORK_USE_DEBUG == 1)
	if (eSourceDebug == MyIpData[s].source)
	{
		MyIpService_Debug(s);
	}	
	#endif
}

BYTE Get_TCP_PURPOSE(eSource src)
{
	switch (src)
	{
		case eSourceUART1: return TCP_PURPOSE_MYIPDATA_UART1;
		case eSourceUART2: return TCP_PURPOSE_MYIPDATA_UART2;
		case eSourceFlyByWire: return TCP_PURPOSE_MYIPDATA_FLYBYWIRE;
		case eSourceMAVLink: return TCP_PURPOSE_MYIPDATA_MAVLINK;
		case eSourceDebug: return TCP_PURPOSE_MYIPDATA_DEBUG;
		default: return TCP_PURPOSE_MYIPDATA_CUSTOM;
	}	
}	
// Service the Telemetry system by checking for a TCP connection
// and then sending/recieveing data from the network accordingly
void ServiceMyIpTCP(BYTE s)
{
	//if (s >= NumSockets())
	//	return;

	BYTE TCPpurpose;
	static BOOL tcpSocketOpenErrorHasBeenShown = FALSE;
	
	if (eTCP != MyIpData[s].type)
		return;
	
	// Handle session state
	switch(MyIpData[s].state)
	{
		case eSM_DO_NOTHING:
			break;
			
		case eSM_HOME:
			TCPpurpose = Get_TCP_PURPOSE(MyIpData[s].source);
			
			if (NULL == MyIpData[s].serverIP)
			{
				// We are the server, start listening
				MyIpData[s].socket = TCPOpen((DWORD)(PTR_BASE)0, TCP_OPEN_SERVER, MyIpData[s].port, TCPpurpose);
			}
			else
			{
				// Client Mode: Connect a socket to the remote TCP server
				MyIpData[s].socket = TCPOpen((DWORD)(PTR_BASE)MyIpData[s].serverIP, TCP_OPEN_ROM_HOST, MyIpData[s].port, TCPpurpose);
			}	

			// Abort operation if no TCP socket of type TCP_PURPOSE_MYIPDATA_xxxxxx is available
			// If this ever happens, you need to go add one to TCPIPConfig.h
			if (INVALID_SOCKET == MyIpData[s].socket)
			{
				if (FALSE == tcpSocketOpenErrorHasBeenShown)
				{
					putrsUART((ROM char*)"\r\nERROR!!! Not enough TCP_PURPOSE_MYIPDATA_ entries of type ");
					putcUART('0' + (TCPpurpose / 10));
					putcUART('0' + (TCPpurpose % 10));
					putrsUART((ROM char*)", add more entries in TCPIPConfig.h\r\n");
					tcpSocketOpenErrorHasBeenShown = TRUE;	
				}
				break;
			}
	
	
			// Eat the first TCPWasReset() response so we don't 
			// infinitely create and reset/destroy client mode sockets
			TCPWasReset(MyIpData[s].socket);
			MyIpData[s].state++;
			MyIpData[s].connectTimer = TickGet();
			break;

		case eSM_SOCKET_OBTAINED:	
			if (TCPIsPutReady(MyIpData[s].socket))
			{
				MyIpData[s].state++;
				MyIpOnConnect(s);
			}
			/*
			// TODO Fix this timeout mechanism. The problem is probably in the disconnect method
			else if ((NULL != MyIpData[s].serverIP) && // is a client
					(TickGet() - MyIpData[s].connectTimer) > TCP_TELEMETRY_CONNECT_TIMEOUT))	// is timed out
			{
				// if our DNS takes too long to resolve then lets try again
				// If we are a client socket, close the socket and attempt to reconnect
				TCPDisconnect(MyIpData[s].socket);
				TCPDisconnect(MyIpData[s].socket);
				MyIpData[s].socket = INVALID_SOCKET;
				MyIpData[s].state = eSM_HOME;
				MyIpData[s].connectRetries++;
			}
			*/
			break;

		case eSM_CONNECTED:
			if (TCPWasReset(MyIpData[s].socket))
			{
				if (NULL == MyIpData[s].serverIP)
				{
					// If we are a server socket, go back to listening
					MyIpData[s].state = eSM_SOCKET_OBTAINED;
				}
				else
				{
					// TODO Fix this disconnect mechanism
					// If we are a client socket, close the socket and attempt to reconnect
					TCPDisconnect(MyIpData[s].socket);
					TCPDisconnect(MyIpData[s].socket);
					MyIpData[s].socket = INVALID_SOCKET;
					MyIpData[s].state = eSM_HOME;
				}
				break;
			}

			SendAsyncTxData_Bulk(s); 		// fill IP packet via array writes (efficient and complicated)
			//SendAsyncTxData_Single(s);	// fill IP packet via repeated byte writes (slow and simple)
		
			if (MyIpThreadSafeEOLcheck(s,TRUE))
			{
				TCPFlush(MyIpData[s].socket);
			}
			
			// Process Incoming data
			MyIpProcessRxData(s);
			break;
	} // switch
}


// Service the Telemetry system by checking for a TCP connection
// and then sending/recieveing data from the network accordingly
void ServiceMyIpUDP(BYTE s)
{
	//if (s >= NumSockets())
	//	return;

	if (eUDP != MyIpData[s].type)
		return;

	if (!MACIsLinked())
	{
		// if not hooked up then there's nothing to do.
		MyIpData[s].socket = INVALID_UDP_SOCKET;
		MyIpData[s].state = eSM_DO_NOTHING;
	}
#if defined(STACK_USE_DHCP_CLIENT)
	// Wait until DHCP module is finished
	else if ((eSM_DO_NOTHING != MyIpData[s].state) && !DHCPIsBound(0))
	{
		MyIpData[s].socket = INVALID_UDP_SOCKET;
		MyIpData[s].state = eSM_DO_NOTHING;
	}
#endif
	else if (eSM_DO_NOTHING == MyIpData[s].state)
	{
		MyIpData[s].state = eSM_HOME;
	}	
		
		

	switch(MyIpData[s].state)
	{
		case eSM_DO_NOTHING:
			break;
		
		case eSM_HOME:
			if (NULL == MyIpData[s].serverIP)
			{
				// Server mode, also known as SPAM mode
				// Set the socket's destination to be a broadcast over our IP 
				NODE_INFO Remote;
				memset(&Remote, 0xFF, sizeof(Remote));
	
				// Open a UDP socket for outbound transmission
				MyIpData[s].socket = UDPOpenEx((DWORD)(PTR_BASE)&Remote,UDP_OPEN_NODE_INFO,0,MyIpData[s].port); // client
			}
			else
			{
				// Client mode, connect to remote listening server.
				MyIpData[s].socket = UDPOpenEx((DWORD)(PTR_BASE)MyIpData[s].serverIP,UDP_OPEN_ROM_HOST,0,MyIpData[s].port);
			}
			
			// Abort operation if no UDP sockets are available
			// If this ever happens, incrementing MAX_UDP_SOCKETS in 
			// StackTsk.h may help (at the expense of more global memory 
			// resources).
			if (INVALID_UDP_SOCKET != MyIpData[s].socket)
			{
				MyIpData[s].state++;
				MyIpData[s].connectTimer = TickGet();
			}
			else
			{
				// Since the UDP sockets themselves are connectionless, they get shared. This will happen
				// at boot while the other modules like DHCP, SNTP, NetBIOS and others are sharing resources.
				// If the port never gets a socket, increment MAX_UDP_SOCKETS.
				//putrsUART((ROM char*)"\r\nERROR!!! Not enough MAX_UDP_SOCKETS entries, increase them in TCPIPConfig.h");
			}
			break;
		
		case eSM_SOCKET_OBTAINED:
			if (UDPIsPutReady(MyIpData[s].socket))
			{
				MyIpData[s].state++;
				MyIpOnConnect(s);
			}
			/*
			else if ((NULL != MyIpData[s].serverIP) && // is a client
					(TickGet() - MyIpData[s].connectTimer) > UDP_TELEMETRY_CONNECT_TIMEOUT))	// is timed out
			{
				// if our DNS takes too long to resolve then lets try again
				// If we are a client socket, close the socket and attempt to reconnect
				UDPClose(MyIpData[s].socket);
				MyIpData[s].socket = INVALID_UDP_SOCKET;
				MyIpData[s].state = eSM_HOME;
				MyIpData[s].connectRetries++;
			}
			*/
			break;

		case eSM_CONNECTED:
			SendAsyncTxData_Bulk(s); 		// fill IP packet via array writes (efficient and complicated)
			//SendAsyncTxData_Single(s);	// fill IP packet via repeated byte writes (slow and simple)

			// Send the packet
			if (MyIpThreadSafeEOLcheck(s,TRUE))
			{
				UDPFlush();
			}	
			
			// Process Incoming data
			MyIpProcessRxData(s);
			break;
	} // switch
}

void MyIpOnConnect(BYTE s)
{
	//if (s >= NumSockets())
	//	return;

	switch (MyIpData[s].source)
	{
	#if (NETWORK_USE_UART1 == 1)
	case eSourceUART1:
		MyIpOnConnect_UART1(s);
		break;
	#endif
		
	#if (NETWORK_USE_UART2 == 1)
	case eSourceUART2:
		MyIpOnConnect_UART2(s);
		break;
	#endif
		
	#if (NETWORK_USE_FLYBYWIRE == 1)
	case eSourceFlyByWire:
		MyIpOnConnect_FlyByWire(s);
		break;
	#endif

	#if (NETWORK_USE_MAVLINK == 1)
	case eSourceMAVLink:
		MyIpOnConnect_MAVLink(s);
		break;
	#endif

	#if (NETWORK_USE_DEBUG == 1)
	case eSourceDebug:
		MyIpOnConnect_Debug(s);
		break;
	#endif
	
	default:
		break;
	} // switch source
}

void MyIpProcessRxData(BYTE s)
{
	//if (s >= NumSockets())
	//	return;

	switch (MyIpData[s].source)
	{
	#if (NETWORK_USE_UART1 == 1)
	case eSourceUART1:
		MyIpProcessRxData_UART1(s);
		break;
	#endif
		
	#if (NETWORK_USE_UART2 == 1)
	case eSourceUART2:
		MyIpProcessRxData_UART2(s);
		break;
	#endif
		
	#if (NETWORK_USE_FLYBYWIRE == 1)
	case eSourceFlyByWire:
		MyIpProcessRxData_FlyByWire(s);
		break;
	#endif

	#if (NETWORK_USE_MAVLINK == 1)
	case eSourceMAVLink:
		MyIpProcessRxData_MAVLink(s);
		break;
	#endif

	#if (NETWORK_USE_DEBUG == 1)
	case eSourceDebug:
		MyIpProcessRxData_Debug(s);
		break;
	#endif
	
	default:
		break;
	} // switch source
}	

// fill IP packet via array writes (efficient and complicated)
void SendAsyncTxData_Bulk(BYTE s)
{
	//if (s >= NumSockets())
	//	return;

	int wrote = 0;
	int tx_buffer_available = 0;
	int head, index, len;
	
	head = MyIpThreadSafeReadBufferHead(s);
	if (head != MyIpData[s].buffer_tail)
	{
		if (eTCP == MyIpData[s].type)
		{
			tx_buffer_available = TCPIsPutReady(MyIpData[s].socket);
		}	
		else //if (eUDP == MyIpData[s].type)
		{
			tx_buffer_available = UDPIsPutReady(MyIpData[s].socket);
		}
	}
	
	// if we're not connected then IsPutReady will return 0. Send data if we can
	while ((MyIpData[s].buffer_tail != head) && (tx_buffer_available > 0))
	{
		index = MyIpData[s].buffer_tail + 1;
	
		if (MyIpData[s].buffer_tail < head) // usual (easy) case
		{
			len = head - MyIpData[s].buffer_tail;
			if (eTCP == MyIpData[s].type)
			{
				wrote = TCPPutArray(MyIpData[s].socket, &MyIpData[s].buffer[index], len);
			}
			else //if (eUDP == MyIpData[s].type)
			{
				wrote = UDPPutArray(&MyIpData[s].buffer[index], len);
			}
		}
		else // circular buffer is wrapping around
		{
			if (MyIpData[s].buffer_tail >= (TX_BUFFER_SIZE-1))
			{
				// only one byte before the end, just put it in by itself (single byte Put)
				if (eTCP == MyIpData[s].type)
				{
					wrote = TCPPut(MyIpData[s].socket, MyIpData[s].buffer[0]);
				}
				else //if (eUDP == MyIpData[s].type)
				{
					wrote = UDPPut(MyIpData[s].buffer[0]);
				}
			}
			else
			{
				// fill from tail to end-of-buffer, let the next pass get the lower 0-to-head portion
				len = TX_BUFFER_SIZE - MyIpData[s].buffer_tail - 1;
				if (eTCP == MyIpData[s].type)
				{
					wrote = TCPPutArray(MyIpData[s].socket, &MyIpData[s].buffer[index], len);
				}
				else //if (eUDP == MyIpData[s].type)
				{
					wrote = UDPPutArray(&MyIpData[s].buffer[index], len);
				}
			}
		}
		
		if (wrote)
		{
			tx_buffer_available -= wrote;
			MyIpData[s].buffer_tail += wrote;
			if (MyIpData[s].buffer_tail >= TX_BUFFER_SIZE)
			{
				MyIpData[s].buffer_tail = 0;
			}	
		}
		else
		{
			// something is wrong, we tried to write and failed so lets stop trying
			tx_buffer_available = 0;
		}
		head = MyIpThreadSafeReadBufferHead(s); // refresh
	} // while
}

/*
// fill IP packet via repeated byte writes (slow and simple)
void SendAsyncTxData_Single(BYTE s)
{
	//if (s >= NumSockets())
	//	return;

	int head, index;
	BYTE txData;
	
	// This is only needed for UDP to set the active socket
	if (eUDP == MyIpData[s].type)
	{
		if (FALSE == UDPIsPutReady(MyIpData[s].socket))
			return;
	}

	head = MyIpThreadSafeReadBufferHead(s);
	while (MyIpData[s].buffer_tail != head) // if there's data to write
	{
		index = MyIpData[s].buffer_tail + 1;
		if (index >= TX_BUFFER_SIZE)
		{
			index = 0;
		}

		txData = MyIpData[s].buffer[index];
		// write if we can
		if (eTCP == MyIpData[s].type)
		{
			if (FALSE == TCPPut(MyIpData[s].socket, txData))
				return;
		}
		else if (eUDP == MyIpData[s].type)
		{
			// socket set by the above UDPIsPutReady() call
			if (FALSE == UDPPut(txData))
				return;
		}

		// write was successful, increment indexes
		MyIpData[s].buffer_tail = index;
		// refresh the head index in case new data was just added
		head = MyIpThreadSafeReadBufferHead(s);
	} // while
}
*/
	
#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPDATA_C_


