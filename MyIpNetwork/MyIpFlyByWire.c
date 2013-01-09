
#ifndef _MYIPFLYBYWIRE_C_
#define _MYIPFLYBYWIRE_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_FLYBYWIRE == 1)

#include "defines.h"
#include "TCPIP Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpFlyByWire.h"


	#define LENGTH_OF_HEADER	(3)
	#define LENGTH_OF_PAYLOAD	(10)
	#define LENGTH_OF_PACKET	(LENGTH_OF_HEADER + LENGTH_OF_PAYLOAD)

//////////////////////////////////////
// Local Functions
void ProcessPWMdata_FlyByWire(BYTE* buf);

//////////////////////////
// Module Variables
DWORD taskTimer_FlyByWire[MAX_NUM_INSTANCES_OF_MODULES];


void MyIpOnConnect_FlyByWire(BYTE s)
{
	// Print any one-time connection annoucement text
	LoadStringSocket(s, "\r\nYou've connected to FlyByWire on "); // 36 chars
	LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
	LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
	LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
	LoadStringSocket(s, "\r\n"); // 2 chars
	MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_FlyByWire(BYTE s)
{
	BYTE i = MyIpData[s].instance;
	// This gets called once for every socket we're configured to use for this module.
	taskTimer_FlyByWire[i] = 0;
}

void MyIpService_FlyByWire(BYTE s)
{
	BYTE i = MyIpData[s].instance;
	
	if ((TickGet() - taskTimer_FlyByWire[i]) > (TICK_SECOND))
	{
		// once per second lets send a "." heartbeat
		taskTimer_FlyByWire[i] = TickGet();
		LoadNetworkAsyncTxBufferSocket(s, '.');
		MyIpData[s].sendPacket = TRUE; // send right away
	}
} 

BOOL MyIpThreadSafeSendPacketCheck_FlyByWire(BYTE s, BOOL doClearFlag)
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


int MyIpThreadSafeReadBufferHead_FlyByWire(BYTE s)
{
	// since this data comes from, and goes to, the idle thread we
	//  don't need to deal with any thread issues
	return MyIpData[s].buffer_head;
}


void MyIpProcessRxData_FlyByWire(BYTE s)
{
	BYTE buf[LENGTH_OF_PACKET];
	
	if (eTCP == MyIpData[s].type)
	{
		while (TCPIsGetReady(MyIpData[s].socket) >= LENGTH_OF_PACKET)
		{
			TCPGetArray(MyIpData[s].socket, buf, LENGTH_OF_PACKET);
			if ((buf[0] == 'F') && (buf[1] == 'b') && (buf[2] == 'W'))
			{
				ProcessPWMdata_FlyByWire(buf);
			}
		}
	}
	else //if (eUDP == MyIpData[s].type)
	{
		while (UDPIsGetReady(MyIpData[s].socket) >= LENGTH_OF_PACKET)
		{
			UDPGetArray(buf, LENGTH_OF_PACKET);
			if ((buf[0] == 'F') && (buf[1] == 'b') && (buf[2] == 'W'))
			{
				ProcessPWMdata_FlyByWire(buf);
			}
		}
	}
}

void ProcessPWMdata_FlyByWire(BYTE* buf)
{
	// [0,1,2] = "FbW" Header packet
	// [3,4] = AILERON_INPUT_CHANNEL (LSB, MSB)
	// [5,6] = ELEVATOR_INPUT_CHANNEL (LSB, MSB)
	// [7,8] = MODE_SWITCH_INPUT_CHANNEL (LSB, MSB)
	// [9,10] = RUDDER_INPUT_CHANNEL (LSB, MSB)
	// [11,12] = THROTTLE_INPUT_CHANNEL (LSB, MSB)
	
	BYTE buf_index = LENGTH_OF_HEADER;
	WORD_VAL tempPWM;
	
	tempPWM.v[0] = buf[buf_index++]; // LSB first
	tempPWM.v[1] = buf[buf_index++];
	udb_pwIn[AILERON_INPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL] = tempPWM.Val;
	
	tempPWM.v[0] = buf[buf_index++];
	tempPWM.v[1] = buf[buf_index++];
	udb_pwIn[ELEVATOR_INPUT_CHANNEL] = udb_pwTrim[ELEVATOR_INPUT_CHANNEL] = tempPWM.Val;
	
	tempPWM.v[0] = buf[buf_index++];
	tempPWM.v[1] = buf[buf_index++];
	udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = udb_pwTrim[MODE_SWITCH_INPUT_CHANNEL] = tempPWM.Val;
	
	tempPWM.v[0] = buf[buf_index++];
	tempPWM.v[1] = buf[buf_index++];
	udb_pwIn[RUDDER_INPUT_CHANNEL] = udb_pwTrim[RUDDER_INPUT_CHANNEL] = tempPWM.Val;
	
	tempPWM.v[0] = buf[buf_index++];
	tempPWM.v[1] = buf[buf_index++];
	udb_pwIn[THROTTLE_INPUT_CHANNEL] = udb_pwTrim[THROTTLE_INPUT_CHANNEL] = tempPWM.Val;
}	
	
#endif // (NETWORK_USE_FLYBYWIRE == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPFLYBYWIRE_C_
