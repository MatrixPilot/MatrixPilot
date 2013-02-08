#ifndef _MYIPPWMREPORT_C_
#define _MYIPPWMREPORT_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_PWMREPORT == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpPWMreport.h"

//////////////////////////
// Module Variables
DWORD taskTimer_PWMreport[MAX_NUM_INSTANCES_OF_MODULES];

void MyIpOnConnect_PWMreport(BYTE s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to PWMreport on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_PWMreport(BYTE s)
{
    // This gets called once for every socket we're configured to use for this module.
    BYTE i = MyIpData[s].instance;
    taskTimer_PWMreport[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_PWMreport(BYTE s)
{
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;

    BYTE i = MyIpData[s].instance;
    BYTE pwmIndex;

    if ((TickGet() - taskTimer_PWMreport[i]) > ((TICK_SECOND)/10)) // 10Hz
    {
        taskTimer_PWMreport[i] = TickGet();

        //ByteToSocket(s, 12);
        
        itoaSocket(s,(NUM_INPUTS+1));  ByteToSocket(s, ',');
        itoaSocket(s,(NUM_OUTPUTS+1)); ByteToSocket(s, ',');

        for (pwmIndex=0; pwmIndex < (NUM_INPUTS+1); pwmIndex++)
        {
            itoaSocket(s,udb_pwIn[pwmIndex]);
            ByteToSocket(s, ',');
        }

        for (pwmIndex=0; pwmIndex < (NUM_OUTPUTS+1); pwmIndex++)
        {
            itoaSocket(s,udb_pwOut[pwmIndex]);
            ByteToSocket(s, ',');
        }

        StringToSocket(s, "\r\n");
        MyIpData[s].sendPacket = TRUE;
    }
}

BOOL MyIpThreadSafeSendPacketCheck_PWMreport(BYTE s, BOOL doClearFlag)
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


int MyIpThreadSafeReadBufferHead_PWMreport(BYTE s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_PWMreport(BYTE s)
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

	
#endif // (NETWORK_USE_PWMREPORT == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPPWMREPORT_C_

