#include <libheis/elev.h>

int stop = 0;

int safety_get_status () {
    if (elev_get_stop_signal()) {
        stop = 1;
        elev_set_stop_lamp(1);
        return 1;
    }
}

void safety_reset () {
    stop = 0;
    elev_set_stop_lamp(0);
}
