#include "libUDB_internal.h"
#include "uart1.h"

#if 0
#ifdef USE_DEBUG_IO

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
	
	U1BRG = (int16_t)(FCY / (16*baud) - 1);
 */

#define FREQOSC         32000000
#define CLK_PHASES      2
#define UART_BAUD(x)    ((int16_t)((FREQOSC / CLK_PHASES) / ((int32_t)16 * x) - 1))


extern int16_t __C30_UART;

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


int16_t uart1_getc(void)
{
	int16_t result;

	_U1RXIE = 0;
	if (RX1TailPtr != RX1HeadPtr) {
		result = (uint8_t)*RX1TailPtr++;
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
		udb_led_toggle(LED_BLUE);
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


uint16_t write(int16_t handle, void *buffer, uint16_t len)
{
	int16_t i;

	for (i = len; i; --i) {
		uart1_putc(*(char*)buffer++);
	}
	return(len);
}


void udb_gps_callback_received_byte(char ch)
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


int16_t udb_gps_callback_get_byte_to_send(void)
{
	int16_t result;

	if (TX1TailPtr != TX1HeadPtr) {
		result = (uint8_t)*TX1TailPtr++;
		if (TX1TailPtr >= vUART1TXFIFO + sizeof(vUART1TXFIFO)) {
			TX1TailPtr = vUART1TXFIFO;
		}
	} else {
		result = -1;
	}
	return result;
}


#endif //  USE_DEBUG_IO
#endif // 0
