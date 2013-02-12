#ifndef _MYIPHELPERS_H_
#define _MYIPHELPERS_H_

#include "GenericTypeDefs.h"
#include "MyIpData.h"


void StringToSocket(const BYTE s, const char* buf);
void StringToSrc(const eSource src, const char* buf);
void ArrayToSrc(const eSource src, const BYTE* buf, const INT16 len);
void ultoaSrc(const eSource src, const unsigned long data);
void itoaSrc(const eSource src, const int data);
void uitoaSrc(const eSource src, const unsigned int data);
void ltoaSrc(const eSource src, const long data);
void itoaSocket(const BYTE s, const INT16 value);
void ltoaSocket(const BYTE s, const INT32 value);
void uitoaSocket(const BYTE s, const UINT16 value);
void ultoaSocket(const BYTE s,const  UINT32 value);
void itoa(const INT16 Value, char* Buffer);
void ltoa(const INT32 Value, char* Buffer);


void ByteToSocket(const BYTE s, const BYTE data);
void ArrayToSocket(const BYTE s, const BYTE* data, DWORD len);
void ByteToSrc(const eSource src, const BYTE data);
BOOL MyIpIsConnectedSrc(const eSource src);
BOOL MyIpIsConnectedSocket(const BYTE s);
BYTE parseCSV(const BYTE* bufCSV, const INT16 len, INT32* result, const BYTE commaLimit);
char MyIphex_char_val(const unsigned char inchar);

#endif // _MYIPHELPERS_H_