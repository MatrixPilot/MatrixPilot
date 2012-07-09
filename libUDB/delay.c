#include "delay.h"


#define FCY  16000000

void delay_us(unsigned long us)
{

	long i = FCY/1000/1000/12*us;
	for(;i > 0 ;i--)
		asm("clrwdt");
}

void delay_ms(unsigned long ms)
{
	unsigned long i = FCY/1000/12*ms;
    for(;i > 0 ;i--)
        asm("clrwdt");
}
