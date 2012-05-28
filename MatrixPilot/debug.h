#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdarg.h>

#define USE_DEBUG_IO

void udb_init_debug_io(void);
void dprintf(char* message, ...);


#endif // __DEBUG_H
