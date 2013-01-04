#ifndef _MYIPDEBUG_C_
#define _MYIPDEBUG_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_DEBUG == 1)

#include "TCPIP Stack/TCPIP.h"
#include "defines.h"

#include "MyIpData.h"
#include "MyIpDebug.h"

#include "MyIpFlyByWire.h"


void MyIpOnConnect_Debug(BYTE s)
{
	// Print any one-time connection annoucement text
	LoadStringSocket(s, "\r\nYou've connected to Debug on "); // 33 chars
	LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
	LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
	LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
	LoadStringSocket(s, "\r\n"); // 2 chars
	MyIpData[s].foundEOL = TRUE; // send right away
}

void MyIpInit_Debug(void)
{
	// Nothing to do here.
}

void MyIpService_Debug(BYTE s)
{
	// don't bother queuing data if no one is listening
	if (FALSE == MyIpIsConnectedSocket(s))
		return;
	
	static DWORD timer = 0;
	static DWORD dwTime = 0;
	DWORD tick = TickGet();
  
	if ((tick - timer) > ((TICK_SECOND)/10))
	{
		timer = tick;
		LoadNetworkAsyncTxBufferSocket(s, 12);	// Clear Screen
	#if defined(STACK_USE_SNTP_CLIENT)
		if(dwTime != SNTPGetUTCSeconds())
		{
			dwTime = SNTPGetUTCSeconds();
		}
		LoadStringSocket(s, "\r\nUTC = ");
	#else
		dwTime++;
		LoadStringSocket(s, "\r\nCounter = ");
	#endif
		LoadPrintSocket(s, dwTime, 0);
		LoadStringSocket(s, "\r\n");

		#if (NETWORK_USE_FLYBYWIRE == 1)
		LoadStringSocket(s, "\r\nIsConnected = "); LoadPrintSocket(s,MyIpIsConnectedSocket(s),0);
		LoadStringSocket(s, "\r\nAileron  = ");	LoadPrintSocket(s,udb_pwIn[AILERON_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nElevator = ");	LoadPrintSocket(s,udb_pwIn[ELEVATOR_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nMode     = ");	LoadPrintSocket(s,udb_pwIn[MODE_SWITCH_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nRudder   = ");	LoadPrintSocket(s,udb_pwIn[RUDDER_INPUT_CHANNEL],0);
		LoadStringSocket(s, "\r\nThrottle = ");	LoadPrintSocket(s,udb_pwIn[THROTTLE_INPUT_CHANNEL],0);
		#endif	
		

	}	
}

BOOL MyIpThreadSafeEOLcheck_Debug(BYTE s, BOOL doClearFlag)
{
	return true; // flush all the time.
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
			// No Rx data parsing implemented
		}	
	} while (successfulRead);
}

	
#endif // (NETWORK_USE_DEBUG == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPDEBUG_C_

