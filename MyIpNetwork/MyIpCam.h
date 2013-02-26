
#ifndef _MYIPCAM_TRACKING_H_
#define _MYIPCAM_TRACKING_H_

void MyIpInit_CamTracking(const uint8_t s);
void MyIpService_CamTracking(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_CamTracking(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_CamTracking(const uint8_t s);
void MyIpProcessRxData_CamTracking(const uint8_t s);
void MyIpOnConnect_CamTracking(const uint8_t s);

#endif // _MYIPCAM_TRACKING_H_

