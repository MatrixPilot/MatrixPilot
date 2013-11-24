/* 
 * File:   multiEvent.h
 * Author: Matt
 *
 * Created on den 24 november 2013, 19:32
 */

#ifndef MULTIEVENT_H
#define	MULTIEVENT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MULTI_EVENTS_MAX            10
#define MAX_USERS_IN_MULTI_EVENT    10

#define MULTI_CALLBACKS_MAX         10
#define MAX_USERS_IN_MULTI_CALLBACK 10

#define INVALID_MULTI_HANDLE        0

    // Initialise multi events
    void init_multi_events(void);

    // regsister multi event master
    // unique_id = unique identifier
    // pevents = an array of event handles to trigger on this event
    // max_events = the size of events array
    // returns handle to multi event
    uint16_t register_multi_event_master(uint32_t unique_id);

    // Register multi event user
    // unique_id = unique identifier matching the master
    // event_handle = The event handle to trigger on this event
    void register_multi_event_user(uint32_t unique_id, uint16_t event_handle);

    // Trigger the multi event with its handle
    // Triggers all of the registered users
    void multi_event_trigger(uint16_t multi_event_handle);


    // Initialise multi callbacks
    void init_multi_callbacks(void);

    // regsister multi event master
    // unique_id = unique identifier
    // pevents = an array of event handles to trigger on this event
    // max_events = the size of events array
    // returns handle to multi event
    uint16_t register_multi_callback_master(uint32_t unique_id);

    // Register multi event user
    // unique_id = unique identifier matching the master
    // event_handle = The event handle to trigger on this event
    void register_multi_callback_user(uint32_t unique_id, uint16_t event_handle);

    // Call the multi callback with its handle
    // Directly calls all of the callback functions
    void multi_callback_trigger(uint16_t multi_callback_handle);


#ifdef	__cplusplus
}
#endif

#endif	/* MULTIEVENT_H */

