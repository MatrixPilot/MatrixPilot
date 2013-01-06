#ifndef _MYIPUART2_C_
#define _MYIPUART2_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_UART2 == 1)

#include "TCPIP Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpUART2.h"

//////////////////////////////////////
// Local Functions

//////////////////////////
// Module Variables


void MyIpOnConnect_UART2(BYTE s)
{
	// Print any one-time connection annoucement text
	LoadStringSocket(s, "\r\nYou've connected to UART2 on "); // 31 chars
	LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
	LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
	LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
	LoadStringSocket(s, "\r\n"); // 2 chars
	MyIpData[s].sendPacket = TRUE; // send right away
}


void MyIpInit_UART2(BYTE s)
{
}

void MyIpService_UART2(BYTE s)
{
	// Nothing to do here, it's all done in ISRs
}

BOOL MyIpThreadSafeSendPacketCheck_UART2(BYTE s, BOOL doClearFlag)
{
	BYTE isrState;
	BOOL sendpacket;
	
	isrState = _U2TXIE;
	_U2TXIE = 0; // inhibit the UART2 ISR from changing this on us during a read
	sendpacket = MyIpData[s].sendPacket;
	if (doClearFlag)
	{
		MyIpData[s].sendPacket = FALSE;
	}
	_U2TXIE = isrState; // resume ISR
	return sendpacket;
}


int MyIpThreadSafeReadBufferHead_UART2(BYTE s)
{
	BYTE isrState;
	int head;
	
	isrState = _U2TXIE;
	_U2TXIE = 0; // inhibit the UART2 ISR from loading more data for a moment (protect _head reads)
	head = MyIpData[s].buffer_head;
	_U2TXIE = isrState; // resume ISR

	return head;
}

void MyIpProcessRxData_UART2(BYTE s)
{
	// make sure the socket is for us
	//if (MyTelemetry[s].source != eSourceUART2)
	//	return;

	BYTE isrState, rxData;
	BOOL successfulRead;
	
	do
	{
		if (MyIpData[s].type == eTCP)
		{
			successfulRead = TCPGet(MyIpData[s].socket, &rxData);
		}
		else //if (MyTelemetry[s].type == eUDP)
		{
			successfulRead = UDPGet(&rxData);
		}
		
		if (successfulRead)
		{
			isrState = _U2RXIE;
			_U2RXIE = 0;
			udb_serial_callback_received_byte(rxData);
			_U2RXIE = isrState;
		}	
	} while (successfulRead);
}

	
#endif // (NETWORK_USE_UART2 == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPUART2_C_

