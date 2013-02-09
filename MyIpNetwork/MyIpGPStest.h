
#ifndef _MYIPGPSTEST_H_
#define _MYIPGPSTEST_H_

#include "GenericTypeDefs.h"
#include "libUDB_defines.h"

typedef enum {
    GpsSpoofMode_Disabled = 0,
    GpsSpoofMode_Override,
    GpsSpoofMode_Offset,

    GpsSpoofMode_SIZE
} EGpsSpoofMode;

typedef struct
{
    EGpsSpoofMode Mode;
    union longbbbb Lat;
    union longbbbb Long;
    union longbbbb Alt;
} SGpsSpoof;

extern SGpsSpoof GpsSpoof;

void MyIpInit_GPStest(const BYTE s);
void MyIpService_GPStest(const BYTE s);
BOOL MyIpThreadSafeSendPacketCheck_GPStest(const BYTE s, const BOOL doClearFlag);
int MyIpThreadSafeReadBufferHead_GPStest(const BYTE s);
void MyIpProcessRxData_GPStest(const BYTE s);
void MyIpOnConnect_GPStest(const BYTE s);

#endif // _MYIPGPSTEST_H_

