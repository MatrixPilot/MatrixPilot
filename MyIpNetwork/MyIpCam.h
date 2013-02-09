
#ifndef _MYIPCAM_TRACKING_H_
#define _MYIPCAM_TRACKING_H_

#include "GenericTypeDefs.h"

void MyIpInit_CamTracking(const BYTE s);
void MyIpService_CamTracking(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_CamTracking(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_CamTracking(const BYTE s);
void MyIpProcessRxData_CamTracking(const BYTE s);
void MyIpOnConnect_CamTracking(const BYTE s);

#endif // _MYIPCAM_TRACKING_H_

