#ifndef _MYIPDEBUG_C_
#define _MYIPDEBUG_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_DEBUG == 1)

#include "TCPIP Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpDebug.h"

//////////////////////////
// Module Variables
DWORD taskTimer_Debug[MAX_NUM_INSTANCES_OF_MODULES];
DWORD dwTime_Debug[MAX_NUM_INSTANCES_OF_MODULES];


void MyIpOnConnect_Debug(BYTE s)
{
	// Print any one-time connection annoucement text
	LoadStringSocket(s, "\r\nYou've connected to Debug on "); // 33 chars
	LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
	LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
	LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
	LoadStringSocket(s, "\r\n"); // 2 chars
	MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_Debug(BYTE s)
{
	// This gets called once for every socket we're configured to use for this module.
	BYTE i = MyIpData[s].instance;
	taskTimer_Debug[i] = 0;
	dwTime_Debug[i] = 0;
}

void MyIpService_Debug(BYTE s)
{
	
	// don't bother queuing data if no one is listening
	if (FALSE == MyIpIsConnectedSocket(s))
		return;
	BYTE allS, connectionCount;
	BYTE i = MyIpData[s].instance;
	  
	if ((TickGet() - taskTimer_Debug[i]) > ((TICK_SECOND)/10)) // 10Hz
	{
		taskTimer_Debug[i] = TickGet();
		LoadNetworkAsyncTxBufferSocket(s, 12);	// Clear Screen
		
	#if defined(STACK_USE_SNTP_CLIENT)
		if(dwTime_Debug[i] != SNTPGetUTCSeconds())
		{
			dwTime_Debug[i] = SNTPGetUTCSeconds();
		}
		LoadStringSocket(s, "\r\nUTC = ");
		
	#else
		dwTime_Debug[i]++;
		LoadStringSocket(s, "\r\nCounter = ");
	#endif
	
		LoadPrintSocket(s, dwTime_Debug[i], 0);
		LoadStringSocket(s, "\r\n");

		#if (NETWORK_USE_FLYBYWIRE == 1)
		connectionCount = 0;
		for (allS = 0; allS < NumSockets(); allS++)
		{
			if ((MyIpData[allS].source == eSourceFlyByWire) && MyIpIsConnectedSocket(allS))
			{
				connectionCount++;
			}	
		}	
		
		LoadStringSocket(s, "\r\nIsConnected = "); LoadPrintSocket(s,connectionCount,0);
		LoadStringSocket(s, "\r\nAileron  = ");	LoadPrintSocket(s,udb_pwIn[AILERON_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nElevator = ");	LoadPrintSocket(s,udb_pwIn[ELEVATOR_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nMode     = ");	LoadPrintSocket(s,udb_pwIn[MODE_SWITCH_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nRudder   = ");	LoadPrintSocket(s,udb_pwIn[RUDDER_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nThrottle = ");	LoadPrintSocket(s,udb_pwIn[THROTTLE_INPUT_CHANNEL],0);
		#endif	
		
		MyIpData[s].sendPacket = TRUE;
	}	
}

BOOL MyIpThreadSafeSendPacketCheck_Debug(BYTE s, BOOL doClearFlag)
{
	// since this data comes from, and goes to, the idle thread we
	//  don't need to deal with any thread issues
	BOOL sendpacket = MyIpData[s].sendPacket;
	if (doClearFlag)
	{
		MyIpData[s].sendPacket = FALSE;
	}
	return sendpacket;
}


int MyIpThreadSafeReadBufferHead_Debug(BYTE s)
{
	// since this data comes from, and goes to, the idle thread we
	//  don't need to deal with any thread issues
	return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_Debug(BYTE s)
{
	BYTE rxData;
	BOOL successfulRead;
	
	do
	{
		if (eTCP == MyIpData[s].type)
		{
			successfulRead = TCPGet(MyIpData[s].socket, &rxData);
		}
		else //if (eUDP == MyIpData[s].type)
		{
			successfulRead = UDPGet(&rxData);
		}
		
		if (successfulRead)
		{
			// No Rx data parsing implemented
		}	
	} while (successfulRead);
}

	
#endif // (NETWORK_USE_DEBUG == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPDEBUG_C_

