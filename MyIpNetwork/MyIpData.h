
#ifndef _MYIPDATA_H_
#define _MYIPDATA_H_

#include "GenericTypeDefs.h"

//////////////////////////
// Defines
#define TX_BUFFER_SIZE                  (500)
#define NUM_SOCKETS                     (sizeof(MyIpData)/sizeof(MyIpData[0]))
#define MAX_NUM_INSTANCES_OF_MODULES    (3)
#define TCP_TELEMETRY_CONNECT_TIMEOUT   ((TICK_SECOND*5)
#define UDP_TELEMETRY_CONNECT_TIMEOUT   ((TICK_SECOND*5)

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
    //eSourceHILSim, // unimplemented future idea
    //eSourcePLANE2PLANE, // unimplemented future idea
    //eSourceOSD, // unimplemented future idea
    eSource_MAX, // quantity of eSources available to choose from
} eSource;

typedef struct
{
    BYTE buffer[TX_BUFFER_SIZE];        // circular buffer data
    eMyIpState state;                   // state machine
    DWORD buffer_head;                  // circular buffer index head (increments on loading)
    DWORD buffer_tail;                  // circular buffer index tail (incremented on sending data)
    DWORD connectTimer;                 // time we started a new IP connection, used to reset state machine
    WORD connectRetries;                // keep track of connection retry attempts
    BYTE socket;                        // used as either TCP_SOCKET or UDP_SOCKET typedef
    BOOL sendPacket;                    // Keep filling packet until we're ready to send the packet
    BYTE instance;                      // index of the instances of this module

    eSource source;                     // enum eSource, usually eSourceUART2 for MavLink/_Extra telemetry
    eMyIpType type;                     // either eTCP or eUDP
    char* serverIP;                     // for isServer=FALSE this is the server IP we connect to. For isServer=TRUE use NULL
    WORD port;                          // IP port for server or client. Recommend TCP(23) for Telnet and UDP(14550) for qGroundControl
} MyIpDataType;

extern MyIpDataType MyIpData[];


void LoadPrintSrc(eSource src, unsigned long data, unsigned char spacing);
void LoadNetworkAsyncTxBufferSrc(eSource src, BYTE data);
void MyIpSetSendPacketFlagSrc(eSource src);
void LoadStringSrc(eSource src, char* buf);
BOOL MyIpIsConnectedSrc(eSource src);
DWORD IsMyIpBufferReady(BYTE s);

void itoaSocket(BYTE s, INT16 value);
void ltoaSocket(BYTE s, INT32 value);
void uitoaSocket(BYTE s, UINT16 value);
void ultoaSocket(BYTE s, UINT32 value);

void itoa(INT16 Value, char* Buffer);
void ltoa(INT32 Value, char* Buffer);

void LoadPrintSocket(BYTE s, unsigned long data, unsigned char spacing);
void LoadNetworkAsyncTxBufferSocket(BYTE s, BYTE data);
void LoadNetworkAsyncTxBufferSocketArray(BYTE s, BYTE* data, DWORD len);
void MyIpSetSendPacketFlagSocket(BYTE s);
void InitMyIpData(void);
BOOL ServiceMyIpTCP(BYTE s, BOOL isLinked);
void ServiceMyIpUDP(BYTE s);
void ServiceMyIpData(BYTE s);
void LoadStringSocket(BYTE s, char* buf);
unsigned int NumSockets(void);
BOOL MyIpIsConnectedSocket(BYTE s);



#endif // _MYIPDATA_H_

