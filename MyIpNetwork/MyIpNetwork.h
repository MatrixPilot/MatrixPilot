
#ifndef _MYIPNETWORK_H_
#define _MYIPNETWORK_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
   
#include "TCPIP Stack/TCPIP.h"

typedef enum
{
	eSourceUART1 = 0,
	eSourceUART2, // This is the only mode that is currently supported
	eSourcePLANE_TO_PLANE,
	eSourceCAMERA,
	eSourceOSD,
} eSource;

   // Private helper functions.
void ServiceMyIpNetwork(void);
void init_MyIpNetwork(void);
void LoadNetworkAsyncTxBuffer(BYTE data, eSource src);


#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPNETWORK_H_

