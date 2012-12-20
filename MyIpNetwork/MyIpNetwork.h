
#ifndef _MYETHERNET_H_
#define _MYETHERNET_H_

#include "GenericTypeDefs.h"
#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))


//#define BAUD_RATE       (4800)		// bps

//extern unsigned char AN0String[8];

   
   // Private helper functions.
	// These may or may not be present in all applications.
void ServiceMyIpNetwork(void);
void init_MyIpNetwork(void);


// An actual function defined in MainDemo.c for displaying the current IP 
// address on the UART and/or LCD.

#endif // #if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYETHERNET_H_

