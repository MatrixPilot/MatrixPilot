
#ifndef _MYIPDATA_H_
#define _MYIPDATA_H_

//////////////////////////
// Defines
#define TX_BUFFER_SIZE                  (1000)
#define NUM_SOCKETS                     (sizeof(MyIpData)/sizeof(MyIpData[0]))
#define MAX_NUM_INSTANCES_OF_MODULES    (3)
#define TCP_TELEMETRY_CONNECT_TIMEOUT   ((TICK_SECOND*3)
#define UDP_TELEMETRY_CONNECT_TIMEOUT   ((TICK_SECOND*3)

//////////////////////////
// Enums
typedef enum //BYTE
{
    eSM_HOME = 0,
    eSM_SOCKET_OBTAINED,
    eSM_CONNECTED,
} eMyIpState;

typedef enum //BYTE
{
    eTCP = 0,
    eUDP,
} eMyIpType;


// What to do when adding a new eSource module
// 1) make a new MyIpMODULENAME.c and .h
// 2) Add entry to enum eSource
// 3) Add new entry to TCP_PURPOSE_MYIPDATA_MODULENAME in TCPIP.h
// 4) .. and more
typedef enum  
{
    eSourceUART1 = 0,
    eSourceUART2,
    eSourceFlyByWire,
    eSourceMAVLink,
    eSourceDebug,
    eSourceADSB,
    eSourceLOGO,
    eSourceCamTracking,
    eSourceGPStest,
    eSourcePWMreport,
    eSourceXPlane,
    eSourceTelemetryEXTRA,
    eSourceGroundStation,
    //eSourceHILSim, // unimplemented future idea
    //eSourcePLANE2PLANE, // unimplemented future idea
    //eSourceOSD, // unimplemented future idea
    eSource_MAX, // quantity of eSources available to choose from
} eSource;

typedef struct
{
    uint8_t buffer[TX_BUFFER_SIZE];        // circular buffer data
    eMyIpState state;                   // state machine
    uint16_t buffer_head;                  // circular buffer index head (increments on loading)
    uint16_t buffer_tail;                  // circular buffer index tail (incremented on sending data)
    uint32_t connectTimer;                 // time we started a new IP connection, used to reset state machine
    uint16_t connectRetries;                // keep track of connection retry attempts
    uint8_t socket;                        // used as either TCP_SOCKET or UDP_SOCKET typedef
    boolean sendPacket;                    // Keep filling packet until we're ready to send the packet
    uint8_t instance;                      // index of the instances of this module

    eSource source;                     // enum eSource, usually eSourceUART2 for MavLink/_Extra telemetry
    eMyIpType type;                     // either eTCP or eUDP
    char* serverIP;                     // for isServer=FALSE this is the server IP we connect to. For isServer=TRUE use NULL
    uint16_t port;                          // IP port for server or client. Recommend TCP(23) for Telnet and UDP(14550) for qGroundControl
} MyIpDataType;

extern MyIpDataType MyIpData[];


unsigned int NumSockets(void);
void MyIpSetSendPacketFlagSrc(const eSource src);
uint32_t IsMyIpBufferReady(const uint8_t s);
void MyIpSetSendPacketFlagSocket(const uint8_t s);
void InitMyIpData(void);
boolean ServiceMyIpTCP(const uint8_t s, const boolean isLinked);
void ServiceMyIpUDP(const uint8_t s);
void ServiceMyIpData(const uint8_t s);



#endif // _MYIPDATA_H_

