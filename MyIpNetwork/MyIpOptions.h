
#ifndef _MYIPOPTIONS_H_
#define _MYIPOPTIONS_H_

//////////////////////////
// Variables
MyIpDataType MyIpData[] __attribute__ ((far)) =
{
    #if (NETWORK_USE_UART1 == 1)
    //{ {},0,0,0,0,0,0,0,0,     eSourceUART1,eUDP, NULL, 27015},
    //{ {},0,0,0,0,0,0,0,0,     eSourceUART1,eUDP, "192.168.11.100", 27015},
    { {},0,0,0,0,0,0,0,0,       eSourceUART1,eTCP, NULL, 27015},
    #endif
    #if (NETWORK_USE_UART2 == 1)
//    { {},0,0,0,0,0,0,0,0,       eSourceUART2,eTCP, "76.102.60.245", 3011},
//    { {},0,0,0,0,0,0,0,0,       eSourceUART2,eTCP, "76.102.60.245", 3011},
    { {},0,0,0,0,0,0,0,0,       eSourceUART2,eTCP,"192.168.1.39", 3004},
    #endif
    #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED)
    //{ {},0,0,0,0,0,0,0,0,     eSourceFlyByWire,eTCP, "76.102.60.245", 3003},
    //{ {},0,0,0,0,0,0,0,0,     eSourceFlyByWire,eTCP, NULL, 3004},
    { {},0,0,0,0,0,0,0,0,     eSourceFlyByWire,eUDP, "76.102.60.245", 3014},
    #endif
    #if (NETWORK_USE_MAVLINK == 1)
    //{ {},0,0,0,0,0,0,0,0,     eSourceMAVLink,eUDP, "192.168.11.100", 14550},  // UDB MAV Link stream
    { {},0,0,0,0,0,0,0,0,     eSourceMAVLink,eUDP, NULL, 14550},  // UDB MAV Link stream
    #endif
    #if (NETWORK_USE_DEBUG == 1)
    // { {},0,0,0,0,0,0,0,0,    eSourceDebug,eTCP, "tompittenger.diskstation.me", 3015},    // Telnet server
    { {},0,0,0,0,0,0,0,0,       eSourceDebug,eTCP, NULL, 23},   // Telnet server
    #endif
    #if (NETWORK_USE_ADSB == 1)
    // { {},0,0,0,0,0,0,0,0,    eSourceADSB,eTCP, "tompittenger.diskstation.me", 3015},
    //{ {},0,0,0,0,0,0,0,0,       eSourceADSB,eTCP, NULL, 3001},
    //{ {},0,0,0,0,0,0,0,0,       eSourceADSB,eTCP, NULL, 3002},
    { {},0,0,0,0,0,0,0,0,       eSourceADSB,eTCP, NULL, 3003},
    #endif
    #if (NETWORK_USE_LOGO == 1)
    { {},0,0,0,0,0,0,0,0,       eSourceLOGO,eTCP, NULL, 3005},
    #endif
    #if (NETWORK_USE_CAM_TRACKING == 1)
    { {},0,0,0,0,0,0,0,0,       eSourceCamTracking,eTCP, NULL, 3006},
    #endif
    #if (NETWORK_USE_GPSTEST == 1)
    { {},0,0,0,0,0,0,0,0,       eSourceGPStest,eTCP, NULL, 3007},
    #endif
    #if (NETWORK_USE_PWMREPORT == 1)
    { {},0,0,0,0,0,0,0,0,       eSourcePWMreport,eTCP, NULL, 3008},
    #endif

    // other examples:
    //{ {},0,0,0,0,0,0,0,0,     eSource????,eUDP, "192.168.11.100", 14550}, // qGroundControl
    //{ {},0,0,0,0,0,0,0,0,0,   eSource????,eTCP, NULL, 23},                // TCP Server, listening on port 23, use this for telnet
    //{ {},0,0,0,0,0,0,0,0,0,   eSource????,eTCP, "192.168.11.100", 24},    // TCP Client connecting to 192.168.11.100:24, use this for HK-GCS
    //{ {},0,0,0,0,0,0,0,0,0,   eSource????,eUDP, NULL,14550},              // UDP Server, listening on port 14550, Use this for qGroundControl or HK-GCS
    //{ {},0,0,0,0,0,0,0,0,0,   eSource????,eUDP, "192.168.11.100", 14550}, // UDP Client connecting to 192.168.11.100:14550, Use this for HK-GCS
    //{ {},0,0,0,0,0,0,0,0,0,   eSource????,eTCP, "username.DynDNS.com", 23},   // You can even use URLs!
};
// ** HUMAN ERROR WARNING!!!
// Check that there are enough TCP_PURPOSE_TELEMETRY slots in TCPIPConfig.h for as many eTCP sockets you have
// Also check that there are no more than MAX_NUM_INSTANCES_OF_MODULES of any given module instance

#endif // _MYIPOPTIONS_H_
