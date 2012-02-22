// REPLACE THIS FILE WITH YOUR OWN CODE.
// READ ELEV.H FOR INFORMATION ON HOW TO USE THE ELEVATOR FUNCTIONS.

#include "config.h"

#include <stdio.h>
#include <pthread.h>

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

  printf("%i\n", elev_get_floor_sensor_signal());
  printf("Initializing elevator car. Please wait.\n");
  car_init();
  printf("Initialized. Ready.\n");

  while(1) {
      // check IO
      if(ui_check_buttons()) {
        // TODO: check order queue
      }
  }

  return 0;
}

