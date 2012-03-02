#include "config.h"

#include "car.h"

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include <libheis/elev.h>

#include "debug.h"
#include "door.h"
#include "safety.h"
#include "ui.h"

int last_floor;
car_state_t car_current_state, car_last_state;
car_event_t new_event;

/* PRIVATE FUNCTIONS */

/* FIXME */
static void
stop_motor (void)
{
    elev_set_speed (SPEED_HALT);
}


static void
stop_button_pressed (void)
{
    ui_clear_orders();
    stop_motor();
}


static void
halt_elevator (void)
{
    int floor = elev_get_floor_sensor_signal();

    DEBUG(("Halting\n"));

    door_set_timer();

    elev_set_floor_indicator(floor);
    stop_motor();
}


/* Direction is determined by the current state if we're moving, or the last state if not */
static direction_t
get_current_direction (void)
{
    car_state_t state;
    if (car_current_state == 1 || car_current_state == 2) {
        state = car_current_state;
    } else {
        state = car_last_state;
    }

    switch (state) {
      case CAR_MOVING_UP:
        return UP;
      case CAR_MOVING_DOWN:
        return DOWN;
      default:
        return NONE;
    }
}


static direction_t
get_opposite_direction (void)
{
    return get_current_direction() == UP ? DOWN : UP;
}


/* Dummy action. No state transition. */
static void
action_dummy (void)
{
    int floor = elev_get_floor_sensor_signal();

    if (floor != -1)
      elev_set_floor_indicator(floor);
}


/* From CAR_MOVING_UP, go to CAR_IDLE */
static void
action_moving_up_halt (void)
{
    DEBUG(("action_moving_up_halt\n"));
    int floor = elev_get_floor_sensor_signal();

    halt_elevator();

    ui_remove_order(ORDER_CAR, floor);
    /* If there is no order up, we expedite any order up */
    if (ui_check_order(ORDER_UP, floor)) {
      ui_remove_order(ORDER_UP, floor);
      } else {
        ui_remove_order(ORDER_DOWN, floor);
      }

    car_last_state = car_current_state;
    car_current_state = CAR_IDLE;
  }


/* From CAR_MOVING_UP, go to CAR_STOPPED */
static void
action_moving_up_stop (void)
{
    stop_button_pressed();
    DEBUG(("action_moving_up_stop\n"));

    car_last_state = car_current_state;
    car_current_state = CAR_STOPPED;
}


/* From CAR_MOVING_DOWN, go to CAR_IDLE */
static void
action_moving_down_halt (void)
{
    int floor = elev_get_floor_sensor_signal();

    DEBUG(("action_moving_down_halt\n"));
    halt_elevator();

    ui_remove_order(ORDER_CAR, floor);
    /* If there is no order down, we expedite any order up */
    if (ui_check_order(ORDER_DOWN, floor)) {
      ui_remove_order(ORDER_DOWN, floor);
      } else {
        ui_remove_order(ORDER_UP, floor);
      }

    car_last_state = car_current_state;
    car_current_state = CAR_IDLE;
}


/* From DOWN, go to CAR_STOPPED*/
static void
action_moving_down_stop (void)
{
    DEBUG(("action_moving_down_stop\n"));
    stop_button_pressed();

    car_last_state = car_current_state;
    car_current_state = CAR_STOPPED;
}


/* From CAR_IDLE, go to CAR_MOVING_UP */
static void
action_idle_start_up(void)
{
    DEBUG(("action_idle_start_up\n"));
    elev_set_speed (SPEED_UP);

    car_last_state = car_current_state;
    car_current_state = CAR_MOVING_UP;
}


/* From CAR_IDLE, go to CAR_MOVING_DOWN */
static void
action_idle_start_down (void)
{
    DEBUG(("action_idle_start_down\n"));
    elev_set_speed (SPEED_DOWN);

    car_last_state = car_current_state;
    car_current_state = CAR_MOVING_DOWN;
}


