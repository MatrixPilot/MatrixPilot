
#include "defines.h"
#if (USE_NETWORK == 1)

#include "MyIpHelpers.h"
#include "TCPIP_Stack/TCPIP.h"

// Pass a CSV string and an array to populate. Comma Count is how many
// commans you want to parse which dictates the restul array size.
// We return how many CSV's were parsed. This should match commaCount
// unless something went wrong.
uint8_t parseCSV(const uint8_t* bufCSV, const int16_t len, int32_t* result, const uint8_t commaLimit)
{
    uint8_t i, digitCount, charIndex = 0, parseThis;
    int32_t digitValue;
    boolean isNeg;

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
                result[i] *= (int32_t)10;
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


boolean MyIpIsConnectedSocket(const uint8_t s)
{
    if (s >= NumSockets())
        return FALSE;
    boolean isConnected = (eSM_CONNECTED == MyIpData[s].state);

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
boolean MyIpIsConnectedSrc(const eSource src)
{
    uint8_t s;
    boolean result = FALSE;

    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            result |= MyIpIsConnectedSocket(s);
        }
    }
    return result;
}


void StringToSocket_withTypecast(const uint8_t s, const int8_t* buf)
{
    while (*buf) { ByteToSocket(s, *buf++); }
}
void StringToSrc_withTypecast(const eSource src, const int8_t* buf)
{
    while (*buf) { ByteToSrc(src, *buf++); }
}
void ArrayToSrc(const eSource src, const uint8_t* buf, const int16_t len)
{
    int16_t lenLocal = len;
    while (*buf && lenLocal--) { ByteToSrc(src, *buf++); }
}

void ultoaSrc(const eSource src, const unsigned long data)
{
    uint8_t s;
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
    uint8_t s;
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
    uint8_t s;
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
    uint8_t s;
    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            ltoaSocket(s, data);
        }
    } // for
}
void itoaSocket(const uint8_t s, const int16_t value)
{
    int8_t buf[20];
    itoa(value, (int8_t*)buf);
    StringToSocket(s, buf);
}
void ltoaSocket(const uint8_t s, const int32_t value)
{
    int8_t buf[20];
    ltoa(value, buf);
    StringToSocket(s, buf);
}
void uitoaSocket(const uint8_t s, const uint16_t value)
{
    uint8_t buf[20];
    uitoa(value, buf);
    StringToSocket(s, (int8_t*)buf);
}
void ultoaSocket(const uint8_t s, const uint32_t value)
{
    uint8_t buf[20];
    ultoa(value, buf);
    StringToSocket(s, (int8_t*)buf);
}
void ftoaSocket(const uint8_t s, float value, uint8_t decCount)
{
    ltoaSocket(s,(int32_t)value);

    if (decCount > 0)
    {
        ByteToSocket(s, '.');
        value = fabs(value) - abs((int32_t)value); // remove integer and rectify
        ltoaSocket(s,(int32_t)(value * pow(10,decCount))); // shift upwards into INT land
    }
}

void itoa(int16_t value, int8_t* Buffer)
{
    if (value < 0)
    {
        *Buffer++ = '-';
        value = -value;
    }
    uitoa((uint16_t)value, (uint8_t*)Buffer);
}
void ltoa(int32_t value, int8_t* Buffer)
{
    if (value < 0)
    {
        *Buffer++ = '-';
        value = -value;
    }
    ultoa((uint32_t)value, (uint8_t*)Buffer);
}

// This is sometimes called from within an interrupt (i.e. _U2TXInterrupt) when sending data.
// In the case of UART2 it takes a copy of the outgoing byte and loads it into a circular buffer
// which will later be asynchonously read in the idle thread to transmit it
void ByteToSrc(const eSource src, const uint8_t data)
{
    uint8_t s;
    for (s = 0; s < NumSockets(); s++)
    {
        // selectively load the sockets with routed data instead of loading them all with the same data.
        if (src == MyIpData[s].source)
        {
            ByteToSocket(s, data);
        } // if
    } // for s
}

void ByteToSocket(const uint8_t s, const uint8_t data)
{
    if (s >= NumSockets())
        return;

    MyIpData[s].buffer_head++;
    if (MyIpData[s].buffer_head >= TX_BUFFER_SIZE)
        MyIpData[s].buffer_head = 0;
    MyIpData[s].buffer[MyIpData[s].buffer_head] = data;
}

void ArrayToSocket(const uint8_t s, const uint8_t* data, const uint32_t len)
{
    if (s >= NumSockets())
    return;

    uint32_t localLen = len;
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


int8_t MyIphex_char_val(const uint8_t inchar)
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

#endif


