#include "libUDB_internal.h"

#define OSD_CS		_LATE0
#define OSD_SCK 	_LATE2
#define OSD_MOSI 	_LATE4
#define OSD_MISO 	0


void osd_spi_write(char address, char byte) ;
unsigned char osd_spi_read(char address) ;

void osd_spi_write_location(char row, char column) ;
void osd_spi_write_string(const unsigned char *str) ;


const unsigned char sample[] = {0x1D, 0x0B, 0x17, 0x1A, 0x16, 0x0F, 0xFF} ;


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


extern signed char calculated_heading ;

void osd_countdown(int countdown)
{
	unsigned char x ;
	
	if (countdown == 960)
	{
		osd_spi_write_byte(0xFF) ;	// Finish sending a string, in case that was happening (Prep for reset)
	}
	else if (countdown == 961)
	{
		osd_spi_write(0, 0x02) ;	// VM0: Reset the OSD
	}
	else if (countdown == 950)
	{
		osd_spi_write(0, 0x08) ;	// VM0: enable display of OSD image
	}
	else if (countdown == 949)
	{
		// automatic black level control, have to read, augment and rewrite
		// The data sheet is rather specific about this
		//x = osd_spi_read(0xEC) ;	// OSDBD
		//x &= 0xEF ;
	}
	else if (countdown == 948)
	{
		// osd_spi_write(0x6C, x) ;	// OSDBL
	}
	else if (countdown == 947)
	{
		osd_spi_write(0x04, 0) ;	// DMM set to 0
	}
	else if (countdown == 946)
	{
		osd_spi_write(0, 0x08) ;	// VM0: enable display of OSD image
		
		osd_spi_write_location(9, 11) ;
		osd_spi_write_string(sample) ;
	}
	else if (countdown < 940) {
		osd_spi_write(0, 0x08) ;	// VM0: enable display of OSD image
		
		
		osd_spi_write_location(6, 18) ;
		osd_spi_write_int(calculated_heading) ;
		
		osd_spi_write_location(6, 7) ;
		osd_spi_write_int(countdown) ;
	}
}
