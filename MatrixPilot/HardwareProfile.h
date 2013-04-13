#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H


////    #define GetSystemClock()        16000000UL
//    #define GetPeripheralClock()    16000000UL

    #define GetSystemClock()        64000000UL
    #define GetPeripheralClock()    64000000UL

	#define FOSC		128000000LL		// clock-frequecy in Hz with suffix LL (64-bit-long), eg. 32000000LL for 32MHz
	#define FCY      	(FOSC/2)		// MCU is running at FCY MIPS

	#define delay_us(x)	__delay32(((x*FCY)/1000000L))	// delays x us
	#define delay_ms(x)	__delay32(((x*FCY)/1000L))		// delays x ms

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

#endif  //HARDWARE_PROFILE_H
