
#ifndef _MYIPFLYBYWIRE_C_
#define _MYIPFLYBYWIRE_C_

#include "MyIpConfig.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_FLYBYDATALINK == 1) && (FLYBYWIRE_ENABLED == 1)

#include "TCPIP Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpFlyByWire.h"
#include "FlyByWire.h"
#include "MyIpHelpers.h"


//////////////////////////////////////
// Local Functions

//////////////////////////
// Module Variables
uint32_t taskTimer_FlyByWire[MAX_NUM_INSTANCES_OF_MODULES];


void MyIpOnConnect_FlyByWire(const uint8_t s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to FlyByWire on "); // 36 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_FlyByWire(const uint8_t s)
{
	BYTE i = MyIpData[s].instance;
	// This gets called once for every socket we're configured to use for this module.
	taskTimer_FlyByWire[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_FlyByWire(const uint8_t s)
{
    uint8_t i = MyIpData[s].instance;

    if ((TickGet() - taskTimer_FlyByWire[i]) > (TICK_SECOND))
    {
        // once per second lets send a "." heartbeat
        taskTimer_FlyByWire[i] = TickGet();
        ByteToSocket(s, '.');
        MyIpData[s].sendPacket = TRUE; // send right away
    }
} 

boolean MyIpThreadSafeSendPacketCheck_FlyByWire(const uint8_t s, boolean doClearFlag)
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


int MyIpThreadSafeReadBufferHead_FlyByWire(const uint8_t s)
{
	// since this data comes from, and goes to, the idle thread we
	//  don't need to deal with any thread issues
	return MyIpData[s].buffer_head;
}


void MyIpProcessRxData_FlyByWire(const uint8_t s)
{
    uint8_t buf[LENGTH_OF_PACKET];

    if (eTCP == MyIpData[s].type)
    {
        while (TCPIsGetReady(MyIpData[s].socket) >= LENGTH_OF_PACKET)
        {
            TCPGetArray(MyIpData[s].socket, buf, LENGTH_OF_PACKET);
            if ((buf[0] == 'F') && (buf[1] == 'b') && (buf[2] == 'W'))
            {
                fbw_live_commit_buf(buf);
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
                fbw_live_commit_buf(buf);
            }
        }
    }
}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPFLYBYWIRE_C_
