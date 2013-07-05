#if 0

#include "libUDB_internal.h"
#include "oscillator.h"
#include "interrupt.h"
#include "debug.h"
#include "uart2.h"


#ifdef USE_DEBUG_U2


#define UART_BAUD(x)	((int)((FREQOSC / CLK_PHASES) / ((long)16 * x) - 1))

extern int __C30_UART;

//#define UART2_TX_BUFFER_SIZE 256
#define UART2_TX_BUFFER_SIZE 1024
#define UART2_RX_BUFFER_SIZE 128
#define UART2_TX_BUFFER_MASK (UART2_TX_BUFFER_SIZE - 1)
#define UART2_RX_BUFFER_MASK (UART2_RX_BUFFER_SIZE - 1)

#if (UART2_TX_BUFFER_SIZE & UART2_TX_BUFFER_MASK)
    #error UART2 TX Buffer Size NOT a Power of 2
#endif
#if (UART2_RX_BUFFER_SIZE & UART2_RX_BUFFER_MASK)
    #error UART2 RX Buffer Size NOT a Power of 2
#endif

static char vUART2RXFIFO[UART2_TX_BUFFER_SIZE];
static char vUART2TXFIFO[UART2_TX_BUFFER_SIZE];
static volatile char *RX2HeadPtr = vUART2RXFIFO, *RX2TailPtr = vUART2RXFIFO;
static volatile char *TX2HeadPtr = vUART2TXFIFO, *TX2TailPtr = vUART2TXFIFO;

int uart2_getc(void)
{
	int result;

    _U2RXIE = 0;
	if (RX2TailPtr != RX2HeadPtr) {
		result = (unsigned char)*RX2TailPtr++;
		if (RX2TailPtr >= vUART2RXFIFO + sizeof(vUART2RXFIFO)) {
			RX2TailPtr = vUART2RXFIFO;
		}
	} else {
		result = -1;
	}
    _U2RXIE = 1;
	return result;
}

void uart2_putc(char ch)
{
	char* TXHeadPtr;

    _U2TXIE = 0;
	TXHeadPtr = TX2HeadPtr + 1;
    if (TXHeadPtr >= vUART2TXFIFO + sizeof(vUART2TXFIFO)) {
        TXHeadPtr = vUART2TXFIFO;
    }
    if (TXHeadPtr != TX2TailPtr) {
	    *TX2HeadPtr = ch;
		TX2HeadPtr = TXHeadPtr;
		if (U2STAbits.TRMT) {
	        // if the uart is not already transmitting, kickstart it
			_U2TXIF = 1;
		}
	} else {
		// buffer overflow, drop new characters
//		udb_led_toggle(LED_BLUE) ;
	}
    _U2TXIE = 1;
}

void uart2_puts(char *str)
{
    do {
        if (*str != '\0')
            uart2_putc(*str);
    } while (*str++);
}

void uart2_callback_received_byte(char ch)
{
	// Copy the byte into the local FIFO, if it won't cause an overflow
	if (RX2HeadPtr != RX2TailPtr - 1) {
		if ((RX2HeadPtr != vUART2RXFIFO + sizeof(vUART2RXFIFO)) || (RX2TailPtr != vUART2RXFIFO)) {
			*RX2HeadPtr++ = ch;
			if (RX2HeadPtr >= vUART2RXFIFO + sizeof(vUART2RXFIFO))
				RX2HeadPtr = vUART2RXFIFO;
		}
	}
}

int uart2_callback_get_byte_to_send(void)
{
/*	char ch;

	ch = *TX2TailPtr++;
	if (TX2TailPtr >= vUART2TXFIFO + sizeof(vUART2TXFIFO))
		TX2TailPtr = vUART2TXFIFO;
	return ch;
 */
	int result;

	if (TX2TailPtr != TX2HeadPtr) {
		result = (unsigned char)*TX2TailPtr++;
		if (TX2TailPtr >= vUART2TXFIFO + sizeof(vUART2TXFIFO)) {
			TX2TailPtr = vUART2TXFIFO;
		}
	} else {
		result = -1;
	}
	return result;
}

void uart2_init(void)
{
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

//	U2MODEbits.BRGH = 1;	// Bit3 4 clocks per bit period
	U2MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period

	U2MODEbits.PDSEL = 0;	// Bits1,2 8bit, No Parity
	U2MODEbits.STSEL = 0;	// Bit0 One Stop Bit
	
	U2STAbits.UTXISEL1 = 0;	//Bit15 Int when Char is transferred (1/2 config!)
	U2STAbits.UTXINV = 0;	//Bit14 N/A, IRDA config
	U2STAbits.UTXISEL0 = 1;	//Bit13 Other half of Bit15
	//						//Bit12
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

	_U2TXIP = 4;	// Mid Range Interrupt Priority level, no urgent reason
	_U2RXIP = 4;	// Mid Range Interrupt Priority level, no urgent reason

	_U2TXIF = 0;	// Clear the Transmit Interrupt Flag
	_U2TXIE = 1;	// Enable Transmit Interrupts
	_U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	_U2RXIE = 1;	// Enable Recieve Interrupts

	U2MODEbits.UARTEN = 1;	// And turn the peripheral on
	U2STAbits.UTXEN = 1;
	
//	uart2_set_rate(57600);
	uart2_set_rate(115200);
	__C30_UART = 2;
}

void uart2_set_rate(long rate)
{
	U2BRG = UART_BAUD(rate) ;
}

int uart2_check_rate(long rate)
{
	return ( U2BRG == UART_BAUD(rate) ) ;
}

void uart2_serial_start_sending_data(void)
{
	_U2TXIF = 1 ; // fire the tx interrupt
}

void __attribute__((__interrupt__,__no_auto_psv__)) _U2TXInterrupt(void)
{
	interrupt_save_set_corcon ;
	
	_U2TXIF = 0 ; // clear the interrupt 
	
	int txchar = uart2_callback_get_byte_to_send() ;

//	if (U2STAbits.TRMT == 0) {
//	}
	
	if ( txchar != -1 )
	{
		U2TXREG = (unsigned char)txchar ;
	} else {
		// Disable the TX interrupt if we are done so that we don't keep entering this ISR
		_U2TXIE = 0;
	}
	
	interrupt_restore_corcon ;
}

void __attribute__((__interrupt__, __no_auto_psv__)) _U2RXInterrupt(void)
{
	interrupt_save_set_corcon ;
	
	while ( U2STAbits.URXDA )
	{
		unsigned char rxchar = U2RXREG ;
		uart2_callback_received_byte(rxchar) ;
	}

	U2STAbits.OERR = 0 ;
	
	_U2RXIF = 0 ; // clear the interrupt

	interrupt_restore_corcon ;
	return ;
}

unsigned int write(int handle, void *buffer, unsigned int len)
{
    int i;

    for (i = len; i; --i) {
		uart2_putc(*(char*)buffer++);
    }
    return(len);
}

#endif //  USE_DEBUG_U2

#endif // 0
