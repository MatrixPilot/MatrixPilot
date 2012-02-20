#include "libUDB_internal.h"

#if (USE_OSD == 1)

#define USE_SPI_HW


#if (BOARD_IS_CLASSIC_UDB == 1)
#define OSD_CS		_LATE0
#define OSD_SCK 	_LATE2
#define OSD_MOSI 	_LATE4
// #define OSD_MISO 	0

#define OSD_CS_TRIS		_TRISE0
#define OSD_SCK_TRIS 	_TRISE2
#define OSD_MOSI_TRIS 	_TRISE4
//#define OSD_MISO_TRIS 	0

#else // UDB4 uses SPI1 port

#define OSD_CS		_LATB2
#define OSD_SCK 	_LATF6
#define OSD_MOSI 	_LATF8
#define OSD_MISO 	_RF7

#define OSD_CS_TRIS		_TRISB2
#define OSD_SCK_TRIS 	_TRISF6
#define OSD_MOSI_TRIS 	_TRISF8
#define OSD_MISO_TRIS 	_TRISF7

#endif


void udb_init_osd( void )
{
#ifdef USE_SPI_HW
	// setup the SPI peripheral
	SPI1STAT = 0x0;				// disable the SPI module (just in case)
	SPI1CON1 = 0x0161;			// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 1; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b000, PPRE = 0b01
	SPI1CON1bits.CKE = 0x01;
	SPI1CON1bits.CKP = 0x00;
	SPI1STAT = 0x8000;			// enable the SPI module

	OSD_MISO_TRIS = 1 ;
	OSD_CS_TRIS = 0;
	OSD_SCK_TRIS = 0;
	OSD_MOSI_TRIS = 0 ;
	OSD_SCK = 1;
	OSD_MOSI  = 1 ;
#else
//	OSD_MISO_TRIS = 1 ;
	OSD_CS_TRIS = OSD_SCK_TRIS = OSD_MOSI_TRIS = 0 ;
	OSD_SCK = OSD_MOSI  = 1 ;
#endif
	OSD_CS = 1 ;
}


void spi_write_raw_byte(unsigned char byte)
{
#ifdef USE_SPI_HW
	short temp;	

	temp = SPI1BUF;					// dummy read of the SPI1BUF register to clear the SPIRBF flag
	SPI1BUF = byte;					// write the data out to the SPI peripheral
	Nop(); Nop(); Nop(); 
    while (!SPI1STATbits.SPIRBF) {}	// wait for the data to be sent out
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
#else
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
#endif	
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
