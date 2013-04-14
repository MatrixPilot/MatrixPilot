
#ifndef _MYIPPWMREPORT_H_
#define _MYIPPWMREPORT_H_

void MyIpInit_PWMreport(const uint8_t s);
void MyIpService_PWMreport(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_PWMreport(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_PWMreport(const uint8_t s);
void MyIpProcessRxData_PWMreport(const uint8_t s);
void MyIpOnConnect_PWMreport(const uint8_t s);

#endif // _MYIPPWMREPORT_H_

