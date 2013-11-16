#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H


#include "Compiler.h"
#include "NetConfig.h"
#include "../libUDB/oscillator.h"

// Hardware I/O pin mappings
// These are just dummy values to keep the compiler quiet for stock modules that use them like Telnet
#define BUTTON0_IO      (0)
#define BUTTON1_IO      (0)
#define BUTTON2_IO      (0)
#define BUTTON3_IO      (0)
#define LED0_IO         (0)
#define LED1_IO         (0)
#define LED2_IO         (0)
#define LED3_IO         (0)
#define LED4_IO         (0)
#define LED5_IO         (0)
#define LED6_IO         (0)
#define LED7_IO         (0)



#if defined(__dsPIC33E__)   // AUAV3 Board
//#include <p33Exxxx.h>

  #define UARTTX_TRIS         (TRISFbits.TRISF5)
  #define UARTTX_IO           (PORTFbits.RF5)
  #define UARTRX_TRIS         (TRISFbits.TRISF4)
  #define UARTRX_IO           (PORTFbits.RF4)
 
  #define UBRG                U3BRG
  #define UMODE               U3MODE
  #define USTA                U3STA
  #define BusyUART()          BusyUART2()
  #define CloseUART()         CloseUART2()
  #define ConfigIntUART(a)    ConfigIntUART2(a)
  #define DataRdyUART()       DataRdyUART2()
  #define OpenUART(a,b,c)     OpenUART2(a,b,c)
  #define ReadUART()          ReadUART2()
  #define WriteUART(a)        WriteUART2(a)
  #define getsUART(a,b,c)     getsUART2(a,b,c)
  #define putsUART(a)         putsUART2((uint16_t*)a)
  #define getcUART()          getcUART2()
  #define putcUART(a)         do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
  #define putrsUART(a)        putrsUART2(a)

  #if (NETWORK_INTERFACE == NETWORK_INTERFACE_ETHERNET_ENC624J600)
    #define ENC100_INTERFACE_MODE   (0) // 0 == SPI

    //#define ENC100_CS_TRIS      (TRISGbits.TRISG9) // RG9 == SS2
    //#define ENC100_CS_IO        (LATGbits.LATG9)
    //#define ENC100_CS_TRIS      (TRISBbits.TRISB15) // RB15 == AN15
    //#define ENC100_CS_IO        (LATBbits.LATB15)
    //#define ENC100_CS_TRIS      (TRISBbits.TRISB2) // RB2 == SS1
    //#define ENC100_CS_IO        (LATBbits.LATB2)
    //#define ENC100_CS_TRIS      (TRISAbits.TRISA12) // RA12 == IN1
    //#define ENC100_CS_IO        (LATAbits.LATA12)
    #define ENC100_CS_TRIS      (TRISBbits.TRISB12) // RB12 == IN1
    #define ENC100_CS_IO        (LATBbits.LATB12)

    #define ENC100_SPI_ENABLE   (ENC100_SPISTATbits.SPIEN)
    #define ENC100_SPI_IF       (IFS0bits.SPI2IF)
    #define ENC100_SSPBUF       (SPI2BUF)
    #define ENC100_SPISTAT      (SPI2STAT)
    #define ENC100_SPISTATbits  (SPI2STATbits)
    #define ENC100_SPICON1      (SPI2CON1)
    #define ENC100_SPICON1bits  (SPI2CON1bits)
    #define ENC100_SPICON2      (SPI2CON2)

  #elif (NETWORK_INTERFACE == NETWORK_INTERFACE_ETHERNET_ENC28J60)
    #define ENC_CS_TRIS         (TRISBbits.TRISB15) // RB15 == AN15
    #define ENC_CS_IO           (LATBbits.LATB15)
    //#define ENC_CS_TRIS         (TRISGbits.TRISG9) // RG9 == SS2
    //#define ENC_CS_IO           (LATGbits.LATG9)
    //#define ENC_CS_TRIS         (TRISBbits.TRISB2) // RB2 == SS1
    //#define ENC_CS_IO           (LATBbits.LATB2)

    // SPI SCK, SDI, SDO pins are automatically controlled by the
    // PIC24/dsPIC SPI module
    #define ENC_SPI_IF          (IFS2bits.SPI2IF)
    #define ENC_SSPBUF          (SPI2BUF)
    #define ENC_SPISTAT         (SPI2STAT)
    #define ENC_SPISTATbits     (SPI2STATbits)
    #define ENC_SPICON1         (SPI2CON1)
    #define ENC_SPICON1bits     (SPI2CON1bits)
    #define ENC_SPICON2         (SPI2CON2)

  #elif (NETWORK_INTERFACE == NETWORK_INTERFACE_WIFI_MRF24WG)
    #define MRF24W_IN_SPI2
    #define MRF24WG

    #define WF_CS_TRIS          (TRISGbits.TRISG9)
    #define WF_CS_IO            (LATGbits.LATG9)
    #define WF_SDI_TRIS         (TRISGbits.TRISG7)
    #define WF_SCK_TRIS         (TRISGbits.TRISG6)
    #define WF_SDO_TRIS         (TRISGbits.TRISG8)
    #define WF_RESET_TRIS       (TRISCbits.TRISC2)	// A17 - must also set AD1PCFGHbits
    #define WF_RESET_IO         (LATCbits.LATC2)
    #define WF_INT_TRIS         (TRISAbits.TRISA12)	// IN1 (_INT1Interrupt)- must also set AD1PCFGHbits
    #define WF_INT_IO           (PORTAbits.RA12)
    #define WF_HIBERNATE_TRIS   (TRISCbits.TRISC3)	// A18 - must also set AD1PCFGHbits
    #define	WF_HIBERNATE_IO     (PORTCbits.RC3)
    #define WF_INT_EDGE         (INTCON2bits.INT1EP)
    #define WF_INT_IE           (IEC1bits.INT1IE)
    #define WF_INT_IF           (IFS1bits.INT1IF)

    #define WF_SSPBUF           (SPI2BUF)
    #define WF_SPISTAT          (SPI2STAT)
    #define WF_SPISTATbits      (SPI2STATbits)
    #define WF_SPICON1          (SPI2CON1)
    #define WF_SPICON1bits      (SPI2CON1bits)
    #define WF_SPICON2          (SPI2CON2)
    #define WF_SPI_IE           (IEC2bits.SPI2IE)
    #define WF_SPI_IF           (IFS2bits.SPI2IF)
  #endif // NETWORK_INTERFACE

