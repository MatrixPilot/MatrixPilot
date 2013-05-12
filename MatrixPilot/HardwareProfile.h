#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H


#include "../libUDB/oscillator.h"

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
