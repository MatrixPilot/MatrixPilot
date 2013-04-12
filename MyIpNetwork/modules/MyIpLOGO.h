
#ifndef _MYIPLOGO_H_
#define _MYIPLOGO_H_

void MyIpInit_LOGO(const uint8_t s);
void MyIpService_LOGO(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_LOGO(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_LOGO(const uint8_t s);
void MyIpProcessRxData_LOGO(const uint8_t s);
void MyIpOnConnect_LOGO(const uint8_t s);

#endif // _MYIPLOGO_H_

