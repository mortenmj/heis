#ifndef __INCLUDE_CAR_H__
#define __INCLUDE_CAR_H__

#define SPEED_UP 150
#define SPEED_DOWN -150
#define SPEED_HALT 0

typedef enum {
    CAR_NOEVENT,
    CAR_START_UP,
    CAR_START_DOWN,
    CAR_HALT,
    CAR_STOP,
    N_CAR_EVENTS
} car_event_t;

typedef enum {
    CAR_IDLE,
    CAR_MOVING_UP,
    CAR_MOVING_DOWN,
    CAR_STOPPED,
    N_CAR_STATES
} car_state_t;

void car_init (void);
void car_update_state (void);
car_state_t car_get_current_state (void);

#endif /* __INCLUDE_CAR_H__ */
