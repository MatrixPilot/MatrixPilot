#ifndef _MYIPDEBUG_C_
#define _MYIPDEBUG_C_

#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)

#include "TCPIP_Stack/TCPIP.h"
#include "MyIpData.h"
#include "MyIpDebug.h"
#include "MyIpHelpers.h"

#if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
#include "airspeedPitot.h"
void DebugAirspeedPitotTerminalOutput(const uint8_t s);
void DebugAirspeedPitotTerminalInput(const uint8_t s, const uint8_t rxData);
#endif

#if (NETWORK_INTERFACE == NETWORK_INTERFACE_WIFI_MRF24WG)
    void DebugWiFiStatusTerminalOutput(const uint8_t s);
#endif

#if (NETWORK_USE_FLYBYWIRE == 1)
void DebugFlyByWireTerminalOutput(const uint8_t s);
#endif


//////////////////////////
// Module Variables
uint32_t taskTimer_Debug[MAX_NUM_INSTANCES_OF_MODULES];
uint32_t dwTime_Debug[MAX_NUM_INSTANCES_OF_MODULES];


void MyIpOnConnect_Debug(const uint8_t s)
{
    // Print any one-time connection annoucement text
    StringToSocket(s, "\r\nYou've connected to Debug on "); // 33 chars
    StringToSocket(s, ID_LEAD_PILOT); // 15ish chars
    StringToSocket(s, "'s aircraft. More info at "); // 26 chars
    StringToSocket(s, ID_DIY_DRONES_URL); // 45ish chars
    StringToSocket(s, "\r\n"); // 2 chars
    MyIpData[s].sendPacket = TRUE; // send right away
}

void MyIpInit_Debug(const uint8_t s)
{
    // This gets called once for every socket we're configured to use for this module.
    BYTE i = MyIpData[s].instance;
    taskTimer_Debug[i] = GenerateRandomDWORD() % (TICK_SECOND);
    dwTime_Debug[i] = GenerateRandomDWORD() % (TICK_SECOND);
}

void MyIpService_Debug(const uint8_t s)
{
    // don't bother queuing data if no one is listening
    if (FALSE == MyIpIsConnectedSocket(s))
        return;

#if (NETWORK_INTERFACE == NETWORK_INTERFACE_WIFI_MRF24WG)
  //  DebugWiFiStatusTerminalOutput(s);
#endif

#if (NETWORK_USE_FLYBYWIRE == 1)
//    DebugFlyByWireTerminalOutput(s);
#endif

#if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
    DebugAirspeedPitotTerminalOutput(s);
#endif
}

#if (NETWORK_INTERFACE == NETWORK_INTERFACE_WIFI_MRF24WG)
void DebugWiFiStatusTerminalOutput(const uint8_t s)
{
    // WF_TxPowerGetFactoryMax
    static uint32_t taskTimer_WiFiStatus[MAX_NUM_INSTANCES_OF_MODULES] = {0,0,0};
    static uint16_t sendCount[MAX_NUM_INSTANCES_OF_MODULES] = {0,0,0};
    uint8_t i = MyIpData[s].instance;
    uint8_t rssi;
    int8_t maxTxPower, factoryTxPower;

    if ((TickGet() - taskTimer_WiFiStatus[i]) > ((TICK_SECOND)*10))
    {
        taskTimer_WiFiStatus[i] = TickGet();

        WF_CAGetRssi(&rssi); // the RSSI from the last scan.
        WF_TxPowerGetFactoryMax(&factoryTxPower);
        WF_TxPowerGetMax(&maxTxPower);
        WF_TxPowerSetMax(maxTxPower-1);
        WF_Scan(WF_SCAN_ALL); // rescan

        //ByteToSocket(s, 12); // clear screen
        uitoaSocket(s, sendCount[i]++);
        StringToSocket(s, ", RSSI="); itoaSocket(s,rssi);
        StringToSocket(s, ", FactoryTxPower="); uitoaSocket(s,factoryTxPower);
        StringToSocket(s, ", MaxTxPower="); uitoaSocket(s,maxTxPower);

        StringToSocket(s, "\r\n");
        MyIpData[s].sendPacket = TRUE;
    }

}
#endif

boolean MyIpThreadSafeSendPacketCheck_Debug(const uint8_t s, const boolean doClearFlag)
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


int16_t MyIpThreadSafeReadBufferHead_Debug(const uint8_t s)
{
    // since this data comes from, and goes to, the idle thread we
    //  don't need to deal with any thread issues
    return MyIpData[s].buffer_head;
}

void MyIpProcessRxData_Debug(const uint8_t s)
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
            // ECHO
            ByteToSocket(s, rxData);

#if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
            DebugAirspeedPitotTerminalInput(s,rxData);
