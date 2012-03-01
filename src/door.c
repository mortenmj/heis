#include "config.h"

#include "door.h"

#include <stdio.h>
#include <time.h>
#include <libheis/elev.h>

#include "debug.h"

#define HALT_PAUSE 3

door_state_t door_current_state, door_last_state;
door_event_t new_event;

time_t wait_until = 0;

/* Dummy action. No state transition. */
static void action_dummy (void) {
}

/* From DOOR_OPENED, go to DOOR_CLOSED */
static void action_opened_close (void) {
    printf("Doors opened: closing\n");

    if (elev_get_obstruction_signal()) {
        door_set_timer();
        return;
    }

    elev_set_door_open_lamp(0);

    door_last_state = door_current_state;
    door_current_state = DOOR_CLOSED;
}

/* From DOOR_CLOSED, go to DOOR_OPENED */
static void action_closed_open (void) {
    printf("Doors closed: opening\n");
    elev_set_door_open_lamp(1);

    door_last_state = door_current_state;
    door_current_state = DOOR_OPENED;
}

void (*const door_state_table [N_DOOR_STATES][N_DOOR_EVENTS]) (void) = {

      /* NOEVENT        DOOR_OPEN            DOOR_CLOSE */
    { action_dummy, action_dummy, action_opened_close },                /* events for state DOOR_OPENED */
    { action_dummy, action_closed_open, action_dummy }                  /* events for state DOOR_CLOSED */
};

static door_event_t get_new_event (void)
{
  if (time(NULL) > wait_until) {
      return DOOR_CLOSE;
  } else {
      return DOOR_OPEN;
  }
}

void door_init (void) {
    printf("Initializing elevator door\n");

    door_last_state = DOOR_OPENED;
    door_current_state = DOOR_CLOSED;

    printf("Elevator door initialization complete\n");
}

/* Returns 1 if doors are closed, or 0 */
int door_closed (void) {
    if (door_current_state == DOOR_CLOSED)
      return 1;

    return 0;
}

/* Defines a setpoint at HALT_PAUSE seconds from now */
void door_set_timer() {
    printf("Reset timer\n");
    wait_until = time(NULL) + HALT_PAUSE;
}

/* Update the state machine */
void door_update_state (void) {
    new_event = get_new_event ();
    if (((new_event >= 0) && (new_event <= N_DOOR_EVENTS)) && ((door_current_state >= 0) && (door_current_state <= N_DOOR_STATES))) {
        door_state_table [door_current_state][new_event] ();
    }
}
