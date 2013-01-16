#ifndef DEBUG_H
#define DEBUG_H


#include <stdio.h>


#define USE_DEBUG_IO
//#define USE_DEBUG_U1
#define USE_DEBUG_U2

#ifdef USE_DEBUG_IO
#define DPRINTF printf
#else
#define DPRINTF(args...)
#endif

//	DPRINT( "barom %.1f, %.2f, %.2f, slp %.2f\r\n", (double)temperature / 10.0, (double)pressure / 100.0, (double)altitude, (double)sea_level_pressure / 100.0);

extern int freq_mc;
extern int freq_adc;
extern int freq_rmat;
extern int freq_2hz;
extern int freq_8hz;
extern int freq_task;

extern int freq;
extern int count;
extern int one_hertz;
extern int two_hertz;
extern int one_hertz_2;
extern int two_hertz_2;
extern int forty_hertz;


void print_pid_gains(void);


#endif // DEBUG_H
