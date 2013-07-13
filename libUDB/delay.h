
#ifndef _DELAY_H_
#define _DELAY_H_

#include "oscillator.h"
#include <stdint.h>

void delayUs(uint16_t delayUs);
void delayMs(uint16_t delayMs);

//#define	FCY			16000000L
#define	FCY_MHz		(FCY/1000000UL)
#define	FCY_KHz		(FCY/1000UL)

#endif
