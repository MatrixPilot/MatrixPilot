#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H

    /*******************************************************************/
    /******** USB stack hardware selection options *********************/
    /*******************************************************************/
    //This section is the set of definitions required by the MCHPFSUSB
    //  framework.  These definitions tell the firmware what mode it is
    //  running in, and where it can find the results to some information
    //  that the stack needs.
    //These definitions are required by every application developed with
    //  this revision of the MCHPFSUSB framework.  Please review each
    //  option carefully and determine which options are desired/required
    //  for your application.

    //#define USE_SELF_POWER_SENSE_IO
    #define tris_self_power     TRISAbits.TRISA2    // Input
    #define self_power          1

    //#define USE_USB_BUS_SENSE_IO
    #define tris_usb_bus_sense  TRISBbits.TRISB5    // Input
    #define USB_BUS_SENSE       1

  /*******************************************************************/
    /******** MDD File System selection options ************************/
    /*******************************************************************/
    #define ERASE_BLOCK_SIZE        1024
    #define WRITE_BLOCK_SIZE        128

    /*******************************************************************/
    /*******************************************************************/
    /*******************************************************************/
    /******** Application specific definitions *************************/
    /*******************************************************************/
    /*******************************************************************/
    /*******************************************************************/

    /** Board definition ***********************************************/
    //These defintions will tell the main() function which board is
    //  currently selected.  This will allow the application to add
    //  the correct configuration bits as wells use the correct
    //  initialization functions for the board.  These defitions are only
    //  required in the stack provided demos.  They are not required in
    //  final application design.
//    #define DEMO_BOARD DSPIC33EP512MU810_PIM
//    #define EXPLORER_16
    
    #define GetSystemClock()        4000000UL
    #define GetPeripheralClock()    4000000UL
//    #define GetSystemClock()        16000000UL
//    #define GetPeripheralClock()    16000000UL


    /** LED ************************************************************/
/*
    TRISBbits.TRISB2 = 0; // LED1
    TRISBbits.TRISB3 = 0; // LED2
    TRISBbits.TRISB4 = 0; // LED3
    TRISBbits.TRISB5 = 0; // LED4
 */
    #define mInitAllLEDs()      LATB &= 0x00; TRISB &= 0xFF00;
    
    #define mLED_1              LATBbits.LATB2
    #define mLED_2              LATBbits.LATB3
    #define mLED_3              LATBbits.LATB4
    #define mLED_4              LATBbits.LATB5
    
    #define mLED_1_On()         mLED_1 = 0;
    #define mLED_2_On()         mLED_2 = 0;
    #define mLED_3_On()         mLED_3 = 0;
    #define mLED_4_On()         mLED_4 = 0;
    
    #define mLED_1_Off()        mLED_1 = 1;
    #define mLED_2_Off()        mLED_2 = 1;
    #define mLED_3_Off()        mLED_3 = 1;
    #define mLED_4_Off()        mLED_4 = 1;

    #define mGetLED_1()         mLED_1
    #define mGetLED_2()         mLED_2
    #define mGetLED_3()         mLED_3
    #define mGetLED_4()         mLED_4
    
    #define mLED_1_Toggle()     mLED_1 = !mLED_1;
    #define mLED_2_Toggle()     mLED_2 = !mLED_2;
    #define mLED_3_Toggle()     mLED_3 = !mLED_3;
    #define mLED_4_Toggle()     mLED_4 = !mLED_4;
    
    /** UART2 **********************************************************/
    #define BAUDRATE2       57600UL
    #define BRG_DIV2        4 
    #define BRGH2           1 

    /** UART3 **********************************************************/
    #define BAUDRATE3       57600UL
//    #define BAUDRATE3       115200UL
    #define BRG_DIV3        4 
    #define BRGH3           0

    /** I/O pin definitions ********************************************/
    #define INPUT_PIN 1
    #define OUTPUT_PIN 0

#endif // HARDWARE_PROFILE_H
