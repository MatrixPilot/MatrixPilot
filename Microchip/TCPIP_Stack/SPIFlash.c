/*********************************************************************
 *
 *  SPI Flash Memory Driver
 *  - Tested with SST 25VF016B 
 *  - Expected compatibility with other SST (Microchip) SST25 series 
 *    devices
 *
 *********************************************************************
 * FileName:        SPIFlash.c
 * Dependencies:    SPIFlash.h
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.11b or higher
 *					Microchip C30 v3.23 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2010 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *		ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *		used in conjunction with a Microchip ethernet controller for
 *		the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 *
 * Author               		Date    Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * E. Wood              		3/20/08 Original
 * Dave Collier/H. Schlunder	6/09/10	Update for SST25VF010A
********************************************************************/
#define __SPIFLASH_C

#include "HardwareProfile.h"

#if defined(SPIFLASH_CS_TRIS)

#include "TCPIP Stack/TCPIP.h"

#define READ				0x03    // SPI Flash opcode: Read up up to 25MHz
#define READ_FAST			0x0B    // SPI Flash opcode: Read up to 50MHz with 1 dummy byte
#define ERASE_4K			0x20    // SPI Flash opcode: 4KByte sector erase
#define ERASE_32K			0x52    // SPI Flash opcode: 32KByte block erase
#define ERASE_SECTOR		0xD8    // SPI Flash opcode: 64KByte block erase
#define ERASE_ALL			0x60    // SPI Flash opcode: Entire chip erase
#define WRITE				0x02    // SPI Flash opcode: Write one byte (or a page of up to 256 bytes, depending on device)
#define WRITE_WORD_STREAM	0xAD    // SPI Flash opcode: Write continuous stream of 16-bit words (AAI mode); available on SST25VF016B (but not on SST25VF010A)
#define WRITE_BYTE_STREAM	0xAF    // SPI Flash opcode: Write continuous stream of bytes (AAI mode); available on SST25VF010A (but not on SST25VF016B)
#define RDSR				0x05    // SPI Flash opcode: Read Status Register
#define EWSR				0x50    // SPI Flash opcode: Enable Write Status Register
#define WRSR				0x01    // SPI Flash opcode: Write Status Register
#define WREN				0x06    // SPI Flash opcode: Write Enable
#define WRDI				0x04    // SPI Flash opcode: Write Disable / End AAI mode
#define RDID				0x90    // SPI Flash opcode: Read ID
#define JEDEC_ID			0x9F    // SPI Flash opcode: Read JEDEC ID
#define EBSY				0x70    // SPI Flash opcode: Enable write BUSY status on SO pin
#define DBSY				0x80    // SPI Flash opcode: Disable write BUSY status on SO pin

#define BUSY    0x01    // Mask for Status Register BUSY (Internal Write Operaiton in Progress status) bit
#define WEL     0x02    // Mask for Status Register WEL (Write Enable status) bit
#define BP0     0x04    // Mask for Status Register BP0 (Block Protect 0) bit
#define BP1     0x08    // Mask for Status Register BP1 (Block Protect 1) bit
#define BP2     0x10    // Mask for Status Register BP2 (Block Protect 2) bit
#define BP3     0x20    // Mask for Status Register BP3 (Block Protect 3) bit
#define AAI     0x40    // Mask for Status Register AAI (Auto Address Increment Programming status) bit
#define BPL     0x80    // Mask for Status Register BPL (BPx block protect bit read-only protect) bit

#if defined(__PIC24F__) || defined(__PIC24FK__)
    #define PROPER_SPICON1  (0x001B | 0x0120)   // 1:1 primary prescale, 2:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC33F__) || defined(__PIC24H__) || defined(__dsPIC33E__) || defined (__PIC24E__)
    #define PROPER_SPICON1  (0x000F | 0x0120)   // 1:1 primary prescale, 5:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC30F__)
    #define PROPER_SPICON1  (0x0017 | 0x0120)   // 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__PIC32MX__)
    #define PROPER_SPICON1  (_SPI2CON_ON_MASK  | _SPI2CON_CKE_MASK | _SPI2CON_MSTEN_MASK)
#else
    #define PROPER_SPICON1  (0x20)              // SSPEN bit is set, SPI in master mode, FOSC/4, IDLE state is low level
#endif

// Maximum speed of SPI Flash part in Hz
// Should theoretically operate at 25MHz, but need to account for level-shifting delays
#define SPIFLASH_MAX_SPI_FREQ       (16000000ul)

