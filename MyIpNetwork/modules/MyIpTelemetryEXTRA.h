
#ifndef _MYIPTELEMETRYEXTRA_H_
#define _MYIPTELEMETRYEXTRA_H_

void MyIpInit_TelemetryEXTRA(const uint8_t s);
void MyIpService_TelemetryEXTRA(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_TelemetryEXTRA(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_TelemetryEXTRA(const uint8_t s);
void MyIpProcessRxData_TelemetryEXTRA(const uint8_t s);
void MyIpOnConnect_TelemetryEXTRA(const uint8_t s);

#endif // _MYIPTELEMETRYEXTRA_H_

