
#ifndef _MYIPNETWORK_C_
#define _MYIPNETWORK_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))

#include "defines.h"
#include "TCPIP Stack/TCPIP.h"
#include "MyIpData.h"



//////////////////////////
// Defines
#define TX_BUFFER_SIZE					(50)
#define NUM_SOCKETS						(sizeof(MyTelemetry)/sizeof(MyTelemetry[0]))

#define TCP_TELEMETRY_CONNECT_TIMEOUT	((TICK_SECOND*5)
#define UDP_TELEMETRY_CONNECT_TIMEOUT	((TICK_SECOND*5)



//////////////////////////
// Enums
typedef enum //BYTE
{
	eSM_DO_NOTHING =0,
	eSM_HOME,
	eSM_SOCKET_OBTAINED,
	eSM_CONNECTED,
} eTelemetryState;

typedef enum //BYTE
{
	eTCP = 0,
	eUDP,
} eMyDataType;

struct
{
	BYTE buffer[TX_BUFFER_SIZE];		// circular buffer data
	eTelemetryState state;				// state machine
	int buffer_head;					// circular buffer index head (increments on loading)
	int buffer_tail;					// circular buffer index tail (incremented on sending data)
	DWORD connectTimer;					// time we started a new IP connection, used to reset state machine
	int connectRetries;					// keep track of connection retry attempts
	BYTE socket;						// used as either TCP_SOCKET or UDP_SOCKET typedef
    BOOL foundEOL;						// Keep fillign packet until we find EndOfLine then send the packet

	eSource source;						// enum eSource, usually eSourceUART2 for MavLink/_Extra telemetry
	eMyDataType type;					// either eTCP or eUDP
	char* serverIP;						// for isServer=FALSE this is the server IP we connect to. For isServer=TRUE use NULL
	WORD port;							// IP port for server or client. Recommend TCP(23) for Telnet and UDP(14550) for qGroundControl
} MyTelemetry[] =
{
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eTCP, NULL, 23},				// TCP Server, listening on port 23, use this for telnet
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eTCP, "192.168.11.104", 24},	// TCP Client connecting to 192.168.11.104:23, use this for HK-GCS
	{ {},0,0,0,0,0,0,0,		eSourceUART2,eUDP, NULL,14550},				// UDP Server, listening on port 14550, Use this for qGroundControl or HK-GCS
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eUDP, "192.168.11.104", 14550},	// UDP Client connecting to 192.168.11.104:14550, Use this for HK-GCS
//	{ {},0,0,0,0,0,0,0,		eSourceUART2,eTCP, "username.dynDns.com", 23}, // You can even use URLs!
};
// ** HUMAN ERROR WARNING!!!
// Check that there are enough TCP_PURPOSE_TELEMETRY slots in TCPIPConfig.h for as many eTCP sockets you have


//////////////////////////
// Variables


//////////////////////////////////////
// Local Functions
int ThreadSafe_ReadBufferHead(BYTE s);
BOOL ThreadSafe_EOLcheck(BYTE s, BOOL doClearFlag);
void SendAsyncTxData_Bulk(BYTE s);
//void SendAsyncTxData_Single(BYTE s);
void ProcessIncomingData(BYTE rxData, eSource src);


// This is called from within an tnterrupt (i.e. _U2TXInterrupt) when sending data.
// In the case of UART it takes a copy of the outgoing byte and loads it into a circular buffer
// which can later be asynchonously read in the idle thread to transmit it
void LoadNetworkAsyncTxBuffer(BYTE data, eSource src)
{
	BYTE s;
	
	for (s = 0; s < NUM_SOCKETS; s++)
	{
		// selectively load the sockets with routed data instead of loading them all with the same data.
		// This allows one socket to be UART data and another socket to be from somewhere else (i.e. OSD?)
		if (src == MyTelemetry[s].source)
		{
			MyTelemetry[s].buffer_head++;
			if (MyTelemetry[s].buffer_head >= TX_BUFFER_SIZE)
			{
				MyTelemetry[s].buffer_head = 0;
			}
			MyTelemetry[s].buffer[MyTelemetry[s].buffer_head] = data;
			
			if ('\n' == data)
			{
				MyTelemetry[s].foundEOL = TRUE;
			}
		} // if UARTdata
	} // for s
}
		

