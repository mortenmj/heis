#ifndef __INCLUDE_DOOR_H__
#define __INCLUDE_DOOR_H__

typedef enum door_events {
    DOOR_NOEVENT,
    DOOR_OPEN,
    DOOR_CLOSE,
    N_DOOR_EVENTS
} door_event_t;

typedef enum door_states {
    DOOR_OPENED,
    DOOR_CLOSED,
    N_DOOR_STATES
} door_state_t;

void door_init();
int  door_closed();
void door_set_timer();
void door_update_state();

#endif /* __INCLUDE_MOTOR_H__ */
