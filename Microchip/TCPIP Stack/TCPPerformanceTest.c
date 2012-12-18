/*********************************************************************
 *
 *	TCP Performance Test
 *  Module for Microchip TCP/IP Stack
 *	 -Establishes a connection and then sends out dummy packets 
 *	  from ROM memory
 *	 -Reference: None.  This is for testing only.
 *
 *********************************************************************
 * FileName:        TCPPerformanceTest.c
 * Dependencies:    TCP
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
 * Howard Schlunder     01/29/07	Original
 ********************************************************************/
#define __TCPPERFORMANCETEST_C

#include "TCPIPConfig.h"

#if defined(STACK_USE_TCP_PERFORMANCE_TEST)

#include "TCPIP Stack/TCPIP.h"


// The TCP port to listen on for TCP transmit tests
#define TX_PERFORMANCE_PORT	9762

// The TCP port to listen on for TCP receive tests
#define RX_PERFORMANCE_PORT	9763

void TCPTXPerformanceTask(void);
void TCPRXPerformanceTask(void);

/*****************************************************************************
  Function:
	void TCPPerformanceTask(void)

  Summary:
	Tests the performance of the TCP module.

  Description:
	This function calls both TCPTXPerformanceTask and TCPRXPerformanceTask
	to perform the performance task functions.  Refer to the documentation
	for each of those functions for details.

  Precondition:
	TCP is initialized.

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
void TCPPerformanceTask(void)
{
	TCPTXPerformanceTask();
	TCPRXPerformanceTask();
}

/*****************************************************************************
  Function:
	void TCPTXPerformanceTask(void)

  Summary:
	Tests the transmit performance of the TCP module.

  Description:
	This function tests the transmit performance of the TCP module.  To use,
	open a telnet connection to the device on TX_PERFORMANCE_PORT (9762 by 
	default).  The board will rapidly transmit data and report its performance
	to the telnet client.
	
	TCP performance is affected by many factors, including round-trip time 
	and the TCP buffer size.  For faster results, increase the size of the 
	TX buffer size for the TCP_PURPOSE_TCP_PERFORMANCE_TX socket in 
	TCPIPConfig.h.  Round-trip time is affected by the distance to the 
	device, so across the desk will be orders of magnitude faster than 
	across the Internet.
	
	This function is particularly useful after development to determine the
	impact of your application code on the stack's performance.  A before and
	after comparison will indicate if your application is unacceptably
	blocking the processor or taking too long to execute.

  Precondition:
	TCP is initialized.

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
void TCPTXPerformanceTask(void)
{
	static TCP_SOCKET MySocket = INVALID_SOCKET;
	static DWORD dwTimeStart;
	static DWORD dwBytesSent;
	static DWORD_VAL dwVLine;
	BYTE vBuffer[10];
	static BYTE vBytesPerSecond[12];
	WORD w;
	DWORD dw;
	QWORD qw;
	
	// Start the TCP server, listening on PERFORMANCE_PORT
	if(MySocket == INVALID_SOCKET)
	{
		MySocket = TCPOpen(0, TCP_OPEN_SERVER, TX_PERFORMANCE_PORT, TCP_PURPOSE_TCP_PERFORMANCE_TX);
	
		// Abort operation if no TCP socket of type TCP_PURPOSE_TCP_PERFORMANCE_TEST is available
		// If this ever happens, you need to go add one to TCPIPConfig.h
		if(MySocket == INVALID_SOCKET)
			return;

		// Initialize cumulative line transmission counter
		dwVLine.Val = 0;
	}
	
	// See how many bytes we can write to the TX FIFO
	// If we can't fit a single line of data in, then 
	// lets just wait for now.
	w = TCPIsPutReady(MySocket);
	if(w < 12+27+5+32u)
		return;

	// Upon connection initialize timer and byte count variables
	if(TCPIsConnected(MySocket))
	{
		if(TCPWasReset(MySocket))
		{
			dwTimeStart = TickGet();
			dwBytesSent = 0;
			vBytesPerSecond[0] = 0;	// Initialize empty string right now
		}
	}

	vBuffer[0] = '0';
	vBuffer[1] = 'x';

	// Transmit as much data as the TX FIFO will allow
	while(w >= 12+27+5+32u)
	{
		dwVLine.Val = TickGet();
		
		// Convert line counter to ASCII hex string
		vBuffer[2] = btohexa_high(dwVLine.v[3]);
		vBuffer[3] = btohexa_low(dwVLine.v[3]);
		vBuffer[4] = btohexa_high(dwVLine.v[2]);
		vBuffer[5] = btohexa_low(dwVLine.v[2]);
		vBuffer[6] = btohexa_high(dwVLine.v[1]);
		vBuffer[7] = btohexa_low(dwVLine.v[1]);
		vBuffer[8] = btohexa_high(dwVLine.v[0]);
		vBuffer[9] = btohexa_low(dwVLine.v[0]);

		dwVLine.Val++;
	
		// Place all data in the TCP TX FIFO
		TCPPutArray(MySocket, vBuffer, sizeof(vBuffer));
		TCPPutROMString(MySocket, (ROM BYTE*)": We are currently achieving ");
		TCPPutROMArray(MySocket, (ROM BYTE*)"       ", 5-strlen((char*)vBytesPerSecond));
		TCPPutString(MySocket, vBytesPerSecond);
		TCPPutROMString(MySocket, (ROM BYTE*)"00 bytes/second TX throughput.\r\n");

		w -= 12+27+5+32;
		dwBytesSent += 12+27+5+32;
	}
	
	// Send everything immediately
	TCPFlush(MySocket);

	// Calculate exact bytes/second, less truncation
	dw = TickGet() - dwTimeStart;
	if(dw > TICK_SECOND/10)
	{
//		qw = ((QWORD)dwBytesSent) * (TICK_SECOND/100/8);
//		qw = ((QWORD)dwBytesSent) * (TICK_SECOND/100);
//		qw /= dw;
		qw = ((QWORD)dwBytesSent)*(TICK_SECOND/100) + (dw>>1);
		qw /= dw;
		ultoa((DWORD)qw, vBytesPerSecond);
		dwTimeStart += dw;
		
		dwBytesSent = 0;
		//dwBytesSent -= dwBytesSent>>4;
	}
}

/*****************************************************************************
  Function:
	void TCPRXPerformanceTask(void)

  Summary:
	Tests the receive performance of the TCP module.

  Description:
	This function tests the receive performance of the TCP module.  To use,
	open a telnet connection to the device on RX_PERFORMANCE_PORT (9763 by 
	default).  Then use your telnet utility to upload a large file to the
	device.  Each second the board will report back how many bytes were 
	received in the previous second.
	
	TCP performance is affected by many factors, including round-trip time 
	and the TCP buffer size.  For faster results, increase the size of the 
	RX buffer size for the TCP_PURPOSE_TCP_PERFORMANCE_RX socket in 
	TCPIPConfig.h.  Round-trip time is affected by the distance to the 
	device, so across the desk will be orders of magnitude faster than 
	across the Internet.
	
	This function is particularly useful after development to determine the
	impact of your application code on the stack's performance.  A before and
	after comparison will indicate if your application is unacceptably
	blocking the processor or taking too long to execute.

  Precondition:
	TCP is initialized.

  Parameters:
	None

  Returns:
	None
  ***************************************************************************/
