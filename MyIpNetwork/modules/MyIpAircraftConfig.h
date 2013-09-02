
#ifndef _MYIPAIRCRAFTCONFIG_H_
#define _MYIPAIRCRAFTCONFIG_H_

void MyIpInit_AircraftConfig(const uint8_t s);
void MyIpService_AircraftConfig(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_AircraftConfig(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_AircraftConfig(const uint8_t s);
void MyIpProcessRxData_AircraftConfig(const uint8_t s);
void MyIpOnConnect_AircraftConfig(const uint8_t s);

#endif // _MYIPAIRCRAFTCONFIG_H_

