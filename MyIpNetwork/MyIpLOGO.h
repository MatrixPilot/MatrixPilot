
#ifndef _MYIPLOGO_H_
#define _MYIPLOGO_H_

#include "GenericTypeDefs.h"

void MyIpInit_LOGO(BYTE s);
void MyIpService_LOGO(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_LOGO(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_LOGO(BYTE s);
void MyIpProcessRxData_LOGO(BYTE s);
void MyIpOnConnect_LOGO(BYTE s);

#endif // _MYIPLOGO_H_

