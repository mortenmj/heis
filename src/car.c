#include <stdio.h>

#include <libheis/elev.h>

#include "car.h"

void (*const state_table [MAX_STATES][MAX_EVENTS]) (void) = {

      /* NOEVENT      START_UP     START_DOWN        STOP_NORMAL            STOP_EMERGENCY */
    { action_dummy, action_dummy, action_dummy, action_up_stop_normal, action_up_stop_emergency },                                 /* events for state UP */
    { action_dummy, action_dummy, action_dummy, action_down_stop_normal, action_down_stop_emergency },                             /* events for state DOWN */
    { action_dummy, action_normal_stop_start_up, action_normal_stop_start_down, action_dummy, action_normal_stop_stop_emergency }, /* events for state STOP_NORMAL */
    { action_dummy, action_dummy, action_dummy, action_emergency_stop_stop_normal, action_dummy }                                  /* events for state STOP_EMERGENCY */
};

int last_floor;

/* Dummy action. Does nothing */
void action_dummy (void) {
}

/* From the UP state, perform a normal stop */
void action_up_stop_normal (void) {
    printf("Moving up: Performing normal stop\n");
    elev_set_speed (0);
    elev_set_button_lamp(BUTTON_COMMAND, elev_get_floor_sensor_signal(), 0);

    current_state = NORMAL_STOP;
}

/* From the UP state, perform an emergency stop */
void action_up_stop_emergency (void) {
    printf("Moving up: Performing emergency stop\n");
    elev_set_speed (0);

    current_state = EMERGENCY_STOP;
}

/* From the DOWN state, perform a normal stop */
void action_down_stop_normal (void) {
    printf("Moving down: Performing normal stop\n");
    elev_set_speed (0);
    elev_set_button_lamp(BUTTON_COMMAND, elev_get_floor_sensor_signal(), 0);

    current_state = NORMAL_STOP;
}

/* From the DOWN state, perform an emergency stop */
void action_down_stop_emergency (void) {
    printf("Moving down: Performing emergency stop\n");
    elev_set_speed (0);

    current_state = EMERGENCY_STOP;
}

/* From the NORMAL_STOP state, start moving up */
void action_normal_stop_start_up(void) {
    printf("In normal stop: Moving up\n");
    elev_set_speed (300);

    current_state = UP;
}

/* From the NORMAL_STOP state, start moving down */
void action_normal_stop_start_down (void) {
    printf("In normal stop: Moving down\n");
    elev_set_speed (-300);

    current_state = DOWN;
}

/* From the NORMAL_STOP state, go to the EMERGENCY_STOP state */
void action_normal_stop_stop_emergency (void) {
    printf("In normal stop: Performing emergency stop\n");
    current_state = EMERGENCY_STOP;
}

/* From the EMERGENCY_STOP state, go to the NORMAL_STOP state */
void action_emergency_stop_stop_normal (void) {
    printf("In emergency stop: Performing normal stop\n");
    elev_set_speed (0);

    current_state = NORMAL_STOP;
}

enum events get_new_event (void)
{
  int floor = elev_get_floor_sensor_signal();

  last_floor = floor;
}

void motor_init (void) {
    if (elev_get_floor_sensor_signal() == -1) {
      elev_set_speed(-300);
      while (elev_get_floor_sensor_signal() == -1);
    }

    elev_set_speed(0);
    current_state = NORMAL_STOP;
}
    
/* Update the state machine */
void motor_update (void) {
    new_event = get_new_event ();
    if (((new_event >= 0) && (new_event <= MAX_EVENTS)) && ((current_state >= 0) && (current_state <= MAX_STATES))) 
        state_table [current_state][new_event] ();
}