void TCPRXPerformanceTask(void)
{
	static TCP_SOCKET MySocket = INVALID_SOCKET;
	static DWORD dwTimeStart;
	static DWORD dwBytesRead;
	BYTE vBuffer[12];
	WORD w, wGetLen;
	DWORD dw;
	QWORD qw;
	
	// Start the TCP server, listening on RX_PERFORMANCE_PORT
	if(MySocket == INVALID_SOCKET)
	{
		MySocket = TCPOpen(0, TCP_OPEN_SERVER, RX_PERFORMANCE_PORT, TCP_PURPOSE_TCP_PERFORMANCE_RX);
	
		// Abort operation if no TCP socket of type TCP_PURPOSE_TCP_PERFORMANCE_TEST_RX is available
		// If this ever happens, you need to go add one to TCPIPConfig.h
		if(MySocket == INVALID_SOCKET)
			return;

		dwTimeStart = TickGet();
		dwBytesRead = 0;
	}
	
	// Read all data out of the TCP RX FIFO
	w = TCPIsGetReady(MySocket);
	if(w == 0u)
		return;

	dwBytesRead += w;
	wGetLen = sizeof(vBuffer);
	while(w)
	{
		if(w < sizeof(vBuffer))
			wGetLen = w;
		TCPGetArray(MySocket, vBuffer, wGetLen);
		w -= wGetLen;
	}
	
	dw = TickGet() - dwTimeStart;
	if(dw > TICK_SECOND)
	{
		if(TCPIsPutReady(MySocket) < 40u)
			return;

		dwTimeStart = TickGet();

		// Calculate exact bytes/second, with rounding
		qw = (QWORD)dwBytesRead * TICK_SECOND;
		qw += dw>>1;
		qw /= dw;
		ultoa((DWORD)qw, vBuffer);
		TCPPutString(MySocket, vBuffer);
		TCPPutROMString(MySocket, (ROM BYTE*)" bytes/second\r\n");
		
		dwBytesRead = 0;
	}
	
	
}

#endif //#if defined(STACK_USE_TCP_PERFORMANCE_TEST)
