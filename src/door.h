#ifndef __INCLUDE_DOOR_H__
#define __INCLUDE_DOOR_H__

#include <stdbool.h>

typedef enum {
    DOOR_NOEVENT,
    DOOR_OPEN,
    DOOR_CLOSE,
    N_DOOR_EVENTS
} door_event_t;

typedef enum {
    DOOR_OPENED,
    DOOR_CLOSED,
    N_DOOR_STATES
} door_state_t;

void door_init (void);
bool  door_closed (void);
void door_set_timer (void);
void door_update_state (void);

#endif /* __INCLUDE_DOOR_H__ */
