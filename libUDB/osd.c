#include "libUDB_internal.h"

#if (USE_OSD == 1)


#if (BOARD_IS_CLASSIC_UDB == 1)
#define OSD_CS		_LATE0
#define OSD_SCK 	_LATE2
#define OSD_MOSI 	_LATE4
// #define OSD_MISO 	0

#else // UDB4 uses SPI1 port
#define OSD_CS		_LATF7 // _LATB2
#define OSD_SCK 	_LATF8 // _LATF6
#define OSD_MOSI 	_LATF6 // _LATF7
// #define OSD_MISO 	0  // _LATF8
#endif


void udb_init_osd( void )
{
#if (BOARD_IS_CLASSIC_UDB == 1)
	_TRISE0 = _TRISE2 = _TRISE4 = 0 ;
	_LATE0  = _LATE2  = _LATE4  = 1 ;
#else
	_TRISF6 = _TRISF7 = _TRISF8 = 0 ;
	_LATF6  = _LATF7  = _LATF8  = 1 ;
#endif
}


void spi_write_raw_byte(unsigned char byte)
{
	unsigned char SPICount ;						// Counter used to clock out the data
	
	for (SPICount = 0; SPICount < 8; SPICount++)	// Prepare to clock out the Address byte
	{
		unsigned char outBit = ((byte & 0x80) != 0) ; // Check for a 1 and set the MOSI line appropriately
		if (outBit) OSD_MOSI = 1 ;					// Write this bit using the bit-set / bit-clear instrictions
		else 		OSD_MOSI = 0 ;
		
		OSD_SCK = 1 ;								// Toggle the clock line up
#if ( BOARD_IS_CLASSIC_UDB == 1 && CLOCK_CONFIG == CRYSTAL_CLOCK )
		Nop(); Nop(); Nop();						// Kill some time with SCK high to make a more solid pulse
#else
		Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
#endif
		byte <<= 1 ;								// Shift to get the next bit
		OSD_SCK = 0 ;								// Toggle the clock line back down
	}
	
	return ;
}


void osd_spi_write_byte(char byte)
{
	OSD_CS = 1 ;				// Make sure we start with active-low CS high
	OSD_SCK = 0 ;				// and CK low
	
	OSD_CS = 0 ;				// Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(byte) ;	// Send the data
	
	OSD_CS = 1 ;				// Set active-low CS high to end the SPI cycle 
	
	Nop(); Nop(); Nop(); Nop();	// Kill some time with CS high to make a more solid pulse
	
	OSD_MOSI = 0 ;
	
	return ;
}


void osd_spi_write(char addr, char byte)
{
	OSD_CS = 1 ;				// Make sure we start with active-low CS high
	OSD_SCK = 0 ;				// and CK low
	
	OSD_CS = 0 ;				// Set active-low CS low to start the SPI cycle 
	spi_write_raw_byte(addr) ;	// Send the Address
	spi_write_raw_byte(byte) ;	// Send the data
	
	OSD_CS = 1 ;				// Set active-low CS high to end the SPI cycle 
	
	Nop(); Nop(); Nop(); Nop();	// Kill some time with CS high to make a more solid pulse
	
	OSD_MOSI = 0 ;
	
	return ;
}


/*
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
	
	Nop(); Nop(); Nop(); Nop();	// Kill some time with CS high to make a more solid pulse
	
	return SPIData ;
}
*/


void osd_spi_write_location(int loc)
{
	osd_spi_write(0x05, (unsigned char)(loc>>8)) ;	// DMAH
	osd_spi_write(0x06, (unsigned char)(loc & 0xFF)) ;	// DMAL
	
	return ;
}


void osd_spi_write_string(const unsigned char *str)
{
	osd_spi_write(0x04,1) ;		// DMM: Enable auto-increment mode
	
	while (1)
	{
		osd_spi_write_byte(*str) ;	// Disables auto-increment mode when sending 0xFF at the end of a string
		if (*str == 0xFF) break ;
		str++ ;
	}
	
	return ;
}


void osd_spi_write_vertical_string_at_location(int loc, const unsigned char *str)
{
	while (1)
	{
		if (*str == 0xFF) break ;
		if (loc >= 480) break ;			// 30*16
		osd_spi_write_location(loc) ;
		osd_spi_write(0x07, *str) ;
		str++ ;
		loc += 30 ;
	}
	
	return ;
}


void osd_spi_erase_chars(unsigned char n)
{
	osd_spi_write(0x04,1) ;		// DMM: Enable auto-increment mode
	
	while (n)
	{
		osd_spi_write_byte(0) ;	// Write a blank space
		n-- ;
	}
	osd_spi_write_byte(0xFF) ;	// Disable auto-increment mode 
	
	return ;
}


void osd_spi_write_number(long val, char num_digits, char decimal_places, char num_flags, char header, char footer)
{
	boolean startWriting = 0 ;
	long d;
	
	osd_spi_write(0x04,1) ;			// DMM: Enable auto-increment mode
	
	if (header)
		osd_spi_write_byte(header) ;
	
	if (num_flags & NUM_FLAG_SIGNED)
	{
		if (val < 0)
		{
			osd_spi_write_byte(0x49) ;	// '-'
			val = -val ;
		}
		else
			osd_spi_write_byte(0x00) ;	// ' '
	}
	
	switch (num_digits)
	{
		case 0:
		case 10:
			d = (val / 1000000000) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(0x80 + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x8A) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*1000000000 ;
			
		case 9:
			d = (val / 100000000) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 9) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*100000000 ;
		
		case 8:
			d = (val / 10000000) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 8) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*10000000 ;
		
		case 7:
			d = (val / 1000000) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 7) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*1000000 ;
		
		case 6:
			d = (val / 100000) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 6) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*100000 ;
		
		case 5:
			d = (val / 10000) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 5) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*10000 ;
		
		case 4:
			d = (val / 1000) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 4) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*1000 ;
		
		case 3:
			d = (val / 100) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 3) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*100 ;
		
		case 2:
			d = (val / 10) ;
			if (d) startWriting = 1 ;
			if (startWriting)
				osd_spi_write_byte(((decimal_places == 2) ? 0xE0 : 0x80) + d) ;
			else if (num_digits && (num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x80) ;
			else if (num_digits && !(num_flags & NUM_FLAG_ZERO_PADDED))
				osd_spi_write_byte(0x00) ;
			val -= d*10 ;
		
		case 1:
			osd_spi_write_byte(((decimal_places == 1) ? 0xE0 : 0x80) + val) ;
	}
	
	if (footer)
		osd_spi_write_byte(footer) ;
	
	if (num_digits == 0)
		osd_spi_write_byte(0x00) ;
	
	osd_spi_write_byte(0xFF) ;		// Disables auto-increment mode
	
	return ;
}


#endif
