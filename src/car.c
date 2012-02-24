#include <stdio.h>
#include <unistd.h>
#include <libheis/elev.h>

#include "car.h"
#include "ui.h"

extern int orders[N_ORDER_TYPES][N_FLOORS];
extern int stop;

void (*const state_table [N_STATES][N_EVENTS]) (void) = {

      /* NOEVENT      START_UP     START_DOWN           HALT                   STOP */
    { action_dummy, action_idle_start_up, action_idle_start_down, action_idle_halt, action_idle_stop },                /* events for state IDLE */
    { action_dummy, action_dummy, action_dummy, action_moving_up_halt, action_moving_up_stop },                        /* events for state MOVING_UP */
    { action_dummy, action_dummy, action_dummy, action_moving_down_halt, action_moving_down_stop },                    /* events for state MOVING_DOWN */
    { action_dummy, action_stopped_start_up, action_stopped_start_down, action_stopped_halt, action_dummy }            /* events for state STOPPED */
};

int last_floor;
state_t current_state, last_state;
event_t new_event;

static void smooth_stop () {
    printf("Smooth stop!\n");

    switch(current_state) {
      case MOVING_UP:
        //elev_set_speed (SPEED_DOWN);
        //usleep(HALT_PAUSE);
        elev_set_speed (SPEED_HALT);
        break;
      case MOVING_DOWN:
        //elev_set_speed (SPEED_UP);
        //usleep(HALT_PAUSE);
        elev_set_speed (SPEED_HALT);
        break;
    }
}

static direction_t get_current_direction () {
    switch (current_state) {
      case MOVING_UP:
        return UP;
      case MOVING_DOWN:
        return DOWN;
      default:
        return NONE;
    }
}

/* Dummy action. No state transition. */
void action_dummy (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);
}

/* From MOVING_UP, go to IDLE */
void action_moving_up_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving up: Halting\n");

    smooth_stop();

    ui_remove_order(ORDER_CAR, floor);
    if (floor < N_FLOORS-1)
      ui_remove_order(ORDER_UP, floor);

    current_state = IDLE;
  }

/* From MOVING_UP, go to STOPPED */
void action_moving_up_stop (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving up: Stopping\n");
    elev_set_speed (SPEED_HALT);

    current_state = STOPPED;
}

/* From MOVING_DOWN, go to IDLE */
void action_moving_down_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving down: Halting\n");

    smooth_stop();

    ui_remove_order(ORDER_CAR, floor);
    if (floor > 0)
      ui_remove_order(ORDER_DOWN, floor);

    current_state = IDLE;
}

/* From DOWN, go to STOPPED*/
void action_moving_down_stop (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving down: Stopping\n");
    elev_set_speed (SPEED_HALT);

    current_state = STOPPED;
}

/* From IDLE, go to MOVING_UP */
void action_idle_start_up(void) {
    printf("In idle: Moving up\n");
    elev_set_speed (SPEED_UP);

    current_state = MOVING_UP;
}

/* From IDLE, go to MOVING_DOWN */
void action_idle_start_down (void) {
    printf("In idle: Moving down\n");
    elev_set_speed (SPEED_DOWN);

    current_state = MOVING_DOWN;
}

/* From IDLE, return to IDLE */
void action_idle_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    printf("In idle, halting\n");

    ui_remove_order(ORDER_CAR, floor);

    current_state = HALT;
}

/* From IDLE, go to STOPPED */
void action_idle_stop (void) {
    printf("In idle: Stopping\n");
    current_state = STOPPED;
}

/* From STOPPED, go to IDLE */
void action_stopped_halt (void) {
    printf("In stopped: Halting\n");
    smooth_stop();

    current_state = IDLE;
}

/* From STOPPED, go to MOVING_UP */
void action_stopped_start_up (void) {
    printf("In stopped: Moving up\n");
    elev_set_speed (SPEED_UP);

    stop = 0;

    current_state = MOVING_UP;
}

/* From STOPPED, go to MOVING_DOWN */
void action_stopped_start_down(void) {
    printf("In stopped: Moving down\n");
    elev_set_speed (SPEED_DOWN);

    stop = 0;

    current_state = MOVING_DOWN;
}

event_t get_new_event (void)
{
  int floor = elev_get_floor_sensor_signal();
  int next_floor;

  /* Check the stop button */
  if (stop) {
      printf("Stop button pressed; stopping\n");
      ui_clear_orders();
      return STOP;
  }

  /* TODO: Always halt at the top and bottom floors */

  /* See if the elevator has been ordered to the current floor*/
  if (floor != -1) {
      if (ui_check_order(ORDER_CAR, floor)) {
          printf("Someone inside the elevator wants out. Halting\n");
          return HALT;
      }

      /* If someone wants on in the direction we're going, and there are no orders further up/down, we let them on */
      if (ui_check_order((order_type_t)get_current_direction(), floor) && ui_get_nearest_order((order_type_t)get_current_direction(), floor) == -1) {
          printf("No orders further in current direction (%i). Halting\n", get_current_direction());
          return HALT;
      } else {
          printf("There are people waiting at a later floor in the current direction (%i), expediting those first\n", get_current_direction());
      }

      /* TODO: Alternately, if there are no other orders in the current direction, we'll pick up someone going the other way */
  }

  /* See if anyone has selected a floor */
  next_floor = ui_get_nearest_order(ORDER_CAR, 0);

  if (next_floor != -1) {
    if (next_floor > floor) {
        return START_UP;
    } else {
        return START_DOWN;
    }
  }

  /* See if anyone has ordered the car */
  next_floor = ui_get_nearest_order(ORDER_UP, 0);
  if (next_floor != -1) {
      if (next_floor > floor) {
          return START_UP;
      } else {
          return START_DOWN;
      }
  }

  next_floor = ui_get_nearest_order(ORDER_DOWN, N_FLOORS-1);
  if (next_floor != -1) {
      if (next_floor > floor) {
          return START_UP;
      } else {
          return START_DOWN;
      }
  }

  /* Nothing to do */
  return NOEVENT;
}

void car_init (void) {
  if (elev_get_floor_sensor_signal() == -1) {
    elev_set_speed (SPEED_DOWN);
    
    /* Loop until we reach a floor */
    while (elev_get_floor_sensor_signal() == -1);
  }

  current_state = MOVING_DOWN;
  smooth_stop();
  current_state = IDLE;
}
  
/* Update the state machine */
void car_update_state (void) {
    new_event = get_new_event ();
    if (((new_event >= 0) && (new_event <= N_EVENTS)) && ((current_state >= 0) && (current_state <= N_STATES))) {
        state_table [current_state][new_event] ();
    } else {
        /* invalid */
    }
}
