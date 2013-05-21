#ifndef HARDWARE_PROFILE_H
#define HARDWARE_PROFILE_H


#include "../libUDB/oscillator.h"


#define BAUDRATEX       115200UL
#define BRGHX           1
#if (BRGHX == 0)
	#define BRG_DIVX        16
#elif (BRGHX == 1)
	#define BRG_DIVX        4
#else
	#error Must select a valid BRGHX value
#endif


#if (BOARD_TYPE == AUAV3_BOARD)

#endif // BOARD_TYPE

#endif  //HARDWARE_PROFILE_H
