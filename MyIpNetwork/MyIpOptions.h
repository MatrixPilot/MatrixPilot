#ifndef _MYIPOPTIONS_H_
#define _MYIPOPTIONS_H_

#include "MyIpKnownPorts.h"


//////////////////////////
// Variables
MyIpDataType MyIpData[] __attribute__ ((far)) =
{
    #if (NETWORK_USE_UART1 == 1)
    //{ {},0,0,0,0,0,0,0,0,     eSourceUART1,eUDP, NULL, MYIP_UART1_PORT_1},
    //{ {},0,0,0,0,0,0,0,0,     eSourceUART1,eUDP, "192.168.11.100", MYIP_UART1_PORT_1},
      { {},0,0,0,0,0,0,0,0,       eSourceUART1,eTCP, NULL, MYIP_UART1_PORT_1},
    #endif
    #if (NETWORK_USE_UART2 == 1)
//    { {},0,0,0,0,0,0,0,0,       eSourceUART2,eTCP, "76.102.60.245", MYIP_UART2_PORT_1},
//    { {},0,0,0,0,0,0,0,0,       eSourceUART2,eUDP, "crashandburn.no-ip.org", MYIP_QGROUND_CONTROL_UDP_PORT},
//    { {},0,0,0,0,0,0,0,0,         eSourceUART2,eUDP, "192.168.1.103", MYIP_PORT_QGROUND_CONTROL_1}, // QGCS client on MacBook
      { {},0,0,0,0,0,0,0,0,       eSourceUART2,eTCP, NULL, MYIP_UART2_PORT_1},
//    { {},0,0,0,0,0,0,0,0,       eSourceUART2,eUDP, "192.168.1.109", MYIP_QGROUND_CONTROL_UDP_PORT},
    #endif
    #if (NETWORK_USE_FLYBYWIRE == 1) && (FLYBYWIRE_ENABLED)
    //{ {},0,0,0,0,0,0,0,0,     eSourceFlyByWire,eTCP, "76.102.60.245", MYIP_FLYBYWIRE_PORT_1},
    //{ {},0,0,0,0,0,0,0,0,     eSourceFlyByWire,eTCP, NULL, MYIP_FLYBYWIRE_PORT_1},
//    { {},0,0,0,0,0,0,0,0,     eSourceFlyByWire,eUDP, "192.168.1.108", MYIP_FLYBYWIRE_PORT_1}, //Phil AlienWare PC
    #endif
    #if (NETWORK_USE_MAVLINK == 1)
    //{ {},0,0,0,0,0,0,0,0,     eSourceMAVLink,eUDP, "192.168.11.100", MYIP_QGROUND_CONTROL_UDP_PORT},  // UDB MAV Link stream
      { {},0,0,0,0,0,0,0,0,     eSourceMAVLink,eUDP, NULL, MYIP_MAVLINK_PORT_1},  // UDB MAV Link stream
    #endif
    #if (NETWORK_USE_DEBUG == 1)
    // { {},0,0,0,0,0,0,0,0,    eSourceDebug,eTCP, "tompittenger.diskstation.me", 3015},    // Telnet server
    { {},0,0,0,0,0,0,0,0,       eSourceDebug,eTCP, NULL, MYIP_TELNET_PORT},   // Telnet server
    #endif
    #if (NETWORK_USE_ADSB == 1)
    // { {},0,0,0,0,0,0,0,0,    eSourceADSB,eTCP, "tompittenger.diskstation.me", 3015},
    //{ {},0,0,0,0,0,0,0,0,       eSourceADSB,eTCP, NULL, MYIP_ADSB_PORT_1},
    //{ {},0,0,0,0,0,0,0,0,       eSourceADSB,eTCP, NULL, MYIP_ADSB_PORT_1},
    { {},0,0,0,0,0,0,0,0,       eSourceADSB,eTCP, NULL, MYIP_ADSB_PORT_1},
    #endif
    #if (NETWORK_USE_LOGO == 1)
    { {},0,0,0,0,0,0,0,0,       eSourceLOGO,eTCP, NULL, MYIP_LOGO_PORT_1},
    #endif
    #if (NETWORK_USE_CAM_TRACKING == 1)
    { {},0,0,0,0,0,0,0,0,       eSourceCamTracking,eTCP, NULL, MYIP_CAM_TRACKING_PORT_1},
    #endif
    #if (NETWORK_USE_GPSTEST == 1)
    { {},0,0,0,0,0,0,0,0,       eSourceGPStest,eTCP, NULL, MYIP_GPSTEST_PORT_1},
    #endif
    #if (NETWORK_USE_PWMREPORT == 1)
    { {},0,0,0,0,0,0,0,0,       eSourcePWMreport,eTCP, NULL, MYIP_PWMREPORT_PORT_1},
    //{ {},0,0,0,0,0,0,0,0,       eSourcePWMreport,eTCP, "crashandburn.no-ip.org", 3118},
    #endif
    #if (NETWORK_USE_XPLANE == 1)
    { {},0,0,0,0,0,0,0,0,       eSourceXPlane,eUDP, "192.168.1.3", 49000},
    //{ {},0,0,0,0,0,0,0,0,       eSourceXPlane,eUDP, NULL, 49001},
    //{ {},0,0,0,0,0,0,0,0,       eSourceXPlane,eUDP, NULL, 49002},
    //{ {},0,0,0,0,0,0,0,0,       eSourceXPlane,eUDP, "192.168.1.3", 49000},
    #endif
    #if (NETWORK_USE_TELEMETRY_EXTRA == 1)
    // { {},0,0,0,0,0,0,0,0,    eSourceTelemetryEXTRA,eTCP, "192.168.1.3", 3015},
    #endif
    #if (NETWORK_USE_GROUND_STATION == 1)
    // { {},0,0,0,0,0,0,0,0,    eSourceGroundStation,eTCP, "192.168.1.3", 3015},
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
