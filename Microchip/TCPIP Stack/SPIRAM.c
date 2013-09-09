/*********************************************************************
 *
 * Data SPI RAM Access Routines
 *  -Tested with AMI Semiconductor N256S0830HDA
 *
 *********************************************************************
 * FileName:        SPIRAM.c
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
 * \file SPIRAM.c
 * \author Howard Henry Schlunder
 * \date 25 July 2007
********************************************************************/
#define __SPIRAM_C

#include "HardwareProfile.h"

#if defined(SPIRAM_CS_TRIS)

#include "TCPIP Stack/TCPIP.h"


// SPI SRAM opcodes
#define READ    0x03    // Read data from memory array beginning at selected address
#define WRITE   0x02    // Write data to memory array beginning at selected address
#define RDSR    0x05    // Read Status register
#define WRSR    0x01    // Write Status register

#define SPIRAM_MAX_SPI_FREQ     (15000000ul)    // Hz

#if defined(__PIC24F__) || defined(__PIC24FK__)
    #define PROPER_SPICON1  (0x001B | 0x0120)   // 1:1 primary prescale, 2:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC30F__)
    #define PROPER_SPICON1  (0x0017 | 0x0120)   // 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__dsPIC33F__) || defined(__PIC24H__)  || defined(__dsPIC33E__) || defined(__PIC24E__)
    #define PROPER_SPICON1  (0x000F | 0x0120)   // 1:1 primary prescale, 5:1 secondary prescale, CKE=1, MASTER mode
#elif defined(__PIC32MX__)
    #define PROPER_SPICON1  (_SPI2CON_ON_MASK  | _SPI2CON_CKE_MASK | _SPI2CON_MSTEN_MASK)
#else
    #define PROPER_SPICON1  (0x20)              // SSPEN bit is set, SPI in master mode, FOSC/4, IDLE state is low level
#endif


#if defined (__18CXX)
    #define ClearSPIDoneFlag()  {SPIRAM_SPI_IF = 0;}
    #define WaitForDataByte()   {while(!SPIRAM_SPI_IF); SPIRAM_SPI_IF = 0;}
    #define SPI_ON_BIT          (SPIRAM_SPICON1bits.SSPEN)
#elif defined(__C30__)
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while ((SPIRAM_SPISTATbits.SPITBF == 1) || (SPIRAM_SPISTATbits.SPIRBF == 0));
    }

    #define SPI_ON_BIT          (SPIRAM_SPISTATbits.SPIEN)
#elif defined( __PIC32MX__ )
    #define ClearSPIDoneFlag()
    static inline __attribute__((__always_inline__)) void WaitForDataByte( void )
    {
        while (!SPIRAM_SPISTATbits.SPITBE || !SPIRAM_SPISTATbits.SPIRBF);
    }

    #define SPI_ON_BIT          (SPIRAM_SPICON1bits.ON)
#else
    #error Determine SPI flag mechanism
#endif

/*********************************************************************
 * Function:        void SPIRAMInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize SPI module to communicate to serial
 *                  RAM.
 *
 * Note:            Code sets SPI clock to Fosc/4.
 ********************************************************************/
void SPIRAMInit(void)
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

    SPIRAM_CS_IO = 1;
    SPIRAM_CS_TRIS = 0;     // Drive SPI RAM chip select pin

    SPIRAM_SCK_TRIS = 0;    // Set SCK pin as an output
    SPIRAM_SDI_TRIS = 1;    // Make sure SDI pin is an input
    SPIRAM_SDO_TRIS = 0;    // Set SDO pin as an output

    // Save SPI state (clock speed)
    SPICON1Save = SPIRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    ClearSPIDoneFlag();
    #if defined(__C30__)
        SPIRAM_SPICON2 = 0;
        SPIRAM_SPISTAT = 0;    // clear SPI
        SPIRAM_SPISTATbits.SPIEN = 1;
    #elif defined(__C32__)
        SPIRAM_SPIBRG = (GetPeripheralClock()-1ul)/2ul/SPIRAM_MAX_SPI_FREQ;
        SPIRAM_SPICON1bits.CKE = 1;
        SPIRAM_SPICON1bits.MSTEN = 1;
        SPIRAM_SPICON1bits.ON = 1;
    #elif defined(__18CXX)
        SPIRAM_SPISTATbits.CKE = 1;     // Transmit data on rising edge of clock
        SPIRAM_SPISTATbits.SMP = 0;     // Input sampled at middle of data output time
    #endif

    // Set Burst mode
    // Activate chip select
    SPIRAM_CS_IO = 0;

    // Send Write Status Register opcode
    SPIRAM_SSPBUF = WRSR;
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Set status register to 0b01000000 to enable burst mode
    SPIRAM_SSPBUF = 0x40;
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Deactivate chip select
    SPIRAM_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}


