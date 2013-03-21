#include <spi.h>
#include <stdint.h>
#include <stdio.h>
#include "AT45D.h"


#define DF_CS			_LATE7
#define DF_SCK 			_LATG6
#define DF_MOSI 		_LATG8
#define DF_MISO 		_RG7

#define DF_CS_TRIS		_TRISE7
#define DF_SCK_TRIS 	_TRISG6
#define DF_MOSI_TRIS 	_TRISG8
#define DF_MISO_TRIS 	_TRISG7

// configuration for the Atmel AT45DB321D device
#define PAGE_BITS 10
#define PAGE_SIZE 528
//#define PageBits 9
//#define PageSize 264


// DataFlash commands
#define StatusReg			0xD7	// Read Status register
#define ReadMfgID			0x9F	// Read Manufacturer and Device ID

#define FlashPageRead		0xD2	// Main memory page read
#define Buf1Read			0xD4	// Buffer 1 read, use 0xD1 for 33MHz read
#define Buf2Read			0xD6	// Buffer 2 read, use 0xD3 for 33MHz read
#define Buf1ToFlashWE		0x83	// Buffer 1 to main memory page program with built-in erase
#define Buf2ToFlashWE		0x86	// Buffer 2 to main memory page program with built-in erase
#define Buf1ToFlash			0x88	// Buffer 1 to main memory page program without built-in erase
#define Buf2ToFlash			0x89	// Buffer 2 to main memory page program without built-in erase
#define Buf1Write			0x84	// Buffer 1 write
#define Buf2Write			0x87	// Buffer 2 write
#define FlashToBuf1Transfer 0x53	// Main memory page to buffer 1 transfer
#define FlashToBuf2Transfer 0x55	// Main memory page to buffer 2 transfer
#define FlashToBuf1Compare	0x60	// Main memory page to buffer 1 compare
#define FlashToBuf2Compare	0x61	// Main memory page to buffer 2 compare
//#define AutoPageReWrBuf1	0x58	// Auto page rewrite through buffer 1
//#define AutoPageReWrBuf2	0x59	// Auto page rewrite through buffer 2
//#define FlashProgBuf1		0x82	// Main memory page program through buffer 1
//#define FlashProgBuf2		0x85	// Main memory page program through buffer 2

#define PageEraseCmd		0x81	// Page erase
#define BlockEraseCmd		0x50	// Block (eight pages) erase
#define SectorEraseCmd		0x7C	// Sector erase

#define ContArrayRead              0x68    // Continuous Array Read (Note : Only A/B-parts supported)
#define EnterDeepPowerdown         0xB9    // Enter Deep Powerdown mode
#define ExitDeepPowerdown          0xAB    // Exit Deep powerdown mode


static void DF_CS_inactive(void)
{
	DF_CS = 1 ;
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
}

static void DF_CS_active(void)
{
	DF_CS = 0 ;
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
}

static void DF_reset(void) 
{
	DF_CS_inactive();
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
	DF_CS_active();
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
}

static unsigned char DF_SPI_RW(unsigned char output)
{
	unsigned char result;

	result = SPI1BUF;					// dummy read of the SPIBUF register to clear the SPIRBF flag
	SPI2BUF = output;					// write the data out to the SPI peripheral
	Nop(); Nop(); Nop(); 
    while (!SPI2STATbits.SPIRBF) {}	// wait for the data to be sent out
	Nop(); Nop(); Nop(); Nop(); Nop(); Nop();	// Kill some time with SCK high to make a more solid pulse
	result = SPI2BUF;
	return result;
}

static void Read_DF_ID(void)
{
	unsigned char manufacturer;
	unsigned char deviceID_1;
	unsigned char deviceID_2;
	unsigned char ext_str_len;

    DF_CS_active();
    DF_SPI_RW(ReadMfgID);
	manufacturer = DF_SPI_RW(0x00);
    deviceID_1   = DF_SPI_RW(0x00);
    deviceID_2   = DF_SPI_RW(0x00);
    ext_str_len  = DF_SPI_RW(0x00);
    DF_CS_inactive();

    if (manufacturer == 0x1f) 
	{
		printf("Atmel ");
	}
    if (deviceID_1 == 0x27) 
	{
		printf("32 Mb DataFlash");
	}
	printf("\r\n");
}

void init_dataflash(void)
{
	SPI2STAT = 0x0;				// disable the SPI module (just in case)
	SPI2CON1 = 0x0161;			// FRAMEN = 0, SPIFSD = 0, DISSDO = 0, MODE16 = 0; SMP = 0; CKP = 1; CKE = 1; SSEN = 0; MSTEN = 1; SPRE = 0b000, PPRE = 0b01
	SPI2CON1bits.CKE = 0x01;
	SPI2CON1bits.CKP = 0x00;
	SPI2STAT = 0x8000;			// enable the SPI module

	DF_MISO_TRIS = 1;
	DF_CS_TRIS = 0;
	DF_SCK_TRIS = 0;
	DF_MOSI_TRIS = 0;
	DF_SCK = 1;
	DF_MOSI  = 1;
	DF_CS_inactive();

    Read_DF_ID();
}

