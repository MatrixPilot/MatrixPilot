//#include "HardwareProfile.h"
//#include "Compiler.h"
//#include <GenericTypeDefs.h>
#include "defines.h"
#include <uart.h>

#include <string.h>
#include <stdio.h>

    #define putrsUART2(x) putsUART2(x)

#include "debug.h"


///////////////////////////////////////////////////////////////////
// private helper functions

/* 
// Debug function prototypes
extern void _pic32mxsk_init (void);
extern void  __attribute__ ((noinline,nomips16,weak)) db_puts (const unsigned char *s, int len);
extern void  __attribute__ ((noinline,nomips16,weak)) db_gets (unsigned char *s, int len);
extern void __attribute__ ((weak)) _mon_write (const char * s, unsigned int count) __attribute__((alias("_p32mxsk_write")));
extern void __attribute__ ((weak)) _mon_putc (char c) __attribute__((alias("_p32mxsk_putc")));
extern void __attribute__ ((weak)) _mon_getc (int canblock) __attribute__((alias("_p32mxsk_getc")));
 */
/*
void _mon_getc(int canblock)
{
}
 */

// Baud Rate Generator -- See section 19.3.1 of datasheet.
// Fcy = FREQOSC / CLK_PHASES
// UXBRG = (Fcy/(16*BaudRate))-1
// UXBRG = ((32000000/2)/(16*9600))-1
// UXBRG = 103
#define UDB_DEBUG_BAUD(x)		((int)((FREQOSC / CLK_PHASES) / ((long)16 * x) - 1))

void udb_debug_set_rate(long rate)
{
	U2BRG = UDB_DEBUG_BAUD(rate) ;
	return ;
}

void udb_init_debug_io(void)
{
/*
	// configure U2MODE
	U2MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	//						// Bit14
	U2MODEbits.USIDL = 0;	// Bit13 Continue in Idle
	U2MODEbits.IREN = 0;	// Bit12 No IR translation
	U2MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
	//						// Bit10
	U2MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U2MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
	U2MODEbits.LPBACK = 0;	// Bit6 No Loop Back
	U2MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
	U2MODEbits.URXINV = 0;	// Bit4 IdleState = 1  (for dsPIC)
	U2MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
	U2MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U2MODEbits.STSEL = 0;	// Bit0 One Stop Bit
	
	// Load all values in for U2STA SFR
	U2STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U2STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
	U2STAbits.UTXISEL0 = 1;	//Bit13 Other half of Bit15
	//				 		//Bit12
	U2STAbits.UTXBRK = 0;	//Bit11 Disabled
	//U2STAbits.UTXEN = 1;	//Bit10 TX pins controlled by periph (handled below)
	//U2STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
	//U2STAbits.TRMT = 0;	//Bit8 *Read Only bit*
	U2STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
	U2STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
	//U2STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
	//U2STAbits.PERR = 0;	//Bit3 *Read Only Bit*
	//U2STAbits.FERR = 0;	//Bit2 *Read Only Bit*
	U2STAbits.OERR = 0;		//Bit1 *Read Only Bit*
	//U2STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

//	_U2TXIP = 4;	// Mid Range Interrupt Priority level, no urgent reason
//	_U2RXIP = 4;	// Mid Range Interrupt Priority level, no urgent reason

//	_U2TXIF = 0;	// Clear the Transmit Interrupt Flag
//	_U2TXIE = 1;	// Enable Transmit Interrupts
//	_U2RXIF = 0;	// Clear the Recieve Interrupt Flag
//	_U2RXIE = 1;	// Enable Recieve Interrupts

	U2MODEbits.UARTEN = 1;	// And turn the peripheral on

	U2STAbits.UTXEN = 1;
*/
// now relying on udb_init_USART() to set the peripheral up
// here we just initialise buffers, flags etc, and set the baud rate we want
	
	udb_debug_set_rate(57600) ;

	return ;
}


static void USARTPut(char c)
{
    while( !U2STAbits.TRMT);
    U2TXREG = c;
}

static void putsUART(char *s)
{
    char c;

    while( (c = *s++) )
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
    vsprintf(buffer, fmt, ap);
    va_end(ap);

	putsUART(buffer);
}
