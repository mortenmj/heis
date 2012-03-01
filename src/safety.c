#include "config.h"

#include <libheis/elev.h>

#include "debug.h"

int stop = 0;

/* Check stop button, update state, return value */
int safety_update_state () {
    if (elev_get_stop_signal()) {
      stop = 1;
      elev_set_stop_lamp(1);
    }

    return stop;
}

int safety_get_state () {
    return stop;
}

void safety_reset () {
    stop = 0;
    elev_set_stop_lamp(0);
}