/*****************************************************************************
* Function name : Read_DF_status
*
* Returns : One status byte
*
* Purpose : The status register can be used to determine the device’s 
				ready/busy status, 
				page size, 
				a main memory page to buffer compare operation result, 
				the sector protection status, 
				or the device density. 

The status register can be read at any time, including during an internally self-timed program or erase operation.

Ready/busy status is indicated using bit 7 of the status register. If bit 7 is a one, then the device is not busy and is ready to
accept the next command. If bit 7 is a zero, then the device is in a busy state. Since the data in the status register is constantly
updated, the user must toggle the SCK pin to check the ready/busy status.
There are several operations that can cause the device to be in a busy state:
? Main memory page to buffer transfer
? Main memory page to buffer compare
? Buffer to main memory page program
? Main memory page program through buffer
? Page erase, block erase, sector erase, and chip erase
? Auto page rewrite

The result of the most recent main memory page to buffer compare operation is indicated using bit 6 of the status register. If bit
6 is a zero, then the data in the main memory page matches the data in the buffer. If bit 6 is a one, then at least one bit of the
data in the main memory page does not match the data in the buffer.

Bit 1 of the status register is used to provide information to the user whether sector protection has been enabled or disabled,
either by the software-controlled or hardware-controlled method. A logic one indicates that sector protection has been enabled,
and logic zero indicates that sector protection has been disabled.

Bit 0 o the status register indicates whether the page size of the main memory array is configured for a “power of two” binary
page size (512 bytes) or a standard DataFlash page size (528 bytes). If bit 0 is a one, then the page size is set to 512 bytes. If
bit 0 is a zero, then the page size is set to 528 bytes.

The device density is indicated using bits 5, 4, 3, and 2 of the status register. For the AT45DB321D, the four bits are 1101. The
decimal value of these four binary bits does not equate to the device density — the four bits represent a combinational code
relating to differing densities of DataFlash devices. The device density is not the same as the density code indicated in the
JEDEC device ID information. The device density is provided only for backward compatibility.

Status Register Format
Bit 7	Bit 6	Bit 5	Bit 4	Bit 3	Bit 2	Bit 1	Bit 0
RDY/BSY	COMP	1		1		0		1		Protect Page-size

******************************************************************************/
static unsigned char Read_DF_status(void)
{
    unsigned char result;

    DF_CS_active();
    result = DF_SPI_RW(StatusReg);
    result = DF_SPI_RW(0x00);
    DF_CS_inactive();

    return result;
}

/*****************************************************************************
Chip Erase(1)
The entire main memory can be erased at one time by using the chip erase command.
To execute the chip erase command, a four-byte command sequence, C7H, 94H, 80H, and 9AH, must be clocked into the
device

WARNING: DO NOT USE!
******************************************************************************/
void Chip_Erase(void)
{
//	DF_CS_active();
//	DF_SPI_RW(0xC7);
//	DF_SPI_RW(0x94);
//	DF_SPI_RW(0x80);
//	DF_SPI_RW(0x9A);
//	DF_CS_inactive();
}

/*****************************************************************************
*
* Function name : Page_To_Buffer
*
* Parameters : BufferNo -> Decides usage of either buffer 1 or 2
*
* PageAdr -> Address of page to be transferred to buffer
*
* Purpose : Transfers a page from flash to Dataflash SRAM buffer

To perform a main memory page to buffer transfer for:

the standard page size (528 bytes), three address bytes comprised of 
1 don’t care bit, 
13 page address bits (PA12 - PA0) that specify the page in main memory that is to be transferred, and 
10 don’t care bits. 

the binary page size (512 bytes), three address bytes consisting of 
2 don’t care bits, 
13 page address bits (A21 - A9) that specify the page in the main memory that is to be transferred, and 
9 don’t care bits.

******************************************************************************/
void Page_To_Buffer (unsigned int PageAdr, unsigned char BufferNo)
{
    DF_CS_active();
    if (1 == BufferNo) //transfer flash page to buffer 1
    {
        DF_SPI_RW(FlashToBuf1Transfer);
        DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS))); //upper part of page address
        DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8))); //lower part of page address
        DF_SPI_RW(0x00); //don't cares
    }
#ifdef USE_BUFFER2
    else if (2 == BufferNo) //transfer flash page to buffer 2
    {
        DF_SPI_RW(FlashToBuf2Transfer);
        DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS)));	//upper part of page address
        DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8))); //lower part of page address
        DF_SPI_RW(0x00);	//don't cares
    }
