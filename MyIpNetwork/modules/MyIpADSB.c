#ifndef _MYIPADSB_C_
#define _MYIPADSB_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_ADSB == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpADSB.h"
#include "euler_angles.h"
#include "MyIpHelpers.h"

//////////////////////////
// Module Variables
ROM uint8_t CALLSIGN[] = "CaBs-MatrixPilot";

typedef struct {
    int8_t callSign[sizeof (CALLSIGN)];
    int32_t gpsLat;
    int32_t gpsLong;
    int32_t heading;
    int32_t altitude;
    int32_t groundSpeed;
    int32_t climbRate;
} MyIpADSBtype;

uint32_t taskTimer1_ADSB[MAX_NUM_INSTANCES_OF_MODULES];
MyIpADSBtype ADSB_logicalData;

void MyIpOnConnect_ADSB(uint8_t s) {
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to ADSB on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_ADSB(uint8_t s) {
    // This gets called once for every socket we're configured to use for this module.
    uint8_t i = MyIpData[s].instance;
    taskTimer1_ADSB[i] = GenerateRandomDWORD() % (TICK_SECOND);

    // write this once, it never changes
    memcpy(ADSB_logicalData.callSign, CALLSIGN, sizeof (CALLSIGN));
}

void MyIpService_ADSB(uint8_t s) {
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;

    uint8_t i = MyIpData[s].instance;

    // Option 3, ASCII data which is human readable (great for debugging)
    if ((TickGet() - taskTimer1_ADSB[i]) > TICK_SECOND) // 2Hz
    {
        taskTimer1_ADSB[i] = TickGet();

        // Create logical data.

        // ------------- THREAD SAFE READS -----------
        //_T7IE = 0; // (_TTRIGGERIE) disable Timer7 for background task
        // TODO Should we be using the GPS values or the IMU values for this?
        ADSB_logicalData.gpsLat = lat_gps.WW;
        ADSB_logicalData.gpsLong = lon_gps.WW;
        ADSB_logicalData.altitude = alt_sl_gps.WW;
        //_T7IE = 1;

        //_T6IE = 0; // (_THEARTBEATIE) disable Timer6 for PWM task
        // TODO This should be changed because if another IRQ (such as U2TxISR)
        // fires then we coudl be stuck here with T6 disabled for a while
        ADSB_logicalData.groundSpeed = air_speed_3DIMU;
        //_T6IE = 1;

        // ----- *** NOT THREAD SAFE READS *** -------
        ADSB_logicalData.heading = get_geo_heading_angle();
        ADSB_logicalData.climbRate = 156; //climb_gps.BB;
        // -------------------------------------------


        StringToSocket(s, ADSB_logicalData.callSign); ByteToSocket(s, ',');
        ltoaSocket(s, ADSB_logicalData.gpsLat); ByteToSocket(s, ',');
        ltoaSocket(s, ADSB_logicalData.gpsLong); ByteToSocket(s, ',');
        ltoaSocket(s, ADSB_logicalData.heading);  ByteToSocket(s, ',');
        ltoaSocket(s, ADSB_logicalData.altitude); ByteToSocket(s, ',');
        ltoaSocket(s, ADSB_logicalData.groundSpeed); ByteToSocket(s, ',');
        ltoaSocket(s, ADSB_logicalData.climbRate); StringToSocket(s, "\r\n");

    }

}

boolean MyIpThreadSafeSendPacketCheck_ADSB(uint8_t s, boolean doClearFlag) {
    // since this data comes from, and goes to, the idle thread we
    // don't need to deal with any thread issues
    boolean sendpacket = MyIpData[s].sendPacket;
    if (doClearFlag) {
        MyIpData[s].sendPacket = FALSE;
    }
    return sendpacket;
}

int16_t MyIpThreadSafeReadBufferHead_ADSB(uint8_t s) {
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_ADSB(uint8_t s) {
    uint8_t rxData;
    boolean successfulRead;

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


#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_ADSB == 1)
#endif // _MYIPADSB_C_

