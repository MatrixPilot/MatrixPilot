// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


typedef int16_t (*int16_callback_fptr_t)(void);
typedef void (*callback_uint8_fptr_t)(uint8_t);

//void udb_init_GPS(void);
void udb_init_GPS(int16_callback_fptr_t tx_fptr, callback_uint8_fptr_t rx_fptr);
void udb_gps_set_rate(int32_t rate);
boolean udb_gps_check_rate(int32_t rate);
void udb_gps_start_sending_data(void);
void udb_serial_stop_sending_data(void);

// Implement this callback to tell the UDB what byte is next to send on the GPS.
// Return -1 to stop sending data.
int16_t udb_gps_callback_get_byte_to_send(void);        // Callback

// Implement this callback to handle receiving a byte from the GPS
void udb_gps_callback_received_byte(uint8_t rxchar);    // Callback


void udb_init_USART(int16_callback_fptr_t tx_fptr, callback_uint8_fptr_t rx_fptr);
void udb_serial_set_rate(int32_t rate);
boolean udb_serial_check_rate(int32_t rate);
void udb_serial_start_sending_data(void);

// Implement this callback to tell the UDB what byte is next to send on the serial port.
// Return -1 to stop sending data.
int16_t udb_serial_callback_get_byte_to_send(void);     // Callback

// Implement this callback to handle receiving a byte from the serial port
void udb_serial_callback_received_byte(uint8_t rxchar); // Callback