#if defined (__18CXX)
    #define ClearSPIDoneFlag()  {SPIFLASH_SPI_IF = 0;}
    #define WaitForDataByte()   {while(!SPIFLASH_SPI_IF); SPIFLASH_SPI_IF = 0;}
    #define SPI_ON_BIT          (SPIFLASH_SPICON1bits.SSPEN)
#elif defined(__C30__)
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while ((SPIFLASH_SPISTATbits.SPITBF == 1) || (SPIFLASH_SPISTATbits.SPIRBF == 0));
    }

    #define SPI_ON_BIT          (SPIFLASH_SPISTATbits.SPIEN)
#elif defined( __PIC32MX__ )
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while (!SPIFLASH_SPISTATbits.SPITBE || !SPIFLASH_SPISTATbits.SPIRBF);
    }

    #define SPI_ON_BIT          (SPIFLASH_SPICON1bits.ON)
#else
    #error Determine SPI flag mechanism
#endif

// Internal pointer to address being written
static DWORD dwWriteAddr;

// SPI Flash device capabilities
static union
{
	unsigned char v;
	struct
	{
		unsigned char bWriteWordStream : 1;	// Supports AAI Word opcode (0xAD)
		unsigned char bWriteByteStream : 1;	// Supports AAI Byte opcode (0xAF)
		unsigned char bPageProgram : 1;		// Supports Byte program opcode with up to 256 bytes/page (0x02)
		unsigned char filler : 5;
	} bits;
} deviceCaps;


static void _SendCmd(BYTE cmd);
static void _WaitWhileBusy(void);
//static void _GetStatus(void);


/*****************************************************************************
  Function:
    void SPIFlashInit(void)

  Description:
    Initializes SPI Flash module.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is only called once during the lifetime of the application.

  Internal:
    This function sends WRDI to clear any pending write operation, and also
    clears the software write-protect on all memory locations.
  ***************************************************************************/
void SPIFlashInit(void)
{
	BYTE i;
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif

    SPIFLASH_CS_IO = 1;
    SPIFLASH_CS_TRIS = 0;   // Drive SPI Flash chip select pin

    SPIFLASH_SCK_TRIS = 0;  // Set SCK pin as an output
    SPIFLASH_SDI_TRIS = 1;  // Make sure SDI pin is an input
    SPIFLASH_SDO_TRIS = 0;  // Set SDO pin as an output

    // Save SPI state (clock speed)
    SPICON1Save = SPIFLASH_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    ClearSPIDoneFlag();
    #if defined(__C30__)
        SPIFLASH_SPICON2 = 0;
        SPIFLASH_SPISTAT = 0;    // clear SPI
        SPIFLASH_SPISTATbits.SPIEN = 1;
    #elif defined(__C32__)
        SPIFLASH_SPIBRG = (GetPeripheralClock()-1ul)/2ul/SPIFLASH_MAX_SPI_FREQ;
    #elif defined(__18CXX)
        SPIFLASH_SPISTATbits.CKE = 1;       // Transmit data on rising edge of clock
        SPIFLASH_SPISTATbits.SMP = 0;       // Input sampled at middle of data output time
    #endif

	// Read Device ID code to determine supported device capabilities/instructions
	{
	    // Activate chip select
	    SPIFLASH_CS_IO = 0;
	    ClearSPIDoneFlag();
	
	    // Send instruction
	    SPIFLASH_SSPBUF = RDID;
	    WaitForDataByte();
	    Dummy = SPIFLASH_SSPBUF;	
		
		// Send 3 byte address (0x000000), discard Manufacture ID, get Device ID
	    for(i = 0; i < 5; i++)
	    {
		    SPIFLASH_SSPBUF = 0x00;
		    WaitForDataByte();
		    Dummy = SPIFLASH_SSPBUF;
		}

	    // Deactivate chip select
	    SPIFLASH_CS_IO = 1;
		
		// Decode Device Capabilities Flags from Device ID
		deviceCaps.v = 0x00;
		switch(Dummy)
		{
			case 0x43:	// SST25LF020(A)	(2 Mbit)	0xAF, 14us, AAI Byte
			case 0x48:	// SST25VF512(A)	(512 Kbit)	0xAF, 14us, AAI Byte
			case 0x49:	// SST25VF010A		(1 Mbit)	0xAF, 14us, AAI Byte
				deviceCaps.bits.bWriteByteStream = 1;
				break;
				
			case 0x4B:	// SST25VF064C		(64 Mbit)	0x02, 1.5ms/256 byte page, no AAI
				deviceCaps.bits.bPageProgram = 1;
				break;

			//case 0x01:	// SST25WF512		(512 Kbit)	0xAD, 50us, AAI Word
			//case 0x02:	// SST25WF010		(1 Mbit)	0xAD, 50us, AAI Word
			//case 0x03:	// SST25WF020		(2 Mbit)	0xAD, 50us, AAI Word
			//case 0x04:	// SST25WF040		(4 Mbit)	0xAD, 50us, AAI Word
			//case 0x05:	// SST25WF080		(8 Mbit)	0xAD, 14us, AAI Word
			//case 0x41:	// SST25VF016B		(16 Mbit)	0xAD,  7us, AAI Word
			//case 0x4A:	// SST25VF032B		(32 Mbit)	0xAD,  7us, AAI Word
			//case 0x8C:	// SST25VF020B		(2 Mbit)	0xAD,  7us, AAI Word
			//case 0x8D:	// SST25VF040B		(4 Mbit)	0xAD,  7us, AAI Word
			//case 0x8E:	// SST25VF080B		(8 Mbit)	0xAD,  7us, AAI Word				
			// Assume AAI Word programming is supported for the above commented 
			// devices and unknown devices.
			default:	
				deviceCaps.bits.bWriteWordStream = 1;
		}
	}


    // Clear any pre-existing AAI write mode
    // This may occur if the PIC is reset during a write, but the Flash is
    // not tied to the same hardware reset.
    _SendCmd(WRDI);

    // Execute Enable-Write-Status-Register (EWSR) instruction
    _SendCmd(EWSR);

    // Clear Write-Protect on all memory locations
    SPIFLASH_CS_IO = 0;
    SPIFLASH_SSPBUF = WRSR;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;
    SPIFLASH_SSPBUF = 0x00; // Clear all block protect bits
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;
    SPIFLASH_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}


