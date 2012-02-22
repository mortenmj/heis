#ifndef __INCLUDE_MOTOR_H__
#define __INCLUDE_MOTOR_H__

#define SPEED_UP 300
#define SPEED_DOWN -300
#define SPEED_HALT 0

enum states {UP, DOWN, NORMAL_STOP, EMERGENCY_STOP, MAX_STATES} current_state;
enum events {NOEVENT, START_UP, START_DOWN, STOP_NORMAL, STOP_EMERGENCY, MAX_EVENTS} new_event;

void car_init (void);
void car_update (void);

void action_down_stop_normal (void);
void action_down_stop_emergency (void);

void action_up_stop_normal (void);
void action_up_stop_emergency (void);

void action_normal_stop_start_up (void);
void action_normal_stop_start_down (void);
void action_normal_stop_stop_emergency (void);

void action_emergency_stop_stop_normal (void);

void action_dummy (void);

enum events get_new_event (void);

#endif /* __INCLUDE_MOTOR_H__ */