/*********************************************************************
 * Function:        void SPIRAMGetArray(WORD wAddress, BYTE *vData, WORD wLength)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            None
 ********************************************************************/
void SPIRAMGetArray(WORD wAddress, BYTE *vData, WORD wLength)
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
    if(vData == NULL)
        return;
    if(wLength == 0u)
        return;

    // Save SPI state (clock speed)
    SPICON1Save = SPIRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Activate chip select
    SPIRAM_CS_IO = 0;
    ClearSPIDoneFlag();

    // Send READ opcode
    SPIRAM_SSPBUF = READ;
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Send address
    SPIRAM_SSPBUF = ((BYTE*)&wAddress)[1];
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    SPIRAM_SSPBUF = ((BYTE*)&wAddress)[0];
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Read data
    while(wLength--)
    {
        SPIRAM_SSPBUF = 0;
        WaitForDataByte();
        *vData++ = SPIRAM_SSPBUF;
    };

    // Deactivate chip select
    SPIRAM_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}


/*********************************************************************
 * Function:        void SPIRAMPutArray(WORD wAddress, BYTE *vData, WORD wLength)
 *
 * PreCondition:
 *
 * Input:
 *
 * Output:
 *
 * Side Effects:    None
 *
 * Overview:
 *
 * Note:            None
 ********************************************************************/
void SPIRAMPutArray(WORD wAddress, BYTE *vData, WORD wLength)
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

    // Ignore operations when the source data is NULL
    if(vData == NULL)
        return;
    if(wLength == 0u)
        return;

    // Save SPI state (clock speed)
    SPICON1Save = SPIRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Activate chip select
    SPIRAM_CS_IO = 0;
    ClearSPIDoneFlag();

    // Send WRITE opcode
    SPIRAM_SSPBUF = WRITE;
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Send address
    SPIRAM_SSPBUF = ((BYTE*)&wAddress)[1];
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    SPIRAM_SSPBUF = ((BYTE*)&wAddress)[0];
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Write data
    while(wLength--)
    {
        SPIRAM_SSPBUF = *vData++;
        WaitForDataByte();
        Dummy = SPIRAM_SSPBUF;
    };

    // Deactivate chip select
    SPIRAM_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}

#if defined(__18CXX)
void SPIRAMPutROMArray(WORD wAddress, ROM BYTE *vData, WORD wLength)
{
    volatile BYTE Dummy;
    BYTE vSPIONSave;
    BYTE SPICON1Save;

    // Ignore operations when the source data is NULL
    if(vData == NULL)
        return;
    if(wLength == 0u)
        return;

    // Save SPI state (clock speed)
    SPICON1Save = SPIRAM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Activate chip select
    SPIRAM_CS_IO = 0;
    ClearSPIDoneFlag();

    // Send WRITE opcode
    SPIRAM_SSPBUF = WRITE;
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Send address
    SPIRAM_SSPBUF = ((BYTE*)&wAddress)[1];
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    SPIRAM_SSPBUF = ((BYTE*)&wAddress)[0];
    WaitForDataByte();
    Dummy = SPIRAM_SSPBUF;

    // Write data
    while(wLength--)
    {
        SPIRAM_SSPBUF = *vData++;
        WaitForDataByte();
        Dummy = SPIRAM_SSPBUF;
    };

    // Deactivate chip select
    SPIRAM_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    SPIRAM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;
}
#endif


#endif //#if defined(SPIRAM_CS_TRIS)