/*****************************************************************************
  Function:
    void SPIFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)

  Description:
    Reads an array of bytes from the SPI Flash module.

  Precondition:
    SPIFlashInit has been called, and the chip is not busy (should be
    handled elsewhere automatically.)

  Parameters:
    dwAddress - Address from which to read
    vData - Where to store data that has been read
    wLength - Length of data to read

  Returns:
    None
  ***************************************************************************/
void SPIFlashReadArray(DWORD dwAddress, BYTE *vData, WORD wLength)
{
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif

    // Ignore operations when the destination is NULL or nothing to read
    if(vData == NULL || wLength == 0)
        return;

    // Save SPI state (clock speed)
    SPICON1Save = SPIFLASH_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Activate chip select
    SPIFLASH_CS_IO = 0;
    ClearSPIDoneFlag();

    // Send READ opcode
    SPIFLASH_SSPBUF = READ;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Send address
    SPIFLASH_SSPBUF = ((BYTE*)&dwAddress)[2];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddress)[1];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddress)[0];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Read data
    while(wLength--)
    {
        SPIFLASH_SSPBUF = 0;
        WaitForDataByte();
        *vData++ = SPIFLASH_SSPBUF;
    }

    // Deactivate chip select
    SPIFLASH_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}

/*****************************************************************************
  Function:
    void SPIFlashBeginWrite(DWORD dwAddr)

  Summary:
    Prepares the SPI Flash module for writing.

  Description:
    Prepares the SPI Flash module for writing.  Subsequent calls to
    SPIFlashWrite or SPIFlashWriteArray will begin at this location and
    continue sequentially.

    SPI Flash

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    dwAddr - Address where the writing will begin

  Returns:
    None

  Remarks:
    Flash parts have large sector sizes, and can only erase entire sectors
    at once.  The SST parts for which this library was written have sectors
    that are 4kB in size.  Your application must ensure that writes begin on
    a sector boundary so that the SPIFlashWrite functions will erase the
    sector before attempting to write.  Entire sectors need not be written
    at once, so applications can begin writing to the front of a sector,
    perform other tasks, then later call SPIFlashBeginWrite and point to an
    address in this sector that has not yet been programmed.  However, care
    must taken to ensure that writes are not attempted on addresses that are
    not in the erased state.  The chip will provide no indication that the
    write has failed, and will silently ignore the command.
  ***************************************************************************/
void SPIFlashBeginWrite(DWORD dwAddr)
{
    dwWriteAddr = dwAddr;
}

