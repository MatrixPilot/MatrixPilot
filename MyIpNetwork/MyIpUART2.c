#ifndef _MYIPUART2_C_
#define _MYIPUART2_C_

#include "defines.h"
#if (USE_NETWORK == 1) && (NETWORK_USE_UART2 == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpUART2.h"
#include "MyIpHelpers.h"

//////////////////////////////////////
// Local Functions

//////////////////////////
// Module Variables


void MyIpOnConnect_UART2(const BYTE s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to UART2 on "); // 31 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}


void MyIpInit_UART2(const BYTE s)
{
}

void MyIpService_UART2(const BYTE s)
{
    // Nothing to do here, it's all done in ISRs
}

BOOL MyIpThreadSafeSendPacketCheck_UART2(const BYTE s, const BOOL doClearFlag)
{
    #if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
    return TRUE;

    #else
    BYTE isrState;
    BOOL sendpacket;

    isrState = _U2TXIE;
    _U2TXIE = 0; // inhibit the UART2 ISR from changing this on us during a read
    sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag)
    {
        MyIpData[s].sendPacket = FALSE;
    }
    _U2TXIE = isrState; // resume ISR
    return sendpacket;
    #endif
}


int MyIpThreadSafeReadBufferHead_UART2(const BYTE s)
{
    BYTE isrState;
    int head;

    isrState = _U2TXIE;
    _U2TXIE = 0; // inhibit the UART2 ISR from loading more data for a moment (protect _head reads)
    head = MyIpData[s].buffer_head;
    _U2TXIE = isrState; // resume ISR

    return head;
}

void MyIpProcessRxData_UART2(const BYTE s)
{
    // make sure the socket is for us
    //if (MyTelemetry[s].source != eSourceUART2)
    //	return;

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

	
#endif // #if (USE_NETWORK == 1)
#endif // _MYIPUART2_C_

