#ifndef _MYIPGROUND_STATION_C_
#define _MYIPGROUND_STATION_C_

#include "MyIpConfig.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_GROUND_STATION == 1)

#include "TCPIP Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpGroundStation.h"
#include "MyIpHelpers.h"
#include "../Configuration/telemetry_config.h"

void SendGroundStationPacket(uint8_t s);

//////////////////////////
// Module Variables
uint32_t taskTimer_GroundStation[MAX_NUM_INSTANCES_OF_MODULES];

void MyIpOnConnect_GroundStation(const BYTE s)
{    
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to GroundStation on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_GroundStation(const BYTE s)
{
    // This gets called once for every socket we're configured to use for this module.
    BYTE i = MyIpData[s].instance;
    taskTimer_GroundStation[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_GroundStation(const BYTE s)
{
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;

    uint8_t i = MyIpData[s].instance;

    if ((TickGet() - taskTimer_GroundStation[i]) > ((TICK_SECOND)/8)) // 8Hz
    {
        taskTimer_GroundStation[i] = TickGet();
        SendGroundStationPacket(s);
    }
}

boolean MyIpThreadSafeSendPacketCheck_GroundStation(const uint8_t s, boolean doClearFlag)
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


int MyIpThreadSafeReadBufferHead_GroundStation(const uint8_t s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_GroundStation(const uint8_t s)
{
    uint8_t rxData;
    boolean successfulRead;

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

void SendGroundStationPacket(uint8_t s)
{
}


#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPGROUND_STATION_C_

