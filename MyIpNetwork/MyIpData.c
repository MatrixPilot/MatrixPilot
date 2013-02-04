
#ifndef _MYIPDATA_C_
#define _MYIPDATA_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))

#include "defines.h"
#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"

#if (NETWORK_USE_UART1 == 1)
#include "MyIpUART1.h"
#endif
#if (NETWORK_USE_UART2 == 1)
#include "MyIpUART2.h"
#endif
#if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED == 1)
#include "MyIpFlyByWire.h"
#endif
#if (NETWORK_USE_MAVLINK == 1)
#include "MyIpMAVLink.h"
#endif
#if (NETWORK_USE_DEBUG == 1)
#include "MyIpDebug.h"
#endif
#if (NETWORK_USE_ADSB == 1)
#include "MyIpADSB.h"
#endif


#include "MyIpOptions.h"

//////////////////////////////////////
// Local Functions
int MyIpThreadSafeReadBufferHead(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck(BYTE s, BOOL doClearFlag);
void SendAsyncTxData_Bulk(BYTE s);
void MyIpProcessRxData(BYTE s);
BYTE Get_TCP_PURPOSE(eSource src);
void MyIpOnConnect(BYTE s);


unsigned int NumSockets(void)
{
    // This constant is not able to be global, a worker function is needed.
    return NUM_SOCKETS;
}

BOOL MyIpIsConnectedSocket(BYTE s)
{
    if (s >= NumSockets())
        return FALSE;
    BOOL isConnected = (eSM_CONNECTED == MyIpData[s].state);

    if (eTCP == MyIpData[s].type)
    {
        // easy result because we have a connection state to read from
        return isConnected;
    }
    else //if (eUDP == MyIpData[s].type)
    {
        // TODO make this smarter, right now we are *always* connected
        // since we're connection-less, check if we've recieved any data
        return isConnected;
    }
}
BOOL MyIpIsConnectedSrc(eSource src)
{
    BYTE s;
    BOOL result = FALSE;

    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            result |= MyIpIsConnectedSocket(s);
        }
    }
    return result;
}


void LoadStringSocket(BYTE s, char* buf)
{
    while (*buf) { LoadNetworkAsyncTxBufferSocket(s, *buf++); }
}
void LoadStringSrc(eSource src, char* buf)
{
    while (*buf) { LoadNetworkAsyncTxBufferSrc(src, *buf++); }
}

void LoadPrintSrc(eSource src, unsigned long data, unsigned char spacing)
{
    BYTE s;
    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            LoadPrintSocket(s, data, spacing);
        }
    } // for
}
void itoaSocket(BYTE s, INT16 value)
{
    char buf[20];
    itoa(value, (char*)buf);
    LoadStringSocket(s, buf);
}
void ltoaSocket(BYTE s, INT32 value)
{
    char buf[20];
    ltoa(value, buf);
    LoadStringSocket(s, buf);
}
void uitoaSocket(BYTE s, UINT16 value)
{
    BYTE buf[20];
    uitoa(value, buf);
    LoadStringSocket(s, (char*)buf);
}
void ultoaSocket(BYTE s, UINT32 value)
{
    BYTE buf[20];
    ultoa(value, buf);
    LoadStringSocket(s, (char*)buf);
}

void itoa(INT16 Value, char* Buffer)
{
    if (Value < 0)
    {
        *Buffer++ = '-';
        Value = -Value;
    }
    uitoa((UINT16)Value, (BYTE*)Buffer);
}
void ltoa(INT32 Value, char* Buffer)
{
    if (Value < 0)
    {
        *Buffer++ = '-';
        Value = -Value;
    }
    ultoa((UINT32)Value, (BYTE*)Buffer);
}

