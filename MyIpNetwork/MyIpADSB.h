
#ifndef _MYIPADSB_H_
#define _MYIPADSB_H_

#include "GenericTypeDefs.h"

void MyIpInit_ADSB(BYTE s);
void MyIpService_ADSB(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_ADSB(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_ADSB(BYTE s);
void MyIpProcessRxData_ADSB(BYTE s);
void MyIpOnConnect_ADSB(BYTE s);

#endif // _MYIPADSB_H_

