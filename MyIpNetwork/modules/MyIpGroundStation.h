
#ifndef _MYIPGROUND_STATION_H_
#define _MYIPGROUND_STATION_H_

void MyIpInit_GroundStation(const uint8_t s);
void MyIpService_GroundStation(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_GroundStation(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_GroundStation(const uint8_t s);
void MyIpProcessRxData_GroundStation(const uint8_t s);
void MyIpOnConnect_GroundStation(const uint8_t s);

#endif // _MYIPGROUND_STATION_H_

