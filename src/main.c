// REPLACE THIS FILE WITH YOUR OWN CODE.
// READ ELEV.H FOR INFORMATION ON HOW TO USE THE ELEVATOR FUNCTIONS.

#include "config.h"

#include <stdio.h>

#include <libheis/elev.h>


int main()
{
    // Initialize hardware
    if (!elev_init()) {
        printf(__FILE__ ": Unable to initialize elevator hardware\n");
        return 1;
    }
    
    printf("Press STOP button to stop elevator and exit program.\n");

    elev_set_speed(300);
      
    while (1) {
        // Check if ground floor, if so, turn upwards.
        if (elev_get_floor_sensor_signal() == 0)
            elev_set_speed(300);

        // Check if upper floor, if so, turn downwards.
        else if (elev_get_floor_sensor_signal() == N_FLOORS-1)
            elev_set_speed(-300);

        // Check if stop button, if so, stop elevator and exit program.
        if (elev_get_stop_signal()) {
            elev_set_speed(0);
            break;
        }
    }

    return 0;
}

