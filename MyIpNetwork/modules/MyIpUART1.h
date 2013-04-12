
#ifndef _MYIPUART1_H_
#define _MYIPUART1_H_

void MyIpInit_UART1(const BYTE s);
void MyIpService_UART1(const BYTE s);
boolean MyIpThreadSafeSendPacketCheck_UART1(const BYTE s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_UART1(const BYTE s);
void MyIpProcessRxData_UART1(const BYTE s);
void MyIpOnConnect_UART1(const BYTE s);


#endif // _MYIPUART1_H_

