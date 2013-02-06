
#ifndef _MYIPGPSTEST_H_
#define _MYIPGPSTEST_H_

#include "GenericTypeDefs.h"

void MyIpInit_GPStest(BYTE s);
void MyIpService_GPStest(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_GPStest(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_GPStest(BYTE s);
void MyIpProcessRxData_GPStest(BYTE s);
void MyIpOnConnect_GPStest(BYTE s);

#endif // _MYIPGPSTEST_H_

