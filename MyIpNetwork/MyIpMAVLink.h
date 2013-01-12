
#ifndef _MYIPMAVLink_H_
#define _MYIPMAVLink_H_

#include "GenericTypeDefs.h"

void MyIpInit_MAVLink(BYTE s);
void MyIpService_MAVLink(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_MAVLink(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_MAVLink(BYTE s);
void MyIpProcessRxData_MAVLink(BYTE s);
void MyIpOnConnect_MAVLink(BYTE s);

#endif // _MYIPMAVLink_H_

