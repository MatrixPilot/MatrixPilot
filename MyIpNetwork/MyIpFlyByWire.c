
#ifndef _MYIPFLYBYWIRE_C_
#define _MYIPFLYBYWIRE_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_FLYBYWIRE == 1)

#include "defines.h"
#include "TCPIP Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpFlyByWire.h"



void MyIpOnConnect_FlyByWire(BYTE s)
{
	// Print any one-time connection annoucement text
	LoadStringSocket(s, "\r\nYou've connected to FlyByWire2 on "); // 36 chars
	LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
	LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
	LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
	LoadStringSocket(s, "\r\n"); // 2 chars
	MyIpData[s].foundEOL = TRUE; // send right away
}
	
void MyIpInit_FlyByWire(void)
{
	// Nothing to do for this module
}

void MyIpService_FlyByWire(BYTE s)
{
	// Ensure no more that 3 instances of FlyByWire sockets exists
	static DWORD timer[3] = {0,0,0};
	
	if ((TickGet() - timer[s]) > ((TICK_SECOND)/10))
	{
		timer[s] = TickGet();
		LoadNetworkAsyncTxBufferSocket(s, '.');
	}
	
} 

BOOL MyIpThreadSafeEOLcheck_FlyByWire(BYTE s, BOOL doClearFlag)
{
	// since this data comes from, and goes to, the idle thread we
	//  don't need to deal with any thread issues
	BOOL eolFound = MyIpData[s].foundEOL;
	if (doClearFlag)
	{
		MyIpData[s].foundEOL = FALSE;
	}
	return eolFound;
}


int MyIpThreadSafeReadBufferHead_FlyByWire(BYTE s)
{
	// since this data comes from, and goes to, the idle thread we
	//  don't need to deal with any thread issues
	return MyIpData[s].buffer_head;
}


void MyIpProcessRxData_FlyByWire(BYTE s)
{
#define LENGTH_OF_PACKET	15
#define RXDATA_SIZE 		LENGTH_OF_PACKET+1


	static BYTE rxdata[RXDATA_SIZE];
	static BYTE rxdata_index=0;
	static BYTE rxdata_index_is_lost = 1; // force re-sync on boot
	BYTE rxByte;

	unsigned short tempPWM;
	//WORD_VAL	tempPWM;
	
	BOOL successfulRead;
		

	do
	{
		if (MyIpData[s].type == eTCP)
		{
			successfulRead = TCPGet(MyIpData[s].socket, &rxByte);
		}
		else //if (MyTelemetry[s].type == eUDP)
		{
			successfulRead = UDPGet(&rxByte);
		}

		// several datas come which end with "\r\n".
		// If \r is not found, ignore inbound data until found to re-sync
		if (rxdata_index_is_lost)	// search for start of header packet "WiFi"
		{
			rxdata_index = 0;
			if (rxByte == 'W') // wait for 'W'
			{
				rxdata_index_is_lost = 0;
				rxdata[rxdata_index++] = rxByte;
			}	
		}
		else if (rxdata_index < RXDATA_SIZE)
		{
			rxdata[rxdata_index++] = rxByte;
			if ((rxdata[LENGTH_OF_PACKET-1] == '\r') && (rxdata[LENGTH_OF_PACKET] == '\n') &&
				(rxdata[0] == 'W') )//&& (rxdata[1] == 'i') && (rxdata[2] == 'F') && (rxdata[3] == 'i'))
			{
				// Header and EOL found, process the data. 
				
				// [0,1,2,3] = "WiFi" Header packet
				// [4,5] = AILERON_INPUT_CHANNEL (MSB,LSB)
				// [6,7] = ELEVATOR_INPUT_CHANNEL (MSB,LSB)
				// [8,9] = MODE_SWITCH_INPUT_CHANNEL (MSB,LSB)
				// [10,11] = RUDDER_INPUT_CHANNEL (MSB,LSB)
				// [12,13] = THROTTLE_INPUT_CHANNEL (MSB,LSB)
				// [14,15]= "\r\n"
				
				rxdata_index = 4; // use for temp index. clear after using 
				
				//tempPWM.v[0] = rxdata[rxdata_index++];
				//tempPWM.v[1] = rxdata[rxdata_index++];
				//udb_pwIn[AILERON_INPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL] = tempPWM.Val;
						
				tempPWM = (unsigned short)rxdata[rxdata_index++] << 8;
				tempPWM |= rxdata[rxdata_index++];
				udb_pwIn[AILERON_INPUT_CHANNEL] = udb_pwTrim[AILERON_INPUT_CHANNEL] = tempPWM;
						
				tempPWM = (unsigned short)rxdata[rxdata_index++] << 8;
				tempPWM |= rxdata[rxdata_index++];
				udb_pwIn[ELEVATOR_INPUT_CHANNEL] = udb_pwTrim[ELEVATOR_INPUT_CHANNEL] = tempPWM;
										
				tempPWM = (unsigned short)rxdata[rxdata_index++];
				tempPWM <<= 8; tempPWM |= rxdata[rxdata_index++];
				udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] = udb_pwTrim[MODE_SWITCH_INPUT_CHANNEL] = tempPWM;
								
				tempPWM = (unsigned short)rxdata[rxdata_index++];
				tempPWM <<= 8; tempPWM |= rxdata[rxdata_index++];
				udb_pwIn[RUDDER_INPUT_CHANNEL] = udb_pwTrim[RUDDER_INPUT_CHANNEL] = tempPWM;
						
				tempPWM = (unsigned short)rxdata[rxdata_index++];
				tempPWM <<= 8; tempPWM |= rxdata[rxdata_index++];
				udb_pwIn[THROTTLE_INPUT_CHANNEL] = udb_pwTrim[THROTTLE_INPUT_CHANNEL] = tempPWM;
	
				rxdata_index = 0;
				rxdata[0] = 0; // Forget we saw the header
				rxdata[14] = 0; // Forget we saw the EOL
			}
			else
			{
				// nothing to do, just waiting for EOL.
				// We could be double-checking for correct header.
			}	
		}	
		else
		{
			// buffer overflow, if so something is wrong.
			rxdata_index_is_lost = 1;
			rxdata_index = 0;
		}
		if (successfulRead)
		{
		}	
	} while (successfulRead);
}

#endif // (NETWORK_USE_FLYBYWIRE == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPFLYBYWIRE_C_