#endif
    DF_CS_inactive();

    while(!(Read_DF_status() & 0x80)); //monitor the status register, wait until busy-flag is high
}

/*****************************************************************************
* Function name : Buffer_Read_Byte
*
* Returns : One read byte (any value)
*
* Parameters : BufferNo -> Decides usage of either buffer 1 or 2
*
* IntPageAdr -> Internal page address
*
* Purpose : Reads one byte from one of the Dataflash
*
* internal SRAM buffers
*
For the standard buffer (528 bytes), the opcode must be followed by three address bytes comprised of 
14 don’t care bits and 
10 buffer address bits (BFA9 - BFA0)

For the binary buffer (512 bytes), the opcode must be followed by three address bytes comprised of 
15 don’t care bits and 
9 buffer address bits (BFA8 - BFA0)

Following the address bytes, one don’t care byte must be clocked in to initialize the read operation
******************************************************************************/
unsigned char Buffer_Read_Byte(unsigned char BufferNo, unsigned int IntPageAdr)
{
    unsigned char data;

    DF_CS_active();
    if (1 == BufferNo) //read byte from buffer 1
    {
        DF_SPI_RW(Buf1Read);	//buffer 1 read op-code
        DF_SPI_RW(0x00);	//don't cares
        DF_SPI_RW((unsigned char)(IntPageAdr>>8)); //upper part of internal buffer address
        DF_SPI_RW((unsigned char)(IntPageAdr)); //lower part of internal buffer address
        DF_SPI_RW(0x00);	//don't cares
        data = DF_SPI_RW(0x00);	//read byte
    }
#ifdef USE_BUFFER2
    else if (2 == BufferNo) //read byte from buffer 2
    {
        DF_SPI_RW(Buf2Read); //buffer 2 read op-code
        DF_SPI_RW(0x00); //don't cares
        DF_SPI_RW((unsigned char)(IntPageAdr>>8)); //upper part of internal buffer address
        DF_SPI_RW((unsigned char)(IntPageAdr)); //lower part of internal buffer address
        DF_SPI_RW(0x00); //don't cares
        data = DF_SPI_RW(0x00); //read byte
    }
#endif
    DF_CS_inactive();

    return data; //return the read data byte
}

/*****************************************************************************
* Function name : Buffer_Write_Byte
*
* Parameters : IntPageAdr -> Internal page address to write byte to
*
* BufferAdr -> Decides usage of either buffer 1 or 2
*
* Data -> Data byte to be written
*
* Purpose : Writes one byte to one of the Dataflash
*
* internal SRAM buffers
*
For the standard buffer (528 bytes), the opcode must be followed by three address bytes comprised of 
14 don’t care bits and 
10 buffer address bits (BFA9 - BFA0)

For the binary buffer (512 bytes), the opcode must be followed by three address bytes comprised of 
15 don’t care bits and 
9 buffer address bits (BFA8 - BFA0)

The buffer address bits specify the first byte in the buffer to be written.
******************************************************************************/
void Buffer_Write_Byte(unsigned char BufferNo, unsigned int IntPageAdr, unsigned char Data)
{
    DF_CS_active();
    if (1 == BufferNo) //write byte to buffer 1
    {
        DF_SPI_RW(Buf1Write); //buffer 1 write op-code
        DF_SPI_RW(0x00); //don't cares
        DF_SPI_RW((unsigned char)(IntPageAdr>>8)); //upper part of internal buffer address
        DF_SPI_RW((unsigned char)(IntPageAdr)); //lower part of internal buffer address
        DF_SPI_RW(Data); //write data byte
    }
#ifdef USE_BUFFER2
    else if (2 == BufferNo) //write byte to buffer 2
    {
        DF_SPI_RW(Buf2Write); //buffer 2 write op-code
        DF_SPI_RW(0x00); //don't cares
        DF_SPI_RW((unsigned char)(IntPageAdr>>8)); //upper part of internal buffer address
        DF_SPI_RW((unsigned char)(IntPageAdr)); //lower part of internal buffer address
        DF_SPI_RW(Data); //write data byte
    }
#endif
    DF_CS_inactive();
}

/*****************************************************************************
* Function name : Buffer_To_Page
*
* Parameters : BufferAdr -> Decides usage of either buffer 1 or 2
*			   PageAdr -> Address of flash page to be programmed
*
* Purpose : Transfers a page from Dataflash SRAM buffer to flash
*
For the standard page size (528 bytes), the opcode must be followed by three address bytes consist of 
1 don’t care bit, 
13 page address bits (PA12 - PA0) that specify the page in the main memory to be written, and 
10 don’t care bits.

For the binary page size (512 bytes), the opcode must be followed by three address bytes consisting of 
2 don’t care bits, 
13 page address bits (A21 - A9) that specify the page in the main memory to be written, and
9 don’t care bits.
******************************************************************************/
void Buffer_To_Page(unsigned char BufferNo, unsigned int PageAdr)
{
    DF_CS_active();
    if (1 == BufferNo)
    {
        DF_SPI_RW(Buf1ToFlashWE); //buffer 1 to flash with erase op-code
        DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS))); //upper part of page address
        DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8))); //lower part of page address
        DF_SPI_RW(0x00); //don't cares
    }
