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


#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp              */

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

int16_t main(void) {

    /* Configure the oscillator for the device */
    ConfigureOscillator();
    configurePPS();
    configureAnalogInputs();
    configureDigitalIO();

    /* Initialize IO ports and peripherals */
    InitApp();

    // test some I/O pins

    // Initialize and enable Timer3
    T3CONbits.TON = 0; // Disable Timer
    T3CONbits.TCS = 0; // Select internal instruction cycle clock
    T3CONbits.TGATE = 0; // Disable Gated Timer mode
    T3CONbits.TCKPS = 0b01; // Select 8:1 Prescaler 16MHz/8 = 2MHz 40MHz/8 = 5MHz
    TMR3 = 0x00; // Clear timer register
    PR3 = 10000; // Load the period value
    IEC0bits.T3IE = 0; // disable interrupts
    T3CONbits.TON = 1; // Start timer

    TRISGbits.TRISG0 = 0; // O1/RG0 output

    // Initialize Output Compare Module
    OC1CON1 = 0;
    OC1CON2 = 0;
//    OC1CON1bits.OCTSEL = 1; // Select Timer 3 as output compare time base
    OC1CON1bits.OCTSEL = 0x07;  // peripheral clock input selected
    OC1R = 1000; // Write the duty cycle for the first PWM pulse (1msec = 250 counts)
    OC1RS = 2000; // Write the duty cycle for the second PWM pulse
    OC1CON1bits.OCM = 0b000; // Disable Output Compare Module
    OC1CON2bits.SYNCSEL = 0x1F;
    OC1CON1bits.OCM = 0b110; // Select the Output Compare mode


    /* TODO <INSERT USER APPLICATION CODE HERE> */
    TRISEbits.TRISE1 = 0; // DIG0

    while (1) {
        _LATE1 = 1;
        _LATE1 = 0;

    }
}
