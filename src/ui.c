#include "ui.h"

#include <libheis/elev.h>

/* Arrays for the three button types. When an order is in effect the corresponding
 * variable is 1, otherwise it is 0 */
int orders[2][N_FLOORS] = {0};

/**
 * add_order:
 * @order_type: an #order_type_t 
 * @floor: the order's floor number
 *
 * Add an order to the order queue.
 *
 */
static void add_order(order_type_t order_type, int floor) {
    orders[order_type][floor] = 1;
}

/**
 * remove_order:
 * @order_type: an #order_type_t 
 * @floor: the order's floor number
 *
 * Remove an order from the order queue.
 *
 */
static void remove_order(order_type_t order_type, int floor) {
    orders[order_type][floor] = 0;
}

/**
 * clear_orders:
 *
 * Removes all orders from the order queue.
 *
 */
static void clear_orders(void) {
    for (int button = 0; button < N_BUTTONS; button++) {
      for (int floor = 0; floor < N_FLOORS; floor++) {
        remove_order(button, floor);
      }
    }
}

/**
 * check_buttons:
 *
 * Checks all UI buttons and updates the order queue.
 *
 * Returns: 1 if new orders are present, or 0.
 *
 */
int ui_check_buttons(void) {
    for (int button = 0; button < N_BUTTONS; button++) {
      for (int floor = 0; floor < N_FLOORS; floor++) {
          if (elev_get_button_signal(button, floor)) {
              elev_set_button_lamp(button, floor, 1);
              add_order(button, floor);
          }
        }
    }
}
