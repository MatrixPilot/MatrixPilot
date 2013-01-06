
#ifndef _MYIPDEBUG_H_
#define _MYIPDEBUG_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_DEBUG == 1)

#include "GenericTypeDefs.h"

void MyIpInit_Debug(BYTE s);
void MyIpService_Debug(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_Debug(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_Debug(BYTE s);
void MyIpProcessRxData_Debug(BYTE s);
void MyIpOnConnect_Debug(BYTE s);


#endif // (NETWORK_USE_DEBUG == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPDEBUG_H_

