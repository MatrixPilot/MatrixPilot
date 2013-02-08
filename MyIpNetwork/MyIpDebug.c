#ifndef _MYIPDEBUG_C_
#define _MYIPDEBUG_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_DEBUG == 1)

#include "TCPIP_Stack/TCPIP.h"
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
    StringToSocket(s, "\r\nYou've connected to Debug on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_Debug(BYTE s)
{
    // This gets called once for every socket we're configured to use for this module.
    BYTE i = MyIpData[s].instance;
    taskTimer_Debug[i] = GenerateRandomDWORD() % (TICK_SECOND);
    dwTime_Debug[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_Debug(BYTE s)
{
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;
    BYTE i = MyIpData[s].instance;

    #if (NETWORK_USE_FLYBYWIRE == 1)
    BYTE allS, connectionCount;
    #endif

    if ((TickGet() - taskTimer_Debug[i]) > ((TICK_SECOND)/10)) // 10Hz
    {
        taskTimer_Debug[i] = TickGet();
        ByteToSocket(s, 12);	// Clear Screen

        #if defined(STACK_USE_SNTP_CLIENT)
        if(dwTime_Debug[i] != SNTPGetUTCSeconds())
        {
            dwTime_Debug[i] = SNTPGetUTCSeconds();
        }
        StringToSocket(s, "\r\nUTC = ");

        #else
        dwTime_Debug[i]++;
        StringToSocket(s, "\r\nCounter = ");
        #endif

        uitoaSocket(s, dwTime_Debug[i]);
        StringToSocket(s, "\r\n");

        #if (NETWORK_USE_FLYBYWIRE == 1)
        connectionCount = 0;
        for (allS = 0; allS < NumSockets(); allS++)
        {
            if ((MyIpData[allS].source == eSourceFlyByWire) && MyIpIsConnectedSocket(allS))
            {
                connectionCount++;
            }
        }

        StringToSocket(s, "\r\nIsConnected = "); itoaSocket(s,connectionCount);
        StringToSocket(s, "\r\nAileron  = ");	itoaSocket(s,udb_pwIn[AILERON_INPUT_CHANNEL]);
        StringToSocket(s, "\r\nElevator = ");	itoaSocket(s,udb_pwIn[ELEVATOR_INPUT_CHANNEL]);
        StringToSocket(s, "\r\nMode     = ");	itoaSocket(s,udb_pwIn[MODE_SWITCH_INPUT_CHANNEL]);
        StringToSocket(s, "\r\nRudder   = ");	itoaSocket(s,udb_pwIn[RUDDER_INPUT_CHANNEL]);
        StringToSocket(s, "\r\nThrottle = ");	itoaSocket(s,udb_pwIn[THROTTLE_INPUT_CHANNEL]);
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

