#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H


//#define USE_AT45D_DMA

// UART Tx and Rx pins
#define TX_TRIS TRISFbits.TRISF3
#define RX_TRIS TRISFbits.TRISF2

#define SWITCH_PRESSED      0
#define LED_OFF             1
#define LED_ON              0

#define InitLED()           _TRISB2 = 0; _TRISB3 = 0; _TRISB4 = 0; _TRISB5 = 0; _LATB2 = LED_OFF; _LATB3 = LED_OFF; _LATB4 = LED_OFF; _LATB5 = LED_OFF;

#define BlueLED             _LATB2
#define RedLED              _LATB3
#define GreenLED            _LATB4
#define OrangeLED           _LATB5

#define BlinkRedLED()       (RedLED = ((TMR1 & 0x8000) != 0))
#define BlinkBlueLED()      (BlueLED = ((TMR1 & 0x8000) != 0))
#define BlinkGreenLED()     (GreenLED = ((TMR1 & 0x8000) != 0))
#define BlinkOrangeLED()    (OrangeLED = ((TMR1 & 0x8000) != 0))

// Configure switch on pin DIG2 on AUAV3, set to input with internal pull-up
//#define InitSwitch()        _TRISA6 = 1; _CNPUA6 = 1;
//#define ReadSwitchStatus()  _RA6

// Configure switch on pin PGD on AUAV3, set to input with internal pull-up
//#define InitSwitch()        _TRISB0 = 1; _CNPUB0 = 1;
//#define ReadSwitchStatus()  _RB0

// Configure switch on pin PGC on AUAV3, set to input with internal pull-up
#define InitSwitch()        _TRISB1 = 1; _CNPUB1 = 1;
#define ReadSwitchStatus()  _RB1


#endif  //HARDWARE_PROFILE_H
