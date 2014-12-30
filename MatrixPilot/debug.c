#if 0
//#include "HardwareProfile.h"
//#include "Compiler.h"
//#include <GenericTypeDefs.h>
#include "defines.h"
#include "../libUDB/oscillator.h"
#include <uart.h>

#include <string.h>
#define _ADDED_C_LIB 1
#include <stdio.h>

    #define putrsUART2(x) putsUART2(x)

#include "debug.h"


///////////////////////////////////////////////////////////////////
// private helper functions

/* 
// Debug function prototypes
extern void _pic32mxsk_init (void);
extern void  __attribute__ ((noinline,nomips16,weak)) db_puts (const uint8_t *s, int16_t len);
extern void  __attribute__ ((noinline,nomips16,weak)) db_gets (uint8_t *s, int16_t len);
extern void __attribute__ ((weak)) _mon_write (const char * s, uint16_t count) __attribute__((alias("_p32mxsk_write")));
extern void __attribute__ ((weak)) _mon_putc (char c) __attribute__((alias("_p32mxsk_putc")));
extern void __attribute__ ((weak)) _mon_getc (int16_t canblock) __attribute__((alias("_p32mxsk_getc")));
 */
/*
void _mon_getc(int16_t canblock)
{
}
 */

// Baud Rate Generator -- See section 19.3.1 of datasheet.
// Fcy = FREQOSC / CLK_PHASES
// UXBRG = (Fcy/(16*BaudRate))-1
// UXBRG = ((32000000/2)/(16*9600))-1
// UXBRG = 103
#define UDB_DEBUG_BAUD(x)		((int16_t)((FREQOSC / CLK_PHASES) / ((int32_t)16 * x) - 1))

void udb_debug_set_rate(int32_t rate)
{
	U2BRG = UDB_DEBUG_BAUD(rate);
	return;
}

void udb_init_debug_io(void)
{

// now relying on udb_init_USART() to set the peripheral up
// here we just initialise buffers, flags etc, and set the baud rate we want
	
	udb_debug_set_rate(57600);

	return;
}


static void USARTPut(char c)
{
    while(!U2STAbits.TRMT);
    U2TXREG = c;
}

static void putsUART(char *s)
{
    char c;

    while((c = *s++))
        USARTPut(c);
}

void _mon_putc(char c)
{
#if 1
	USARTPut(c);
#else
    char pkt[2];
    pkt[0] = c;
    pkt[1] = '\0';
	putsUART(pkt);
#endif
}

void dprintf(char* fmt,...)
{
    char buffer[128];
    va_list ap;
 

    va_start(ap, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);

	putsUART(buffer);
}
#endif // 0
