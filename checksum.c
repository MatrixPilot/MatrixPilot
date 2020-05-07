// Program to calculate CRC checksum's for Ublox messages
// Pete Hollands, 2nd Feb 2017

#include <stdio.h>
//#include <stdint.h>

int ubxCrc(const char* data, int) ;

const char config_NAV5[] = {
	//0xB5, 0x62, // Header
	0x06, 0x24, // ID
	0x24, 0x00, // Payload length
	0xFF, 0xFF, // Bit Mask, 0XFF means apply all of the config below
	0x06,       // Dynamic Model Number, Airborne with <g Acceleration
	0x02,       // Position Fixing Mode. 3D only.
	0x00, 0x00, // Fixed altitude (mean sea level) for 2D fix mode only.
	0x00, 0x00, // Part of fixed altitude above (4 bytes in total)
	0x10, 0x27, // Fixed Altitude variance for 2D Mode (4 bytes)
	0x00, 0x00, //
	0x05, 0x00, // Minimum Elevation of Sats in degrees (1 byte). Dead Reckoning Limit in Seconds.
	0xFA, 0x00, // Position DOP Mask (2 bytes)
	0xFA, 0x00, // Time DOP Mask (2 bytes))
	0x64, 0x00, // Position Accuracy Mask (2 bytes) Meters)
	0x2C, 0x01, // Time Accuracy Mask (2 bytes))
	0x00, 0x00, // Static Hold Threshold (1 bytes). DGPS timeout (1 bytes))
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	0x00, 0x00, // Reserved
	//0x13, 0x77  // Checksum
};

/*
const uint8_t config_TP5[] = {
	//0xB5, 0x62, // Header
	0x06, 0x31, // ID 
	
	0x20, 0x00, // Payload length
	
	0x00,       // Time Pulse Index
	0x00,       // Message Version Number
	0x00, 0x00, // Reserved
		
	0x32, 0x00, // Antenna cable delay in nano sec
		
	0x00, 0x00, // RF group delay
	
	0x20, 0xA1, // 500000us, Frequency or period time, depending on setting of bit 'isFreq'
	0x07, 0x00, //
	
	0x00, 0x00, // 50000us,Frequency or period time when locked to GPStime, only used if 'lockedOtherSet' is set
	0x00, 0x00, // 
	
	0x50, 0xC3, // Pulse length or duty cycle, depending on'isLength'
	0x00, 0x00, // 
	
	0x00, 0x00, // Pulse length or duty cycle when locked to GPStime, only used if 'lockedOtherSet' is set
	0x00, 0x00, // 
	
	0x00, 0x00, // User configurable time pulse delay
	0x00, 0x00, // 
	
	0x91, 0x00, // Configuration flags 
	0x00, 0x00, // 
	
	//0x00, 0xFF  // Checksum
};
*/

const char config_TP5[] = {
	//0xB5, 0x62, // Header
	0x06, 0x31, // ID 
	
	0x20, 0x00, // Payload length
	
	0x00,       // Time Pulse Index
	0x00,       // Message Version Number
	0x00, 0x00, // Reserved
		
	0x32, 0x00, // Antenna cable delay in nano sec
		
	0x00, 0x00, // RF group delay
	
	0x20, 0xA1, // 500000us, Frequency or period time, depending on setting of bit 'isFreq'
	0x07, 0x00, //
	
	0x00, 0x00, // 50000us,Frequency or period time when locked to GPStime, only used if 'lockedOtherSet' is set
	0x00, 0x00, // 
	
	0x50, 0xC3, // Pulse length or duty cycle, depending on'isLength'
	0x00, 0x00, // 
	
	0x00, 0x00, // Pulse length or duty cycle when locked to GPStime, only used if 'lockedOtherSet' is set
	0x00, 0x00, // 
	
	0x00, 0x00, // User configurable time pulse delay
	0x00, 0x00, // 
	
	0x51, 0x00, // Configuration flags 
	0x00, 0x00, // 
	
	//0xB5, 0x05 // Checksum
};




int ubxCrc(const char* data, int size) {
    int crc_a = 0;
    int crc_b = 0;
    if (size > 0) {
        do {
            crc_a += *data++;
            crc_b += crc_a;
        } while (--size);
        crc_a &= 0xff;
        crc_b &= 0xff;
    }
    return (int)(crc_a | (crc_b << 8));
}
int main(void)
{
	int i  = 0;
	i = ubxCrc(config_NAV5,40);
	//i = ubxCrc(config_TP5,36);
	printf("The crc is 0x%04hx\n", i);
}