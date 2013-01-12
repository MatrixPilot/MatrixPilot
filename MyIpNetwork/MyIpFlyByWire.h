
#ifndef _MYIPFLYBYWIRE_H_
#define _MYIPFLYBYWIRE_H_

#include "GenericTypeDefs.h"

void MyIpInit_FlyByWire(BYTE s);
void MyIpService_FlyByWire(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_FlyByWire(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_FlyByWire(BYTE s);
void MyIpProcessRxData_FlyByWire(BYTE s);
void MyIpOnConnect_FlyByWire(BYTE s);

#endif // _MYIPFLYBYWIRE_H_

