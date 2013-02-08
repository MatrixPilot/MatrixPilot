#ifndef _MYIPGPSTEST_C_
#define _MYIPGPSTEST_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_GPSTEST == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpGPStest.h"

//////////////////////////
// Module Variables


void MyIpOnConnect_GPStest(BYTE s)
{
	// Print any one-time connection annoucement text
	StringToSocket(s, "\r\nYou've connected to GPStest on "); // 33 chars
	StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
	StringToSocket(s, "'s aircraft. More info at "); // 26 chars
	StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
	StringToSocket(s, "\r\n"); // 2 chars
	MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_GPStest(BYTE s)
{
}

void MyIpService_GPStest(BYTE s)
{
}

BOOL MyIpThreadSafeSendPacketCheck_GPStest(BYTE s, BOOL doClearFlag)
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


int MyIpThreadSafeReadBufferHead_GPStest(BYTE s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_GPStest(BYTE s)
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
            //TODO, modify lat_gps.WW , long_gps.WW , alt_sl_gps.WW
        }
    } while (successfulRead);
}

	
#endif // (NETWORK_USE_GPSTEST == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPGPSTEST_C_

