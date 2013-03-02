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

    // test 70 MIPS operation; PLL at 140MHz, FCY = 70MHz
    CLKDIVbits.PLLPRE = 0; // PLL prescaler: divide by 2, postscaler: div by 4(default), PLL divisor: x40, FRCdiv:1(default)
    CLKDIVbits.PLLPOST = 0;
    PLLFBDbits.PLLDIV = 68; // FOSC = 140 MHz (XTAL=8MHz, N1=2, N2=2, M = 70)
}

// This method assigns all ANSELx bits and affects only specific TRISx bits

void configureAnalogInputs(void) {
    // power-on default is all analog inputs selected

    ANSELA = 0; // disable all analog inputs on port A
    ANSELC = 0; // disable all analog inputs on port C
    ANSELD = 0; // disable all analog inputs on port D
    ANSELE = 0; // disable all analog inputs on port E
    ANSELG = 0; // disable all analog inputs on port G

    // enable specific analog inputs on port B
    // AN6:9,13:15 map to:
    // AUAV3 inputs ANA2:3, ANA0:1, V, I, RS
    int mask = ((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 13) | (1 << 14) | (1 << 15));
    ANSELB = mask;

    // set analog pins as inputs
    TRISB |= mask;
}

// This method configures TRISx for all digital IOs

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
    TRISDbits.TRISD2 = 0; // SS3
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
    TRISEbits.TRISE7 = 0; // SS2  (AT45)

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

// This method assigns all PPS registers

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
    //    _RP84R = 0b000111;     // SS1 output RP84
    // in master mode, SS is not used by the SPI module; configure as GP output instead
    // LATE4 is SS1
    _RP127R = 0b000110; // SCK1 input/output RP127
    _SDI1R = 83; // SDI1 input RPI83
    _RP82R = 0b000101; // SDO1 output RP82

    // SPI2: SCK2, SDI2, SDO2 are dedicated pins
    //    _RP87R = 0b001010;     // SS2 output RP87
    // LATE7 is SS2

    // SPI3 SS, SCK, SDI, SDO
    //    _RP66R = 0b100001;     // SS3 output RP66
    // LATD2 is SS3
    _RP127R = 0b100000; // SCK3 output RP65
    _SDI3R = 76; // SDI3 input RPI76
    _RP82R = 0b011111; // SDO3 output RP67

    // INTG (MPU6000 interrupt)
    _INT1R = 124; // RPI124/RG12

    // IC1:8 are Input Capture module inputs
    _IC1R = 64; // IC1 on RP64
    _IC2R = 75; // IC2 on RP75
    _IC3R = 72; // IC3 on RP72
    _IC4R = 31; // IC4 on RP31
    _IC5R = 30; // IC5 on RP30
    _IC6R = 21; // IC6 on RP21
    _IC7R = 20; // IC7 on RP20
    _IC8R = 104; // IC8 on RP104

    // OC1:8 are PWM module outputs

    // temporarily assign REFCLK0 to OC1 pin for PLL testing
//    _RP112R = 0b010000; // OC1 output RP112
    _RP112R = 0b110001; // REFCLK0 output RP112
    REFOCONbits.RODIV = 7;  // divide by 128
    REFOCONbits.ROON = 1;   // enable refclk output

    _RP80R = 0b010001; // OC2 output RP80
    _RP125R = 0b010010; // OC3 output RP125
    _RP71R = 0b010011; // OC4 output RP71
    _RP126R = 0b010100; // OC5 output RP126
    _RP113R = 0b010101; // OC6 output RP113
    _RP109R = 0b010110; // OC7 output RP109
    _RP108R = 0b010111; // OC8 output RP108

    // UART1 RX, TX
    _U1RXR = 78; // U1RX input RPI78
    _RP79R = 0b000001; // U1TX output RP79

    // UART2 RX, TX
    _U2RXR = 100; // U2RX input RP100
    _RP101R = 0b000011; // U2TX output RP79

    // UART3 RX, TX
    _U3RXR = 98; // U3RX input RP98
    _RP79R = 0b011011; // U3TX output RP79

    // UART4 RX, TX
    _U4RXR = 86; // U4RX input RPI86
    _RP79R = 0b011101; // U4TX output RP79


    //*************************************************************
    // Lock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON | (1 << 6));

}