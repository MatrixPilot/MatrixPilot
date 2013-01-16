#include "libUDB_internal.h"
#include "debug.h"
#include "uart1.h"


#ifdef USE_DEBUG_U1

// UART1 is used by the GPS

/* EXAMPLE baud rate calculations from gluonpilot:

#define FOSC 8000000        // We're using an 8MHz Crystal
#define PLL  80/8           // PLL configuration
#define FCY  PLL*FOSC/2     // Resulting Fcy for a dsPic33 == 40,000,000

	// Load a value into Baud Rate Generator.  Example is for 9600.
	// See section 19.3.1 of datasheet.
	//  U2BRG = (Fcy/(16*BaudRate))-1
	//  U2BRG = (40MHz/(16*38400))-1
	//  U2BRG = 65
	
	U1BRG = (int)(FCY / (16*baud) - 1);	
 */

extern int __C30_UART;

#define UART_BAUD(x)	((int)((FREQOSC / CLK_PHASES) / ((long)16 * x) - 1))

#define UART1_TX_BUFFER_SIZE 1024
#define UART1_RX_BUFFER_SIZE 256
#define UART1_TX_BUFFER_MASK (UART1_TX_BUFFER_SIZE - 1)
#define UART1_RX_BUFFER_MASK (UART1_RX_BUFFER_SIZE - 1)

#if (UART1_TX_BUFFER_SIZE & UART1_TX_BUFFER_MASK)
    #error UART1 TX Buffer Size NOT a Power of 2
#endif
#if (UART1_RX_BUFFER_SIZE & UART1_RX_BUFFER_MASK)
    #error UART1 RX Buffer Size NOT a Power of 2
#endif
 
static char vUART1RXFIFO[UART1_TX_BUFFER_SIZE];
static char vUART1TXFIFO[UART1_TX_BUFFER_SIZE];
static volatile char *RX1HeadPtr = vUART1RXFIFO, *RX1TailPtr = vUART1RXFIFO;
static volatile char *TX1HeadPtr = vUART1TXFIFO, *TX1TailPtr = vUART1TXFIFO;


int uart1_getc(void)
{
	int result;

    _U1RXIE = 0;
	if (RX1TailPtr != RX1HeadPtr) {
		result = (unsigned char)*RX1TailPtr++;
		if (RX1TailPtr >= vUART1RXFIFO + sizeof(vUART1RXFIFO)) {
			RX1TailPtr = vUART1RXFIFO;
		}
	} else {
		result = -1;
	}
    _U1RXIE = 1;
	return result;
}

void uart1_putc(char ch)
{
	char* TXHeadPtr;

    _U1TXIE = 0;
	TXHeadPtr = TX1HeadPtr + 1;
    if (TXHeadPtr >= vUART1TXFIFO + sizeof(vUART1TXFIFO)) {
        TXHeadPtr = vUART1TXFIFO;
    }
    if (TXHeadPtr != TX1TailPtr) {
	    *TX1HeadPtr = ch;
		TX1HeadPtr = TXHeadPtr;
		if (U1STAbits.TRMT) {
	        // if the uart is not already transmitting, kickstart it
			_U1TXIF = 1;
		}
	} else {
		// buffer overflow, drop new characters
//		udb_led_toggle(LED_BLUE) ;
	}
    _U1TXIE = 1;
}

void uart1_puts(char *str)
{
    do {
        if (*str != '\0')
            uart1_putc(*str);
    } while (*str++);
}

void uart1_callback_received_byte(char ch)
{
	// Copy the byte into the local FIFO, if it won't cause an overflow
	if (RX1HeadPtr != RX1TailPtr - 1) {
		if ((RX1HeadPtr != vUART1RXFIFO + sizeof(vUART1RXFIFO)) || (RX1TailPtr != vUART1RXFIFO)) {
			*RX1HeadPtr++ = ch;
			if (RX1HeadPtr >= vUART1RXFIFO + sizeof(vUART1RXFIFO))
				RX1HeadPtr = vUART1RXFIFO;
		}
	}
}

