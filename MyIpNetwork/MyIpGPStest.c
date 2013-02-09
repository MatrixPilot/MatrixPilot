#ifndef _MYIPGPSTEST_C_
#define _MYIPGPSTEST_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_GPSTEST == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpGPStest.h"
#include "MyIpHelpers.h"

//////////////////////////
// Module Variables

#define LENGTH_OF_GPSSPPOOF_PACKET 60

BYTE RxCSVbuf[MAX_NUM_INSTANCES_OF_MODULES][LENGTH_OF_GPSSPPOOF_PACKET];
int RxCSVbufIndex[MAX_NUM_INSTANCES_OF_MODULES];

SGpsSpoof GpsSpoof;

void parseGpsSpoofPacket(conf BYTE* bufCSV);


void MyIpOnConnect_GPStest(conf BYTE s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to GPStest on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_GPStest(conf BYTE s)
{
    BYTE i = MyIpData[s].instance;
    RxCSVbufIndex[i] = 0;

    GpsSpoof.Lat.WW = 0;
    GpsSpoof.Long.WW = 0;
    GpsSpoof.Alt.WW = 0;
    GpsSpoof.Mode = GpsSpoofMode_Disabled;
}

void MyIpService_GPStest(conf BYTE s)
{
}

BOOL MyIpThreadSafeSendPacketCheck_GPStest(conf BYTE s, conf BOOL doClearFlag)
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


int MyIpThreadSafeReadBufferHead_GPStest(conf BYTE s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_GPStest(conf BYTE s)
{
    BYTE si = MyIpData[s].instance;

    if (eTCP == MyIpData[s].type)
    {
        while (TCPIsGetReady(MyIpData[s].socket))
        {
            int index = RxCSVbufIndex[si];
            TCPGet(MyIpData[s].socket, &RxCSVbuf[si][index]);

            if (RxCSVbuf[si][index] == '\r')
            {
                parseGpsSpoofPacket(RxCSVbuf[si]);
                RxCSVbufIndex[si] = 0;
            }
            else if (RxCSVbufIndex[si] >= LENGTH_OF_GPSSPPOOF_PACKET)
            {
                RxCSVbufIndex[si] = 0;
            }
            else
                RxCSVbufIndex[si]++;
        }
    }
    else //if (eUDP == MyIpData[s].type)
    {
        while (UDPIsGetReady(MyIpData[s].socket) >= LENGTH_OF_GPSSPPOOF_PACKET)
        {
            //UDPGetArray(buf, LENGTH_OF_GPSSPPOOF_PACKET);
            //parseGpsSpoofPacket(buf);
        }
    }
}



void parseGpsSpoofPacket(conf BYTE* bufCSV)
{
    #define GPS_SPOOF_PARAM_LENGTH (4)
    INT32 gpsData[GPS_SPOOF_PARAM_LENGTH+2];
    BYTE parseCount;

    parseCount = parseCSV(bufCSV, gpsData, GPS_SPOOF_PARAM_LENGTH);

#if (NETWORK_USE_DEBUG == 1)
        static int myCount = 0;
        int i;
        myCount++;
        StringToSrc(eSourceDebug, "\r\n\r\n");
        itoaSrc(eSourceDebug, myCount);
        StringToSrc(eSourceDebug, " converted: ");
        StringToSrc(eSourceDebug, (char*)bufCSV);
        StringToSrc(eSourceDebug, " to logical data: ");
        for (i=0;i<parseCount;i++)
        {
            ltoaSrc(eSourceDebug, gpsData[i]);
            ByteToSrc(eSourceDebug, ',');
        }
#endif

    if ((parseCount >= GPS_SPOOF_PARAM_LENGTH) &&
        (gpsData[0] < GpsSpoofMode_SIZE))
    {
        StringToSrc(eSourceDebug, "\r\nNew GPS data Accepted!");
        GpsSpoof.Mode = gpsData[0];
        GpsSpoof.Lat.WW = gpsData[1];
        GpsSpoof.Long.WW = gpsData[2];
        GpsSpoof.Alt.WW = gpsData[3];
    }
}
	
#endif // (NETWORK_USE_GPSTEST == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPGPSTEST_C_

