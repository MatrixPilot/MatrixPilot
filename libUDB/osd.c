#include "libUDB_internal.h"

#define OSD_CS		_LATE0
#define OSD_SCK 	_LATE2
#define OSD_MOSI 	_LATE4
#define OSD_MISO 	0


void init_osd( void )
{
	_TRISE0 = _TRISE2 = _TRISE4 = 0 ;
	_LATE0  = _LATE2  = _LATE4  = 1 ;
}


void spi_write_raw_byte(unsigned char byte)
{
	unsigned char SPICount ;						// Counter used to clock out the data
	
	for (SPICount = 0; SPICount < 8; SPICount++)	// Prepare to clock out the Address byte
	{
		OSD_MOSI = ( (byte & 0x80) != 0 ) ;			// Check for a 1 and set the MOSI line appropriately
		
		OSD_SCK = 1 ;								// Toggle the clock line
		OSD_SCK = 0 ;
		
		byte <<= 1 ;								// Shift to get the next bit
	}
}


unsigned char spi_read_raw_byte( void )
{
	unsigned char SPICount ;						// Counter used to clock out the data
	unsigned char SPIData = 0 ;						// Counter used to clock out the data
	
	for (SPICount = 0; SPICount < 8; SPICount++)	// Prepare to clock out the Address byte
	{
		SPIData <<= 1 ;								// Rotate the data
		
		OSD_SCK = 1 ;								// Raise the clock to clock the data out of the MAX7456
		if (OSD_MISO) SPIData |= 1 ;				// Read the data bit
		OSD_SCK = 0 ;								// Drop the clock ready for the next bit
	}
	
	return SPIData ;
}


void osd_spi_write_byte(char byte)
{
	OSD_CS = 1 ;				// Make sure we start with active-low CS high
	OSD_SCK = 0 ;				// and CK low
	
	OSD_CS = 0 ;				// Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(byte) ;	// Send the data
	
	OSD_CS = 1 ;				// Set active-low CS high to end the SPI cycle 
	
	OSD_MOSI = 0 ;
}


void osd_spi_write(char addr, char byte)
{
	OSD_CS = 1 ;				// Make sure we start with active-low CS high
	OSD_SCK = 0 ;				// and CK low
	
	OSD_CS = 0 ;				// Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr) ;	// Send the Address
	spi_write_raw_byte(byte) ;	// Send the data
	
	OSD_CS = 1 ;				// Set active-low CS high to end the SPI cycle 
	
	OSD_MOSI = 0 ;
}


unsigned char osd_spi_read(char addr)
{
	unsigned char SPIData = 0 ;
	
	OSD_CS = 1 ;				// Make sure we start with active-low CS high
	OSD_SCK = 0 ;				// and CK low
	
	OSD_CS = 0 ;				// Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr) ;	// Send the Address
	OSD_MOSI = 0 ;
	SPIData = spi_read_raw_byte() ;	// Send the data
	
	OSD_CS = 1 ;				// Set active-low CS high to end the SPI cycle 
	
	return SPIData ;
}


void osd_spi_write_location(char row, char column)
{
	int loc = row*30 + column ;
	
	osd_spi_write(0x05, (unsigned char)(loc>>8)) ;	// DMAH
	osd_spi_write(0x06, (unsigned char)(loc & 0xFF)) ;	// DMAL
}


void osd_spi_write_string(const unsigned char *str)
{
	osd_spi_write(0x04,1) ;		// DMM: Enable auto-increment mode
	
	while (1)
	{
		osd_spi_write_byte(*str) ;	// Disableble auto-increment mode when sending 0xFF at the end of a string
		if (*str == 0xFF) break;
		str++;
	}
}


void osd_spi_write_raw_uint(unsigned int val)
{
	int d = (val / 10000) ;
	osd_spi_write_byte((d) ? d : 0x0A) ;
	val -= d*10000 ;
	
	d = (val / 1000) ;
	osd_spi_write_byte((d) ? d : 0x0A) ;
	val -= d*1000 ;
	
	d = (val / 100) ;
	osd_spi_write_byte((d) ? d : 0x0A) ;
	val -= d*100 ;
	
	d = (val / 10) ;
	osd_spi_write_byte((d) ? d : 0x0A) ;
	val -= d*10 ;
	
	osd_spi_write_byte((val) ? val : 0x0A) ;
	
	osd_spi_write_byte(0xFF) ;		// Disableble auto-increment mode
}


void osd_spi_write_int(int val)
{
	osd_spi_write(0x04,1) ;		// DMM: Enable auto-increment mode
	
	if (val < 0)
		osd_spi_write_byte(0x49) ;	// '-'
	
	osd_spi_write_raw_uint(abs(val)) ;
}


void osd_spi_write_uint(unsigned int val)
{
	osd_spi_write(0x04,1) ;		// DMM: Enable auto-increment mode
	osd_spi_write_raw_uint(val) ;
}
