
#ifndef _MYIPXPLANE_H_
#define _MYIPXPLANE_H_

void MyIpInit_XPlane(const uint8_t s);
void MyIpService_XPlane(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_XPlane(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_XPlane(const uint8_t s);
void MyIpProcessRxData_XPlane(const uint8_t s);
void MyIpOnConnect_XPlane(const uint8_t s);

#endif // _MYIPXPLANE_H_