#endif
        }
    } while (successfulRead);
}

#if (NETWORK_USE_FLYBYWIRE == 1)
void DebugFlyByWireTerminalOutput(const uint8_t s)
{
    uint8_t allS, connectionCount;
    uint8_t i = MyIpData[s].instance;

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
        StringToSocket(s, (int8_t*)"\r\nCounter = ");
        #endif

        uitoaSocket(s, dwTime_Debug[i]);
        StringToSocket(s, (int8_t*)"\r\n");

        connectionCount = 0;
        for (allS = 0; allS < NumSockets(); allS++)
        {
            if ((MyIpData[allS].source == eSourceFlyByWire) && MyIpIsConnectedSocket(allS))
            {
                connectionCount++;
            }
        }

        StringToSocket(s, (int8_t*)"\r\nIsConnected = "); itoaSocket(s,connectionCount);
        StringToSocket(s, (int8_t*)"\r\nAileron  = "); itoaSocket(s,udb_pwIn[AILERON_INPUT_CHANNEL]);
        StringToSocket(s, (int8_t*)"\r\nElevator = "); itoaSocket(s,udb_pwIn[ELEVATOR_INPUT_CHANNEL]);
        StringToSocket(s, (int8_t*)"\r\nMode     = "); itoaSocket(s,udb_pwIn[MODE_SWITCH_INPUT_CHANNEL]);
        StringToSocket(s, (int8_t*)"\r\nRudder   = "); itoaSocket(s,udb_pwIn[RUDDER_INPUT_CHANNEL]);
        StringToSocket(s, (int8_t*)"\r\nThrottle = "); itoaSocket(s,udb_pwIn[THROTTLE_INPUT_CHANNEL]);

        MyIpData[s].sendPacket = TRUE;
    }
}
#endif

#if (ANALOG_AIRSPEED_INPUT_CHANNEL != CHANNEL_UNUSED)
void DebugAirspeedPitotTerminalOutput(const uint8_t s)
{
    uint8_t i = MyIpData[s].instance;

    if ((TickGet() - taskTimer_Debug[i]) > ((TICK_SECOND)/10)) // 10Hz
    {
        taskTimer_Debug[i] = TickGet();
        ByteToSocket(s, 12);	// Clear Screen
/*
    int16_t zeroOffset;
    float lpf_1_coef;
    float lpf_2_coef;
    float oneMinusLpf_2_coef;
    float scalar;
    int16_t filteredAdcValue;
    int16_t value;
*/
        StringToSocket(s, "\r\nRaw ADC  = "); itoaSocket(s,udb_analogInputs[ANALOG_AIRSPEED_INPUT_CHANNEL-1].value);
        StringToSocket(s, "\r\nfilt lpf = "); itoaSocket(s,airspeedPitot.filteredAdcValue);
        StringToSocket(s, "\r\nZ Offset = "); itoaSocket(s,airspeedPitot.zeroOffset);
        StringToSocket(s, "\r\nScalar   = "); ftoaSocket(s,airspeedPitot.scalar,3);
        StringToSocket(s, "\r\nlpf_1_coef= "); ftoaSocket(s,airspeedPitot.lpf_1_coef,3);
        StringToSocket(s, "\r\nlpf_2_coef= "); ftoaSocket(s,airspeedPitot.lpf_2_coef,3);
        StringToSocket(s, "\r\nAirspeed = "); itoaSocket(s,airspeedPitot.value); StringToSocket(s, " m/s");
        StringToSocket(s, "\r\nAirspeed = "); itoaSocket(s,airspeedPitot.value * 2.23); StringToSocket(s, " mph");

        MyIpData[s].sendPacket = TRUE;
    }
}

void DebugAirspeedPitotTerminalInput(const uint8_t s, const uint8_t rxData)
{
    switch (rxData)
    {
    case 'c':
        start_Calibration();
        break;

    case 'S':
        airspeedPitot.scalar += 0.01;
        break;
    case 's':
        airspeedPitot.scalar -= 0.01;
        break;

    case '1':
        airspeedPitot.lpf_1_coef -= 0.01;
        break;
    case '!':
        airspeedPitot.lpf_1_coef += 0.01;
        break;

    case '2':
        airspeedPitot.lpf_2_coef -= 0.01;
        airspeedPitot.oneMinusLpf_2_coef = 1 - airspeedPitot.lpf_2_coef;
        break;
    case '@':
        airspeedPitot.lpf_2_coef += 0.01;
        airspeedPitot.oneMinusLpf_2_coef = 1 - airspeedPitot.lpf_2_coef;
        break;

    }
}
#endif

#endif // #if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE) && (NETWORK_USE_DEBUG == 1)
#endif // _MYIPDEBUG_C_

