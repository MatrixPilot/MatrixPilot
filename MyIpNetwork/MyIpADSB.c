#ifndef _MYIPADSB_C_
#define _MYIPADSB_C_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_ADSB == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "defines.h"
#include "MyIpData.h"
#include "MyIpADSB.h"
#include "euler_angles.h"

//////////////////////////
// Module Variables
DWORD taskTimer1_ADSB[MAX_NUM_INSTANCES_OF_MODULES];
DWORD taskTimer2_ADSB[MAX_NUM_INSTANCES_OF_MODULES];

static ROM BYTE CALLSIGN[] = "ADS-B CallSign";

typedef struct {
    char callSign[sizeof (CALLSIGN)];
    long gpsLat;
    long gpsLong;
    long heading;
    long altitude;
    long groundSpeed;
    long climbRate;
} MyIpADSBtype;

void MyIpOnConnect_ADSB(BYTE s) {
    // Print any one-time connection annoucement text
    LoadStringSocket(s, "\r\nYou've connected to ADSB on "); // 33 chars
    LoadStringSocket(s, ID_LEAD_PILOT); // 15ish chars
    LoadStringSocket(s, "'s aircraft. More info at "); // 26 chars
    LoadStringSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    LoadStringSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_ADSB(BYTE s) {
    // This gets called once for every socket we're configured to use for this module.
    BYTE i = MyIpData[s].instance;
    taskTimer1_ADSB[i] = GenerateRandomDWORD() % (TICK_SECOND);
    taskTimer2_ADSB[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_ADSB(BYTE s) {
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;

    BYTE i = MyIpData[s].instance;
    MyIpADSBtype data;

    memcpy(data.callSign, CALLSIGN, sizeof (CALLSIGN));
    data.gpsLat = lat_gps.WW;
    data.gpsLong = long_gps.WW;
    data.heading = get_geo_heading_angle();
    data.altitude = alt_sl_gps.WW;
    data.groundSpeed = air_speed_3DIMU;
    data.climbRate = 156; // are we calculating this anywhere?

    // PACKET # 1 (even)
    if (MyIpData[s].port == 3001) {
        // Option 1, sending a structure
        if ((TickGet() - taskTimer1_ADSB[i]) > ((TICK_SECOND) / 2)) // 2Hz
        {
            taskTimer1_ADSB[i] = TickGet();
            LoadNetworkAsyncTxBufferSocketArray(s, (BYTE*) & data, sizeof (data));
            MyIpData[s].sendPacket = TRUE;
        }
    } else if (MyIpData[s].port == 3002) {
        // Option 2, sending a packet organized however we'd like
        if ((TickGet() - taskTimer1_ADSB[i]) > ((TICK_SECOND) / 2))// 2Hz
        {
            taskTimer1_ADSB[i] = TickGet();

            // sending an encoded bitstream (example)
            LoadNetworkAsyncTxBufferSocket(s, 0xAA); // some sort of header
            LoadNetworkAsyncTxBufferSocket(s, 0xAB); // some sort of header
            LoadNetworkAsyncTxBufferSocket(s, 0xAC); // some sort of header
            LoadNetworkAsyncTxBufferSocket(s, sizeof (data)); // data length

            LoadNetworkAsyncTxBufferSocketArray(s, (BYTE*) data.callSign, sizeof (data.callSign));
            /*
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[0]);
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[1]);
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[2]);
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLat.v[3]);
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[0]);
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[1]);
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[2]);
            LoadNetworkAsyncTxBufferSocket(s, data.gpsLong.v[3]);
            LoadNetworkAsyncTxBufferSocket(s, data.heading.v[0]);
            LoadNetworkAsyncTxBufferSocket(s, data.heading.v[1]);
            LoadNetworkAsyncTxBufferSocket(s, data.altitude.v[0]);
            LoadNetworkAsyncTxBufferSocket(s, data.altitude.v[1]);
            LoadNetworkAsyncTxBufferSocket(s, data.groundSpeed.v[0]);
            LoadNetworkAsyncTxBufferSocket(s, data.groundSpeed.v[1]);
            LoadNetworkAsyncTxBufferSocket(s, data.climbRate);
            */

            LoadNetworkAsyncTxBufferSocket(s, 0xAD); // some sort of footer, maybe CRC or end-of-packet flag?
            LoadNetworkAsyncTxBufferSocket(s, 0xAE); // some sort of footer, maybe CRC or end-of-packet flag?
            MyIpData[s].sendPacket = TRUE;
        }
    } else if (MyIpData[s].port == 3003) {
        // Option 3, ASCII data which is human readable (great for debugging)
        if ((TickGet() - taskTimer1_ADSB[i]) > ((TICK_SECOND) / 2)) // 2Hz
        {
            taskTimer1_ADSB[i] = TickGet();

            //LoadNetworkAsyncTxBufferSocket(s, 12); //12 is a form feed
            //LoadStringSocket(s, "even packet\r\n\r\n");

            LoadStringSocket(s, data.callSign); LoadNetworkAsyncTxBufferSocket(s, ',');
            itoaSocket(s,data.gpsLat); LoadNetworkAsyncTxBufferSocket(s, ',');
            itoaSocket(s, data.gpsLong); LoadNetworkAsyncTxBufferSocket(s, ',');
            itoaSocket(s, data.heading);  LoadNetworkAsyncTxBufferSocket(s, ',');
            itoaSocket(s, data.altitude); LoadNetworkAsyncTxBufferSocket(s, ',');
            itoaSocket(s, data.groundSpeed); LoadNetworkAsyncTxBufferSocket(s, ',');
            itoaSocket(s, data.climbRate); LoadStringSocket(s, "\r\n");
            
        }
    }


    // Packet #2 (odd)
    /*
    if ((TickGet() - taskTimer2_ADSB[i]) > (TICK_SECOND)) // 1Hz
    {
        taskTimer2_ADSB[i] = TickGet();
        // generate other data in a different packet at a different time interval
        LoadStringSocket(s, "odd packet\r\n");
    }
    */
}

BOOL MyIpThreadSafeSendPacketCheck_ADSB(BYTE s, BOOL doClearFlag) {
    // since this data comes from, and goes to, the idle thread we
    // don't need to deal with any thread issues
    BOOL sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag) {
        MyIpData[s].sendPacket = FALSE;
    }
    return sendpacket;
}

int MyIpThreadSafeReadBufferHead_ADSB(BYTE s) {
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_ADSB(BYTE s) {
    BYTE rxData;
    BOOL successfulRead;

    do {
        if (eTCP == MyIpData[s].type) {
            successfulRead = TCPGet(MyIpData[s].socket, &rxData);
        } else //if (eUDP == MyIpData[s].type)
        {
            successfulRead = UDPGet(&rxData);
        }

        if (successfulRead) {
            // do something with rxData
            // No Rx data parsing implemented
        }
    } while (successfulRead);
}


#endif // (NETWORK_USE_ADSB == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPADSB_C_