#elif defined(__dsPIC33F__) // UDB4 or UDB5 board
//#include <p33Fxxxx.h>

  // Select which UART the STACK_USE_UART and STACK_USE_UART2TCP_BRIDGE
  // options will use.  You can change these to U1BRG, U1MODE, etc. if you
  // want to use the UART1 module instead of UART2.
  // GPS uses UART1, logging and telemetry uses UART2
  #define UARTTX_TRIS         (TRISFbits.TRISF5)
  #define UARTTX_IO           (PORTFbits.RF5)
  #define UARTRX_TRIS         (TRISFbits.TRISF4)
  #define UARTRX_IO           (PORTFbits.RF4)

  #define UBRG                U2BRG
  #define UMODE               U2MODE
  #define USTA                U2STA
  #define BusyUART()          BusyUART2()
  #define CloseUART()         CloseUART2()
  #define ConfigIntUART(a)    ConfigIntUART2(a)
  #define DataRdyUART()       DataRdyUART2()
  #define OpenUART(a,b,c)     OpenUART2(a,b,c)
  #define ReadUART()          ReadUART2()
  #define WriteUART(a)        WriteUART2(a)
  #define getsUART(a,b,c)     getsUART2(a,b,c)
  #define putsUART(a)         putsUART2((uint16_t*)a)
  #define getcUART()          getcUART2()
  #define putcUART(a)         do{while(BusyUART()); WriteUART(a); while(BusyUART()); }while(0)
  #define putrsUART(a)        putrsUART2(a)

  #if (NETWORK_INTERFACE == NETWORK_INTERFACE_ETHERNET_ENC624J600)
    #define ENC100_INTERFACE_MODE   (0) // 0 == SPI

    //#define ENC100_CS_TRIS      (TRISGbits.TRISG9) // RG9 == SS2
    //#define ENC100_CS_IO        (LATGbits.LATG9)
    //#define ENC100_CS_TRIS      (TRISBbits.TRISB15) // RB15 == AN15
    //#define ENC100_CS_IO        (LATBbits.LATB15)
    //#define ENC100_CS_TRIS      (TRISBbits.TRISB2) // RB2 == SS1
    //#define ENC100_CS_IO        (LATBbits.LATB2)
    #define ENC100_CS_TRIS      (TRISAbits.TRISA12) // RA12 == IN1
    #define ENC100_CS_IO        (LATAbits.LATA12)

    #define ENC100_SPI_ENABLE   (ENC100_SPISTATbits.SPIEN)
    #define ENC100_SPI_IF       (IFS0bits.SPI2IF)
    #define ENC100_SSPBUF       (SPI2BUF)
    #define ENC100_SPISTAT      (SPI2STAT)
    #define ENC100_SPISTATbits  (SPI2STATbits)
    #define ENC100_SPICON1      (SPI2CON1)
    #define ENC100_SPICON1bits  (SPI2CON1bits)
    #define ENC100_SPICON2      (SPI2CON2)

  #elif (NETWORK_INTERFACE == NETWORK_INTERFACE_ETHERNET_ENC28J60)
    #define ENC_CS_TRIS         (TRISBbits.TRISB15) // RB15 == AN15
    #define ENC_CS_IO           (LATBbits.LATB15)
    //#define ENC_CS_TRIS         (TRISGbits.TRISG9) // RG9 == SS2
    //#define ENC_CS_IO           (LATGbits.LATG9)
    //#define ENC_CS_TRIS         (TRISBbits.TRISB2) // RB2 == SS1
    //#define ENC_CS_IO           (LATBbits.LATB2)

    // SPI SCK, SDI, SDO pins are automatically controlled by the
    // PIC24/dsPIC SPI module
    #define ENC_SPI_IF          (IFS2bits.SPI2IF)
    #define ENC_SSPBUF          (SPI2BUF)
    #define ENC_SPISTAT         (SPI2STAT)
    #define ENC_SPISTATbits     (SPI2STATbits)
    #define ENC_SPICON1         (SPI2CON1)
    #define ENC_SPICON1bits     (SPI2CON1bits)
    #define ENC_SPICON2         (SPI2CON2)

  #elif (NETWORK_INTERFACE == NETWORK_INTERFACE_WIFI_MRF24WG)
    #define MRF24W_IN_SPI2
    #define MRF24WG

    #define WF_CS_TRIS          (TRISGbits.TRISG9)
    #define WF_CS_IO            (LATGbits.LATG9)
    #define WF_SDI_TRIS         (TRISGbits.TRISG7)
    #define WF_SCK_TRIS         (TRISGbits.TRISG6)
    #define WF_SDO_TRIS         (TRISGbits.TRISG8)
    #define WF_RESET_TRIS       (TRISCbits.TRISC2)	// A17 - must also set AD1PCFGHbits
    #define WF_RESET_IO         (LATCbits.LATC2)
    #define WF_INT_TRIS         (TRISAbits.TRISA12)	// IN1 (_INT1Interrupt)- must also set AD1PCFGHbits
    #define WF_INT_IO           (PORTAbits.RA12)
    #define WF_HIBERNATE_TRIS   (TRISCbits.TRISC3)	// A18 - must also set AD1PCFGHbits
    #define	WF_HIBERNATE_IO     (PORTCbits.RC3)
    #define WF_INT_EDGE         (INTCON2bits.INT1EP)
    #define WF_INT_IE           (IEC1bits.INT1IE)
    #define WF_INT_IF           (IFS1bits.INT1IF)

    #define WF_SSPBUF           (SPI2BUF)
    #define WF_SPISTAT          (SPI2STAT)
    #define WF_SPISTATbits      (SPI2STATbits)
    #define WF_SPICON1          (SPI2CON1)
    #define WF_SPICON1bits      (SPI2CON1bits)
    #define WF_SPICON2          (SPI2CON2)
    #define WF_SPI_IE           (IEC2bits.SPI2IE)
    #define WF_SPI_IF           (IFS2bits.SPI2IF)
  #endif // NETWORK_INTERFACE