// init telemetry variables and states
void InitTelemetry(void)
{
	int i;
	BYTE s; // socket index
	DWORD tick = TickGet();
	
	for (s = 0; s < NUM_SOCKETS; s++)
	{
		if (MyTelemetry[s].type == eTCP)
			MyTelemetry[s].socket = INVALID_SOCKET;
		else
			MyTelemetry[s].socket = INVALID_UDP_SOCKET;
			
		MyTelemetry[s].foundEOL = FALSE;
		MyTelemetry[s].state = eSM_HOME;
		MyTelemetry[s].buffer_head = 0;
		MyTelemetry[s].buffer_tail = 0;
		MyTelemetry[s].connectTimer = tick;
		MyTelemetry[s].connectRetries = 0;
		for (i = 0; i < TX_BUFFER_SIZE; i++)
		{
			MyTelemetry[s].buffer[i] = 0;
		}
		
		// **** do not intialize these, they are done at the
		//  struct declaration and only shown here for completeness
		//MyTelemetry[s].source
		//MyTelemetry[s].port
		//MyTelemetry[s].serverIP
		//MyTelemetry[s].type
	} // for s
}

// Read the circular buffer head index (written to from _U2TXInterrupt) from
// the "idle thread" in a thread-safe manner.
int ThreadSafe_ReadBufferHead(BYTE s)
{
	BYTE isrState;
	int head;
	
	if (s >= NUM_SOCKETS)
		return 0;
	
	switch (MyTelemetry[s].source)
	{
	case eSourceUART2:
		isrState = _U2TXIE;
		_U2TXIE = 0; // inhibit the UART2 ISR from loading more data for a moment (protect _head reads)
		head = MyTelemetry[s].buffer_head;
		_U2TXIE = isrState; // resume ISR
		break;
		
	case eSourceUART1:
	case eSourcePLANE_TO_PLANE:
	case eSourceCAMERA:
	case eSourceOSD:
	default:
		head = 0;
		// future ideas??????
		break;
	}	
	return head;
}

// Read the End-Of-Line flag (set in _U2TXInterrupt) and clear it from
// the "idle thread" in a thread-safe manner
BOOL ThreadSafe_EOLcheck(BYTE s, BOOL doClearFlag)
{
	BYTE isrState;
	BOOL eolFound;
	
	if (s >= NUM_SOCKETS)
		return FALSE;

	switch (MyTelemetry[s].source)
	{
	case eSourceUART2:
		isrState = _U2TXIE;
		_U2TXIE = 0; // inhibit the UART2 ISR from changing this on us during a read
		eolFound = MyTelemetry[s].foundEOL;
		if (doClearFlag)
		{
			MyTelemetry[s].foundEOL = FALSE;
		}
		_U2TXIE = isrState; // resume ISR
		break;
		
	case eSourceUART1:
	case eSourcePLANE_TO_PLANE:
	case eSourceCAMERA:
	case eSourceOSD:
	default:
		eolFound = FALSE;
		break;
	}
	return eolFound;
}

