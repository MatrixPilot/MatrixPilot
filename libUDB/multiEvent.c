#include "multiEvent.h"
#include "events.h"

typedef struct tag_multi_event
{
    uint32_t    unique_id;
    uint16_t    events[MAX_USERS_IN_MULTI_EVENT];
} multi_event;

multi_event multi_events[MULTI_EVENTS_MAX];

typedef void (*multi_callback) (void);

typedef struct tag_multi_callback
{
    uint32_t            unique_id;
    multi_callback      callbacks[MAX_USERS_IN_MULTI_CALLBACK];
} multi_callback;

multi_callback multi_callbacks[MULTI_CALLBACKS_MAX];


// Initialise multi events
void init_multi_events(void)
{
    int8_t users_index;
    int16_t events_index;

    for(events_index = 0; events_index < MULTI_EVENTS_MAX; events_index++ )
    {
        multi_events[events_index].unique_id = INVALID_MULTI_HANDLE;
        for(users_index = 0; users_index < MAX_USERS_IN_MULTI_EVENT; users_index++ )
        {
            multi_events[events_index].events[users_index] = INVALID_EVENT_HANDLE;
        }
    }
}


// Initialise multi callbacks
void init_multi_callbacks(void)
{
    int8_t users_index;
    int16_t events_index;

    for(events_index = 0; events_index < MULTI_EVENTS_MAX; events_index++ )
    {
        multi_callbacks[events_index].unique_id = INVALID_MULTI_HANDLE;
        for(users_index = 0; users_index < MAX_USERS_IN_MULTI_CALLBACK; users_index++ )
        {
            multi_callbacks[events_index].callbacks[users_index] = NULL;
        }
    }
}