#ifdef USE_BUFFER2
    else if (2 == BufferNo)
    {
        DF_SPI_RW(Buf2ToFlashWE); //buffer 2 to flash with erase op-code
        DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS))); //upper part of page address
        DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8))); //lower part of page address
        DF_SPI_RW(0x00); //don't cares
    }
#endif
    DF_CS_inactive();
    while (!(Read_DF_status() & 0x80)); //monitor the status register, wait until busy-flag is high
}

#define DF_reset DF_reset()

//DF_CS_inactive(); DF_CS_active();

#define DF_CS_inactive DF_CS_inactive()
#define DF_CS_active DF_CS_active()

void Activate(void)
{
    DF_CS_active;    // to reset dataflash command decoder
}

void Deactivate(void)
{
    DF_CS_inactive;  // make sure to toggle CS signal in order
}

/*****************************************************************************
*  Function name : EnterDeepPowerDown
*
*  Purpose : Put the chip into deep power-down mode
*
*            After initial power-up, the device will default in standby
*            mode. The Deep Power-down command allows the device to enter
*            into the lowest power consumption mode.

*            To enter the Deep Power-down mode, the CS pin must first be
*            asserted. Once the CS pin has been asserted, an opcode
*            of B9H command must be clocked in via input pin (SI).
*            After the last bit of the command has been clocked in,
*            the CS pin must be de-asserted to initiate the Deep
*            Power-down operation.
*                                      
*            After the CS pin is de-asserted, the will device enter the
*            Deep Power-down mode within 3uS. Once the device has entered
*            the Deep Power-down mode, all instructions are ignored except
*            for the Resume from Deep Power-down command.
*
*            Dataflash current consumption:
*            ?  7 to 15 mA Active Read Current Typical
*            ? 25 to 50 µA Standby Current Typical
*            -  5 to 10 µA Deep Power-down Typical
******************************************************************************/
void EnterDeepPowerDown(void)
{
    DF_CS_active;                    // Assert CS
    DF_SPI_RW(EnterDeepPowerdown);   // Send power-down command
    DF_CS_inactive;                  // Deassert CS
}

/*****************************************************************************
*  Function name : ExitDeepPowerDown
*
*  Purpose : Remove the chip into deep power-down mode
*
*            The Resume from Deep Power-down command takes the device
*            out of the Deep Power-down mode and returns it to the normal
*            standby mode.
*                                      
*            To Resume from Deep Power-down mode, the CS pin must first
*            be asserted and an opcode of ABH command must be clocked in
*            via input pin (SI). After the last bit of the command has
*            been clocked in, the CS pin must be de-asserted to terminate
*            the Deep Power-down mode.
*                                      
*            After the CS pin is de-asserted, the device will return to
*            the normal standby mode within 35uS. The CS pin must remain
*            high during this time before the device can receive any
*            commands.
*                                      
*            After resuming form Deep Powerdown, the device will     return
*            to the normal standby mode.
******************************************************************************/
void ExitDeepPowerDown(void)
{
    DF_CS_active;                  // Assert CS
    DF_SPI_RW(ExitDeepPowerdown);  // Send resume from power-down command
    DF_CS_inactive;                // Deassert CS
}

/*****************************************************************************
*  Function name : ReadDFStatus
*
*  Returns : One status byte
*
*  Purpose : Status info concerning the Dataflash chip
*
*            The status register can be used to determine the device?s
*            ready/busy status, page size, a Main Memory Page to Buffer
*            Compare operation result, the Sector Protection status or
*            the device density. The Status Register can be read at any
*            time, including during an internally self-timed program or
*            erase operation.
*                                      
*            Ready/busy status is indicated using bit 7 of the status
*            register. If bit 7 is a 1, then the device is not busy and
*            is ready to accept the next command. If bit 7 is a 0, then
*            the device is in a busy state.
*                                      
*            The result of the most recent Main Memory Page to Buffer
*            Compare operation is indicated using bit 6 of the status
*            register. If bit 6 is a 0, then the data in the main memory
*            page matches the data in the buffer.
*                                      
*            Bit 1 in the Status Register is used to provide information
*            to the user whether or not the sector protection has been
*            enabled or disabled, either by software-controlled method
*            or hardware-controlled method. A logic 1 indicates that
*            sector protection has been enabled .
*                                      
*            Bit 0 in the Status Register indicates whether the page size
*            of the main memory array is configured for ?power of 2? binary
*            page size (256 bytes) or the DataFlash standard page size (264)
*                                      
*            The device density is indicated using bits 5, 4, 3, and 2 of
*            the status register. For the AT45DB041D, the four bits are 0111.
******************************************************************************/
uint8_t ReadDFStatus(void)
{
	uint8_t result;

	DF_reset;                            //reset dataflash command decoder
	result = DF_SPI_RW(StatusReg);       //send status register read op-code
	result = DF_SPI_RW(0x00);            //dummy write to get result
//	device_id = ((result & 0x3C) >> 2);  //get the device id bits, butterfly dataflash should be 0111
	return result;                       //return the read status register value
}