// Service the Telemetry system by checking for a TCP connection
// and then sending/recieveing data from the network accordingly
void ServiceTCPTelemetry()
{
	BYTE s, rxData;
	static BOOL tcpSocketOpenErrorHasBeenShown = FALSE;
	
	for (s = 0; s < NUM_SOCKETS; s++)
	{
		if (eTCP != MyTelemetry[s].type)
			continue;
		
		// Handle session state
		switch(MyTelemetry[s].state)
		{
			case eSM_DO_NOTHING:
				break;
				
			case eSM_HOME:
				if (NULL == MyTelemetry[s].serverIP)
				{
					// We are the server, start listening
					MyTelemetry[s].socket = TCPOpen((DWORD)(PTR_BASE)0, TCP_OPEN_SERVER, MyTelemetry[s].port, TCP_PURPOSE_TELEMETRY);
				}
				else
				{
					// Client Mode: Connect a socket to the remote TCP server
					MyTelemetry[s].socket = TCPOpen((DWORD)(PTR_BASE)MyTelemetry[s].serverIP, TCP_OPEN_ROM_HOST, MyTelemetry[s].port, TCP_PURPOSE_TELEMETRY);
				}	
	
				// Abort operation if no TCP socket of type TCP_PURPOSE_TELEMETRY is available
				// If this ever happens, you need to go add one to TCPIPConfig.h
				if (INVALID_SOCKET == MyTelemetry[s].socket)
				{
					if (FALSE == tcpSocketOpenErrorHasBeenShown)
					{
						putrsUART((ROM char*)"\r\nERROR!!! Not enough TCP_PURPOSE_TELEMETRY entries, add them in TCPIPConfig.h");
						tcpSocketOpenErrorHasBeenShown = TRUE;	
					}
					break;
				}	
		
		
				// Eat the first TCPWasReset() response so we don't 
				// infinitely create and reset/destroy client mode sockets
				TCPWasReset(MyTelemetry[s].socket);
				MyTelemetry[s].state++;
				MyTelemetry[s].connectTimer = TickGet();
				break;
	
			case eSM_SOCKET_OBTAINED:	
				if (TCPIsPutReady(MyTelemetry[s].socket) >= 140)
				{
					// Print any one-time connection annoucement text
					TCPPutROMString(MyTelemetry[s].socket, "\r\nYou've connected to "); // 22 chars
					TCPPutROMString(MyTelemetry[s].socket, ID_LEAD_PILOT); // 15ish chars
					TCPPutROMString(MyTelemetry[s].socket, "'s aircraft. More info at "); // 26 chars
					TCPPutROMString(MyTelemetry[s].socket, ID_DIY_DRONES_URL); // 45ish chars
					TCPPutROMString(MyTelemetry[s].socket, "\r\n"); // 2 chars
					TCPFlush(MyTelemetry[s].socket); // send right away
					
					MyTelemetry[s].state++;
				}
				/*
				// TODO Fix this timeout mechanism. The problem is in the disconnect method
				else if ((NULL != MyTelemetry[s].serverIP) && // is a client
						(TickGet() - MyTelemetry[s].connectTimer) > TCP_TELEMETRY_CONNECT_TIMEOUT))	// is timed out
				{
					// if our DNS takes too long to resolve then lets try again
					// If we are a client socket, close the socket and attempt to reconnect
					TCPDisconnect(MyTelemetry[s].socket);
					TCPDisconnect(MyTelemetry[s].socket);
					MyTelemetry[s].socket = INVALID_SOCKET;
					MyTelemetry[s].state = eSM_HOME;
					MyTelemetry[s].connectRetries++;
				}
				*/
				break;
	
			case eSM_CONNECTED:
				if (TCPWasReset(MyTelemetry[s].socket))
				{
					if (NULL == MyTelemetry[s].serverIP)
					{
						// If we are a server socket, go back to listening
						MyTelemetry[s].state = eSM_SOCKET_OBTAINED;
					}
					else
					{
						// TODO Fix this disconnect mechanism
						// If we are a client socket, close the socket and attempt to reconnect
						TCPDisconnect(MyTelemetry[s].socket);
						TCPDisconnect(MyTelemetry[s].socket);
						MyTelemetry[s].socket = INVALID_SOCKET;
						MyTelemetry[s].state = eSM_HOME;
					}
					break;
				}

				SendAsyncTxData_Bulk(s); 		// fill IP packet via array writes (efficient and complicated)
				//SendAsyncTxData_Single(s);	// fill IP packet via repeated byte writes (slow and simple)
			
				if (ThreadSafe_EOLcheck(s,TRUE))
				{
					TCPFlush(MyTelemetry[s].socket);
				}
				
				// Process Incoming data
				while (TCPGet(MyTelemetry[s].socket, &rxData))
				{
					ProcessIncomingData(rxData, MyTelemetry[s].source);
				} // if get
				break;
		} // switch
	}	// for s
}

