
#ifndef _MYIPPWMREPORT_H_
#define _MYIPPWMREPORT_H_

#include "GenericTypeDefs.h"

void MyIpInit_PWMreport(const BYTE s);
void MyIpService_PWMreport(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_PWMreport(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_PWMreport(const BYTE s);
void MyIpProcessRxData_PWMreport(const BYTE s);
void MyIpOnConnect_PWMreport(const BYTE s);

#endif // _MYIPPWMREPORT_H_