DWORD IsMyIpBufferReady(BYTE s)
{
    // Determine how much memory is available in the circular buffer
    DWORD head = MyIpThreadSafeReadBufferHead(s);
    DWORD tail = MyIpData[s].buffer_tail;

    if (head >= tail)
    {
        return (TX_BUFFER_SIZE - (head - tail));
    }
    else
    {
        return (tail - head - 1);
    }
}
void LoadPrintSocket(BYTE s, unsigned long data, unsigned char spacing)
{
    // This looks ugly but it only consumes 278 bytes of ROM.
    // TODO: remove this function and convert whoever calls it to use vsnprintf()

    //32 bit
    if ((data > 0xFFFF) || (spacing >= 6))
    {
        if (data >= 1000000000)	LoadNetworkAsyncTxBufferSocket(s, '0'+( data/1000000000));
        else if (spacing == 10) LoadNetworkAsyncTxBufferSocket(s, '0');
        if (data >= 100000000) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%1000000000)/100000000));
        else if (spacing >= 9) LoadNetworkAsyncTxBufferSocket(s, '0');
        if (data >= 10000000) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%100000000)/10000000));
        else if (spacing >= 8) LoadNetworkAsyncTxBufferSocket(s, '0');
        if (data >= 1000000) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%10000000)/1000000));
        else if (spacing >= 7) LoadNetworkAsyncTxBufferSocket(s, '0');
        if (data >= 100000) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%1000000)/100000));
        else if (spacing >= 6) LoadNetworkAsyncTxBufferSocket(s, '0');
    }

    //16 bit
    if ((data > 0xFF) || (spacing >= 4))
    {
        if (data >= 10000) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%100000)/10000));
        else if (spacing >= 5) LoadNetworkAsyncTxBufferSocket(s, '0');
        if (data >= 1000) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%10000)/1000));
        else if (spacing >= 4) LoadNetworkAsyncTxBufferSocket(s, '0');
    }

    //8 bit
    if (data >= 100) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%1000)/100));
    else if (spacing >= 3) LoadNetworkAsyncTxBufferSocket(s, '0');
    if (data >= 10) LoadNetworkAsyncTxBufferSocket(s, '0'+((data%100)/10));
    else if (spacing >= 2) LoadNetworkAsyncTxBufferSocket(s, '0');

    LoadNetworkAsyncTxBufferSocket(s, '0'+(data%10));

}

// This is sometimes called from within an interrupt (i.e. _U2TXInterrupt) when sending data.
// In the case of UART2 it takes a copy of the outgoing byte and loads it into a circular buffer
// which will later be asynchonously read in the idle thread to transmit it
void LoadNetworkAsyncTxBufferSrc(eSource src, BYTE data)
{
    BYTE s;

    for (s = 0; s < NumSockets(); s++)
    {
        // selectively load the sockets with routed data instead of loading them all with the same data.
        if (src == MyIpData[s].source)
        {
            LoadNetworkAsyncTxBufferSocket(s, data);
        } // if
    } // for s
}

void LoadNetworkAsyncTxBufferSocket(BYTE s, BYTE data)
{
    if (s >= NumSockets())
        return;

    MyIpData[s].buffer_head++;
    if (MyIpData[s].buffer_head >= TX_BUFFER_SIZE)
        MyIpData[s].buffer_head = 0;
    MyIpData[s].buffer[MyIpData[s].buffer_head] = data;
}

void LoadNetworkAsyncTxBufferSocketArray(BYTE s, BYTE* data, DWORD len)
{
    if (s >= NumSockets())
    return;

    if (len > TX_BUFFER_SIZE)
        len = TX_BUFFER_SIZE;

    while(len--)
    {
        MyIpData[s].buffer_head++;
        if (MyIpData[s].buffer_head >= TX_BUFFER_SIZE)
            MyIpData[s].buffer_head = 0;
        MyIpData[s].buffer[MyIpData[s].buffer_head] = *data++;
    }
}	