// Service the Telemetry system by checking for a TCP connection
// and then sending/recieveing data from the network accordingly
void ServiceUDPTelemetry()
{
	BYTE rxData;
	BYTE s;
	static BOOL udpSocketOpenErrorHasBeenShown = FALSE;
	
	//if (!MACIsLinked())
	//	return;
		
	for (s = 0; s < NUM_SOCKETS; s++)
	{
		if (eUDP != MyTelemetry[s].type)
			continue;
		
		if (!MACIsLinked())
		{
			// if not hooked up then there's nothing to do.
			MyTelemetry[s].socket = INVALID_UDP_SOCKET;
			MyTelemetry[s].state = eSM_DO_NOTHING;
		}
		else if (eSM_DO_NOTHING == MyTelemetry[s].state)
		{
			MyTelemetry[s].state = eSM_HOME;
		}	
			
			
		#if defined(STACK_USE_DHCP_CLIENT)
		// Wait until DHCP module is finished
		if ((eSM_DO_NOTHING != MyTelemetry[s].state) && !DHCPIsBound(0))
		{
			MyTelemetry[s].socket = INVALID_UDP_SOCKET;
			MyTelemetry[s].state = eSM_DO_NOTHING;
		}	
		else if (eSM_DO_NOTHING == MyTelemetry[s].state)
		{
			MyTelemetry[s].state = eSM_HOME;
		}	
		#endif
	
		switch(MyTelemetry[s].state)
		{
			case eSM_DO_NOTHING:
				break;
			
			case eSM_HOME:
				if (NULL == MyTelemetry[s].serverIP)
				{
					// Server mode, also known as SPAM mode
					// Set the socket's destination to be a broadcast over our IP 
					NODE_INFO Remote;
					memset(&Remote, 0xFF, sizeof(Remote));
		
					// Open a UDP socket for outbound transmission
					MyTelemetry[s].socket = UDPOpenEx((DWORD)(PTR_BASE)&Remote,UDP_OPEN_NODE_INFO,0,MyTelemetry[s].port); // client
				}
				else
				{
					// Client mode, connect to remote listening server.
					MyTelemetry[s].socket = UDPOpenEx((DWORD)(PTR_BASE)MyTelemetry[s].serverIP,UDP_OPEN_ROM_HOST,0,MyTelemetry[s].port);
				}
				
				// Abort operation if no UDP sockets are available
				// If this ever happens, incrementing MAX_UDP_SOCKETS in 
				// StackTsk.h may help (at the expense of more global memory 
				// resources).
				if (INVALID_UDP_SOCKET != MyTelemetry[s].socket)
				{
					MyTelemetry[s].state++;
					MyTelemetry[s].connectTimer = TickGet();
				}
				else
				{
					if (FALSE == udpSocketOpenErrorHasBeenShown)
					{
						putrsUART((ROM char*)"\r\nERROR!!! Not enough MAX_UDP_SOCKETS entries, increase them in TCPIPConfig.h");
						udpSocketOpenErrorHasBeenShown = TRUE;	
					}
				}
				break;
			
			case eSM_SOCKET_OBTAINED:
				if (UDPIsPutReady(MyTelemetry[s].socket) >= 140)
				{
						// Print any one-time connection annoucement text
						UDPPutROMString("\r\nYou've connected to "); // 22 chars
						UDPPutROMString(ID_LEAD_PILOT); // 15ish chars
						UDPPutROMString("'s aircraft. More info at "); // 26 chars
						UDPPutROMString(ID_DIY_DRONES_URL); // 45ish chars
						UDPPutROMString("\r\n"); // 2 chars
						UDPFlush(); // send right away
						
						MyTelemetry[s].state++;
				}
				/*
				else if ((NULL != MyTelemetry[s].serverIP) && // is a client
						(TickGet() - MyTelemetry[s].connectTimer) > UDP_TELEMETRY_CONNECT_TIMEOUT))	// is timed out
				{
					// if our DNS takes too long to resolve then lets try again
					// If we are a client socket, close the socket and attempt to reconnect
					UDPClose(MyTelemetry[s].socket);
					MyTelemetry[s].socket = INVALID_UDP_SOCKET;
					MyTelemetry[s].state = eSM_HOME;
					MyTelemetry[s].connectRetries++;
				}
				*/
				break;
	
			case eSM_CONNECTED:
				SendAsyncTxData_Bulk(s); 		// fill IP packet via array writes (efficient and complicated)
				//SendAsyncTxData_Single(s);	// fill IP packet via repeated byte writes (slow and simple)
	
				// Send the packet
				if (ThreadSafe_EOLcheck(s,TRUE))
				{
					UDPFlush();
				}	
				
				// Process Incoming data
				while (UDPGet(&rxData))
				{
					ProcessIncomingData(rxData, MyTelemetry[s].source);
				} // while get
				break;
		} // switch
	} // for s
}

void ProcessIncomingData(BYTE rxData, eSource src)
{
	BYTE isrState;
	
	switch (src)
	{
	case eSourceUART2:
		_U2RXIE = isrState;
		_U2RXIE = 0;
		udb_serial_callback_received_byte(rxData);
		_U2RXIE = isrState;
		break;
		
	case eSourceUART1:
	case eSourcePLANE_TO_PLANE:
	case eSourceCAMERA:
	case eSourceOSD:
	default:
		break;
	} // switch src
}	

