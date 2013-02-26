/*********************************************************************
 *
 *               Data SPI EEPROM Access Routines
 *
 *********************************************************************
 * FileName:        SPIEEPROM.c
 * Dependencies:    None
 * Processor:       PIC18, PIC24F, PIC24H, dsPIC30F, dsPIC33F, PIC32
 * Compiler:        Microchip C32 v1.05 or higher
 *					Microchip C30 v3.12 or higher
 *					Microchip C18 v3.30 or higher
 *					HI-TECH PICC-18 PRO 9.63PL2 or higher
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
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
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Nilesh Rajbharti     5/20/02     Original (Rev. 1.0)
 * Howard Schlunder     9/01/04     Rewritten for SPI EEPROMs
 * Howard Schlunder     8/10/06     Modified to control SPI module
 *                                  frequency whenever EEPROM accessed
 *                                  to allow bus sharing with different
 *                                  frequencies.
********************************************************************/
#define __SPIEEPROM_C

#include "HardwareProfile.h"

// If the CS line is not defined, SPIEEPROM.c's content will not be compiled.  
// If you are using a serial EEPROM please define the CS pin as EEPROM_CS_TRIS 
// in HardwareProfile.h
#if defined(EEPROM_CS_TRIS)

#include "TCPIP Stack/TCPIP.h"

// IMPORTANT SPI NOTE: The code in this file expects that the SPI interrupt
//      flag (EEPROM_SPI_IF) be clear at all times.  If the SPI is shared with
//      other hardware, the other code should clear the EEPROM_SPI_IF when it is
//      done using the SPI.

// SPI Serial EEPROM buffer size.  To enhance performance while
// cooperatively sharing the SPI bus with other peripherals, bytes
// read and written to the memory are locally buffered. Legal
// sizes are 1 to the EEPROM page size.
#define EEPROM_BUFFER_SIZE              (32)

// Must be the EEPROM write page size, or any binary power of 2 divisor.  If 
// using a smaller number, make sure it is at least EEPROM_BUFFER_SIZE big for 
// max performance.  Microchip 25LC256 uses 64 byte page size, 25LC1024 uses 
// 256 byte page size, so 64 is compatible with both.
#define EEPROM_PAGE_SIZE				(64)

// EEPROM SPI opcodes
#define OPCODE_READ    0x03    // Read data from memory array beginning at selected address
#define OPCODE_WRITE   0x02    // Write data to memory array beginning at selected address
#define OPCODE_WRDI    0x04    // Reset the write enable latch (disable write operations)
#define OPCODE_WREN    0x06    // Set the write enable latch (enable write operations)
#define OPCODE_RDSR    0x05    // Read Status register
#define OPCODE_WRSR    0x01    // Write Status register

#define EEPROM_MAX_SPI_FREQ     (10000000ul)    // Hz

#if defined (__18CXX)
    #define ClearSPIDoneFlag()  {EEPROM_SPI_IF = 0;}
    #define WaitForDataByte()   {while(!EEPROM_SPI_IF); EEPROM_SPI_IF = 0;}
    #define SPI_ON_BIT          (EEPROM_SPICON1bits.SSPEN)
#elif defined(__C30__)
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while ((EEPROM_SPISTATbits.SPITBF == 1) || (EEPROM_SPISTATbits.SPIRBF == 0));
    }

    #define SPI_ON_BIT          (EEPROM_SPISTATbits.SPIEN)
#elif defined( __PIC32MX__ )
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while (!EEPROM_SPISTATbits.SPITBE || !EEPROM_SPISTATbits.SPIRBF);
    }

    #define SPI_ON_BIT          (EEPROM_SPICON1bits.ON)
#else
    #error Determine SPI flag mechanism
#endif

static void DoWrite(void);

static DWORD EEPROMAddress;
static BYTE EEPROMBuffer[EEPROM_BUFFER_SIZE];
static BYTE vBytesInBuffer;

/*********************************************************************
 * Function:        void XEEInit(unsigned char speed)
 *
 * PreCondition:    None
 *
 * Input:           speed - not used (included for compatibility only)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize SPI module to communicate to serial
 *                  EEPROM.
 *
 * Note:            Code sets SPI clock to Fosc/16.
 ********************************************************************/
#if (defined(HPC_EXPLORER) || defined(PIC18_EXPLORER)) && !defined(__18F87J10) && !defined(__18F87J11) && !defined(__18F87J50)
    #define PROPER_SPICON1  (0x20)      /* SSPEN bit is set, SPI in master mode, FOSC/4, IDLE state is low level */
#elif defined(__PIC24F__) || defined(__PIC24FK__)
    #define PROPER_SPICON1  (0x0013 | 0x0120)   /* 1:1 primary prescale, 4:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__dsPIC30F__)
    #define PROPER_SPICON1  (0x0017 | 0x0120)   /* 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__dsPIC33F__) || defined(__PIC24H__) || defined (__dsPIC33E__)|| defined(__PIC24E__)
    #define PROPER_SPICON1  (0x0003 | 0x0120)   /* 1:1 primary prescale, 8:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__PIC32MX__)
    #define PROPER_SPICON1  (_SPI2CON_ON_MASK | _SPI2CON_CKE_MASK | _SPI2CON_MSTEN_MASK)
