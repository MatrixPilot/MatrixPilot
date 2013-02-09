
#ifndef _MYIPUART2_H_
#define _MYIPUART2_H_

#include "GenericTypeDefs.h"

void MyIpInit_UART2(const BYTE s);
void MyIpService_UART2(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_UART2(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_UART2(const BYTE s);
void MyIpProcessRxData_UART2(const BYTE s);
void MyIpOnConnect_UART2(const BYTE s);


#endif // _MYIPUART2_H_