// init telemetry variables and states
void InitMyIpData(void)
    {
    int i;
    BYTE s; // socket index
    DWORD tick = TickGet();
    BYTE instanceCount[eSource_MAX];

    for (i = 0; i < eSource_MAX; i++)
    {
            instanceCount[i] = 0;
    }

    for (s = 0; s < NumSockets(); s++)
    {
        if (eTCP == MyIpData[s].type)
            MyIpData[s].socket = INVALID_SOCKET;
        else //if (eUDP == MyIpData[s].type)
            MyIpData[s].socket = INVALID_UDP_SOCKET;

        MyIpData[s].sendPacket = FALSE;
        MyIpData[s].state = eSM_HOME;
        MyIpData[s].buffer_head = 0;
        MyIpData[s].buffer_tail = 0;
        MyIpData[s].connectTimer = tick;
        MyIpData[s].connectRetries = 0;

        for (i = 0; i < TX_BUFFER_SIZE; i++)
        {
            MyIpData[s].buffer[i] = 0;
        }

        // **** do not intialize these, they are done at the MyIpData[]
        //  struct declaration and only shown here for completeness
        //MyIpData[s].source = user set
        //MyIpData[s].port = user set
        //MyIpData[s].serverIP = user set
        //MyIpData[s].type = user set


        switch (MyIpData[s].source)
        {
        #if (NETWORK_USE_UART1 == 1)
         case eSourceUART1:
            MyIpData[s].instance = instanceCount[eSourceUART1]++;
            MyIpInit_UART1(s);
            break;
        #endif

        #if (NETWORK_USE_UART2 == 1)
        case eSourceUART2:
            MyIpData[s].instance = instanceCount[eSourceUART2]++;
            MyIpInit_UART2(s);
            break;
        #endif

        #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED == 1)
        case eSourceFlyByWire:
            MyIpData[s].instance = instanceCount[eSourceFlyByWire]++;
            MyIpInit_FlyByWire(s);
            break;
        #endif

        #if (NETWORK_USE_MAVLINK == 1)
        case eSourceMAVLink:
            MyIpData[s].instance = instanceCount[eSourceMAVLink]++;
            MyIpInit_MAVLink(s);
            break;
        #endif

        #if (NETWORK_USE_DEBUG == 1)
        case eSourceDebug:
            MyIpData[s].instance = instanceCount[eSourceDebug]++;
            MyIpInit_Debug(s);
            break;
        #endif

        #if (NETWORK_USE_ADSB == 1)
        case eSourceADSB:
            MyIpData[s].instance = instanceCount[eSourceADSB]++;
            MyIpInit_ADSB(s);
            break;
        #endif
        
            default:
            break;
        } // switch eSource



        // "s" can be any number which is the socketID assigned by the IP stack.
        // Lets keep track of which s is this instance's first, second, third (ect) so
        // we can use it as an index within the module for timers and such.
        if (MyIpData[s].instance >= MAX_NUM_INSTANCES_OF_MODULES)
        {
            #if defined(STACK_USE_UART)
            // You've assigned more instances of a module than allowed.
            // Increase MAX_NUM_INSTANCES_OF_MODULES in MyIpData.h
            putrsUART((ROM char*)"\r\nERROR, in eSource type ");
            putcUART('0' + MyIpData[s].source);
            putrsUART((ROM char*)". Increase MAX_NUM_INSTANCES_OF_MODULES in MyIpData.h");
            while(BusyUART());
            #endif

            //Reset();
        }
    } // for s
}

// Read the circular buffer head index (written to from _U2TXInterrupt) from
// the "idle thread" in a thread-safe manner.
int MyIpThreadSafeReadBufferHead(BYTE s)
{
    if (s >= NumSockets())
        return 0;

    int head;

    switch (MyIpData[s].source)
    {
    #if (NETWORK_USE_UART1 == 1)
    case eSourceUART1:
        head = MyIpThreadSafeReadBufferHead_UART1(s);
        break;
    #endif

    #if (NETWORK_USE_UART2 == 1)
    case eSourceUART2:
        head = MyIpThreadSafeReadBufferHead_UART2(s);
        break;
    #endif

    #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED == 1)
    case eSourceFlyByWire:
        head = MyIpThreadSafeReadBufferHead_FlyByWire(s);
        break;
    #endif

    #if (NETWORK_USE_MAVLINK == 1)
    case eSourceMAVLink:
        head = MyIpThreadSafeReadBufferHead_MAVLink(s);
        break;
    #endif

    #if (NETWORK_USE_DEBUG == 1)
    case eSourceDebug:
        head = MyIpThreadSafeReadBufferHead_Debug(s);
        break;
    #endif

    #if (NETWORK_USE_ADSB == 1)
    case eSourceADSB:
        head = MyIpThreadSafeReadBufferHead_ADSB(s);
        break;
    #endif

    default:
        head = 0;
        break;
    }
    return head;
}

void MyIpSetSendPacketFlagSrc(eSource src)
{
    BYTE s;

    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            MyIpData[s].sendPacket = TRUE;
        }
    } // for
}

void MyIpSetSendPacketFlagSocket(BYTE s)
{
    if (s >= NumSockets())
        return;
    MyIpData[s].sendPacket = TRUE;
}