#else
    #define PROPER_SPICON1  (0x21)      /* SSPEN bit is set, SPI in master mode, FOSC/16, IDLE state is low level */
#endif

void XEEInit(void)
{
    EEPROM_CS_IO = 1;
    EEPROM_CS_TRIS = 0;     // Drive SPI EEPROM chip select pin

    EEPROM_SCK_TRIS = 0;    // Set SCK pin as an output
    EEPROM_SDI_TRIS = 1;    // Make sure SDI pin is an input
    EEPROM_SDO_TRIS = 0;    // Set SDO pin as an output

    ClearSPIDoneFlag();
    #if defined(__C30__)
        EEPROM_SPICON1 = PROPER_SPICON1; // See PROPER_SPICON1 definition above
        EEPROM_SPICON2 = 0;
        EEPROM_SPISTAT = 0;    // clear SPI
        EEPROM_SPISTATbits.SPIEN = 1;
    #elif defined(__C32__)
        EEPROM_SPIBRG = (GetPeripheralClock()-1ul)/2ul/EEPROM_MAX_SPI_FREQ;
        EEPROM_SPICON1 = PROPER_SPICON1;
    #elif defined(__18CXX)
        EEPROM_SPICON1 = PROPER_SPICON1; // See PROPER_SPICON1 definition above
        EEPROM_SPISTATbits.CKE = 1;     // Transmit data on rising edge of clock
        EEPROM_SPISTATbits.SMP = 0;     // Input sampled at middle of data output time
    #endif
}


/*********************************************************************
 * Function:        XEE_RESULT XEEBeginRead(DWORD address)
 *
 * PreCondition:    None
 *
 * Input:           address - Address at which read is to be performed.
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Sets internal address counter to given address.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEBeginRead(DWORD address)
{
    // Save the address and emptry the contents of our local buffer
    EEPROMAddress = address;
    vBytesInBuffer = 0;
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        BYTE XEERead(void)
 *
 * PreCondition:    XEEInit() && XEEBeginRead() are already called.
 *
 * Input:           None
 *
 * Output:          BYTE that was read
 *
 * Side Effects:    None
 *
 * Overview:        Reads next byte from EEPROM; internal address
 *                  is incremented by one.
 *
 * Note:            None
 ********************************************************************/
BYTE XEERead(void)
{
    // Check if no more bytes are left in our local buffer
    if(vBytesInBuffer == 0u)
    {
        // Get a new set of bytes
        XEEReadArray(EEPROMAddress, EEPROMBuffer, EEPROM_BUFFER_SIZE);
        EEPROMAddress += EEPROM_BUFFER_SIZE;
        vBytesInBuffer = EEPROM_BUFFER_SIZE;
    }

    // Return a byte from our local buffer
    return EEPROMBuffer[EEPROM_BUFFER_SIZE - vBytesInBuffer--];
}

/*********************************************************************
 * Function:        XEE_RESULT XEEEndRead(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        This function does nothing.
 *
 * Note:            Function is used for backwards compatability with
 *                  I2C EEPROM module.
 ********************************************************************/