/*****************************************************************************
*  Function name : BufferToPage
*
*  Parameters : BufferAdr -> Decides usage of either buffer 1 or 2
*               PageAdr   -> Address of flash page to be programmed
*
*  Purpose : Transfers a page from dataflash SRAM buffer to flash
*                                      
*            Data written into either buffer 1 or buffer 2 can be
*            programmed into the main memory. A 1-byte opcode, 83H
*            for buffer 1 or 86H for buffer 2, must be clocked into
*            the device. For the DataFlash standard page size (264 bytes),
*            the opcode must be followed by three address bytes consist of
*            4 don't care bits, 11 page address bits (PA10 - PA0) that
*            specify the page in the main memory to be written and 9 don't
*            care bits.
*
*            When a low-to-high transition occurs on the CS pin, the part
*            will first erase the selected page in main memory (the erased
*            state is a logic 1) and then program the data stored in the
*            buffer into the specified page in main memory. Both the erase
*            and the programming of the page are internally self-timed and
*            should take place in a maximum time of 35mS. During this time,
*            the status register will indicate that the part is busy.
*                                      
*            Page address bit locations within the 24 address bits:
*            xxxxBA98 7654321x  xxxxxxxx
*                                      
*            The page address range is 0-2047, requiring 11 address bits
*            to express. This is passed in as a 16 bit value in PageAdr:
*            xxxxxBA9 87654321
*                                      
*            To write this out PageAdr is first shifted right 7 bits so
*            that the low byte forms the first address byte:
*            xxxxxBA98
*                                      
*            Then PageAdr is shifted left 1 bit so that the low byte forms
*            the second address byte:
*            7654321x
*                                              
*            The third address byte is written as all zeros.
*                                      

To perform a buffer to main memory page program with built-in erase:

For the standard DataFlash page size (528 bytes), three address bytes consist of 
1 don’t care bit, 
13 page address bits (PA12 - PA0) that specify the page in the main memory to be written, and 
10 don’t care bits. 

For the binary page size (512 bytes), three address bytes consisting of 
2 don’t care bits, 
13 page address bits (A21 - A9) that specify the page in the main memory to be written, and
9 don’t care bits.
******************************************************************************/
void BufferToPage(uint8_t BufferNo, uint16_t PageAdr)
{
//	printf("BufferToPage(BufferNo %u, PageAdr %u)\r\n", BufferNo, PageAdr);
	DF_reset;						   // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)                 // program flash page from buffer 1
		DF_SPI_RW(Buf1ToFlashWE);	   // buffer 1 to flash with erase op-code
	else
		DF_SPI_RW(Buf2ToFlashWE);      // buffer 2 to flash with erase op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS))); //upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8))); //lower part of page address
	DF_SPI_RW(0x00);                   // don't cares
	DF_reset;                          // initiate flash page programming
	while(!(ReadDFStatus() & 0x80));   // monitor the status register, wait until busy-flag is high
}

/*****************************************************************************
*  Function name : PageToBuffer
*
*  Parameters : BufferNo -> Decides usage of either buffer 1 or 2
*               PageAdr  ->  Address of page to be transferred to buffer
*
*  Purpose : Transfers a page from flash to dataflash SRAM buffer
*
*            A page of data can be transferred from the main memory to
*            either buffer 1 or buffer 2. To start the operation for the
*            DataFlash standard page size (264 bytes), a 1-byte opcode,
*            53H for buffer 1 and 55H for buffer 2, must be clocked into
*            the device, followed by three address bytes comprised of 4
*            don't care bits, 11 page address bits (PA10 - PA0), which
*            specify the page in main memory that is to be transferred,
*            and 9 don't care bits.
*
*            The CS pin must be low while toggling the SCK pin to load
*            the opcode and the address bytes from the input pin (SI).
*            The transfer of the page of data from the main memory to
*            the buffer will begin when the CS pin transitions from a
*            low to a high state.
*
*            During the transfer of a page of data (tXFR), the status
*            register can be read to determine whether the transfer
*            has been completed
******************************************************************************/
void PageToBuffer(uint16_t PageAdr, uint8_t BufferNo)
{
//	printf("PageToBuffer(PageAdr %u, BufferNo %u)\r\n", PageAdr, BufferNo);
	DF_reset;						     // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (BufferNo == 1)                   // transfer flash page to buffer 1
		DF_SPI_RW(FlashToBuf1Transfer);  // transfer to buffer 1 op-code
	else
		DF_SPI_RW(FlashToBuf2Transfer);  // transfer to buffer 2 op-code
	DF_SPI_RW((unsigned char)(PageAdr >> (16 - PAGE_BITS))); // upper part of page address
	DF_SPI_RW((unsigned char)(PageAdr << (PAGE_BITS - 8)));  // lower part of page address
	DF_SPI_RW(0x00);                     // don't cares
	DF_reset;                            // init transfer
	while(!(ReadDFStatus() & 0x80));     // monitor the status register, wait until busy-flag is high
}

