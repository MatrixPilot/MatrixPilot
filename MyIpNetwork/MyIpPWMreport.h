
#ifndef _MYIPPWMREPORT_H_
#define _MYIPPWMREPORT_H_

#include "GenericTypeDefs.h"

void MyIpInit_PWMreport(BYTE s);
void MyIpService_PWMreport(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_PWMreport(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_PWMreport(BYTE s);
void MyIpProcessRxData_PWMreport(BYTE s);
void MyIpOnConnect_PWMreport(BYTE s);

#endif // _MYIPPWMREPORT_H_

