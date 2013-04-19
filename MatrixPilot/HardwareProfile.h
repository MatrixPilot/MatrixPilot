#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H


#include "../libUDB/oscillator.h"

#ifndef FOSC
#error Must include oscillator.h before including HardwareProfile.h
// For __delay_us and __delay_ms
//#define FCY (FREQOSC/2)
#endif


#include "defines.h"
#if (NETWORK_INTERFACE != NETWORK_INTERFACE_NONE)
  #include "Compiler.h"
#else
  #define MAX_UDP_SOCKETS       (1) // dummy value to keep compiler quiet when Network interface is disabled
  #define MAX_HTTP_CONNECTIONS  (1) // dummy value to keep compiler quiet when Network interface is disabled
#endif // #if NETWORK_INTERFACE != NETWORK_INTERFACE_NONE

// Hardware I/O pin mappings
// These are just dummy values to keep the compiler quiet for stock modules that use them like Telnet
#define BUTTON0_IO                  (0)
#define BUTTON1_IO                  (0)
#define BUTTON2_IO                  (0)
#define BUTTON3_IO                  (0)
#define LED0_IO                     (0)
#define LED1_IO                     (0)
#define LED2_IO                     (0)
#define LED3_IO                     (0)
#define LED4_IO                     (0)
#define LED5_IO                     (0)
#define LED6_IO                     (0)
#define LED7_IO                     (0)
#define LED_TCP_CONNECTED           LED_BLUE






#if (BOARD_TYPE == AUAV3_BOARD)
//	#define FOSC		128000000LL		// clock-frequecy in Hz with suffix LL (64-bit-long), eg. 32000000LL for 32MHz
	#define FCY      	(FOSC/2)		// MCU is running at FCY MIPS

    #define GetSystemClock()        (FCY)
    #define GetPeripheralClock()    (FCY)

	#define delay_us(x)	__delay32((((((long long)x)*FCY)/1000000L))	// delays x us
	#define delay_ms(x)	__delay32(((((long long)x)*FCY)/1000L))		// delays x ms

    /** UART2 **********************************************************/
/*
    #define BAUDRATE2       57600UL
    #define BRGH2           1 
#if (BRGH2 == 0)
    #define BRG_DIV2        16
#elif (BRGH2 == 1) 
    #define BRG_DIV2        4
#else
	#error Must select a valid BRGH2 value
#endif
 */

    /** UART3 **********************************************************/
    #define BAUDRATE3       115200UL
    #define BRGH3           1
  #if (BRGH3 == 0)
    #define BRG_DIV3        16
  #elif (BRGH3 == 1)
    #define BRG_DIV3        4
  #else
    #error Must select a valid BRGH3 value
  #endif


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
    #define ENC100_CS_TRIS      (TRISBbits.TRISB12) // RA12 == IN1
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
    #define MRF24W_IN_SPI3
//    #define MRF24WG
	#define WF_EXT_INT		2

    #define WF_CS_TRIS          (TRISDbits.TRISD2)
    #define WF_CS_IO            (LATDbits.LATD2)
    #define WF_SDI_TRIS         (TRISDbits.TRISD12)
    #define WF_SCK_TRIS         (TRISDbits.TRISD1)
    #define WF_SDO_TRIS         (TRISDbits.TRISD3)
    #define WF_RESET_TRIS       (TRISEbits.TRISE0)	// A17 - must also set AD1PCFGHbits
    #define WF_RESET_IO         (LATEbits.LATE0)
    #define WF_INT_TRIS         (TRISAbits.TRISA6)	// IN1 (_INT1Interrupt)- must also set AD1PCFGHbits
    #define WF_INT_IO           (PORTAbits.RA6)
    #define WF_HIBERNATE_TRIS   (TRISAbits.TRISA7)	// A18 - must also set AD1PCFGHbits
    #define	WF_HIBERNATE_IO     (PORTAbits.RA7)
    #define WF_INT_EDGE         (INTCON2bits.INT1EP)
    #define WF_INT_IE           (IEC1bits.INT1IE)
    #define WF_INT_IF           (IFS1bits.INT1IF)

    #define WF_SSPBUF           (SPI2BUF)
    #define WF_SPISTAT          (SPI3STAT)
    #define WF_SPISTATbits      (SPI3STATbits)
    #define WF_SPICON1          (SPI3CON1)
    #define WF_SPICON1bits      (SPI3CON1bits)
    #define WF_SPICON2          (SPI3CON2)
    #define WF_SPI_IE           (IEC5bits.SPI3IE)
    #define WF_SPI_IF           (IFS5bits.SPI3IF)
  #endif // NETWORK_INTERFACE


#elif (BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE == UDB5_BOARD)

  // Clock frequency values
  // These directly influence timed events using the Tick module.  They also are used for UART and SPI baud rate generation.
  #define GetSystemClock()        (FREQOSC)               // Hz
  #define GetInstructionClock()   (GetSystemClock()/2)    // Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Might need changing if using Doze modes.
  #define GetPeripheralClock()    (GetSystemClock()/2)    // Normally GetSystemClock()/4 for PIC18, GetSystemClock()/2 for PIC24/dsPIC, and GetSystemClock()/1 for PIC32.  Divisor may be different if using a PIC32 since it's configurable.

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
#endif // BOARD_TYPE

#endif // HARDWARE_PROFILE_H
