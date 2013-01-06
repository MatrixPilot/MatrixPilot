
#ifndef _MYIPFLYBYWIRE_H_
#define _MYIPFLYBYWIRE_H_

#include "options.h"
#if ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#if (NETWORK_USE_FLYBYWIRE == 1)

#include "GenericTypeDefs.h"

void MyIpInit_FlyByWire(BYTE s);
void MyIpService_FlyByWire(BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_FlyByWire(BYTE s, BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_FlyByWire(BYTE s);
void MyIpProcessRxData_FlyByWire(BYTE s);
void MyIpOnConnect_FlyByWire(BYTE s);

#endif // (NETWORK_USE_FLYBYWIRE == 1)
#endif // ((USE_WIFI_NETWORK_LINK == 1) || (USE_ETHERNET_NETWORK_LINK == 1))
#endif // _MYIPFLYBYWIRE_H_

