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


#include "libUDB_internal.h"
//#include "oscillator.h"
#include "interrupt.h"
#include "sio.h"

/*
//
// USAGE:
//
// SIO_DEFINE(UART)
// sio_init(rx_callback, RX_INT_PRI, tx_callback, TX_INT_PRI)
//
//

SIO_DEFINE(1)

void rx_callback(char data)
{
}

char tx_callback(void)
{
	return 0;
}

void foo(void)
{
//	sio_init(rx_callback, INT_PRI_U1RX, tx_callback, INT_PRI_U1RX);
}

//SIO_DEFINE(1, rx_callback, INT_PRI_U1RX, tx_callback, INT_PRI_U1RX)

*/