#endif // __dsPIC33?__




// Select a mass storage device interface type
// This library currently only supports a single physical interface layer at a time

#if defined(__dsPIC33E__)   // AUAV3 Board
#define USE_AT45D_FLASH
#else
// Description: Macro used to enable the SD-SPI physical layer (SD-SPI.c and .h)
#define USE_SD_INTERFACE_WITH_SPI
// Description: Macro used to enable the CF-PMP physical layer (CF-PMP.c and .h)
//#define USE_CF_INTERFACE_WITH_PMP
// Description: Macro used to enable the CF-Manual physical layer (CF-Bit transaction.c and .h)                                                                
//#define USE_MANUAL_CF_INTERFACE
// Description: Macro used to enable the USB Host physical layer (USB host MSD library)
//#define USE_USB_INTERFACE
#endif

// SD Card definitions: Change these to fit your application when using
//   an SD-card-based physical layer
#ifdef USE_SD_INTERFACE_WITH_SPI

            // Description: SD-SPI Chip Select Output bit
//            #define SD_CS               LATBbits.LATB9
            #define SD_CS               LATBbits.LATB2
            // Description: SD-SPI Chip Select TRIS bit
//            #define SD_CS_TRIS          TRISBbits.TRISB9
            #define SD_CS_TRIS          TRISBbits.TRISB2

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
            
#if 0
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
#endif // 0
 
#endif // USE_SD_INTERFACE_WITH_SPI





#endif // HARDWARE_PROFILE_H
