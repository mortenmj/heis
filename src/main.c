#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libheis/elev.h>

#include "car.h"
#include "ui.h"

int main()
{
  int new_order = 0;

  // Initialize hardware
  if (!elev_init()) {
      printf(__FILE__ ": Unable to initialize elevator hardware\n");
      return 1;
  }

  printf ("Initializing elevator car. Please wait.\n");
  car_init();
  printf ("Initialized. Ready.\n");

  while(1) {
    // check IO
    if (ui_check_buttons() || elev_get_floor_sensor_signal() != -1) {
      /* New order received; update state machine */
      car_update_state();

      usleep(50000);
      //system("clear");
      //ui_print_orders();
    }
  }

  return 0;
}

