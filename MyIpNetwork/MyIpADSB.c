#ifndef _MYIPADSB_C_
#define _MYIPADSB_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_ADSB == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpADSB.h"

//////////////////////////
// Module Variables
DWORD taskTimer_ADSB[MAX_NUM_INSTANCES_OF_MODULES];

typedef struct
{
	//unsigned char	callSign[10];
	DWORD_VAL	gpsLat;
	DWORD_VAL	gpsLong;
	WORD_VAL	heading;
	WORD_VAL	altitude;
	WORD_VAL	groundSpeed;
	BYTE    	climbRate;
} MyIpADSBtype;
extern MyIpDataType MyIpData[];


void MyIpOnConnect_ADSB(BYTE s)
{
	// Print any one-time connection annoucement text
	LoadStringSocket(s, "\r\nYou've connected to ADSB on "); // 33 chars
	LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
	LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
	LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
	LoadStringSocket(s, "\r\n"); // 2 chars
	MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_ADSB(BYTE s)
{
	// This gets called once for every socket we're configured to use for this module.
	BYTE i = MyIpData[s].instance;
	taskTimer_ADSB[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_ADSB(BYTE s)
{
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
            return;

    BYTE i = MyIpData[s].instance;
    DWORD packetLen;

    MyIpADSBtype data;
    data.gpsLat.Val         = 1;    // or lat_gps.WW
    data.gpsLong.Val        = 12;   // or long_gps.WW
    data.heading.Val        = 123;  // or rmat[?]
    data.altitude.Val       = 1234; // or alt_sl_gps.WW
    data.groundSpeed.Val    = 12345;// air_speed_3DIMU?? close enough for now
    data.climbRate          = 156;  // are we calculating this anywhere?

#if (1)
    // Option 1, sending a structure
    packetLen = sizeof(data);
    if (((TickGet() - taskTimer_ADSB[i]) > ((TICK_SECOND)/2)) && // 2Hz
        (IsMyIpBufferReady(s) >= packetLen))
    {
        taskTimer_ADSB[i] = TickGet();
        LoadNetworkAsyncTxBufferSocketArray(s, (BYTE*)&data, sizeof(data));
        MyIpData[s].sendPacket = TRUE;
    }

#else
    // Option 2, sending a packet organized however we'd like
    packetLen = (sizeof(data) + 6);
    if (((TickGet() - taskTimer_ADSB[i]) > ((TICK_SECOND)/2)) && // 2Hz
        (IsMyIpBufferReady(s) >= packetLen))
    {
        taskTimer_ADSB[i] = TickGet();
        
        // sending an encoded bitstream (example)
        LoadNetworkAsyncTxBufferSocket(s, 0xAA); // some sort of header
        LoadNetworkAsyncTxBufferSocket(s, 0xAB); // some sort of header
        LoadNetworkAsyncTxBufferSocket(s, 0xAC); // some sort of header
        LoadNetworkAsyncTxBufferSocket(s, sizeof(data)); // data length

        LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[0]);
        LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[1]);
        LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[2]);
        LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[3]);
        LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[0]);
        LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[1]);
        LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[2]);
        LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[3]);
        LoadNetworkAsyncTxBufferSocket(s, data.heading.v[0]);
        LoadNetworkAsyncTxBufferSocket(s, data.heading.v[1]);
        LoadNetworkAsyncTxBufferSocket(s, data.altitude.v[0]);
        LoadNetworkAsyncTxBufferSocket(s, data.altitude.v[1]);
        LoadNetworkAsyncTxBufferSocket(s, data.groundSpeed.v[0]);
        LoadNetworkAsyncTxBufferSocket(s, data.groundSpeed.v[1]);
        LoadNetworkAsyncTxBufferSocket(s, data.climbRate);

        LoadNetworkAsyncTxBufferSocket(s, 0xAD); // some sort of footer, maybe CRC or end-of-packet flag?
        LoadNetworkAsyncTxBufferSocket(s, 0xAE); // some sort of footer, maybe CRC or end-of-packet flag?
        MyIpData[s].sendPacket = TRUE;
    }
#endif
}

BOOL MyIpThreadSafeSendPacketCheck_ADSB(BYTE s, BOOL doClearFlag)
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


int MyIpThreadSafeReadBufferHead_ADSB(BYTE s)
{
	// since this data comes from, and goes to, the idle thread we
	//  don't need to deal with any thread issues
	return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_ADSB(BYTE s)
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

	
#endif // (NETWORK_USE_ADSB == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPADSB_C_

