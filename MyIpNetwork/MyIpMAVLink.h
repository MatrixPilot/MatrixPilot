
#ifndef _MYIPMAVLink_H_
#define _MYIPMAVLink_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_MAVLINK == 1)

#include "GenericTypeDefs.h"

void MyIpInit_MAVLink(void);
void MyIpService_MAVLink(BYTE s);
BOOL MyIpThreadSafeEOLcheck_MAVLink(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_MAVLink(BYTE s);
void MyIpProcessRxData_MAVLink(BYTE s);
void MyIpOnConnect_MAVLink(BYTE s);


#endif // (NETWORK_USE_MAVLINK == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPMAVLink_H_