// fill IP packet via array writes (efficient and complicated)
void SendAsyncTxData_Bulk(BYTE s)
{
	int wrote = 0;
	int tx_buffer_available = 0;
	int head, index, len;
	
	head = ThreadSafe_ReadBufferHead(s);
	if (head != MyTelemetry[s].buffer_tail)
	{
		if (eTCP == MyTelemetry[s].type)
		{
			tx_buffer_available = TCPIsPutReady(MyTelemetry[s].socket);
		}	
		else //if (eUDP == MyTelemetry[s].type)
		{
			tx_buffer_available = UDPIsPutReady(MyTelemetry[s].socket);
		}
	}
	
	// if we're not connected then IsPutReady will return 0. Send data if we can
	while ((MyTelemetry[s].buffer_tail != head) && (tx_buffer_available > 0))
	{
		index = MyTelemetry[s].buffer_tail + 1;
	
		if (MyTelemetry[s].buffer_tail < head) // usual (easy) case
		{
			len = head - MyTelemetry[s].buffer_tail;
			if (eTCP == MyTelemetry[s].type)
			{
				wrote = TCPPutArray(MyTelemetry[s].socket, &MyTelemetry[s].buffer[index], len);
			}
			else //if (eUDP == MyTelemetry[s].type)
			{
				wrote = UDPPutArray(&MyTelemetry[s].buffer[index], len);
			}
		}
		else // circular buffer is wrapping around
		{
			if (MyTelemetry[s].buffer_tail >= (TX_BUFFER_SIZE-1))
			{
				// only one byte before the end, just put it in by itself (single byte Put)
				if (eTCP == MyTelemetry[s].type)
				{
					wrote = TCPPut(MyTelemetry[s].socket, MyTelemetry[s].buffer[0]);
				}
				else //if (eUDP == MyTelemetry[s].type)
				{
					wrote = UDPPut(MyTelemetry[s].buffer[0]);
				}
			}
			else
			{
				// fill from tail to end-of-buffer, let the next pass get the lower 0-to-head portion
				len = TX_BUFFER_SIZE - MyTelemetry[s].buffer_tail - 1;
				if (eTCP == MyTelemetry[s].type)
				{
					wrote = TCPPutArray(MyTelemetry[s].socket, &MyTelemetry[s].buffer[index], len);
				}
				else //if (eUDP == MyTelemetry[s].type)
				{
					wrote = UDPPutArray(&MyTelemetry[s].buffer[index], len);
				}
			}
		}
		
		if (wrote)
		{
			tx_buffer_available -= wrote;
			MyTelemetry[s].buffer_tail += wrote;
			if (MyTelemetry[s].buffer_tail >= TX_BUFFER_SIZE)
			{
				MyTelemetry[s].buffer_tail = 0;
			}	
		}
		else
		{
			// something is wrong, we tried to write and failed so lets stop trying
			tx_buffer_available = 0;
		}
		head = ThreadSafe_ReadBufferHead(s); // refresh
	} // while
}

/*
// fill IP packet via repeated byte writes (slow and simple)
void SendAsyncTxData_Single(BYTE s)
{
	int head, index;
	BYTE txData;
	
	// This is only needed for UDP to set the active socket
	if (eUDP == MyTelemetry[s].type)
	{
		if (FALSE == UDPIsPutReady(MyTelemetry[s].socket))
			return;
	}

	head = ThreadSafe_ReadBufferHead(s);
	while (MyTelemetry[s].buffer_tail != head) // if there's data to write
	{
		index = MyTelemetry[s].buffer_tail + 1;
		if (index >= TX_BUFFER_SIZE)
		{
			index = 0;
		}

		txData = MyTelemetry[s].buffer[index];
		// write if we can
		if (eTCP == MyTelemetry[s].type)
		{
			if (FALSE == TCPPut(MyTelemetry[s].socket, txData))
				return;
		}
		else if (eUDP == MyTelemetry[s].type)
		{
			// socket set by the above UDPIsPutReady() call
			if (FALSE == UDPPut(txData))
				return;
		}

		// write was successful, increment indexes
		MyTelemetry[s].buffer_tail = index;
		// refresh the head index in case new data was just added
		head = ThreadSafe_ReadBufferHead(s);
	} // while
}
*/

	
#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPNETWORK_C_


