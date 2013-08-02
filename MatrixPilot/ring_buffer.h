/* 
 * File:   ring_buffer.h
 * Author: markw
 *
 * Created on August 2, 2013, 8:12 AM
 */

#ifndef RING_BUFFER_H
#define	RING_BUFFER_H

#include <stdbool.h>

bool ring_get(char* b);
int16_t ring_put(char b);
int16_t ring_putn(const char* b, int16_t n);
int16_t ring_available();
int16_t ring_space();
void queue_data(const char* buff, int16_t nbytes);
void queue_string(const char* string);
void queue_prepend(const char* buff, int16_t nbytes);

#endif	/* RING_BUFFER_H */

