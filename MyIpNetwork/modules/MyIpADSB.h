
#ifndef _MYIPADSB_H_
#define _MYIPADSB_H_

void MyIpInit_ADSB(const uint8_t s);
void MyIpService_ADSB(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_ADSB(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_ADSB(const uint8_t s);
void MyIpProcessRxData_ADSB(const uint8_t s);
void MyIpOnConnect_ADSB(const uint8_t s);

#endif // _MYIPADSB_H_