/*****************************************************************************
*  Function name : ContFlashReadEnable
*
*  Parameters : PageAdr    -> Address of flash page where cont.read starts from
*               IntPageAdr -> Internal page address where cont.read starts from
*
*  Purpose : Initiates a continuous read from a location in the DataFlash
*
*            To start a page read from the DataFlash standard page size (264 bytes),
*            an opcode of D2H must be clocked into the device followed by three
*            address bytes (which comprise the 24-bit page and byte address sequence)
*            and 4 don't care bytes. The first 11 bits (PA10 - PA0) of the 20-bit
*            address sequence specify the page in main memory to be read, and the
*            last 9 bits (BA8 - BA0) of the 20-bit address sequence specify the
*            starting byte address within that page.
*
*            The don't care bytes that follow the address bytes are sent to initialize
*            the read operation. Following the don't care bytes, additional pulses
*            on SCK result in data being output on the SO (serial output) pin. The
*            CS pin must remain low during the loading of the opcode, the address
*            bytes, the don't care bytes, and the reading of data. When the end of
*            a page in main memory is reached, the device will continue reading back
*            at the beginning of the same page.
*
*            A low-to-high transition on the CS pin will terminate the read operation
*            and tri-state the output pin (SO).
*
*            The Main Memory Page Read bypasses both data buffers and leaves the
*            contents of the buffers unchanged.
******************************************************************************/
void ContFlashReadEnable(uint16_t PageAdr, uint16_t IntPageAdr)
{
	DF_reset;                            // reset dataflash command decoder
	DF_SPI_RW(ContArrayRead);            // Continuous Array Read op-code
	DF_SPI_RW((uint8_t)(PageAdr >> 7));  // upper part of page address
	DF_SPI_RW((uint8_t)((PageAdr << 1) + (IntPageAdr >> 8)));  // lower part of page address and MSB of int.page adr.
	DF_SPI_RW((uint8_t)(IntPageAdr));    // LSB uint8_t of internal page address
	DF_SPI_RW(0x00);                     // perform 4 dummy writes
	DF_SPI_RW(0x00);                     // in order to intiate DataFlash
	DF_SPI_RW(0x00);                     // address pointers
	DF_SPI_RW(0x00);
}

/*****************************************************************************
*  Function name : BufferReadEnable
*
*  Parameters : BufferNo   -> Decides usage of either buffer 1 or 2
*               IntPageAdr -> Internal page address
*
*  Purpose : Sets up to read data from one of the dataflash internal SRAM buffers
*
*  			 To perform a buffer read from the DataFlash standard
*  			 buffer (264 bytes), the opcode must be clocked into the
*  			 device followed by three address bytes comprised of 15
*  			 don't care bits and 9 buffer address bits (BFA8 - BFA0).
*
*			 Following the address bytes, one don't care byte must be
*			 clocked in to initialize the read operation. The CS pin
*			 must remain low during the loading of the opcode, the
*			 address bytes, the don't care bytes, and the reading of
*			 data. When the end of a buffer is reached, the device will
*			 continue reading back at the beginning of the buffer.
*
*			 A low-to-high transition on the CS pin will terminate the
*			 read operation and tri-state the output pin (SO).
******************************************************************************/
void BufferReadEnable(uint8_t BufferNo, uint16_t IntPageAdr)
{
//	printf("BufferReadEnable(BufferNo %u, IntPageAdr %u)\r\n", BufferNo, IntPageAdr);
	DF_reset;                            // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)                   // read uint8_t from buffer 1
		DF_SPI_RW(Buf1Read);             // buffer 1 read op-code
	else
		DF_SPI_RW(Buf2Read);             // buffer 2 read op-code
	DF_SPI_RW(0x00);                     // don't cares
	DF_SPI_RW((uint8_t)(IntPageAdr>>8)); // upper part of internal buffer address
	DF_SPI_RW((uint8_t)(IntPageAdr));    // lower part of internal buffer address
	DF_SPI_RW(0x00);                     // don't cares to initialize the read operation
}

