#include "delay.h"
//#include "libUDB_defines.h"
#include "libUDB.h"

void delayMs(uint16_t delay)
{
	while (delay > 1000)
	{
		delayMs(1000);
		delay -= 1000;
	}	
	if (!delay) return;

	T3CON = 0x0030;	// 1:256 Prescale, Internal Fp
	TMR3 = 0;
	T3CONbits.TON = 1;
	delay *= (FCY_KHz/256);
	
	while(TMR3 <= delay)
	{
		 // wait here while TMR increments to delayMs
	}
}

void delayUs(uint16_t delay)
{
	while (delay >= 1000)
	{
		delayMs(delay/1000);
		delay -= 1000;
	}
	if (!delay) return;

	T3CON = 0x0000;	//  Prescale Disabled
	TMR3 = 0;
	T3CONbits.TON = 1;
	delay *= FCY_MHz;
	while(TMR3 <= delay)
	{
		 // wait here while TMR increments to delayMs
	}	
}		



