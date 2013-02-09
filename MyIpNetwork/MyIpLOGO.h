
#ifndef _MYIPLOGO_H_
#define _MYIPLOGO_H_

#include "GenericTypeDefs.h"

void MyIpInit_LOGO(const BYTE s);
void MyIpService_LOGO(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_LOGO(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_LOGO(const BYTE s);
void MyIpProcessRxData_LOGO(const BYTE s);
void MyIpOnConnect_LOGO(const BYTE s);

#endif // _MYIPLOGO_H_

