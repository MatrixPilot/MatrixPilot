
#ifndef _MYIPMAVLink_H_
#define _MYIPMAVLink_H_

#include "GenericTypeDefs.h"

void MyIpInit_MAVLink(const BYTE s);
void MyIpService_MAVLink(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_MAVLink(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_MAVLink(const BYTE s);
void MyIpProcessRxData_MAVLink(const BYTE s);
void MyIpOnConnect_MAVLink(const BYTE s);

#endif // _MYIPMAVLink_H_

