
#ifndef _MYIPCAM_TRACKING_H_
#define _MYIPCAM_TRACKING_H_

#include "GenericTypeDefs.h"

void MyIpInit_CamTracking(BYTE s);
void MyIpService_CamTracking(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_CamTracking(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_CamTracking(BYTE s);
void MyIpProcessRxData_CamTracking(BYTE s);
void MyIpOnConnect_CamTracking(BYTE s);

#endif // _MYIPCAM_TRACKING_H_

