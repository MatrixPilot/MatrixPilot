
#ifndef _MYIPFLYBYWIRE_H_
#define _MYIPFLYBYWIRE_H_

void MyIpInit_FlyByWire(const uint8_t s);
void MyIpService_FlyByWire(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_FlyByWire(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_FlyByWire(const uint8_t s);
void MyIpProcessRxData_FlyByWire(const uint8_t s);
void MyIpOnConnect_FlyByWire(const uint8_t s);

#endif // _MYIPFLYBYWIRE_H_

