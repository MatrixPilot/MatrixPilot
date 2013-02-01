/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif
#endif


#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */

#include "system.h"          /* variables/params used by system.c             */

/******************************************************************************/
/* System Level Functions                                                     */
/*                                                                            */
/* Custom oscillator configuration funtions, reset source evaluation          */
/* functions, and other non-peripheral microcontroller initialization         */
/* functions get placed in system.c.                                          */
/*                                                                            */
/******************************************************************************/

/* Refer to the device Family Reference Manual Oscillator section for
information about available oscillator configurations.  Typically
this would involve configuring the oscillator tuning register or clock
switching useing the compiler's __builtin_write_OSCCON functions.
Refer to the C Compiler for PIC24 MCUs and dsPIC DSCs User Guide in the
compiler installation directory /doc folder for documentation on the
__builtin functions.*/

/* TODO Add clock switching code if appropriate.  An example stub is below.   */
void ConfigureOscillator(void) {

#if 0
    /* Disable Watch Dog Timer */
    RCONbits.SWDTEN = 0;

    /* When clock switch occurs switch to Primary Osc (HS, XT, EC) */
    __builtin_write_OSCCONH(0x02); /* Set OSCCONH for clock switch */
    __builtin_write_OSCCONL(0x01); /* Start clock switching */
    while (OSCCONbits.COSC != 0b011);

    /* Wait for Clock switch to occur */
    /* Wait for PLL to lock, only if PLL is needed */
    /* while(OSCCONbits.LOCK != 1); */
#endif
}

void configureAnalogInputs(void) {
    // power-on default is all analog inputs selected

    ANSELA = 0; // disable all analog inputs on port A
    ANSELE = 0; // disable all analog inputs on port E

    // enable certain analog inputs on port B
    ANSELB = 0;
    ANSELBbits.ANSB6 = 1; // ANA2
    ANSELBbits.ANSB7 = 1; // ANA3
    ANSELBbits.ANSB8 = 1; // ANA0
    ANSELBbits.ANSB9 = 1; // ANA1
    ANSELBbits.ANSB13 = 1; // V
    ANSELBbits.ANSB14 = 1; // I
    ANSELBbits.ANSB15 = 1; // RS

    // set analog pins as inputs
    TRISBbits.TRISB6 = 1; // AN6
    TRISBbits.TRISB7 = 1; // AN7
    TRISBbits.TRISB8 = 1; // AN0
    TRISBbits.TRISB9 = 1; // AN1
    TRISBbits.TRISB13 = 1; // AN13
    TRISBbits.TRISB14 = 1; // AN14
    TRISBbits.TRISB15 = 1; // AN15
}

void configureDigitalIO(void) {
    // port A
    TRISAbits.TRISA6 = 1; // DIG2
    TRISAbits.TRISA7 = 1; // DIG1
    TRISAbits.TRISA15 = 1; // I4
    TRISAbits.TRISA14 = 1; // I5
    TRISAbits.TRISA5 = 1; // I6
    TRISAbits.TRISA4 = 1; // I7

    // port B
    TRISBbits.TRISB2 = 0; // LED1
    TRISBbits.TRISB3 = 0; // LED2
    TRISBbits.TRISB4 = 0; // LED3
    TRISBbits.TRISB5 = 0; // LED4

    // port D
    TRISDbits.TRISD0 = 1; // I1
    TRISDbits.TRISD1 = 1; // I2
    TRISDbits.TRISD7 = 0; // O4
    TRISDbits.TRISD8 = 1; // I3

    // port E
    TRISEbits.TRISE0 = 1; // O2
    TRISEbits.TRISE1 = 1; // DIG0
    TRISEbits.TRISE2 = 0; // SD01 (MPU6000)
    TRISEbits.TRISE3 = 1; // SDI1 (MPU6000)
    TRISEbits.TRISE4 = 0; // SS1  (MPU6000)
    TRISEbits.TRISE5 = 0; // GPS_TX
    TRISEbits.TRISE6 = 1; // GPS_RX
    TRISEbits.TRISE7 = 1; // SS2  (AT45)

    // port F
    TRISFbits.TRISF8 = 1; // I8
    TRISFbits.TRISF13 = 0; // O7
    TRISFbits.TRISF12 = 0; // O8

    // port G
    TRISGbits.TRISG0 = 0; // O1
    TRISGbits.TRISG13 = 0; // O3
    TRISGbits.TRISG14 = 0; // O5
    TRISGbits.TRISG1 = 0; // O6

}

void configurePPS(void) {
    // configure PPS registers

    //*************************************************************
    // Unlock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));

    // CAN module 1 I/O
    _C1RXR = 96;
    _RP97R = 0b001110;

    // SPI1 SS, SCK, SDI, SDO
    _RP84R = 0b000111;     // SS1 output RP84
    _RP127R = 0b000110;    // SCK1 input/output RP127
    _SDI1R = 83;           // SDI1 input RPI83
    _RP82R = 0b000101;     // SDO1 output RP82

    // SPI2 SS, SCK, SDI, SDO are dedicated pins

    // SPI3 SS, SCK, SDI, SDO
    _RP66R = 0b100001;     // SS3 output RP66
    _RP127R = 0b100000;    // SCK3 output RP65
    _SDI3R = 76;           // SDI3 input RPI76
    _RP82R = 0b011111;     // SDO3 output RP67

    // INTG (MPU6000 interrupt)
    _INT1R = 124;          // RPI124/RG12

    // O1:8 are PWM module outputs

    // UART1 RX, TX
    _U1RXR = 78;           // U1RX input RPI78
    _RP79R = 0b000001;     // U1TX output RP79


    //*************************************************************
    // Lock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON | (1 << 6));

}