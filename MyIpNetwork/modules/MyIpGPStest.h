
#ifndef _MYIPGPSTEST_H_
#define _MYIPGPSTEST_H_

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

void MyIpInit_GPStest(const uint8_t s);
void MyIpService_GPStest(const uint8_t s);
boolean MyIpThreadSafeSendPacketCheck_GPStest(const uint8_t s, const boolean doClearFlag);
int MyIpThreadSafeReadBufferHead_GPStest(const uint8_t s);
void MyIpProcessRxData_GPStest(const uint8_t s);
void MyIpOnConnect_GPStest(const uint8_t s);

#endif // _MYIPGPSTEST_H_