// Read the End-Of-Line flag (set in _U2TXInterrupt) and clear it from
// the "idle thread" in a thread-safe manner
BOOL MyIpThreadSafeSendPacketCheck(BYTE s, BOOL doClearFlag)
{
    if (s >= NumSockets())
        return FALSE;

    BOOL sendpacket;

    switch (MyIpData[s].source)
    {
    #if (NETWORK_USE_UART1 == 1)
    case eSourceUART1:
        sendpacket = MyIpThreadSafeSendPacketCheck_UART1(s, doClearFlag);
        break;
    #endif

    #if (NETWORK_USE_UART2 == 1)
    case eSourceUART2:
        sendpacket = MyIpThreadSafeSendPacketCheck_UART2(s, doClearFlag);
        break;
    #endif

    #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED == 1)
    case eSourceFlyByWire:
        sendpacket = MyIpThreadSafeSendPacketCheck_FlyByWire(s, doClearFlag);
        break;
    #endif

    #if (NETWORK_USE_MAVLINK == 1)
    case eSourceMAVLink:
        sendpacket = MyIpThreadSafeSendPacketCheck_MAVLink(s, doClearFlag);
        break;
    #endif

    #if (NETWORK_USE_DEBUG == 1)
    case eSourceDebug:
        sendpacket = MyIpThreadSafeSendPacketCheck_Debug(s, doClearFlag);
        break;
    #endif

    #if (NETWORK_USE_ADSB == 1)
    case eSourceADSB:
        sendpacket = MyIpThreadSafeSendPacketCheck_ADSB(s, doClearFlag);
        break;
    #endif

    default:
        sendpacket = TRUE; // default TRUE so we trigger a send of whatever it is
        break;
    } // switch src

    return sendpacket;
}

void ServiceMyIpData(BYTE s)
{
    switch (MyIpData[s].source)
    {
    #if (NETWORK_USE_UART1 == 1)
    case eSourceUART1:
        MyIpService_UART1(s);
        break;
    #endif

    #if (NETWORK_USE_UART2 == 1)
    case eSourceUART2:
        MyIpService_UART2(s);
        break;
    #endif

    #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED == 1)
    case eSourceFlyByWire:
        MyIpService_FlyByWire(s);
        break;
    #endif

    #if (NETWORK_USE_MAVLINK == 1)
    case eSourceMAVLink:
        MyIpService_MAVLink(s);
        break;
    #endif

    #if (NETWORK_USE_DEBUG == 1)
    case eSourceDebug:
        MyIpService_Debug(s);
        break;
    #endif

    #if (NETWORK_USE_ADSB == 1)
    case eSourceADSB:
        MyIpService_ADSB(s);
        break;
    #endif
    
    default:
        break;
    } // switch .source
}

