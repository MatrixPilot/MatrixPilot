#include <stdint.h>


// http://www.ccsinfo.com/forum/viewtopic.php?t=24977
uint16_t crc16(const uint8_t* buf, uint8_t count)
{
	uint16_t crc = 0;
	int8_t i;

	while (count--) {
		crc = crc ^ *buf++ << 8;
		for (i = 0; i < 8; i++) {
			if (crc & 0x8000) {
				crc = crc << 1 ^ 0x1021;
			} else {
				crc = crc << 1;
			}
		}
	}
	return crc;
}
