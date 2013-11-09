/* 
 * File:   sysTick.h
 * Author: Matt
 *
 * Created on den 5 november 2013, 06:11
 */

#ifndef SYSTICK_H
#define	SYSTICK_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "libUDB_internal.h"

#define USE_SYS_TICK 1

    void sys_tick_init(void);
    inline uint16_t get_sys_time(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SYSTICK_H */

