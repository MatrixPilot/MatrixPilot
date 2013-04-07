#ifndef _MYIPGPSTEST_C_
#define _MYIPGPSTEST_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_GPSTEST == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpGPStest.h"
#include "MyIpHelpers.h"

//////////////////////////
// Module Variables

#define GPSSPPOOF_PACKET_MAX_LENGTH 60

BYTE RxCSVbuf[MAX_NUM_INSTANCES_OF_MODULES][GPSSPPOOF_PACKET_MAX_LENGTH];
int RxCSVbufIndex[MAX_NUM_INSTANCES_OF_MODULES];

SGpsSpoof GpsSpoof;

void parseGpsSpoofPacket(const uint8_t* bufCSV, const int16_t len);


void MyIpOnConnect_GPStest(const uint8_t s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to GPStest on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_GPStest(const uint8_t s)
{
    uint8_t i = MyIpData[s].instance;
    RxCSVbufIndex[i] = 0;

    GpsSpoof.Lat.WW = 0;
    GpsSpoof.Long.WW = 0;
    GpsSpoof.Alt.WW = 0;
    GpsSpoof.Mode = GpsSpoofMode_Disabled;
}

void MyIpService_GPStest(const uint8_t s)
{
}

boolean MyIpThreadSafeSendPacketCheck_GPStest(const uint8_t s, const boolean doClearFlag)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    boolean sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag)
    {
        MyIpData[s].sendPacket = FALSE;
    }
    return sendpacket;
}


int16_t MyIpThreadSafeReadBufferHead_GPStest(const uint8_t s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_GPStest(const uint8_t s)
{
    uint8_t si = MyIpData[s].instance;

    if (eTCP == MyIpData[s].type)
    {
        while (TCPIsGetReady(MyIpData[s].socket))
        {
            int16_t index = RxCSVbufIndex[si];
            TCPGet(MyIpData[s].socket, &RxCSVbuf[si][index]);
            //TCPPut(MyIpData[s].socket, RxCSVbuf[si][index]); // ECHO

            if ((RxCSVbuf[si][index] == '\r') ||
                (RxCSVbuf[si][index] == '\n') ||
                ((index+1) >= GPSSPPOOF_PACKET_MAX_LENGTH))
            {
                RxCSVbuf[si][index] = ',';
                parseGpsSpoofPacket(RxCSVbuf[si],index+1);
                RxCSVbufIndex[si] = 0;
            }
            else
              RxCSVbufIndex[si]++;
        }
    }
    else //if (eUDP == MyIpData[s].type)
    {
        while (UDPIsGetReady(MyIpData[s].socket) >= GPSSPPOOF_PACKET_MAX_LENGTH)
        {
            //UDPGetArray(buf, LENGTH_OF_GPSSPPOOF_PACKET);
            //parseGpsSpoofPacket(buf);
        }
    }
}



void parseGpsSpoofPacket(const uint8_t* bufCSV, const int16_t len)
{
    #define GPS_SPOOF_PARAM_LENGTH (4)
    int32_t gpsData[GPS_SPOOF_PARAM_LENGTH+1] = {0,0,0,0,0}; // +1 just in case!
    uint8_t parseCount;

    parseCount = parseCSV(bufCSV, len, gpsData, GPS_SPOOF_PARAM_LENGTH);

#if (NETWORK_USE_DEBUG == 1)
        static int16_t myCount = 0;
        int16_t i;
        myCount++;
        StringToSrc(eSourceDebug, "\r\n\r\n");
        itoaSrc(eSourceDebug, myCount);
        StringToSrc(eSourceDebug, " converted this: ");
        ArrayToSrc(eSourceDebug, bufCSV, len);
        StringToSrc(eSourceDebug, "\r\n to logical data: ");
        for (i=0;i<parseCount;i++)
        {
            ltoaSrc(eSourceDebug, gpsData[i]);
            ByteToSrc(eSourceDebug, ',');
        }
#endif

    if ((parseCount >= GPS_SPOOF_PARAM_LENGTH) &&
        (gpsData[0] < GpsSpoofMode_SIZE))
    {
        GpsSpoof.Mode = gpsData[0];
        GpsSpoof.Lat.WW = gpsData[1];
        GpsSpoof.Long.WW = gpsData[2];
        GpsSpoof.Alt.WW = gpsData[3];
    }
}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPGPSTEST_C_

