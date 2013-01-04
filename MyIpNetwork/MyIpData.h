
#ifndef _MYIPDATA_H_
#define _MYIPDATA_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
   
#include "GenericTypeDefs.h"

typedef enum  
{
	eSourceUART1 = 0,
	eSourceUART2, // This is the only mode that is currently supported
	eSourceFlyByWire,
	eSourceMAVLink,
	eSourceDebug,
	//eSourcePLANE_TO_PLANE, // unimplemented future idea
	//eSourceCAMERA, // unimplemented future idea
	//eSourceOSD, // unimplemented future idea
} eSource;


//////////////////////////
// Defines
#define TX_BUFFER_SIZE					(300)
#define NUM_SOCKETS						(sizeof(MyIpData)/sizeof(MyIpData[0]))

#define TCP_TELEMETRY_CONNECT_TIMEOUT	((TICK_SECOND*5)
#define UDP_TELEMETRY_CONNECT_TIMEOUT	((TICK_SECOND*5)



//////////////////////////
// Enums
typedef enum //BYTE
{
	eSM_DO_NOTHING =0,
	eSM_HOME,
	eSM_SOCKET_OBTAINED,
	eSM_CONNECTED,
} eMyIpState;

typedef enum //BYTE
{
	eTCP = 0,
	eUDP,
} eMyIpType;

typedef struct
{
	BYTE buffer[TX_BUFFER_SIZE];		// circular buffer data
	eMyIpState state;					// state machine
	int buffer_head;					// circular buffer index head (increments on loading)
	int buffer_tail;					// circular buffer index tail (incremented on sending data)
	DWORD connectTimer;					// time we started a new IP connection, used to reset state machine
	int connectRetries;					// keep track of connection retry attempts
	BYTE socket;						// used as either TCP_SOCKET or UDP_SOCKET typedef
    BOOL foundEOL;						// Keep fillign packet until we find EndOfLine then send the packet

	eSource source;						// enum eSource, usually eSourceUART2 for MavLink/_Extra telemetry
	eMyIpType type;						// either eTCP or eUDP
	char* serverIP;						// for isServer=FALSE this is the server IP we connect to. For isServer=TRUE use NULL
	WORD port;							// IP port for server or client. Recommend TCP(23) for Telnet and UDP(14550) for qGroundControl
} MyIpDataType;

extern MyIpDataType MyIpData[];


   // Private helper functions.
void LoadPrintSrc(eSource src, unsigned long data, unsigned char spacing);
void LoadNetworkAsyncTxBufferSrc(eSource src, BYTE data);
void MyIpSetEOLflagSrc(eSource src);
void LoadStringSrc(eSource src, char* buf);
BOOL MyIpIsConnectedSrc(eSource src);

void LoadPrintSocket(BYTE s, unsigned long data, unsigned char spacing);
void LoadNetworkAsyncTxBufferSocket(BYTE s, BYTE data);
void MyIpSetEOLflagSocket(BYTE s);
void InitMyIpData(void);
void ServiceMyIpTCP(BYTE s);
void ServiceMyIpUDP(BYTE s);
void ServiceMyIpData(BYTE s);
void LoadStringSocket(BYTE s, char* buf);
unsigned int NumSockets(void);
BOOL MyIpIsConnectedSocket(BYTE s);



#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPDATA_H_

