#include "config.h"

#include "ui.h"

#include <stdlib.h>
#include <stdio.h>
#include <libheis/elev.h>

#include "car.h"
#include "debug.h"

/* Arrays for elevator orders . When an order is in effect the corresponding
 * variable is 1, otherwise it is 0 */
int orders[N_ORDER_TYPES][N_FLOORS] = { {0} };

/**
 * ui_add_order:
 * @order_type: an #order_type_t 
 * @floor: the order's floor number
 *
 * Add an order to the order queue.  *
 */
void ui_add_order(order_type_t type, int floor) {
    if (type == ORDER_UP && floor == N_FLOORS-1)
      return;

    if (type == ORDER_DOWN && floor == 0)
      return;

    elev_set_button_lamp(type, floor, 1);
    orders[type][floor] = 1;
}

/**
 * ui_check_order:
 * @order_type: an #order_type_t 
 * @floor: the order's floor number
 *
 * Check if an order is registered.
 *
 * Returns: 1 if the spesified order is set, or 0.
 */
int ui_check_order(order_type_t type, int floor) {
    return orders[type][floor];
}

/**
 * ui_get_nearest_order:
 * @type: an #order_type_t
 * @floor: a floor number
 *
 * Finds the nearest order from floor of the given type.
 *
 * Returns: An int specifying the floor with the nearest order, or -1.
 */
int ui_get_nearest_order(order_type_t type, int floor) {
    int distance = N_FLOORS;

    for (int i = 0; i < N_FLOORS; i++) {
        if (ui_check_order(type, i) && ((floor - i) < distance)) {
            distance = floor - i;
        }
    }

    /* No order found */
    if (distance == N_FLOORS) {
        return -1;
    }

    return (floor - distance);
}

/**
 * ui_get_nearest_order_in_direction:
 * @type: an #order_type_t
 * @dir: a #direction_t
 * @floor: a floor number
 *
 * Finds the nearest order from floor of the given type in the given direction.
 *
 * Returns: An int specifying the floor with the nearest order, or -1.
 */
int ui_get_nearest_order_in_direction(order_type_t type, direction_t dir, int floor) {
    if (dir == UP) {
        for (int i = floor; i < N_FLOORS; i++) {
            if (ui_check_order(type, i))
                return i;
        }
    } else if (dir == DOWN) {
        for (int i = floor; i >= 0; i--) {
            if (ui_check_order(type, i))
              return i;
        }
    }

    return -1;
}

/**
 * ui_remove_order:
 * @order_type: an #order_type_t 
 * @floor: the order's floor number
 *
 * Remove an order from the order queue.
 *
 */
void ui_remove_order(order_type_t type, int floor) {
    if (type == ORDER_UP && floor == N_FLOORS-1)
      return;

    if (type == ORDER_DOWN && floor == 0)
      return;

    elev_set_button_lamp(type, floor, 0);
    orders[type][floor] = 0;
}

/**
 * ui_clear_orders:
 *
 * Removes all orders from the order queue.
 *
 */
void ui_clear_orders(void) {
    for (int type = 0; type < N_ORDER_TYPES; type++) {
        for (int floor = 0; floor < N_FLOORS; floor++) {

            /* Ignore orders that don't exist */
            if (type == ORDER_DOWN && floor == 0 || type == ORDER_UP && floor == 3)
              continue;

            ui_remove_order(type, floor);
        }
    }
}

/**
 * ui_print_orders:
 *
 * Print the order queue.
 *
 */
void ui_print_orders(void) {
    for (int i = 0; i < N_ORDER_TYPES; i++) {
        for (int j = 0; j < N_FLOORS; j++) {
            printf("%i ", orders[i][j]);
        }
        printf("\n");
    }
}

/**
 * ui_check_buttons:
 *
 * Checks all UI buttons and updates the order queue.
 *
 * Returns: 1 if new orders are present, or 0.
 *
 */
int ui_check_buttons(void) {
    int new_orders = 0;

    /* Check order buttons */
    for (int button = 0; button < N_BUTTONS; button++) {
        for (int floor = 0; floor < N_FLOORS; floor++) {

            /* Ignore buttons that don't exist */
            if (button == BUTTON_CALL_DOWN && floor == 0 || button == BUTTON_CALL_UP && floor == 3)
              continue;

            if (elev_get_button_signal(button, floor)) {
                if (!ui_check_order((order_type_t) button, floor)) {
                  ui_add_order((order_type_t) button, floor);
                  new_orders = 1;
                }
            }
        }
    }

    return new_orders;
}
