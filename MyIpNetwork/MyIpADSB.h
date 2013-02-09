
#ifndef _MYIPADSB_H_
#define _MYIPADSB_H_

#include "GenericTypeDefs.h"

void MyIpInit_ADSB(const BYTE s);
void MyIpService_ADSB(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_ADSB(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_ADSB(const BYTE s);
void MyIpProcessRxData_ADSB(const BYTE s);
void MyIpOnConnect_ADSB(const BYTE s);

#endif // _MYIPADSB_H_

