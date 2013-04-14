#ifndef _MYIPXPLANE_C_
#define _MYIPXPLANE_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_XPLANE == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpXPlane.h"
#include "MyIpHelpers.h"

// For info about the X Plane UDP protocol, see this file in your xplane folder
// C:\X-Plane 10\Instructions\Sending Data to X-Plane.html
typedef int8_t xchr;
typedef int32_t xint;
typedef float xflt;
typedef long double xdob;
#define strDIM      500
#define vehDIM      10
#define XPLANE_PACKET_HEADER_SIZE    5

typedef struct __attribute__((aligned(4), packed)) {
    xint index; // data index, the index into the list of variables you can output from the Data Output screen in X-Plane.
    xflt data[8]; // the up to 8 numbers you see in the data output screen associated with that selection.. many outputs do not use all 8, though.
}data_struct;

typedef struct __attribute__((aligned(4), packed)) { // play any sound
    xflt freq,vol;
    xchr path[strDIM];
} soun_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xflt var;
    xchr dref_path[strDIM];
} dref_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xchr apt_ID[5] ; // ID to go to, null-termed
    xint type_start ; // 601 for takeoff, 602 for final approach, 603 for far-away final approach
    xint lcl_rwy_ram; // which runway or ramp-start location to use, starting at 0 for each airport
    xint backwards ; // 0 or 1, depending on if you want to appraoch from the other direction
} papt_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xint p;
    xchr path_rel[150];
    xchr att_file[24][40];
}vehN_struct;

typedef struct __attribute__((aligned(4), packed)) { // byte-align 4
    xint p; // this is the plane you wish to control.. 0 for your plane, but you can enter up to 9 here
    xdob lat_lon_ele[3];
    xflt psi_the_phi[3];
    xflt gear_flap_vect[3];
}VEH1_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xint num_p;

    xdob lat_lon_ele[vehDIM][3];
    xflt psi_the_phi[vehDIM][3];
    xflt gear_flap_vect[vehDIM][3];

    xdob lat_view,lon_view,ele_view;
    xflt psi_view,the_view,phi_view;
}vehA_struct;

typedef struct __attribute__((aligned(4), packed)) {	 // object name: draw any object in the world in the sim
    xint index;
    xchr path[strDIM];
}objN_struct;

typedef struct __attribute__((aligned(4), packed)) {
    xint index; // (0->20 in the lsit below)
    xchr str_ipad_them[16]; // IP's we are sending to, in english
    xchr str_port_them[6]; // ports are easier to work with in STRINGS!
    xint use_ip ; // to use this option, 0 not to.
}iset_struct;

void SendXplanePacket(uint8_t s);

//////////////////////////
// Module Variables
uint32_t taskTimer_XPlane[MAX_NUM_INSTANCES_OF_MODULES];

void MyIpOnConnect_XPlane(const BYTE s)
{
    /*
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to XPlane on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
    */
}

void MyIpInit_XPlane(const BYTE s)
{
    // This gets called once for every socket we're configured to use for this module.
    BYTE i = MyIpData[s].instance;
    taskTimer_XPlane[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_XPlane(const BYTE s)
{
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;

    uint8_t i = MyIpData[s].instance;

    if ((TickGet() - taskTimer_XPlane[i]) > ((TICK_SECOND)/10)) // 10Hz
    {
        taskTimer_XPlane[i] = TickGet();
        SendXplanePacket(s);
    }
}

boolean MyIpThreadSafeSendPacketCheck_XPlane(const uint8_t s, boolean doClearFlag)
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


int MyIpThreadSafeReadBufferHead_XPlane(const uint8_t s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_XPlane(const uint8_t s)
{
    uint8_t rxData;
    boolean successfulRead;
    //boolean didRead = false;

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
            //didRead = true;
            // No Rx data parsing implemented
        }
    } while (successfulRead);

    //if (didRead)
    //    SendXplanePacket(s);
}

void SendXplanePacket(uint8_t s)
{
    int dataCount = 0;
    static int packetCount = 0;
// { -1219950467, 374124664, 2.00 }	// A point in Baylands Park in Sunnyvale, CA
#if 0
    uint8_t i;
    data_struct packet;

    packet.index = 20; // lat, long, alt
    packet.data[0] = 37.4124664; //((double)lat_gps.WW)/10000000;
    packet.data[1] = -121.9950467; //((double)long_gps.WW)/10000000;
    packet.data[2] = 10.00; //(double)alt_sl_gps.WW; // meters?
    for (i=3;i<8;i++)
        packet.data[i] = 0;//-999;
    ByteToSocket(s, 'D'); dataCount++;
    ByteToSocket(s, 'A'); dataCount++;
    ByteToSocket(s, 'T'); dataCount++;
    ByteToSocket(s, 'A'); dataCount++;
    ByteToSocket(s, '0'); dataCount++;
    //ByteToSocket(s, '0'); dataCount++; // some silly MAC compatibilty thing, I think it's for older versions
    ArrayToSocket(s, (BYTE*)&packet,sizeof(packet));
    MyIpData[s].sendPacket = TRUE;
    //ByteToSocket(s, '0'); dataCount++; // some silly MAC compatibilty thing, I think it's for older versions

#elif 1

    uint8_t i;
    VEH1_struct packet;

    packet.p = 0;
    packet.lat_lon_ele[0] = 37.4124664; //((double)lat_gps.WW)/10000000;
    packet.lat_lon_ele[1] = -121.9950467; //((double)long_gps.WW)/10000000;
    packet.lat_lon_ele[2] = 10.00; //(double)alt_sl_gps.WW; // meters?

    for (i=0;i<3;i++)
    {
        packet.psi_the_phi[i] = 0;
        packet.gear_flap_vect[i] = 0;
    }

    ByteToSocket(s, 'V'); dataCount++;
    ByteToSocket(s, 'E'); dataCount++;
    ByteToSocket(s, 'H'); dataCount++;
    ByteToSocket(s, '1'); dataCount++;
    ByteToSocket(s, '0'); dataCount++;
    //ByteToSocket(s, '0'); dataCount++; // some silly MAC compatibilty thing, I think it's for older versions
    ArrayToSocket(s, (BYTE*)&packet,sizeof(packet));
    MyIpData[s].sendPacket = TRUE;
    //ByteToSocket(s, '0'); dataCount++; // some silly MAC compatibilty thing, I think it's for older versions

#endif

#if (NETWORK_USE_DEBUG == 1)
    uitoaSrc(eSourceDebug, packetCount);
    StringToSrc(eSourceDebug, " Sending ");
    uitoaSrc(eSourceDebug, sizeof(packet) +  dataCount);
    StringToSrc(eSourceDebug, " bytes\r\n");
#endif
}

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
#endif // _MYIPXPLANE_C_

