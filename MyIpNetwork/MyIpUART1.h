
#ifndef _MYIPUART1_H_
#define _MYIPUART1_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_UART1 == 1)

#include "GenericTypeDefs.h"

void MyIpInit_UART1(BYTE s);
void MyIpService_UART1(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_UART1(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_UART1(BYTE s);
void MyIpProcessRxData_UART1(BYTE s);
void MyIpOnConnect_UART1(BYTE s);


#endif // (NETWORK_USE_UART1 == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPUART1_H_

