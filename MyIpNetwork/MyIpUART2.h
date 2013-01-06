
#ifndef _MYIPUART2_H_
#define _MYIPUART2_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_UART2 == 1)

#include "GenericTypeDefs.h"

void MyIpInit_UART2(BYTE s);
void MyIpService_UART2(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_UART2(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_UART2(BYTE s);
void MyIpProcessRxData_UART2(BYTE s);
void MyIpOnConnect_UART2(BYTE s);


#endif // (NETWORK_USE_UART2 == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPUART2_H_

