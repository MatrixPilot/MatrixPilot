
#ifndef _MYIPDEBUG_H_
#define _MYIPDEBUG_H_

void MyIpInit_Debug(const uint8_t s);
void MyIpService_Debug(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_Debug(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_Debug(const uint8_t s);
void MyIpProcessRxData_Debug(const uint8_t s);
void MyIpOnConnect_Debug(const uint8_t s);

#endif // _MYIPDEBUG_H_

