#ifndef _MYIPMAVLink_C_
#define _MYIPMAVLink_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_MAVLINK == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpMAVLink.h"


//////////////////////////////////////
// Local Functions

//////////////////////////
// Module Variables



void MyIpOnConnect_MAVLink(BYTE s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to MAVLink on "); // 36 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}
	
void MyIpInit_MAVLink(BYTE s)
{
}

void MyIpService_MAVLink(BYTE s)
{
    // Nothing to do here, it's all done in ISRs
}

BOOL MyIpThreadSafeSendPacketCheck_MAVLink(BYTE s, BOOL doClearFlag)
{
    BYTE isrState;
    BOOL sendpacket;

    // THIS MODULE NEEDS TESTING.
    // The ISR Masking is a placeholder, it probably needs
    //  a completely different thread protection scheme.

    isrState = _U2TXIE;
    _U2TXIE = 0; // inhibit the MAVLink ISR from changing this on us during a read
    sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag)
    {
        MyIpData[s].sendPacket = FALSE;
    }
    _U2TXIE = isrState; // resume ISR
    return sendpacket;
}


int MyIpThreadSafeReadBufferHead_MAVLink(BYTE s)
{
    BYTE isrState;
    int head;

    // THIS MODULE NEEDS TESTING.
    // The ISR Masking is a placeholder, it probably needs
    //  a completely different thread protection scheme.

    isrState = _U2TXIE;
    _U2TXIE = 0; // inhibit the MAVLink ISR from loading more data for a moment (protect _head reads)
    head = MyIpData[s].buffer_head;
    _U2TXIE = isrState; // resume ISR

    return head;
}

void MyIpProcessRxData_MAVLink(BYTE s)
{
    // make sure the socket is for us
    //if (MyTelemetry[s].source != eSourceMAVLink)
    //	return;

    // THIS MODULE NEEDS TESTING.
    // The ISR Masking is a placeholder, it probably needs
    //  a completely different thread protection scheme.

    BYTE isrState, rxData;
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
            isrState = _U2RXIE;
            _U2RXIE = 0;
            udb_serial_callback_received_byte(rxData);
            _U2RXIE = isrState;
        }
    } while (successfulRead);
}

	
#endif // (NETWORK_USE_MAVLINK == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPMAVLink_C_

