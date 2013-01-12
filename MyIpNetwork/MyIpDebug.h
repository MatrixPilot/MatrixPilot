
#ifndef _MYIPDEBUG_H_
#define _MYIPDEBUG_H_

#include "GenericTypeDefs.h"

void MyIpInit_Debug(BYTE s);
void MyIpService_Debug(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_Debug(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_Debug(BYTE s);
void MyIpProcessRxData_Debug(BYTE s);
void MyIpOnConnect_Debug(BYTE s);

#endif // _MYIPDEBUG_H_

