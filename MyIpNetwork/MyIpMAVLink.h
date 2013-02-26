
#ifndef _MYIPMAVLink_H_
#define _MYIPMAVLink_H_

void MyIpInit_MAVLink(const uint8_t s);
void MyIpService_MAVLink(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_MAVLink(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_MAVLink(const uint8_t s);
void MyIpProcessRxData_MAVLink(const uint8_t s);
void MyIpOnConnect_MAVLink(const uint8_t s);

#endif // _MYIPMAVLink_H_

