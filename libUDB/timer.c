#include "timer.h"

// If a repeating timer is running it must be stopped before restarting

typedef struct
{
    TIMER_STATUS  status;
    uint16_t    timeout;
    uint16_t    repeat_time;
    uint16_t    event_handle;
} TIMER_DATA;

TIMER_DATA timers[TIMER_MAX_TIMERS];

uint16_t timer_time = 0;

inline uint16_t get_sys_time(void) { return timer_time; };


void timer_init(void)
{
    uint16_t index;
    for(index = 0; index < TIMER_MAX_TIMERS; index ++)
    {
        timers[index].event_handle = INVALID_HANDLE;
        timers[index].timeout = 0;
        timers[index].status = TIMER_STATUS_UNASSIGNED;
    }
}

// Register a new timer.  Returns a handle to the new timer.
uint16_t timer_register(void)
{
    uint16_t index;
    for(index = 0; index < TIMER_MAX_TIMERS; index ++)
    {
        if(timers[index].status == TIMER_STATUS_UNASSIGNED)
        {
            timers[index].status = TIMER_STATUS_STOPPED;
            return index;
        }
    }
    return TIMER_INVALID_HANDLE;
};

// inline code polling timers
inline void timer_tick(void)
{
    uint16_t index;
    TIMER_DATA* ptimer;

    // Increment the timer
    timer_time++;

    for(index = 0; index < TIMER_MAX_TIMERS; index ++)
    {
        ptimer = &timers[index];
        switch(ptimer->status)            // Only service active timers
        {
            case TIMER_STATUS_UNASSIGNED:
            case TIMER_STATUS_STOPPED:
                break;
            case TIMER_STATUS_RUNNING:
                if( (timer_time - ptimer->timeout) < TIMER_MAX_TIME )
                {
                    if(timer_time >= ptimer->timeout)
                    {
                        trigger_event(ptimer->event_handle);

                        if(ptimer->repeat_time == 0)
                        {
                            ptimer->status = TIMER_STATUS_STOPPED;
                            ptimer->event_handle = INVALID_HANDLE;
                        }
                        else
                        {
                            ptimer->timeout = timer_time + ptimer->repeat_time;
                        }
                    }
                }
                break;
        }
    }
};

void timer_start(uint16_t timer_handle, uint16_t time, boolean repeat, uint16_t event_handle)
{
    TIMER_DATA* ptimer = &timers[timer_handle];
    if(ptimer->status != TIMER_STATUS_STOPPED) return;

    ptimer->timeout = time + timer_time;

    if(repeat == true)
        ptimer->repeat_time = time;
    else
        ptimer->repeat_time = 0;

    ptimer->event_handle = event_handle;
    ptimer->status = TIMER_STATUS_RUNNING;
}

// Maybe timer_stop is not thread safe

void timer_stop(uint16_t timer_handle)
{
    TIMER_DATA* ptimer = &timers[timer_handle];
    ptimer->status = TIMER_STATUS_STOPPED;
    ptimer->event_handle = INVALID_HANDLE;
}