
#ifndef _MYIPDEBUG_H_
#define _MYIPDEBUG_H_

#include "GenericTypeDefs.h"

void MyIpInit_Debug(const BYTE s);
void MyIpService_Debug(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_Debug(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_Debug(const BYTE s);
void MyIpProcessRxData_Debug(const BYTE s);
void MyIpOnConnect_Debug(const BYTE s);

#endif // _MYIPDEBUG_H_

