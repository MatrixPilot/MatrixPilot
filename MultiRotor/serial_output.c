#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../libUDB/libUDB.h"
#include "../libUDB/oscillator.h"
#include "../libUDB/interrupt.h"
#include "../libUDB/serialIO.h"

#define SERIAL_BUFFER_SIZE	1024
char serial_buffer[SERIAL_BUFFER_SIZE] ;

static boolean udb_serial_stop_sending_flag = 0;

extern int16_callback_fptr_t serial_callback_get_byte_to_send ;
extern callback_uint8_fptr_t serial_callback_received_byte ;


void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void)
{
	_U2TXIF = 0; // clear the interrupt
    // prevent losing a character when stop and start sending data
    if (U2STAbits.UTXBF == 0)
    {
        indicate_loading_inter;
        set_ipl_on_output_pin;
        interrupt_save_set_corcon;

//	int16_t txchar = udb_serial_callback_get_byte_to_send();
        int16_t txchar = -1;
        if (serial_callback_get_byte_to_send && ! udb_serial_stop_sending_flag)
        {
            txchar = serial_callback_get_byte_to_send();
        }
        if (txchar != -1)
        {
            U2TXREG = (uint8_t)txchar;
        }
        interrupt_restore_corcon;
        unset_ipl_on_output_pin;
    }
}


void udb_serial_start_sending_data(void)
{
	 udb_serial_stop_sending_flag = false;
	_U2TXIF = 1; // fire the tx interrupt
}

void udb_serial_stop_sending_data(void)
{
	udb_serial_stop_sending_flag  = true; 
}

int vsnprintf (char * s, size_t n, const char * format, va_list arg );

int16_t end_index = 0 ;
void serial_output(const char* format, ...)
{
	int16_t start_index;
	int16_t remaining;
	va_list arglist;

	va_start(arglist, format);

	start_index = end_index;
	remaining = SERIAL_BUFFER_SIZE - start_index;

	if (remaining > 1)
	{
		udb_serial_stop_sending_data();
		int16_t wrote = vsnprintf((char*)(&serial_buffer[start_index]), (size_t)remaining, format, arglist);
		end_index = start_index + wrote;
		udb_serial_start_sending_data();
	}
	va_end(arglist);
}

