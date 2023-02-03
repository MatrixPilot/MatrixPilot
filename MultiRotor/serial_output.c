#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../libUDB/libUDB.h"
#include "../libUDB/oscillator.h"
#include "../libUDB/interrupt.h"
#include "../libUDB/serialIO.h"

#define SERIAL_BUFFER_SIZE	1536
#define NUM_CHUNKS_TO_BUFFER 16

// Set up two serial buffers, and swap back and forth between then as we buffer
// and send long messages in bursts, to create longer times between each burst
// of data for the ESP32 on the receiving end to have uninterrupted time to
// write the data to flash.

#ifdef USE_PACKETIZED_TELEMERTY
uint8_t serial_buffer[2][SERIAL_BUFFER_SIZE] ;
uint16_t end_index[2] = {0, 0};
uint8_t write_buffer_index = 0;
uint8_t read_buffer_index = 0;
uint16_t packet_data_start;
uint16_t packet_data_length;
uint8_t num_chunks_buffered = 0;
void finalize_packet();
#else
uint8_t serial_buffer[SERIAL_BUFFER_SIZE] ;
uint16_t end_index = 0;
#endif

uint16_t sb_index = 0 ;

static boolean udb_serial_stop_sending_flag = 0;

extern int16_callback_fptr_t serial_callback_get_byte_to_send ;
extern callback_uint8_fptr_t serial_callback_received_byte ;

int vsnprintf (char * s, size_t n, const char * format, va_list arg );


void __attribute__((__interrupt__, __no_auto_psv__)) _U2TXInterrupt(void)
{
	_U2TXIF = 0; // clear the interrupt
    // prevent losing a character when stop and start sending data
    if (U2STAbits.UTXBF == 0)
    {
        indicate_loading_inter;
        set_ipl_on_output_pin;
        interrupt_save_set_corcon;

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
    udb_serial_stop_sending_flag = true; 
}


#ifdef USE_PACKETIZED_TELEMERTY

// Super Simple Packetization protocol:
// 0xDED1            - Start a new file
// 0xDED3            - Close current file
// 0xDED2LLLLXXXX... - Send a packet of data 0xLLLL bytes long
//                     High length byte, low length byte, then N data bytes
// 
// We bundle up NUM_CHUNKS_TO_BUFFER messages into one packet, so that we're
// able to burst a bunch of data at once, leaving some time in between these
// large packets, since the ESP32 needs uninterrupted time to write this data
// to flash storage in between packets.

void serial_output(const char* format, ...)
{
	uint16_t start_index;
	uint16_t remaining;
    uint16_t wrote;
    va_list arglist;
    
    va_start(arglist, format);
    
	start_index = end_index[write_buffer_index];
	remaining = SERIAL_BUFFER_SIZE - start_index;

	if (remaining > 5)
	{
        if (num_chunks_buffered == 0) {
            serial_buffer[write_buffer_index][start_index++] = 0xDE;
            serial_buffer[write_buffer_index][start_index++] = 0xD2;
            serial_buffer[write_buffer_index][start_index++] = 0x00; // Save space for length bytes
            serial_buffer[write_buffer_index][start_index++] = 0x00;
            packet_data_start = start_index;
            packet_data_length = 0;
        }
        
		wrote = vsnprintf((char*)(&serial_buffer[write_buffer_index][start_index]), (size_t)remaining, format, arglist);
        
        packet_data_length += wrote;
		end_index[write_buffer_index] = start_index + wrote;
        num_chunks_buffered++;
        
        if (num_chunks_buffered >= NUM_CHUNKS_TO_BUFFER) {
            finalize_packet();
            read_buffer_index = write_buffer_index;
            write_buffer_index = !write_buffer_index;
            udb_serial_start_sending_data();
        }
	}
    
    va_end(arglist);
}

void finalize_packet()
{
    if (num_chunks_buffered && packet_data_start) {
        // Go back and write the length bytes
        uint8_t lenH = packet_data_length/256;
        uint8_t lenL = packet_data_length%256;
        serial_buffer[write_buffer_index][packet_data_start-2] = lenH;
        serial_buffer[write_buffer_index][packet_data_start-1] = lenL;
        num_chunks_buffered = 0;
    }
}

void serial_output_start_end_packet(boolean isStart)
{
    while (end_index[read_buffer_index]) ;
    
    finalize_packet();
    
	int16_t remaining = SERIAL_BUFFER_SIZE - end_index[write_buffer_index];
    if (remaining > 2) {
        serial_buffer[write_buffer_index][end_index[write_buffer_index]++] = 0xDE;
        serial_buffer[write_buffer_index][end_index[write_buffer_index]++] = (isStart) ? 0xD1 : 0xD3;
        read_buffer_index = write_buffer_index;
        write_buffer_index = !write_buffer_index;
        udb_serial_start_sending_data();
    }
}


int16_t udb_serial_callback_get_byte_to_send(void)
{
    if (sb_index < end_index[read_buffer_index]) {
    	return serial_buffer[read_buffer_index][ sb_index++ ];
	}
    else {
        sb_index = 0;
        end_index[read_buffer_index] = 0;
        udb_serial_stop_sending_data();
        return -1;
    }
}

#else

void serial_output(const char* format, ...)
{
	uint16_t start_index;
	uint16_t remaining;
    uint16_t wrote;
	va_list arglist;

	va_start(arglist, format);

	start_index = end_index;
	remaining = SERIAL_BUFFER_SIZE - start_index;

	if (remaining > 1)
	{
		udb_serial_stop_sending_data();
		wrote = vsnprintf((char*)(&serial_buffer[start_index]), (size_t)remaining, format, arglist);
		end_index = start_index + wrote;
		udb_serial_start_sending_data();
	}
	va_end(arglist);
}

void serial_output_start_end_packet(boolean isStart) {}

int16_t udb_serial_callback_get_byte_to_send(void)
{
	uint8_t txchar = serial_buffer[ sb_index++ ];
	if (txchar)
	{
		return txchar;
	}
	else
	{
		sb_index = 0;
		end_index = 0;
	}
	return -1;
}
#endif


// Don't respond to serial input
void udb_serial_callback_received_byte(uint8_t rxchar)
{
	// Do nothing
	return ;
}


