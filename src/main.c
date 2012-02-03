// REPLACE THIS FILE WITH YOUR OWN CODE.
// READ ELEV.H FOR INFORMATION ON HOW TO USE THE ELEVATOR FUNCTIONS.

#include "config.h"

#include <stdio.h>

#include <libheis/elev.h>


int main()
{
    printf("Initializing motor. Please wait.\n");
    motor_init();
    printf("Motor initialized. Ready.\n");

    while (1) {
        motor_update ();
    }

    return 0;
}