BYTE Get_TCP_PURPOSE(eSource src)
{
    switch (src)
    {
        default:
        case eSourceUART1: return TCP_PURPOSE_MYIPDATA_UART1;
        case eSourceUART2: return TCP_PURPOSE_MYIPDATA_UART2;
        case eSourceFlyByWire: return TCP_PURPOSE_MYIPDATA_FLYBYWIRE;
        case eSourceMAVLink: return TCP_PURPOSE_MYIPDATA_MAVLINK;
        case eSourceDebug: return TCP_PURPOSE_MYIPDATA_DEBUG;
        case eSourceADSB: return TCP_PURPOSE_MYIPDATA_ADSB;
    }
}	
// Service the Telemetry system by checking for a TCP connection
// and then sending/recieveing data from the network accordingly
BOOL ServiceMyIpTCP(BYTE s, BOOL isLinked)
{
    //if (s >= NumSockets())
    //	return;

    BYTE TCPpurpose;
    BOOL isConnected = FALSE;

    if (eTCP != MyIpData[s].type)
        return FALSE;

    // Handle session state
    switch(MyIpData[s].state)
    {
    case eSM_HOME:
        TCPpurpose = Get_TCP_PURPOSE(MyIpData[s].source);

        if (NULL == MyIpData[s].serverIP)
        {
            // We are the server, start listening
            MyIpData[s].socket = TCPOpen((DWORD)(PTR_BASE)0, TCP_OPEN_SERVER, MyIpData[s].port, TCPpurpose);
        }
        else
        {
            // Client Mode: Connect a socket to the remote TCP server
            MyIpData[s].socket = TCPOpen((DWORD)(PTR_BASE)MyIpData[s].serverIP, TCP_OPEN_ROM_HOST, MyIpData[s].port, TCPpurpose);
        }

        // Abort operation if no TCP socket of type TCP_PURPOSE_MYIPDATA_xxxxxx is available
        // If this ever happens, you need to go add one to TCPIPConfig.h
        if (INVALID_SOCKET == MyIpData[s].socket)
        {
            #if defined(STACK_USE_UART)
            putrsUART((ROM char*)"\r\nERROR, not enough TCP_PURPOSE_MYIPDATA_ type ");
            putcUART('0' + (TCPpurpose / 10));
            putcUART('0' + (TCPpurpose % 10));
            putrsUART((ROM char*)", add more in TCPIPConfig.h");
            while(BusyUART());
            #endif

            //Reset();
        }


        // Eat the first TCPWasReset() response so we don't
        // infinitely create and reset/destroy client mode sockets
        TCPWasReset(MyIpData[s].socket);
        MyIpData[s].state++;
        MyIpData[s].connectTimer = TickGet();
        break;

    case eSM_SOCKET_OBTAINED:
        if (TCPIsPutReady(MyIpData[s].socket))
        {
            MyIpData[s].state++;
            MyIpOnConnect(s);
        }
        /*
        // TODO Fix this timeout mechanism. The problem is probably in the disconnect method
        else if ((NULL != MyIpData[s].serverIP) && // is a client
                (TickGet() - MyIpData[s].connectTimer) > TCP_TELEMETRY_CONNECT_TIMEOUT))	// is timed out
        {
            // if our DNS takes too long to resolve then lets try again
            // If we are a client socket, close the socket and attempt to reconnect
            TCPDisconnect(MyIpData[s].socket);
            TCPDisconnect(MyIpData[s].socket);
            MyIpData[s].socket = INVALID_SOCKET;
            MyIpData[s].state = eSM_HOME;
            MyIpData[s].connectRetries++;
        }
        */
        break;

    case eSM_CONNECTED:
        isConnected = TRUE;
        if (!isLinked || TCPWasReset(MyIpData[s].socket))
        {
            if (NULL == MyIpData[s].serverIP)
            {
                // If we are a server socket, go back to listening
                MyIpData[s].state = eSM_SOCKET_OBTAINED;
            }
            else
            {
                // TODO Fix this disconnect mechanism
                // If we are a client socket, close the socket and attempt to reconnect
                TCPDisconnect(MyIpData[s].socket);
                TCPDisconnect(MyIpData[s].socket);
                MyIpData[s].socket = INVALID_SOCKET;
                MyIpData[s].state = eSM_HOME;
            }
            break;
        }
        else
        {
            SendAsyncTxData_Bulk(s); 		// fill IP packet via array writes (efficient and complicated)
            //SendAsyncTxData_Single(s);	// fill IP packet via repeated byte writes (slow and simple)

            if (MyIpThreadSafeSendPacketCheck(s,TRUE))
            {
                TCPFlush(MyIpData[s].socket);
            }

            // Process Incoming data
            MyIpProcessRxData(s);
        }
        break;
    } // switch

    return isConnected;
}


