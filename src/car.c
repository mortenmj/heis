#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <libheis/elev.h>

#include "car.h"
#include "ui.h"

extern int orders[N_ORDER_TYPES][N_FLOORS];
extern int stop;

int last_floor;
time_t wait_until;
state_t current_state, last_state;
event_t new_event;

static void stop_motor () {
    DEBUG_PRINT("Stopping motor\n");
    elev_set_speed (SPEED_HALT);
}

static void halt() {
    int floor = elev_get_floor_sensor_signal();

    DEBUG_PRINT("Halting\n");

    wait_until = time(NULL) + HALT_PAUSE;
    elev_set_door_open_lamp(1);

    elev_set_floor_indicator(floor);
    stop_motor();
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

static direction_t get_opposite_direction () {
    return get_current_direction() == UP ? DOWN : UP;
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

    halt();

    ui_remove_order(ORDER_CAR, floor);
    /* If there is no order up, we expedite any order up */
    if (ui_check_order(ORDER_UP, floor)) {
      ui_remove_order(ORDER_UP, floor);
      } else {
        ui_remove_order(ORDER_DOWN, floor);
      }

    last_state = current_state;
    current_state = IDLE;
  }

/* From MOVING_UP, go to STOPPED */
static void action_moving_up_stop (void) {
    stop_motor();

    last_state = current_state;
    current_state = STOPPED;
}

/* From MOVING_DOWN, go to IDLE */
static void action_moving_down_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    halt();

    ui_remove_order(ORDER_CAR, floor);
    /* If there is no order down, we expedite any order up */
    if (ui_check_order(ORDER_DOWN, floor)) {
      ui_remove_order(ORDER_DOWN, floor);
      } else {
        ui_remove_order(ORDER_UP, floor);
      }

    last_state = current_state;
    current_state = IDLE;
}

/* From DOWN, go to STOPPED*/
static void action_moving_down_stop (void) {
    stop_motor();

    last_state = current_state;
    current_state = STOPPED;
}

/* From IDLE, go to MOVING_UP */
static void action_idle_start_up(void) {
    elev_set_door_open_lamp(0);
    elev_set_speed (SPEED_UP);

    last_state = current_state;
    current_state = MOVING_UP;
}

/* From IDLE, go to MOVING_DOWN */
static void action_idle_start_down (void) {
    elev_set_door_open_lamp(0);
    elev_set_speed (SPEED_DOWN);

    last_state = current_state;
    current_state = MOVING_DOWN;
}

/* From IDLE, return to IDLE */
static void action_idle_halt (void) {
    int floor = elev_get_floor_sensor_signal();

    if (floor < 0)
        return;

    halt();

    ui_remove_order(ORDER_CAR, floor);

    if (ui_check_order(get_current_direction(), floor)) {
      ui_remove_order(get_current_direction(), floor);
      } else {
        ui_remove_order(get_opposite_direction(), floor);
      }
}

/* From IDLE, go to STOPPED */
static void action_idle_stop (void) {
    last_state = current_state;
    current_state = STOPPED;
}

/* From STOPPED, go to IDLE */
static void action_stopped_halt (void) {
    stop_motor();

    last_state = current_state;
    current_state = IDLE;
}

/* From STOPPED, go to MOVING_UP */
static void action_stopped_start_up (void) {
    elev_set_door_open_lamp(0);
    elev_set_speed (SPEED_UP);

    stop = 0;

    last_state = current_state;
    current_state = MOVING_UP;
}

/* From STOPPED, go to MOVING_DOWN */
static void action_stopped_start_down(void) {
    elev_set_door_open_lamp(0);
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
  direction_t opp_dir = get_opposite_direction();

  /* Check the stop button */
  if (stop) {
      DEBUG_PRINT("Stop button pressed; stopping\n");
      ui_clear_orders();
      return STOP;
  }

  /* At a floor */
  if (floor != -1) {
      if (time(NULL) < wait_until) {
          DEBUG_PRINT("Waiting at floor.\n");
          return NOEVENT;
      }

      /* WORKS */
      /* Halt if the elevator has been ordered to the current floor*/
      order = ui_check_order(ORDER_CAR, floor);
      if (order) {
          DEBUG_PRINT("The elevator has been ordered to this floor. Halting.\n");
          return HALT;
      }

      /* WORKS */
      /* Halt if someone wants to go the same direction we're going */
      order = ui_check_order((order_type_t)dir, floor);
      if (order) {
          DEBUG_PRINT("Someone wants to go the same way the elevator is moving. Halting.\n");
          return HALT;
      }

      /* WORKS */
      /* If there are no orders further in our current direction */
      /* Halt if someone wants to go the opposite direction we're going */
      order = ui_check_order(opp_dir, floor);
      if (order && (ui_get_nearest_order_in_direction(ORDER_CAR, dir, floor) != -1) && (ui_get_nearest_order_in_direction((order_type_t)dir, dir, floor) != -1)) {
          DEBUG_PRINT("Someone wants to go the opposite way the elevator is moving, but we have no other orders. Halting.\n");
          return HALT;
      }

      /* WORKS */
      /* TODO: Only go to a floor if it's in the direction we're moving */
      /* See if anyone in the elevator has selected a floor */
      next_floor = ui_get_nearest_order(ORDER_CAR, floor);
      if (next_floor != -1) {
        if (next_floor > floor) {
            return START_UP;
        } else {
            return START_DOWN;
        }
      }

      /* See if someone wants to go up */
      next_floor = ui_get_nearest_order(ORDER_UP, floor);
      if (next_floor != -1) {
        DEBUG_PRINT(("Up order found at floor %i, current floor is %i\n", next_floor, floor));
        if (next_floor == floor) { 
            return HALT;
        } else if (next_floor > floor) {
            return START_UP;
        } else {
            return START_DOWN;
        }
      }

      /* See if someone wants to go down */
      next_floor = ui_get_nearest_order(ORDER_DOWN, floor);
      if (next_floor != -1) {
        DEBUG_PRINT(("Down order found at floor %i, current floor is %i\n", next_floor, floor));
        if (next_floor == floor) {
            return HALT;
        } else if  (next_floor > floor) {
            return START_UP;
        } else {
            return START_DOWN;
        }
      }
  }

  /* Nothing to do */
  return NOEVENT;
}

void car_init (void) {
    DEBUG_PRINT("Initializing\n");
    if (elev_get_floor_sensor_signal() == -1) {
      elev_set_speed (SPEED_DOWN);
      
      /* Loop until we reach a floor */
      while (elev_get_floor_sensor_signal() == -1);
    }

    stop_motor();
    last_state = MOVING_DOWN;
    current_state = IDLE;
    DEBUG_PRINT("Initialization complete\n");
}
  
/* Update the state machine */
void car_update_state (void) {
    new_event = get_new_event ();
    if (((new_event >= 0) && (new_event <= N_EVENTS)) && ((current_state >= 0) && (current_state <= N_STATES))) {
        DEBUG_PRINT(("New event: %i\n", new_event));
        state_table [current_state][new_event] ();
    } else {
        /* invalid */
    }
}