/*****************************************************************************
*  Function name : BufferReadByte
*
*  Returns : One read byte (any value)
*
*  Parameters : BufferNo   -> Decides usage of either buffer 1 or 2
*               IntPageAdr -> Internal page address
*
*  Purpose : Reads one byte from one of the dataflash
*            internal SRAM buffers
******************************************************************************/
uint8_t BufferReadByte(uint8_t BufferNo, uint16_t IntPageAdr)
{
	BufferReadEnable(BufferNo, IntPageAdr);
	return DF_SPI_RW(0x00);
}

/*****************************************************************************
*  Function name : BufferReadStr
*
*  Parameters : BufferNo    -> Decides usage of either buffer 1 or 2
*               IntPageAdr  -> Internal page address
*               No_of_bytes -> Number of bytes to be read
*               *BufferPtr  -> address of buffer to be used for read bytes
*
*  Purpose : Reads one or more bytes from one of the dataflash
*            internal SRAM buffers, and puts read bytes into
*            buffer pointed to by *BufferPtr
******************************************************************************/
void BufferReadStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
{
	uint16_t i;

    BufferReadEnable(BufferNo, IntPageAdr);
    for (i = 0; i < No_of_bytes; i++) {
        *(BufferPtr) = DF_SPI_RW(0x00);      // read byte and put it in buffer pointed to by *BufferPtr
//		printf("%02x ", *(BufferPtr));
        BufferPtr++;                         // point to next element in buffer
    }
//	printf("\r\n");
}

/*****************************************************************************
*  Function name : ReadNextByte
*
*  Returns : The next byte read
*
*  Purpose : After the dataflash is configured to read bytes this
*            routine may be used to read single bytes without resending
*            the read parameters or resetting the chip.
******************************************************************************/
uint8_t ReadNextByte(void)
{
	return DF_SPI_RW(0x00);
}

/*****************************************************************************
*  Function name :	BufferWriteEnable
*
*  Parameters :	IntPageAdr  ->  Internal page address to start writing from
*				BufferAdr   ->  Decides usage of either buffer 1 or 2
*                                      
*  Purpose : Sets up for writting bytes to the specified buffer.
*            The user must ensure that CS goes high to terminate
*            this mode before accessing other dataflash functions.
*
*            Data can be clocked in from the input pin (SI) into either
*            buffer 1 or buffer 2. To load data into the DataFlash standard
*            buffer (264 bytes), a 1-byte opcode, 84H for buffer 1 or 87H
*            for buffer 2, must be clocked into the device, followed by
*            three address bytes comprised of 15 don't care bits and 9
*            buffer address bits (BFA8 - BFA0). The 9 buffer address bits
*            specify the first byte in the buffer to be written.
*
*            After the last address byte has been clocked into the device,
*            data can then be clocked in on subsequent clock cycles. If the
*            end of the data buffer is reached, the device will wrap around
*            back to the beginning of the buffer.
*
*            Data will continue to be loaded into the buffer until a
*            low-to-high transition is detected on the CS pin.
******************************************************************************/
void BufferWriteEnable(uint8_t BufferNo, uint16_t IntPageAdr)
{
//	printf("BufferWriteEnable(BufferNo %u, IntPageAdr %u)\r\n", BufferNo, IntPageAdr);
	DF_reset;                            // reset dataflash command decoder
	// Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
	// You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
	// No matter what, a buffer will be selected.
	if (1 == BufferNo)                   // write enable to buffer 1
		DF_SPI_RW(Buf1Write);            // buffer 1 write op-code
	else
		DF_SPI_RW(Buf2Write);            // buffer 2 write op-code
	DF_SPI_RW(0x00);                     // Don't care
	DF_SPI_RW((uint8_t)(IntPageAdr>>8)); // Upper part of internal buffer address
	DF_SPI_RW((uint8_t)(IntPageAdr));    // Lower part of internal buffer address
}

/*****************************************************************************
*  Function name :	BufferWriteByte
*
*  Parameters :		IntPageAdr -> Internal page address to write byte to
*					BufferAdr  -> Specifies which buffer to write to
*					Data       -> Data byte to be written
*
*  Purpose :		Writes one byte to one of the dataflas hinternal SRAM buffers
******************************************************************************/
void BufferWriteByte(uint8_t BufferNo, uint16_t IntPageAdr, uint8_t Data)
{
	BufferWriteEnable(BufferNo, IntPageAdr);
	DF_SPI_RW(Data);
}

/*****************************************************************************
*  Function name : BufferWriteStr
*
*  Parameters : BufferNo    -> Decides usage of either buffer 1 or 2
*               IntPageAdr  -> Internal page address
*               No_of_bytes -> Number of bytes to be written
*               *BufferPtr  -> address of buffer to be used for copy of bytes
*                              from buffer to dataflash buffer 1 (or 2)
*
*  Purpose : Copies one or more bytes to one of the dataflash internal
*            SRAM buffers from MPU SRAM buffer pointed to by *BufferPtr
******************************************************************************/
void BufferWriteStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_bytes, uint8_t *BufferPtr)
{
	uint16_t i;

    BufferWriteEnable(BufferNo, IntPageAdr);
    for (i = 0; i < No_of_bytes; i++) {
//		printf("%02x ", (unsigned char)*BufferPtr);
        DF_SPI_RW(*BufferPtr);        //write byte pointed at by *BufferPtr to dataflash buffer location
        BufferPtr++;                            //point to next element in buffer
    }
//	printf("\r\n");
}