// Service the Telemetry system by checking for a TCP connection
// and then sending/recieveing data from the network accordingly
void ServiceMyIpUDP(BYTE s)
{
    //if (s >= NumSockets())
    //	return;

    if (eUDP != MyIpData[s].type)
        return;


#if defined(STACK_USE_DHCP_CLIENT)
    // Wait until DHCP module is finished
    if (!DHCPIsBound(0))
    {
        return;
    }
#endif

    switch(MyIpData[s].state)
    {
    case eSM_HOME:
        if (NULL == MyIpData[s].serverIP)
        {
            // Server mode, also known as SPAM mode
            // Open a UDP socket for outbound transmission
            MyIpData[s].socket = UDPOpenEx(0,UDP_OPEN_SERVER,MyIpData[s].port,MyIpData[s].port);
        }
        else
        {
            // Client mode, connect to remote listening server.
            MyIpData[s].socket = UDPOpenEx((DWORD)(PTR_BASE)MyIpData[s].serverIP,UDP_OPEN_ROM_HOST,MyIpData[s].port,MyIpData[s].port);
        }

        // Abort operation if no UDP sockets are available
        // If this ever happens, incrementing MAX_UDP_SOCKETS in
        // StackTsk.h may help (at the expense of more global memory
        // resources).
        if (INVALID_UDP_SOCKET != MyIpData[s].socket)
        {
            MyIpData[s].state++;
            MyIpData[s].connectTimer = TickGet();
        }
        else
        {
            #if defined(STACK_USE_UART)
            // If the port never gets a socket, increment MAX_UDP_SOCKETS.
            putrsUART((ROM char*)"\r\nERROR!!! Not enough MAX_UDP_SOCKETS available, increase them in TCPIPConfig.h");
            while(BusyUART());
            #endif

            //Reset();
        }
        break;

    case eSM_SOCKET_OBTAINED:
        if (UDPIsPutReady(MyIpData[s].socket))
        {
                MyIpData[s].state++;
                MyIpOnConnect(s);
        }
        /*
        else if ((NULL != MyIpData[s].serverIP) && // is a client
                        (TickGet() - MyIpData[s].connectTimer) > UDP_TELEMETRY_CONNECT_TIMEOUT))	// is timed out
        {
                // if our DNS takes too long to resolve then lets try again
                // If we are a client socket, close the socket and attempt to reconnect
                UDPClose(MyIpData[s].socket);
                MyIpData[s].socket = INVALID_UDP_SOCKET;
                MyIpData[s].state = eSM_HOME;
                MyIpData[s].connectRetries++;
        }
        */
        break;

    case eSM_CONNECTED:
        SendAsyncTxData_Bulk(s); 		// fill IP packet via array writes (efficient and complicated)
        //SendAsyncTxData_Single(s);	// fill IP packet via repeated byte writes (slow and simple)

        // Process Incoming data
        MyIpProcessRxData(s);

        // Send the packet
        if (MyIpThreadSafeSendPacketCheck(s,TRUE))
        {
            UDPFlush();
        }
        break;
    } // switch
}

void MyIpOnConnect(BYTE s)
{
    //if (s >= NumSockets())
    //	return;

    switch (MyIpData[s].source)
    {
    #if (NETWORK_USE_UART1 == 1)
    case eSourceUART1:
        MyIpData[s].buffer_tail = MyIpThreadSafeReadBufferHead_UART1(s);
        MyIpOnConnect_UART1(s);
        break;
    #endif

    #if (NETWORK_USE_UART2 == 1)
    case eSourceUART2:
        MyIpData[s].buffer_tail = MyIpThreadSafeReadBufferHead_UART2(s);
        MyIpOnConnect_UART2(s);
        break;
    #endif

    #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED == 1)
    case eSourceFlyByWire:
        MyIpData[s].buffer_tail = MyIpThreadSafeReadBufferHead_FlyByWire(s);
        MyIpOnConnect_FlyByWire(s);
        break;
    #endif

    #if (NETWORK_USE_MAVLINK == 1)
    case eSourceMAVLink:
        MyIpData[s].buffer_tail = MyIpThreadSafeReadBufferHead_MAVLink(s);
        MyIpOnConnect_MAVLink(s);
        break;
    #endif

    #if (NETWORK_USE_DEBUG == 1)
    case eSourceDebug:
        MyIpData[s].buffer_tail = MyIpThreadSafeReadBufferHead_Debug(s);
        MyIpOnConnect_Debug(s);
        break;
    #endif

    #if (NETWORK_USE_ADSB == 1)
    case eSourceADSB:
        MyIpData[s].buffer_tail = MyIpThreadSafeReadBufferHead_ADSB(s);
        MyIpOnConnect_ADSB(s);
        break;
    #endif

    default:
        break;
    } // switch source
}

void MyIpProcessRxData(BYTE s)
{
    //if (s >= NumSockets())
    //	return;

    switch (MyIpData[s].source)
    {
    #if (NETWORK_USE_UART1 == 1)
    case eSourceUART1:
        MyIpProcessRxData_UART1(s);
        break;
    #endif

    #if (NETWORK_USE_UART2 == 1)
    case eSourceUART2:
        MyIpProcessRxData_UART2(s);
        break;
    #endif

    #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED == 1)
    case eSourceFlyByWire:
        MyIpProcessRxData_FlyByWire(s);
        break;
    #endif

    #if (NETWORK_USE_MAVLINK == 1)
    case eSourceMAVLink:
        MyIpProcessRxData_MAVLink(s);
        break;
    #endif

    #if (NETWORK_USE_DEBUG == 1)
    case eSourceDebug:
        MyIpProcessRxData_Debug(s);
        break;
    #endif

    #if (NETWORK_USE_ADSB == 1)
    case eSourceADSB:
        MyIpProcessRxData_ADSB(s);
        break;
    #endif

    default:
        break;
    } // switch source
}	

