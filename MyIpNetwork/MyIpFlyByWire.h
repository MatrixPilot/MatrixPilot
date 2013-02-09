
#ifndef _MYIPFLYBYWIRE_H_
#define _MYIPFLYBYWIRE_H_

#include "GenericTypeDefs.h"

void MyIpInit_FlyByWire(const BYTE s);
void MyIpService_FlyByWire(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_FlyByWire(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_FlyByWire(const BYTE s);
void MyIpProcessRxData_FlyByWire(const BYTE s);
void MyIpOnConnect_FlyByWire(const BYTE s);

#endif // _MYIPFLYBYWIRE_H_

