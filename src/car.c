#include <stdio.h>
#include <unistd.h>
#include <libheis/elev.h>

#include "car.h"
#include "ui.h"

extern int orders[N_ORDER_TYPES][N_FLOORS];
extern int stop;

int last_floor;
state_t current_state, last_state;
event_t new_event;

static void smooth_stop () {
    elev_set_speed (SPEED_HALT);
    printf("smooth stop\n");
}

/* Direction is determined by the current state (if we're moving) or the last state (if not) */
static direction_t get_current_direction () {
    state_t state;
    if (current_state == 1 || current_state == 2) {
        state = current_state;
    } else {
        state = last_state;
    }

    switch (state) {
      case MOVING_UP:
        return UP;
      case MOVING_DOWN:
        return DOWN;
      default:
        return NONE;
    }
}

/* Dummy action. No state transition. */
static void action_dummy (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);
}

/* From MOVING_UP, go to IDLE */
static void action_moving_up_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving up: Halting\n");

    smooth_stop();

    ui_remove_order(ORDER_CAR, floor);
    if (floor < N_FLOORS-1)
      ui_remove_order(ORDER_UP, floor);

    last_state = current_state;
    current_state = IDLE;
  }

/* From MOVING_UP, go to STOPPED */
static void action_moving_up_stop (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving up: Stopping\n");
    elev_set_speed (SPEED_HALT);

    last_state = current_state;
    current_state = STOPPED;
}

/* From MOVING_DOWN, go to IDLE */
static void action_moving_down_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving down: Halting\n");

    smooth_stop();

    ui_remove_order(ORDER_CAR, floor);
    if (floor > 0)
      ui_remove_order(ORDER_DOWN, floor);

    last_state = current_state;
    current_state = IDLE;
}

/* From DOWN, go to STOPPED*/
static void action_moving_down_stop (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    elev_set_floor_indicator(floor);

    printf("Moving down: Stopping\n");
    elev_set_speed (SPEED_HALT);

    last_state = current_state;
    current_state = STOPPED;
}

/* From IDLE, go to MOVING_UP */
static void action_idle_start_up(void) {
    printf("In idle: Moving up\n");
    elev_set_speed (SPEED_UP);

    last_state = current_state;
    current_state = MOVING_UP;
}

/* From IDLE, go to MOVING_DOWN */
static void action_idle_start_down (void) {
    printf("In idle: Moving down\n");
    elev_set_speed (SPEED_DOWN);

    last_state = current_state;
    current_state = MOVING_DOWN;
}

/* From IDLE, return to IDLE */
static void action_idle_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    printf("In idle, halting\n");

    ui_remove_order(ORDER_CAR, floor);
}

/* From IDLE, go to STOPPED */
static void action_idle_stop (void) {
    printf("In idle: Stopping\n");

    last_state = current_state;
    current_state = STOPPED;
}

/* From STOPPED, go to IDLE */
static void action_stopped_halt (void) {
    printf("In stopped: Halting\n");
    smooth_stop();

    last_state = current_state;
    current_state = IDLE;
}

/* From STOPPED, go to MOVING_UP */
static void action_stopped_start_up (void) {
    printf("In stopped: Moving up\n");
    elev_set_speed (SPEED_UP);

    stop = 0;

    last_state = current_state;
    current_state = MOVING_UP;
}

/* From STOPPED, go to MOVING_DOWN */
static void action_stopped_start_down(void) {
    printf("In stopped: Moving down\n");
    elev_set_speed (SPEED_DOWN);

    stop = 0;

    last_state = current_state;
    current_state = MOVING_DOWN;
}

void (*const state_table [N_STATES][N_EVENTS]) (void) = {

      /* NOEVENT      START_UP     START_DOWN           HALT                   STOP */
    { action_dummy, action_idle_start_up, action_idle_start_down, action_idle_halt, action_idle_stop },                /* events for state IDLE */
    { action_dummy, action_dummy, action_dummy, action_moving_up_halt, action_moving_up_stop },                        /* events for state MOVING_UP */
    { action_dummy, action_dummy, action_dummy, action_moving_down_halt, action_moving_down_stop },                    /* events for state MOVING_DOWN */
    { action_dummy, action_stopped_start_up, action_stopped_start_down, action_stopped_halt, action_dummy }            /* events for state STOPPED */
};

event_t get_new_event (void)
{
  int next_floor;
  int order, nearest;
  int floor = elev_get_floor_sensor_signal();
  direction_t dir = get_current_direction();

  /* Check the stop button */
  /*
  if (stop) {
      printf("Stop button pressed; stopping\n");
      ui_clear_orders();
      return STOP;
  }
  */

  /* TODO: Always halt at the top and bottom floors */

  /* At a floor */
  if (floor != -1) {
      /* WORKS */
      /* See if the elevator has been ordered to the current floor*/
      order = ui_check_order(ORDER_CAR, floor);
      if (order) {
          printf("Someone inside the elevator wants out.\n");
          return HALT;
      }

      /* WORKS */
      /* See if someone wants on in the direction we're going */
      order = ui_check_order((order_type_t)dir, floor);
      printf("Direction is %i, checking if someone wants to go the same way\n", dir);
      if (order) {
          printf("Someone wants to go the way direction we're going\n");
          return HALT;
      }

      /* WORKS */
      /* See if anyone has selected a floor */
      next_floor = ui_get_nearest_order(ORDER_CAR, 0);
      if (next_floor != -1) {
        if (next_floor > floor) {
            printf("Someone in the elevator wants up\n");
            return START_UP;
        } else {
            printf("Someone in the elevator wants down\n");
            return START_DOWN;
        }
      }
  }

  /* Nothing to do */
  return NOEVENT;
}

void car_init (void) {
    printf("Initializing\n");
    if (elev_get_floor_sensor_signal() == -1) {
      elev_set_speed (SPEED_DOWN);
      
      /* Loop until we reach a floor */
      while (elev_get_floor_sensor_signal() == -1);
    }

    smooth_stop();
    last_state = IDLE;
    current_state = IDLE;
    printf("Initialized\n");
}
  
/* Update the state machine */
void car_update_state (void) {
    new_event = get_new_event ();
    if (((new_event >= 0) && (new_event <= N_EVENTS)) && ((current_state >= 0) && (current_state <= N_STATES))) {
        state_table [current_state][new_event] ();
        printf("Current state: %i, last state: %i\n", current_state, last_state);
    } else {
        /* invalid */
    }
}