// fill IP packet via array writes (efficient and complicated)
void SendAsyncTxData_Bulk(BYTE s)
{
    //if (s >= NumSockets())
    //	return;

    int wrote = 0;
    int tx_buffer_available = 0;
    int head, index, len;

    head = MyIpThreadSafeReadBufferHead(s);
    if (MyIpData[s].buffer_tail != head)
    {
        if (eTCP == MyIpData[s].type)
        {
            tx_buffer_available = TCPIsPutReady(MyIpData[s].socket);
        }
        else //if (eUDP == MyIpData[s].type)
        {
            tx_buffer_available = UDPIsPutReady(MyIpData[s].socket);
        }
    }

    // if we're not connected then IsPutReady will return 0. Send data if we can
    while ((MyIpData[s].buffer_tail != head) && (tx_buffer_available > 0))
    {
        index = MyIpData[s].buffer_tail + 1;

        if (MyIpData[s].buffer_tail < head) // usual (easy) case
        {
            len = head - MyIpData[s].buffer_tail;
            if (eTCP == MyIpData[s].type)
            {
                wrote = TCPPutArray(MyIpData[s].socket, &MyIpData[s].buffer[index], len);
            }
            else //if (eUDP == MyIpData[s].type)
            {
                wrote = UDPPutArray(&MyIpData[s].buffer[index], len);
            }
        }
        else // circular buffer is wrapping around
        {
            if (MyIpData[s].buffer_tail >= (TX_BUFFER_SIZE-1))
            {
                // only one byte before the end, just put it in by itself (single byte Put)
                if (eTCP == MyIpData[s].type)
                {
                    wrote = TCPPut(MyIpData[s].socket, MyIpData[s].buffer[0]);
                }
                else //if (eUDP == MyIpData[s].type)
                {
                    wrote = UDPPut(MyIpData[s].buffer[0]);
                }
            }
            else
            {
                // fill from tail to end-of-buffer, let the next pass get the lower 0-to-head portion
                len = TX_BUFFER_SIZE - MyIpData[s].buffer_tail - 1;
                if (eTCP == MyIpData[s].type)
                {
                    wrote = TCPPutArray(MyIpData[s].socket, &MyIpData[s].buffer[index], len);
                }
                else //if (eUDP == MyIpData[s].type)
                {
                    wrote = UDPPutArray(&MyIpData[s].buffer[index], len);
                }
            }
        }

        if (wrote)
        {
            tx_buffer_available -= wrote;
            MyIpData[s].buffer_tail += wrote;
            if (MyIpData[s].buffer_tail >= TX_BUFFER_SIZE)
            {
                MyIpData[s].buffer_tail = 0;
            }
        }
        else
        {
            // something is wrong, we tried to write and failed so lets stop trying
            tx_buffer_available = 0;
        }
        head = MyIpThreadSafeReadBufferHead(s); // refresh
    } // while
}

/*
// fill IP packet via repeated byte writes (slow and simple)
void SendAsyncTxData_Single(BYTE s)
{
    //if (s >= NumSockets())
    //	return;

    int head, index;
    BYTE txData;

    // This is only needed for UDP to set the active socket
    if (eUDP == MyIpData[s].type)
    {
        if (FALSE == UDPIsPutReady(MyIpData[s].socket))
                return;
    }

    head = MyIpThreadSafeReadBufferHead(s);
    while (MyIpData[s].buffer_tail != head) // if there's data to write
    {
        index = MyIpData[s].buffer_tail + 1;
        if (index >= TX_BUFFER_SIZE)
        {
            index = 0;
        }

        txData = MyIpData[s].buffer[index];
        // write if we can
        if (eTCP == MyIpData[s].type)
        {
            if (FALSE == TCPPut(MyIpData[s].socket, txData))
                return;
        }
        else //if (eUDP == MyIpData[s].type)
        {
            // socket set by the above UDPIsPutReady() call
            if (FALSE == UDPPut(txData))
                return;
        }

        // write was successful, increment indexes
        MyIpData[s].buffer_tail = index;
        // refresh the head index in case new data was just added
        head = MyIpThreadSafeReadBufferHead(s);
    } // while
}
*/
	
#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPDATA_C_


