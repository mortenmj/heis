#ifndef __INCLUDE_MOTOR_H__
#define __INCLUDE_MOTOR_H__

#define SPEED_UP 150
#define SPEED_DOWN -150
#define SPEED_HALT 0
#define HALT_PAUSE 7500
#define STOP_PAUSE 0

typedef enum events {
    NOEVENT,
    START_UP,
    START_DOWN,
    HALT,
    STOP,
    N_EVENTS
} event_t;

typedef enum states {
    IDLE,
    MOVING_UP,
    MOVING_DOWN,
    STOPPED,
    N_STATES
} state_t;

void car_init (void);
void car_update_state (void);

void action_moving_up_halt (void);
void action_moving_up_stop (void);

void action_moving_down_halt (void);
void action_moving_down_stop (void);

void action_idle_start_up (void);
void action_idle_start_down (void);
void action_idle_stop (void);

void action_stopped_start_up (void);
void action_stopped_start_down (void);
void action_stopped_halt (void);

void action_dummy (void);

enum events get_new_event (void);

state_t car_get_current_state (void);

#endif /* __INCLUDE_MOTOR_H__ */