int uart1_callback_get_byte_to_send(void)
{
/*	char ch;

	ch = *TX1TailPtr++;
	if (TX1TailPtr >= vUART1TXFIFO + sizeof(vUART1TXFIFO))
		TX1TailPtr = vUART1TXFIFO;
	return ch;
 */
	int result;

	if (TX1TailPtr != TX1HeadPtr) {
		result = (unsigned char)*TX1TailPtr++;
		if (TX1TailPtr >= vUART1TXFIFO + sizeof(vUART1TXFIFO)) {
			TX1TailPtr = vUART1TXFIFO;
		}
	} else {
		result = -1;
	}
	return result;
}

void uart1_init(void)
{
	U1MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
	//						// Bit14
	U1MODEbits.USIDL = 0;	// Bit13 Continue in Idle
	U1MODEbits.IREN = 0;	// Bit12 No IR translation
	U1MODEbits.RTSMD = 0;	// Bit11 Simplex Mode
	//						// Bit10
	U1MODEbits.UEN = 0;		// Bits8,9 TX,RX enabled, CTS,RTS not
	U1MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
	U1MODEbits.LPBACK = 0;	// Bit6 No Loop Back
	U1MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
	U1MODEbits.URXINV = 0;	// Bit4 IdleState = 1  (for dsPIC)

//	U1MODEbits.BRGH = 1;	// Bit3 4 clocks per bit period
	U1MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period

	U1MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U1MODEbits.STSEL = 0;	// Bit0 One Stop Bit
	
	U1STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U1STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
	U1STAbits.UTXISEL0 = 1;	//Bit13 Other half of Bit15
	//						//Bit12
	U1STAbits.UTXBRK = 0;	//Bit11 Disabled
	//U1STAbits.UTXEN = 1;	//Bit10 TX pins controlled by periph (handled below)
	//U1STAbits.UTXBF = 0;	//Bit9 *Read Only Bit*
	//U1STAbits.TRMT = 0;	//Bit8 *Read Only bit*
	U1STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
	U1STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled
	//U1STAbits.RIDLE = 0;	//Bit4 *Read Only Bit*
	//U1STAbits.PERR = 0;	//Bit3 *Read Only Bit*
	//U1STAbits.FERR = 0;	//Bit2 *Read Only Bit*
	U1STAbits.OERR = 0;		//Bit1 *Read Only Bit*
	//U1STAbits.URXDA = 0;	//Bit0 *Read Only Bit*

	_U1TXIP = 4;	// Mid Range Interrupt Priority level, no urgent reason
	_U1RXIP = 4;	// Mid Range Interrupt Priority level, no urgent reason

	_U1TXIF = 0;	// Clear the Transmit Interrupt Flag
	_U1TXIE = 1;	// Enable Transmit Interrupts
	_U1RXIF = 0;	// Clear the Recieve Interrupt Flag
	_U1RXIE = 1;	// Enable Recieve Interrupts

	U1MODEbits.UARTEN = 1;	// And turn the peripheral on
	U1STAbits.UTXEN = 1;
	
	uart1_set_rate(57600);
//	uart1_set_rate(115200);
	__C30_UART = 1;
}

void uart1_set_rate(long rate)
{
	U1BRG = UART_BAUD(rate) ;
}

int uart1_check_rate(long rate)
{
	return ( U1BRG == UART_BAUD(rate) ) ;
}

void uart1_serial_start_sending_data(void)
{
	_U1TXIF = 1 ; // fire the tx interrupt
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U1TXInterrupt(void)
{
	interrupt_save_set_corcon ;
	
	_U1TXIF = 0 ; // clear the interrupt 
	
	int txchar = uart1_callback_get_byte_to_send() ;

//	if (U1STAbits.TRMT == 0) {
//	}
	
	if ( txchar != -1 )
	{
		U1TXREG = (unsigned char)txchar ;
	} else {
		// Disable the TX interrupt if we are done so that we don't keep entering this ISR
		_U1TXIE = 0;
	}
	
	interrupt_restore_corcon ;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U1RXInterrupt(void)
{
	interrupt_save_set_corcon ;
	
	while ( U1STAbits.URXDA )
	{
		unsigned char rxchar = U1RXREG ;
		uart1_callback_received_byte(rxchar) ;
	}

	U1STAbits.OERR = 0 ;
	
	_U1RXIF = 0 ; // clear the interrupt

	interrupt_restore_corcon ;
	return ;
}

unsigned int write(int handle, void *buffer, unsigned int len)
{
    int i;

    for (i = len; i; --i) {
		uart1_putc(*(char*)buffer++);
    }
    return(len);
}

#endif //  USE_DEBUG_U1
