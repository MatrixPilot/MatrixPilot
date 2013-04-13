#ifndef __FSIO_DBG_H__


#include "FSIO.h"

void log_init(void);
void log_test(void);
void log_trig(void);

void DisplayFS(void);
int fs_log(char* str);
int fs_nextlog(char* filename);
int fs_openlog(char* filename);
int fs_closelog(void);
int fs_showconfig(FSFILE * file);
int fs_openconfig(char* filename);
int TestFS(void);


#endif // __FSIO_DBG_H__
