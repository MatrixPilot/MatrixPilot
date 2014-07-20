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

#ifndef RING_BUF_H
#define RING_BUF_H


#define RBUF_SIZE 16

typedef struct ring_buf_t
{
	unsigned char buf[RBUF_SIZE];
	int head;
	int tail;
	int count;
} ring_buf_t;

void ring_buf_init(ring_buf_t* _this);
int  ring_buf_get(ring_buf_t* _this);
void ring_buf_put(ring_buf_t* _this, const unsigned char c);
int  ring_buf_full(ring_buf_t* _this);
int  ring_buf_empty(ring_buf_t* _this);
void ring_buf_flush(ring_buf_t* _this, const int clearBuffer);


#endif // RING_BUF_H