/* From CAR_IDLE, return to CAR_IDLE */
static void
action_idle_halt (void)
{
    int floor = elev_get_floor_sensor_signal();

    DEBUG(("action_idle_halt\n"));

    if (ui_check_order(ORDER_CAR, floor)) {
        ui_remove_order(ORDER_CAR, floor);
        printf("order status %i\n", ui_check_order(ORDER_CAR, floor));
        halt_elevator();
        return;
    }

    if (ui_check_order(get_current_direction(), floor)) {
        ui_remove_order(get_current_direction(), floor);
        halt_elevator();
    } else {
        ui_remove_order(get_opposite_direction(), floor);
        halt_elevator();
    }
}


/* From CAR_IDLE, go to CAR_STOPPED */
static void
action_idle_stop (void)
{
    DEBUG(("action_idle_stop\n"));
    stop_button_pressed();

    car_last_state = car_current_state;
    car_current_state = CAR_STOPPED;
}


/* From CAR_STOPPED, go to CAR_IDLE */
static void
action_stopped_halt (void)
{
    DEBUG(("action_stopped_halt\n"));
    stop_motor();

    car_last_state = car_current_state;
    car_current_state = CAR_IDLE;
}


/* From CAR_STOPPED, go to CAR_MOVING_UP */
static void
action_stopped_start_up (void)
{
    DEBUG(("action_stopped_start_up\n"));
    elev_set_speed (SPEED_UP);

    car_last_state = car_current_state;
    car_current_state = CAR_MOVING_UP;
}


/* From CAR_STOPPED, go to CAR_MOVING_DOWN */
static void
action_stopped_start_down(void)
{
    DEBUG(("action_stopped_start_down\n"));
    elev_set_speed (SPEED_DOWN);

    car_last_state = car_current_state;
    car_current_state = CAR_MOVING_DOWN;
}


/* From CAR_STOPPED, go to CAR_STOPPED */
static void
action_stopped_stop(void)
{
    DEBUG(("action_stopped_start_stop\n"));
    /* If there are car orders, we should reset the stop button */
    if (ui_get_nearest_order(ORDER_CAR, 0) != -1) {
        safety_reset();
    }
}


void (*const car_state_table [N_CAR_STATES][N_CAR_EVENTS]) (void) = {

      /* CAR_NOEVENT      CAR_START_UP     CAR_START_DOWN           CAR_HALT                   CAR_STOP */
    { action_dummy, action_idle_start_up, action_idle_start_down, action_idle_halt, action_idle_stop },                /* events for state CAR_IDLE */
    { action_dummy, action_dummy, action_dummy, action_moving_up_halt, action_moving_up_stop },                        /* events for state CAR_MOVING_UP */
    { action_dummy, action_dummy, action_dummy, action_moving_down_halt, action_moving_down_stop },                    /* events for state CAR_MOVING_DOWN */
    { action_dummy, action_stopped_start_up, action_stopped_start_down, action_stopped_halt, action_stopped_stop }     /* events for state CAR_STOPPED */
};
  

