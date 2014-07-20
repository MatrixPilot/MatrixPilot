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


#include "defines.h"
#include "ring_buf.h"

unsigned int modulo_inc(const unsigned int value, const unsigned int modulus)
{
	unsigned int my_value = value + 1;
	if (my_value >= modulus)
	{
		my_value  = 0;
	}
	return my_value;
}

unsigned int modulo_dec(const unsigned int value, const unsigned int modulus)
{
	unsigned int my_value = (0==value) ? (modulus - 1) : (value - 1);
	return my_value;
}

void ring_buf_init(ring_buf_t* _this)
{
	/*****
		The following clears:
			-> buf
			-> head
			-> tail
			-> count
		and sets head = tail
	***/
	memset(_this, 0, sizeof(*_this));
}

int ring_buf_empty(ring_buf_t* _this)
{
	return (0 == _this->count);
}

int ring_buf_full(ring_buf_t* _this)
{
	return (_this->count >= RBUF_SIZE);
}

int ring_buf_get(ring_buf_t* _this)
{
	int c;
	if (_this->count > 0)
	{
		c = _this->buf[_this->tail];
		_this->tail = modulo_inc(_this->tail, RBUF_SIZE);
		--_this->count;
	}
	else
	{
		c = -1;
	}
	return c;
}

void ring_buf_put(ring_buf_t* _this, const unsigned char c)
{
	if (_this->count < RBUF_SIZE)
	{
		_this->buf[_this->head] = c;
		_this->head = modulo_inc(_this->head, RBUF_SIZE);
		++_this->count;
	}
}

void ring_buf_flush(ring_buf_t* _this, const int clearBuffer)
{
	_this->count = 0;
	_this->head = 0;
	_this->tail = 0;
	if (clearBuffer)
	{
		memset(_this->buf, 0, sizeof(_this->buf));
	}
}

