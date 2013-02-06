
#ifndef _MYIPCAM_H_
#define _MYIPCAM_H_

#include "GenericTypeDefs.h"

void MyIpInit_Cam(BYTE s);
void MyIpService_Cam(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_Cam(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_Cam(BYTE s);
void MyIpProcessRxData_Cam(BYTE s);
void MyIpOnConnect_Cam(BYTE s);

#endif // _MYIPCAM_H_

