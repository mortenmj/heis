#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libheis/elev.h>

#include "car.h"
#include "safety.h"
#include "ui.h"

int main()
{
  // Initialize hardware
  if (!elev_init()) {
      printf(__FILE__ ": Unable to initialize elevator hardware\n");
      return 1;
  }

  car_init();

  while(1) {
      safety_get_status();
      ui_check_buttons();
      car_update_state();

      usleep(50000);
  }

  return 0;
}
