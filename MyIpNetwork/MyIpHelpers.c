
#include "MyIpHelpers.h"
#include "TCPIP_Stack/TCPIP.h"

// Pass a CSV string and an array to populate. Comma Count is how many
// commans you want to parse which dictates the restul array size.
// We return how many CSV's were parsed. This should match commaCount
// unless something went wrong.
BYTE parseCSV(const BYTE* bufCSV, const INT16 len, INT32* result, const BYTE commaLimit)
{
    BYTE i, digitCount, charIndex = 0, parseThis;
    INT32 digitValue;
    BOOL isNeg;

    for (i=0;i<commaLimit;i++)
    {
        isNeg = FALSE;
        digitCount = 0;
        result[i] = 0;
        if (charIndex >= len)
        {
            return i;
        }
        parseThis = bufCSV[charIndex];
        while (parseThis != ',')
        {
            // convert from ASCII number digit to Binary
            digitValue = parseThis - '0';

            if ((digitCount == 0) && (parseThis == '-') && !isNeg)
            {
                isNeg = true;
            }
            else if (digitCount > 10)
            {
                // 2^32 == 4294967296 which is 10 digits
                // too many digits
                if (digitCount == 0)
                    i--;
                return i;
            }
            else if ((parseThis == ' ') || (parseThis == '.'))
            {
                // skip over spaces and decimal places
            }
            else if (digitValue >= 10)
            {
                // was not a valid ASCII number digit,
                // it was either below ASCII '0' or above '9'
                if ((digitCount == 0) && (i > 0))
                    i--;
                return i;
            }
            else
            {
                result[i] *= (INT32)10;
                result[i] += digitValue;
                digitCount++;
            }

            ++charIndex;
            if (charIndex >= len)
            {
                return i;
            }
            parseThis = bufCSV[charIndex];
        }
        if (isNeg)
            result[i] = -result[i];
        charIndex++; // skip over the ','
    }
    return i;
}


BOOL MyIpIsConnectedSocket(const BYTE s)
{
    if (s >= NumSockets())
        return FALSE;
    BOOL isConnected = (eSM_CONNECTED == MyIpData[s].state);

    if (eTCP == MyIpData[s].type)
    {
        // easy result because we have a connection state to read from
        return isConnected;
    }
    else //if (eUDP == MyIpData[s].type)
    {
        // TODO make this smarter, right now we are *always* connected
        // since we're connection-less, check if we've recieved any data
        return isConnected;
    }
}
BOOL MyIpIsConnectedSrc(const eSource src)
{
    BYTE s;
    BOOL result = FALSE;

    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            result |= MyIpIsConnectedSocket(s);
        }
    }
    return result;
}


void StringToSocket(const BYTE s, const char* buf)
{
    while (*buf) { ByteToSocket(s, *buf++); }
}
void StringToSrc(const eSource src, const char* buf)
{
    while (*buf) { ByteToSrc(src, *buf++); }
}
void ArrayToSrc(const eSource src, const BYTE* buf, const INT16 len)
{
    INT16 lenLocal = len;
    while (*buf && lenLocal--) { ByteToSrc(src, *buf++); }
}

void ultoaSrc(const eSource src, const unsigned long data)
{
    BYTE s;
    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            ultoaSocket(s, data);
        }
    } // for
}
void itoaSrc(const eSource src, const int data)
{
    BYTE s;
    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            itoaSocket(s, data);
        }
    } // for
}
void uitoaSrc(const eSource src, const unsigned int data)
{
    BYTE s;
    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            uitoaSocket(s, data);
        }
    } // for
}
void ltoaSrc(const eSource src, const long data)
{
    BYTE s;
    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            ltoaSocket(s, data);
        }
    } // for
}
void itoaSocket(const BYTE s, const INT16 value)
{
    char buf[20];
    itoa(value, (char*)buf);
    StringToSocket(s, buf);
}
void ltoaSocket(const BYTE s, const INT32 value)
{
    char buf[20];
    ltoa(value, buf);
    StringToSocket(s, buf);
}
void uitoaSocket(const BYTE s, const UINT16 value)
{
    BYTE buf[20];
    uitoa(value, buf);
    StringToSocket(s, (char*)buf);
}
void ultoaSocket(const BYTE s, const UINT32 value)
{
    BYTE buf[20];
    ultoa(value, buf);
    StringToSocket(s, (char*)buf);
}

void itoa(const INT16 Value, char* Buffer)
{
    INT16 localValue = Value;
    if (localValue < 0)
    {
        *Buffer++ = '-';
        localValue = -localValue;
    }
    uitoa((UINT16)localValue, (BYTE*)Buffer);
}
void ltoa(const INT32 Value, char* Buffer)
{
    INT32 localValue = Value;
    if (Value < 0)
    {
        *Buffer++ = '-';
        localValue = -localValue;
    }
    ultoa((UINT32)localValue, (BYTE*)Buffer);
}

// This is sometimes called from within an interrupt (i.e. _U2TXInterrupt) when sending data.
// In the case of UART2 it takes a copy of the outgoing byte and loads it into a circular buffer
// which will later be asynchonously read in the idle thread to transmit it
void ByteToSrc(const eSource src, const BYTE data)
{
    BYTE s;

    for (s = 0; s < NumSockets(); s++)
    {
        // selectively load the sockets with routed data instead of loading them all with the same data.
        if (src == MyIpData[s].source)
        {
            ByteToSocket(s, data);
        } // if
    } // for s
}

void ByteToSocket(const BYTE s, const BYTE data)
{
    if (s >= NumSockets())
        return;

    MyIpData[s].buffer_head++;
    if (MyIpData[s].buffer_head >= TX_BUFFER_SIZE)
        MyIpData[s].buffer_head = 0;
    MyIpData[s].buffer[MyIpData[s].buffer_head] = data;
}

void ArrayToSocket(const BYTE s, const BYTE* data, const DWORD len)
{
    if (s >= NumSockets())
    return;

    DWORD localLen = len;
    if (localLen > TX_BUFFER_SIZE)
        localLen = TX_BUFFER_SIZE;

    while(localLen--)
    {
        MyIpData[s].buffer_head++;
        if (MyIpData[s].buffer_head >= TX_BUFFER_SIZE)
            MyIpData[s].buffer_head = 0;
        MyIpData[s].buffer[MyIpData[s].buffer_head] = *data++;
    }
}


char MyIphex_char_val(const unsigned char inchar)
{
    if (inchar >= '0' && inchar <= '9')
    {
        return (inchar - '0') ;
    }
    else if (inchar >= 'A' && inchar <= 'F')
    {
        return (inchar - 'A' + 10) ;
    }
    return -1 ;
}


