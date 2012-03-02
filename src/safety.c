#include "config.h"

#include <stdbool.h>

#include <libheis/elev.h>

#include "debug.h"

bool stop = false;


/**
 * safety_get_state:
 * 
 * Checks if the stop button is pressed, and sets the stop button flag if it is.
 * 
 * Returns: true if the stop button is set, or false.
 */
bool
safety_update_state (void)
{
  if (elev_get_stop_signal())
    {
      stop = true;
      elev_set_stop_lamp(1);
    }

  return stop;
}


/**
 * safety_get_state:
 * 
 * Returns: true if the stop button flag is set, or false.
 *
 */
bool
safety_get_state (void)
{
  return stop;
}


/**
 * safety_get_state:
 * 
 * Resets the stop button flag.
 *
 */
void
safety_reset (void) {
  stop = false;
  elev_set_stop_lamp(0);
}