/*****************************************************************************
  Function:
    void SPIFlashWrite(BYTE vData)

  Summary:
    Writes a byte to the SPI Flash part.

  Description:
    This function writes a byte to the SPI Flash part.  If the current
    address pointer indicates the beginning of a 4kB sector, the entire
    sector will first be erased to allow writes to proceed.  If the current
    address pointer indicates elsewhere, it will be assumed that the sector
    has already been erased.  If this is not true, the chip will silently
    ignore the write command.

  Precondition:
    SPIFlashInit and SPIFlashBeginWrite have been called, and the current
    address is either the front of a 4kB sector or has already been erased.

  Parameters:
    vData - The byte to write to the next memory location.

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashWrite(BYTE vData)
{
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif

    // Save SPI state (clock speed)
    SPICON1Save = SPIFLASH_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // If address is a boundary, erase a sector first
    if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0u)
        SPIFlashEraseSector(dwWriteAddr);

    // Enable writing
    _SendCmd(WREN);

    // Activate the chip select
    SPIFLASH_CS_IO = 0;
    ClearSPIDoneFlag();

    // Issue WRITE command with address
    SPIFLASH_SSPBUF = WRITE;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[2];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[1];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[0];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Write the byte
    SPIFLASH_SSPBUF = vData;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;
    dwWriteAddr++;

    // Deactivate chip select and wait for write to complete
    SPIFLASH_CS_IO = 1;
    _WaitWhileBusy();

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}

/*****************************************************************************
  Function:
    void SPIFlashWriteArray(BYTE* vData, WORD wLen)

  Summary:
    Writes an array of bytes to the SPI Flash part.

  Description:
    This function writes an array of bytes to the SPI Flash part.  When the
    address pointer crosses a sector boundary (and has more data to write),
    the next sector will automatically be erased.  If the current address
    pointer indicates an address that is not a sector boundary and is not
    already erased, the chip will silently ignore the write command until the
    next sector boundary is crossed.

  Precondition:
    SPIFlashInit and SPIFlashBeginWrite have been called, and the current
    address is either the front of a sector or has already been erased.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashWriteArray(BYTE* vData, WORD wLen)
{
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif
    BOOL isStarted;
    BYTE vOpcode;
    BYTE i;

	// Do nothing if no data to process
	if(wLen == 0u)
		return;

    // Save SPI state (clock speed)
    SPICON1Save = SPIFLASH_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // If starting at an odd address, write a single byte
    if((dwWriteAddr & 0x01) && wLen)
    {
        SPIFlashWrite(*vData);
        vData++;
        wLen--;
    }

	// Assume we are using AAI Word program mode unless changed later
	vOpcode = WRITE_WORD_STREAM;	

    isStarted = FALSE;

    // Loop over all remaining WORDs
    while(wLen > 1)
    {
        // Don't do anything until chip is ready
        _WaitWhileBusy();

        // If address is a sector boundary
        if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0)
            SPIFlashEraseSector(dwWriteAddr);

        // If not yet started, initiate AAI mode
        if(!isStarted)
        {
            // Enable writing
            _SendCmd(WREN);

			// Select appropriate programming opcode.  The WRITE_WORD_STREAM 
			// mode is the default if neither of these flags are set.
	        if(deviceCaps.bits.bWriteByteStream)
	            vOpcode = WRITE_BYTE_STREAM;
			else if(deviceCaps.bits.bPageProgram)
			{
				// Note: Writing one byte at a time is extremely slow (ex: ~667 
				// bytes/second write speed on SST SST25VF064C).  You can 
				// improve this by over a couple of orders of magnitude by 
				// writing a function to write full pages of up to 256 bytes at 
				// a time.  This is implemented this way only because I don't 
				// have an SST25VF064C handy to test with right now. -HS
				while(wLen--)
			        SPIFlashWrite(*vData++);
				return;
			}

            // Activate the chip select
            SPIFLASH_CS_IO = 0;
            ClearSPIDoneFlag();

            // Issue WRITE_xxx_STREAM command with address
			SPIFLASH_SSPBUF = vOpcode;
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[2];
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[1];
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            SPIFLASH_SSPBUF = ((BYTE*)&dwWriteAddr)[0];
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;

            isStarted = TRUE;
        }
        // Otherwise, just write the AAI command again
        else
        {
            // Assert the chip select pin
            SPIFLASH_CS_IO = 0;
            ClearSPIDoneFlag();

            // Issue the WRITE_STREAM command for continuation
            SPIFLASH_SSPBUF = vOpcode;
            WaitForDataByte();
            Dummy = SPIFLASH_SSPBUF;
        }

        // Write a byte or two
        for(i = 0; i <= deviceCaps.bits.bWriteWordStream; i++)
        {
	        SPIFLASH_SSPBUF = *vData++;
	        dwWriteAddr++;
	        wLen--;
	        WaitForDataByte();
	        Dummy = SPIFLASH_SSPBUF;
		}

        // Release the chip select to begin the write
        SPIFLASH_CS_IO = 1;

        // If a boundary was reached, end the write
        if((dwWriteAddr & SPI_FLASH_SECTOR_MASK) == 0)
        {
            _WaitWhileBusy();
            _SendCmd(WRDI);
            isStarted = FALSE;
        }
    }

    // Wait for write to complete, then exit AAI mode
    _WaitWhileBusy();
    _SendCmd(WRDI);

    // If a byte remains, write the odd address
    if(wLen)
        SPIFlashWrite(*vData);

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}


/*****************************************************************************
  Function:
    void SPIFlashEraseSector(DWORD dwAddr)

  Summary:
    Erases a sector.

  Description:
    This function erases a sector in the Flash part.  It is called
    internally by the SPIFlashWrite functions whenever a write is attempted
    on the first byte in a sector.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    dwAddr - The address of the sector to be erased.

  Returns:
    None

  Remarks:
    See Remarks in SPIFlashBeginWrite for important information about Flash
    memory parts.
  ***************************************************************************/
