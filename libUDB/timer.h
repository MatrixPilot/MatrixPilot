/* 
 * File:   timer.h
 * Author: Matt
 *
 * Created on den 5 november 2013, 05:56
 */

#ifndef TIMER_H
#define	TIMER_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "events.h"

#define TIMER_MAX_TIMERS        10
#define TIMER_INVALID_HANDLE    0xFFFF
#define TIMER_MAX_TIME          0x7FFE

typedef enum tagTIMER_STATUS
{
    TIMER_STATUS_UNASSIGNED = 0,
    TIMER_STATUS_STOPPED,
    TIMER_STATUS_RUNNING,
} TIMER_STATUS;

    void timer_init(void);

    // Register a new timer.  Returns a handle to the new timer.
    uint16_t timer_register(void);

    // unregister a timer
    uint16_t timer_unregister(uint16_t handle);

    // poll software timers
    inline void timer_tick(void);

    void timer_start(uint16_t timer_handle, uint16_t time, boolean repeat, uint16_t event_handle);
    void timer_stop(uint16_t timer_handle);


#ifdef	__cplusplus
}
#endif

#endif	/* TIMER_H */

