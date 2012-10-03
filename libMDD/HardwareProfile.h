/******************************************************************************
 *
 *                Microchip Memory Disk Drive File System
 *
 ******************************************************************************
 * FileName:        HardwareProfile.h
 * Dependencies:    None
 * Processor:       PIC18/PIC24/dsPIC30/dsPIC33/PIC32
 * Compiler:        C18/C30/C32
 * Company:         Microchip Technology, Inc.
 *
*****************************************************************************/
/********************************************************************
 Change History:
  Rev            Description
  ----           -----------------------
  1.3.4          Added support for PIC18F8722,PIC24FJ256DA210,
                 dsPIC33E & PIC24E microcontrollers.
                 Added macro "SPI_INTERRUPT_FLAG_ASM" for PIC18F
                 microcontrollers to support SD card SPI driver.
********************************************************************/

#ifndef _HARDWAREPROFILE_H_
#define _HARDWAREPROFILE_H_

#define __dsPIC33E__

// LED pins
#define LED_BLUE			LATEbits.LATE4
#define LED_ORANGE			LATEbits.LATE3
#define LED_GREEN			LATEbits.LATE2
#define LED_RED				LATEbits.LATE1

// Baud Rate Generator -- See section 19.3.1 of datasheet.
// Fcy = FREQOSC / CLK_PHASES
// UXBRG = (Fcy/(16*BaudRate))-1
// UXBRG = ((32000000/2)/(16*9600))-1
// UXBRG = 103

#define UDB_BAUD(x) ((int)((FREQOSC / CLK_PHASES) / ((long)4 * x) - 1))

// LED states
#define LED_ON		0
#define LED_OFF		1


// Define your clock speed here
    #define GetSystemClock()        160000000
    #define GetPeripheralClock()    (GetSystemClock() / 2)
    #define GetInstructionClock()   (GetSystemClock() / 2)

    // Clock values
    #define MILLISECONDS_PER_TICK       10                      // Definition for use with a tick timer
    #define TIMER_PRESCALER             TIMER_PRESCALER_64      // Definition for use with a tick timer
    #define TIMER_PERIOD                9375                    // Definition for use with a tick timer

// Select your interface type
// This library currently only supports a single physical interface layer at a time


// Description: Macro used to enable the SD-SPI physical layer (SD-SPI.c and .h)
#define USE_SD_INTERFACE_WITH_SPI

// Description: Macro used to enable the CF-PMP physical layer (CF-PMP.c and .h)
//#define USE_CF_INTERFACE_WITH_PMP

// Description: Macro used to enable the CF-Manual physical layer (CF-Bit transaction.c and .h)                                                                
//#define USE_MANUAL_CF_INTERFACE

// Description: Macro used to enable the USB Host physical layer (USB host MSD library)
//#define USE_USB_INTERFACE


/*********************************************************************/
/******************* Pin and Register Definitions ********************/
/*********************************************************************/

/* SD Card definitions: Change these to fit your application when using
   an SD-card-based physical layer                                   */
#ifdef USE_SD_INTERFACE_WITH_SPI

            // Description: SD-SPI Chip Select Output bit
//            #define SD_CS               LATBbits.LATB9
//            #define SD_CS               LATBbits.LATB2
            #define SD_CS               LATGbits.LATG9
            // Description: SD-SPI Chip Select TRIS bit
//            #define SD_CS_TRIS          TRISBbits.TRISB9
//            #define SD_CS_TRIS          TRISBbits.TRISB2
            #define SD_CS_TRIS          TRISGbits.TRISG9

		    // Description: SD-SPI Analog/Digital Select ANSEL bit
//            #define SD_CS_ANSEL			ANSELBbits.ANSB9
            
            // Description: SD-SPI Card Detect Input bit
            #define SD_CD               PORTAbits.RA12
            // Description: SD-SPI Card Detect TRIS bit
            #define SD_CD_TRIS          TRISAbits.TRISA12

            // Description: SD-SPI Write Protect Check Input bit
            #define SD_WE               PORTGbits.RG1
            // Description: SD-SPI Write Protect Check TRIS bit
            #define SD_WE_TRIS          TRISGbits.TRISG1

		    // Description: SD-SPI Analog/Digital Select ANSEL bit
//            #define SD_SCK_ANSEL	ANSELGbits.ANSG6
//            #define SD_SDI_ANSEL	ANSELGbits.ANSG7
//            #define SD_SDO_ANSEL	ANSELGbits.ANSG8
            
#if 1
            // Description: The main SPI control register
            #define SPICON1             SPI2CON1
            // Description: The SPI status register
            #define SPISTAT             SPI2STAT
            // Description: The SPI Buffer
            #define SPIBUF              SPI2BUF
            // Description: The receive buffer full bit in the SPI status register
            #define SPISTAT_RBF         SPI2STATbits.SPIRBF
            // Description: The bitwise define for the SPI control register (i.e. _____bits)
            #define SPICON1bits         SPI2CON1bits
            // Description: The bitwise define for the SPI status register (i.e. _____bits)
            #define SPISTATbits         SPI2STATbits
            // Description: The enable bit for the SPI module
            #define SPIENABLE           SPI2STATbits.SPIEN
            // Description: The definition for the SPI baud rate generator register
            #define SPIBRG			    SPI2BRG

           // Tris pins for SCK/SDI/SDO lines

            // Description: The TRIS bit for the SCK pin
            #define SPICLOCK            TRISGbits.TRISG6
            // Description: The TRIS bit for the SDI pin
            #define SPIIN               TRISGbits.TRISG7
            // Description: The TRIS bit for the SDO pin
            #define SPIOUT              TRISGbits.TRISG8
#else
            // Description: The main SPI control register
            #define SPICON1             SPI1CON1
            // Description: The SPI status register
            #define SPISTAT             SPI1STAT
            // Description: The SPI Buffer
            #define SPIBUF              SPI1BUF
            // Description: The receive buffer full bit in the SPI status register
            #define SPISTAT_RBF         SPI1STATbits.SPIRBF
            // Description: The bitwise define for the SPI control register (i.e. _____bits)
            #define SPICON1bits         SPI1CON1bits
            // Description: The bitwise define for the SPI status register (i.e. _____bits)
            #define SPISTATbits         SPI1STATbits
            // Description: The enable bit for the SPI module
            #define SPIENABLE           SPI1STATbits.SPIEN
            // Description: The definition for the SPI baud rate generator register
            #define SPIBRG			    SPI1BRG

           // Tris pins for SCK/SDI/SDO lines

            // Description: The TRIS bit for the SCK pin
            #define SPICLOCK            TRISFbits.TRISF6
            // Description: The TRIS bit for the SDI pin
            #define SPIIN               TRISFbits.TRISF7
            // Description: The TRIS bit for the SDO pin
            #define SPIOUT              TRISFbits.TRISF8
#endif
 
#endif


//#include <uart2.h>
//#include "uart1.h"

#endif