XEE_RESULT XEEEndRead(void)
{
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEEReadArray(DWORD address,
 *                                          BYTE *buffer,
 *                                          WORD length)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           address     - Address from where array is to be read
 *                  buffer      - Caller supplied buffer to hold the data
 *                  length      - Number of bytes to read.
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Reads desired number of bytes in sequential mode.
 *                  This function performs all necessary steps
 *                  and releases the bus when finished.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEReadArray(DWORD address,
                        BYTE *buffer,
                        WORD length)
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
    SPICON1Save = EEPROM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    EEPROM_CS_IO = 0;

    // Send READ opcode
    EEPROM_SSPBUF = OPCODE_READ;
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    // Send address
    #if defined(USE_EEPROM_25LC1024)
    EEPROM_SSPBUF = ((DWORD_VAL*)&address)->v[2];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;
    #endif

    EEPROM_SSPBUF = ((DWORD_VAL*)&address)->v[1];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((DWORD_VAL*)&address)->v[0];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    while(length--)
    {
        EEPROM_SSPBUF = 0;
        WaitForDataByte();
        Dummy = EEPROM_SSPBUF;
        if(buffer != NULL)
            *buffer++ = Dummy;
    };

    EEPROM_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;


    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEEBeginWrite(DWORD address)
 *
 * PreCondition:    None
 *
 * Input:           address     - address to be set for writing
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Modifies internal address counter of EEPROM.
 *
 * Note:            Unlike XEESetAddr() in xeeprom.c for I2C EEPROM
 *                  memories, this function is used only for writing
 *                  to the EEPROM.  Reads must use XEEBeginRead(),
 *                  XEERead(), and XEEEndRead().
 *                  This function does not use the SPI bus.
 ********************************************************************/
XEE_RESULT XEEBeginWrite(DWORD address)
{
	vBytesInBuffer = 0;
    EEPROMAddress = address;
    return XEE_SUCCESS;
}


/*********************************************************************
 * Function:        XEE_RESULT XEEWrite(BYTE val)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           val - Byte to be written
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Writes a byte to the write cache, and if full, 
 *					commits the write.  Also, if a write boundary is 
 *					reached the write is committed.  When finished 
 *					writing, XEEEndWrite() must be called to commit 
 *					any unwritten bytes from the write cache.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEWrite(BYTE val)
{
	EEPROMBuffer[vBytesInBuffer++] = val;
	if(vBytesInBuffer >= sizeof(EEPROMBuffer))
		DoWrite();
	else if((((BYTE)EEPROMAddress + vBytesInBuffer) & (EEPROM_PAGE_SIZE-1)) == 0u)
		DoWrite();

    return XEE_SUCCESS;
}


/*****************************************************************************
  Function:
    XEE_RESULT XEEWriteArray(BYTE *val, WORD wLen)

  Summary:
    Writes an array of bytes to the EEPROM part.

  Description:
    This function writes an array of bytes to the EEPROM at the address 
    specified when XEEBeginWrite() was called.  Page boundary crossing is 
    handled internally.
    
  Precondition:
    XEEInit() was called once and XEEBeginWrite() was called.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    The internal write cache is flushed at completion, so it is unnecessary 
    to call XEEEndWrite() after calling this function.  However, if you do 
    so, no harm will be done.
  ***************************************************************************/
void XEEWriteArray(BYTE *val, WORD wLen)
{
	while(wLen--)
		XEEWrite(*val++);
	
	XEEEndWrite();
}


/*********************************************************************
 * Function:        XEE_RESULT XEEEndWrite(void)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Commits any last uncommitted bytes in cache to 
 *					physical storage.
 *
 * Note:            Call this function when you no longer need to 
 *					write any more bytes at the selected address.
 ********************************************************************/
XEE_RESULT XEEEndWrite(void)
{
	if(vBytesInBuffer)
		DoWrite();

    return XEE_SUCCESS;
}

static void DoWrite(void)
{
    BYTE i;
    volatile BYTE vDummy;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif

    // Save SPI state
    SPICON1Save = EEPROM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Set the Write Enable latch
    EEPROM_CS_IO = 0;
    EEPROM_SSPBUF = OPCODE_WREN;
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;
    EEPROM_CS_IO = 1;

    // Send WRITE opcode
    EEPROM_CS_IO = 0;
    EEPROM_SSPBUF = OPCODE_WRITE;
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;

    // Send address
    #if defined(USE_EEPROM_25LC1024)
    EEPROM_SSPBUF = ((DWORD_VAL*)&EEPROMAddress)->v[2];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;
    #endif

    EEPROM_SSPBUF = ((DWORD_VAL*)&EEPROMAddress)->v[1];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((DWORD_VAL*)&EEPROMAddress)->v[0];
    WaitForDataByte();
    vDummy = EEPROM_SSPBUF;


    for(i = 0; i < vBytesInBuffer; i++)
    {
        // Send the byte to write
        EEPROM_SSPBUF = EEPROMBuffer[i];
        WaitForDataByte();
        vDummy = EEPROM_SSPBUF;
    }

    // Begin the write
    EEPROM_CS_IO = 1;

	// Update write address and clear write cache
    EEPROMAddress += vBytesInBuffer;
    vBytesInBuffer = 0;

    // Restore SPI State
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;


    // Wait for write to complete
    while( XEEIsBusy() );
}


/*********************************************************************
 * Function:        BOOL XEEIsBusy(void)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           None
 *
 * Output:          FALSE if EEPROM is not busy
 *                  TRUE if EEPROM is busy
 *
 * Side Effects:    None
 *
 * Overview:        Reads the status register
 *
 * Note:            None
 ********************************************************************/
BOOL XEEIsBusy(void)
{
    volatile BYTE_VAL result;
    BYTE vSPIONSave;
    #if defined(__18CXX)
    BYTE SPICON1Save;
    #elif defined(__C30__)
    WORD SPICON1Save;
    #else
    DWORD SPICON1Save;
    #endif

    // Save SPI state
    SPICON1Save = EEPROM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    EEPROM_CS_IO = 0;
    // Send RDSR - Read Status Register opcode
    EEPROM_SSPBUF = OPCODE_RDSR;
    WaitForDataByte();
    result.Val = EEPROM_SSPBUF;

    // Get register contents
    EEPROM_SSPBUF = 0;
    WaitForDataByte();
    result.Val = EEPROM_SSPBUF;
    EEPROM_CS_IO = 1;

    // Restore SPI State
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;

    return result.bits.b0;
}


#endif //#if defined(EEPROM_CS_TRIS)