/* Return the next event to process */
static car_event_t
get_new_event (void)
{
  int next_floor;
  int order, nearest;
  int floor = elev_get_floor_sensor_signal();
  direction_t dir = get_current_direction();
  direction_t opp_dir = get_opposite_direction();

  /* Check the stop button */
  if (safety_get_state()) {
      return CAR_STOP;
  }

  /* Check that the doors are closed */
  if (!door_closed()) {
      return CAR_NOEVENT;
  }

  if (car_current_state == CAR_STOPPED && !safety_get_state()) {
      next_floor = ui_get_nearest_order(ORDER_CAR, last_floor);
      if (next_floor != -1) {
        if (next_floor > last_floor) {
            return CAR_START_UP;
        } else if (next_floor < last_floor) {
            return CAR_START_DOWN;
        } else if (next_floor == last_floor) {
            /* We're above the last floor */
            if (get_current_direction() == UP) {
                return CAR_START_DOWN;
            } else if (get_current_direction() == DOWN) {
                return CAR_START_UP;
            } else {
                DEBUG(("Something went wrong resuming from stop. Falling back to reinitializing the elevator\n"));
                car_init();
            }
        }
      }
  }

  /* At a floor */
  if (floor != -1) {
      last_floor = floor;

      /* WORKS */
      /* Halt if the elevator has been ordered to the current floor*/
      order = ui_check_order(ORDER_CAR, floor);
      if (order) {
          DEBUG(("The elevator has been ordered to this floor. Halting.\n"));
          return CAR_HALT;
      }

      /* WORKS */
      /* Halt if someone wants to go the same direction we're going */
      order = ui_check_order((order_type_t)dir, floor);
      if (order) {
          DEBUG(("Someone wants to go the same way the elevator is moving. Halting.\n"));
          return CAR_HALT;
      }

      /* WORKS */
      /* If there are no orders further in our current direction */
      /* Halt if someone wants to go the opposite direction we're going */
      order = ui_check_order(opp_dir, floor);
      if (order && (ui_get_nearest_order_in_direction(ORDER_CAR, dir, floor) != -1) && (ui_get_nearest_order_in_direction((order_type_t)dir, dir, floor) != -1)) {
          DEBUG(("Someone wants to go the opposite way the elevator is moving, and we have no other orders. Halting.\n"));
          return CAR_HALT;
      }

      /* WORKS */
      /* TODO: Only go to a floor if it's in the direction we're moving */
      /* See if anyone in the elevator has selected a floor */
      next_floor = ui_get_nearest_order(ORDER_CAR, floor);
      if (next_floor != -1) {
        if (next_floor > floor) {
            return CAR_START_UP;
        } else {
            return CAR_START_DOWN;
        }
      }

      /* See if someone wants to go up */
      next_floor = ui_get_nearest_order(ORDER_UP, floor);
      if (next_floor != -1) {
        DEBUG(("Up order found at floor %i, current floor is %i\n", next_floor, floor));
        if (next_floor == floor) { 
            return CAR_HALT;
        } else if (next_floor > floor) {
            return CAR_START_UP;
        } else {
            return CAR_START_DOWN;
        }
      }

      /* See if someone wants to go down */
      next_floor = ui_get_nearest_order(ORDER_DOWN, floor);
      if (next_floor != -1) {
        DEBUG(("Down order found at floor %i, current floor is %i\n", next_floor, floor));
        if (next_floor == floor) {
            return CAR_HALT;
        } else if  (next_floor > floor) {
            return CAR_START_UP;
        } else {
            return CAR_START_DOWN;
        }
      }

      /* Make sure we don't go past the first or last floor */
      if ((floor == 0 && car_current_state == CAR_MOVING_DOWN) || (floor == N_FLOORS-1 && car_current_state == CAR_MOVING_UP)) {
          DEBUG(("Failsafe: stop at top/bottom\n"));
          return CAR_HALT;
      }
  }

  /* Nothing to do */
  return CAR_NOEVENT;
}


/* PUBLIC FUNCTIONS */

void
car_init (void)
{
    if (elev_get_floor_sensor_signal() == -1) {
      elev_set_speed (SPEED_DOWN);
      
      /* Loop until we reach a floor */
      while (elev_get_floor_sensor_signal() == -1);

      last_floor = elev_get_floor_sensor_signal();
    }

    stop_motor();
    car_last_state = CAR_STOPPED;
    car_current_state = CAR_IDLE;
}


/* Update the state machine */
void
car_update_state (void)
{
    new_event = get_new_event ();
    if (((new_event >= 0) && (new_event <= N_CAR_EVENTS)) && ((car_current_state >= 0) && (car_current_state <= N_CAR_STATES))) {
        car_state_table [car_current_state][new_event] ();
    }
}
