#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libheis/elev.h>

#include "car.h"
#include "debug.h"
#include "door.h"
#include "safety.h"
#include "ui.h"

int
main()
{
  // Initialize hardware
  if (!elev_init())
    {
      DEBUG((__FILE__ ": Unable to initialize elevator hardware\n"));
      return 1;
    }

  door_init();
  car_init();

  while(1)
    {
      safety_update_state();
      ui_check_buttons();
      car_update_state();
      door_update_state();

      int floor = elev_get_floor_sensor_signal();
      if (floor != -1)
        elev_set_floor_indicator(floor);

      usleep(50000);
    }

  return 0;
}