void SPIFlashEraseSector(DWORD dwAddr)
{
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif

    // Save SPI state (clock speed)
    SPICON1Save = SPIFLASH_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Enable writing
    _SendCmd(WREN);

    // Activate the chip select
    SPIFLASH_CS_IO = 0;
    ClearSPIDoneFlag();

    // Issue ERASE command with address
    SPIFLASH_SSPBUF = ERASE_4K;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddr)[2];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddr)[1];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    SPIFLASH_SSPBUF = ((BYTE*)&dwAddr)[0];
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Deactivate chip select to perform the erase
    SPIFLASH_CS_IO = 1;

    // Wait for erase to complete
    _WaitWhileBusy();

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIFLASH_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}


/*****************************************************************************
  Function:
    static void _SendCmd(BYTE cmd)

  Summary:
    Sends a single-byte command to the SPI Flash part.

  Description:
    This function sends a single-byte command to the SPI Flash part.  It is
    used for commands such as WREN, WRDI, and EWSR that must have the chip
    select activated, then deactivated immediately after the command is
    transmitted.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    cmd - The single-byte command code to send

  Returns:
    None
  ***************************************************************************/
static void _SendCmd(BYTE cmd)
{
    // Activate chip select
    SPIFLASH_CS_IO = 0;
    ClearSPIDoneFlag();

    // Send instruction
    SPIFLASH_SSPBUF = cmd;
    WaitForDataByte();
    cmd = SPIFLASH_SSPBUF;

    // Deactivate chip select
    SPIFLASH_CS_IO = 1;
}


/*****************************************************************************
  Function:
    static void _WaitWhileBusy(void)

  Summary:
    Waits for the SPI Flash part to indicate it is idle.

  Description:
    This function waits for the SPI Flash part to indicate it is idle.  It is
    used in the programming functions to wait for operations to complete.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
static void _WaitWhileBusy(void)
{
    volatile BYTE Dummy;

    // Activate chip select
    SPIFLASH_CS_IO = 0;
    ClearSPIDoneFlag();

    // Send Read Status Register instruction
    SPIFLASH_SSPBUF = RDSR;
    WaitForDataByte();
    Dummy = SPIFLASH_SSPBUF;

    // Poll the BUSY bit
    do
    {
        SPIFLASH_SSPBUF = 0x00;
        WaitForDataByte();
        Dummy = SPIFLASH_SSPBUF;
    } while(Dummy & BUSY);

    // Deactivate chip select
    SPIFLASH_CS_IO = 1;
}

/*****************************************************************************
  Function:
    static void _GetStatus()

  Summary:
    Reads the status register of the part.

  Description:
    This function reads the status register of the part.  It was written
    for debugging purposes, and is not needed for normal operation.  Place
    a breakpoint at the last instruction and check the "status" variable to
    see the result.

  Precondition:
    SPIFlashInit has been called.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/
//static void _GetStatus()
//{
//	volatile BYTE Dummy;
//  static BYTE statuses[16];
//  static BYTE *status = statuses;
//
//  // Activate chip select
//  SPIFLASH_CS_IO = 0;
//  ClearSPIDoneFlag();
//
//  // Send Read Status Register instruction
//  SPIFLASH_SSPBUF = RDSR;
//  WaitForDataByte();
//  Dummy = SPIFLASH_SSPBUF;
//
//  SPIFLASH_SSPBUF = 0x00;
//  WaitForDataByte();
//  *status = SPIFLASH_SSPBUF;
//  status++;
//
//  // Deactivate chip select
//  SPIFLASH_CS_IO = 1;
//
//  if(status == &statuses[10])
//      statuses[15] = 0;
//}

#endif //#if defined(SPIFLASH_CS_TRIS)