/*****************************************************************************
*  Function name :	WriteNextByte
*
*  Returns :		Write the next byte
*
*  Parameters :		None
*
*  Purpose :		After the dataflash is configured to write bytes this
*                   routine may be used to write single bytes without resending
*                   the parameters or resetting the chip.
******************************************************************************/
void WriteNextByte(uint8_t data)
{
	DF_SPI_RW(data);
}

/*****************************************************************************
*  Function name :	PageBufferCompare
*
*  Returns :		0 match, 1 if mismatch
*
*  Parameters :		BufferAdr -> Decides usage of either buffer 1 or 2
*                   PageAdr   -> Address of flash page to be compared with buffer
*
*  Purpose :		Compares Buffer with Flash-Page
*
*                   A page of data in main memory can be compared to the data
*                   in buffer 1 or buffer 2. To initiate the operation for the
*                   DataFlash standard page size, a 1-byte opcode, 60H for buffer
*                   1 and 61H for buffer 2, must be clocked into the device,
*                   followed by three address bytes consisting of 4 don't care
*                   bits, 11 page address bits (PA10 - PA0) that specify the page
*                   in the main memory that is to be compared to the buffer,
*                   and 9 don't care bits.
*
*                   The CS pin must be low while toggling the SCK pin to load
*                   the opcode and the address bytes from the input pin (SI).
*                   On the low-to-high transition of the CS pin, the data bytes
*                   in the selected main memory page will be compared with the
*                   data bytes in buffer 1 or buffer 2. During this time (tCOMP),
*                   the status register will indicate that the part is busy. On
*                   completion of the compare operation, bit 6 of the status
*                   register is updated with the result of the compare.
*******************************************************************************/
uint8_t PageBufferCompare(uint8_t BufferNo, uint16_t PageAdr)
{
    uint8_t stat;
       
    DF_reset;                              // reset dataflash command decoder
    // Note that this test selects either Buffer 1 or the other buffer, whatever you call it.
    // You can call it Buffer 0 or Buffer 2 and it will work as long as you are consistant.
    // No matter what, a buffer will be selected.
    if (1 == BufferNo)                                                                      
        DF_SPI_RW(FlashToBuf1Compare);  
    else
        DF_SPI_RW(FlashToBuf2Compare);                                          
    DF_SPI_RW((uint8_t)(PageAdr >> 7));    // upper part of page address
    DF_SPI_RW((uint8_t)(PageAdr << 1));    // lower part of page address
    DF_SPI_RW(0x00);                       // don't cares
    DF_reset;                              // reset dataflash command decoder
    do {
        stat = ReadDFStatus();
    } while(!(stat & 0x80));               // monitor the status register, wait until busy-flag is high
    return (stat & 0x40);
}

/*****************************************************************************
*  Function name :	PageErase
*
*  Parameters :		PageAdr -> Address of flash page to be erased
*
*  Purpose :		Sets all bits in the given page (all bytes are 0xff)
*
*                   The Page Erase command can be used to individually erase
*                   any page in the main memory array allowing the Buffer to
*                   Main Memory Page Program to be utilized at a later time.
*                                      
*                   To perform a page erase in the DataFlash standard page
*                   size (264 bytes), an opcode of 81H must be loaded into
*                   the device, followed by three address bytes comprised of
*                   4 don't care bits, 11 page address bits (PA10 - PA0) that
*                   specify the page in the main memory to be erased and 9
*                   don't care bits.
*
*                   When a low-to-high transition occurs on the CS pin, the
*                   part will erase the selected page (the erased state is a
*                   logical 1). The erase operation is internally self-timed
*                   and should take place in a maximum time of 32mS. During
*                   this time, the status register will indicate that the part
*                   is busy.
******************************************************************************/
void PageErase(uint16_t PageAdr)
{
    DF_reset;                            // reset dataflash command decoder
    DF_SPI_RW(PageEraseCmd);             // Page erase op-code
    DF_SPI_RW((uint8_t)(PageAdr >> 7));  // upper part of page address
    DF_SPI_RW((uint8_t)(PageAdr << 1));  // lower part of page address and MSB of int.page adr.
    DF_SPI_RW(0x00);                     // dont cares
    DF_reset;                            // initiate flash page erase
    while(!(ReadDFStatus() & 0x80));     // monitor the status register, wait until busy-flag is high
}

