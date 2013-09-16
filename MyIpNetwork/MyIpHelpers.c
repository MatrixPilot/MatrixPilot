#include <math.h>

#include "MyIpConfig.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)

#include "MyIpHelpers.h"
#include "TCPIP Stack/TCPIP.h"

// Pass a CSV string and an array to populate. Comma Count is how many
// commans you want to parse which dictates the restul array size.
// We return how many CSV's were parsed. This should match commaCount
// unless something went wrong.
uint8_t parseCSV(const uint8_t* bufCSV, const int16_t len, int32_t* result, const uint8_t commaLimit)
{
    uint8_t i, digitCount, charIndex = 0, parseThis;
    int32_t digitValue;
    boolean isNeg;

    for (i = 0; i < commaLimit; i++)
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

void StringToSocket(const uint8_t s, const char* buf)
{
    while (*buf) { ByteToSocket(s, *buf++); }
}

void StringToSrc(const eSource src, const char* buf)
{
    while (*buf)
    {
      ByteToSrc(src, *buf++);
    }
}

void ArrayToSrc(const eSource src, const uint8_t* buf, const int16_t len)
{
    uint8_t s;
    for (s = 0; s < NumSockets(); s++)
    {
      if (src == MyIpData[s].source)
      {
          ArrayToSocket(s, buf, len);
      } // if
    } // for s
}

void ToHexToSocket(const uint8_t s, const uint32_t value, const uint8_t size)
{
    switch(size)
    {
    case 32:
        ByteToSocket(s ,ToAsciiHex4bit(value >> 28));
        ByteToSocket(s, ToAsciiHex4bit(value >> 24));
    case 24:
        ByteToSocket(s, ToAsciiHex4bit(value >> 20));
        ByteToSocket(s, ToAsciiHex4bit(value >> 16));
    case 16:
        ByteToSocket(s, ToAsciiHex4bit(value >> 12));
        ByteToSocket(s, ToAsciiHex4bit(value >> 8));
    case 8:
        ByteToSocket(s, ToAsciiHex4bit(value >> 4));
    case 4:
        ByteToSocket(s, ToAsciiHex4bit(value));
        break;
    }
}
void ToHexToSrc(const eSource src, const uint32_t value, const uint8_t size)
{
    uint8_t s;

    for (s = 0; s < NumSockets(); s++)
    {
        if (src == MyIpData[s].source)
        {
            ToHexToSocket(s, value, size);
        }
    } // for

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
    uint8_t buf[20];

    itoa(value, buf);
    StringToSocket(s, (char *) buf);
}

void ltoaSocket(const uint8_t s, const int32_t value)
{
    uint8_t buf[20];

    ltoa(value, buf);
    StringToSocket(s, (char *) buf);
}

void uitoaSocket(const uint8_t s, const uint16_t value)
{
    uint8_t buf[20];

    uitoa(value, buf);
    StringToSocket(s, (char *) buf);
}

void ultoaSocket(const uint8_t s, const uint32_t value)
{
    uint8_t buf[20];

    ultoa(value, buf);
    StringToSocket(s, (char *)buf);
}

void ftoaSocket(const uint8_t s, const float value, const uint8_t decCount)
{
    ltoaSocket(s,(int32_t)value);
    if (decCount > 0)
    {
        ByteToSocket(s, '.');
        float valueRectified = fabs(value) - abs((int32_t)value); // remove integer and rectify
        ltoaSocket(s,(int32_t)(valueRectified * pow(10,decCount))); // shift upwards into INT land
    }
}

void itoa(int16_t value, uint8_t* Buffer)
{
    if (value < 0)
    {
        *Buffer++ = '-';
        value = -value;
    }
    uitoa((uint16_t)value, Buffer);
}

void ltoa(int32_t value, uint8_t* Buffer)
{
    if (value < 0)
    {
        *Buffer++ = '-';
        value = -value;
    }
    ultoa((uint32_t)value, Buffer);
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

int8_t MyIphex_AsciiToBinary(const uint8_t inchar)
{
    if (inchar >= '0' && inchar <= '9')
    {
        return (inchar - '0');
    }
    else if (inchar >= 'A' && inchar <= 'F')
    {
        return (inchar - 'A' + 10);
    }
    return -1;
}

float ReverseFloat(const float inFloat)
{
    float retVal;
    char *floatToConvert = (char*)&inFloat;
    char *returnFloat = (char*)&retVal;

    // swap the bytes into a temporary buffer
    returnFloat[0] = floatToConvert[3];
    returnFloat[1] = floatToConvert[2];
    returnFloat[2] = floatToConvert[1];
    returnFloat[3] = floatToConvert[0];

    return retVal;
}

uint8_t ToAsciiHex4bit(const uint8_t value)
{
    uint8_t value_local = value & 0x0F;

    if (value_local <= 9)
    {
        return value_local + '0';
    }
    else //if (value_local <= 0xF) // since we do &=0xF, this will always be true
    {
        return value_local - 10 + 'A';
    }
    //return '?'; // something is wrong
}

void printAircraftState(uint8_t s)
{
/*
  AIRCRAFT_FLIGHT_MODE_STATE arecraftState = getAircraftState();
  switch (arecraftState)
  {
  case smCALIBRATING:
    StringToSocket(s, "CALIBRATING");
    break;
  case smWAITING_FOR_GPS_LOCK:
    StringToSocket(s, "WAITING_FOR_GPS_LOCK");
    break;
  case smFLYING_MANUAL:
    StringToSocket(s, "FLYING_MANUAL");
    break;
  case smFLYING_STABILIZED:
    StringToSocket(s, "FLYING_STABILIZED");
    break;
  case smFLYING_WAYPOINT:
    StringToSocket(s, "FLYING_WAYPOINT");
    break;
#if (CATAPULT_LAUNCH_INPUT_CHANNEL != CHANNEL_UNUSED)
  case smFLYING_MANUAL_ARMED_FOR_LAUNCH:
    StringToSocket(s, "FLYING_MANUAL_ARMED_FOR_LAUNCH");
    break;
  case smFLYING_STABILIZED_ARMED_FOR_LAUNCH:
    StringToSocket(s, "FLYING_STABILIZED_ARMED_FOR_LAUNCH");
    break;
  case smFLYING_WAYPOINT_ARMED_FOR_LAUNCH:
    StringToSocket(s, "FLYING_WAYPOINT_ARMED_FOR_LAUNCH");
    break;
#endif
  case smFLYING_SIGNAL_LOST__RETURNING_HOME:
    StringToSocket(s, "FLYING_SIGNAL_LOST__RETURNING_HOME");
    break;
  case smLANDING:
    StringToSocket(s, "LANDING");
    break;
  case smLANDED:
    StringToSocket(s, "LANDED");
    break;
  default:
  case smUNKNOWN:
    StringToSocket(s, "UNKNOWN=");
    ByteToSocket(s, arecraftState);
    break;
  }
 */
}
#endif

