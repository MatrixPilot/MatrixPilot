
#ifndef _MYIPDATA_H_
#define _MYIPDATA_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
   
#include "GenericTypeDefs.h"

typedef enum
{
	eSourceUART1 = 0,
	eSourceUART2, // This is the only mode that is currently supported
	eSourcePLANE_TO_PLANE,
	eSourceCAMERA,
	eSourceOSD,
} eSource;

   // Private helper functions.
void LoadNetworkAsyncTxBuffer(BYTE data, eSource src);
void InitTelemetry(void);
void ServiceTCPTelemetry();
void ServiceUDPTelemetry();


#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPDATA_H_

