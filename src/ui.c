#include "ui.h"

#include <stdio.h>
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
    printf("Order type: %i, floor number: %i\n", order_type, floor);

    elev_set_button_lamp(order_type, floor, 1);
    orders[order_type][floor] = 1;
}

/**
 * check_order:
 * @order_type: an #order_type_t
 * @floor: the order's floor number
 *
 * Check if an order is registered.
 *
 * Returns: 1 if the spesified order is set, or 0.
 */
static int check_order(order_type_t order_type, int floor) {
    return orders[order_type][floor];
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
    elev_set_button_lamp(order_type, floor, 0);
    orders[order_type][floor] = 0;
}

/**
 * clear_orders:
 *
 * Removes all orders from the order queue.
 *
 */
static void clear_orders(void) {
    /* Clear up orders */
    for (int floor = 0; floor < N_FLOORS-1; floor++) {
        remove_order(ORDER_UP, floor);
    }

    /* Clear down orders */
    for (int floor = 1; floor < N_FLOORS; floor++) {
        remove_order(ORDER_DOWN, floor);
    }

    /* Clear car orders */
    for (int floor = 0; floor < N_FLOORS; floor++) {
        remove_order(ORDER_CAR, floor);
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
  int new_orders = 0;

  /* Check stop button */
  if (elev_get_stop_signal()) {
      clear_orders();
  }

  /* Check hall buttons */

  /* Check up */
  for (int floor = 0; floor < N_FLOORS-1; floor++) {
    if (elev_get_button_signal(BUTTON_CALL_UP, floor)) {
        if (!check_order(ORDER_UP, floor)) {
          add_order(ORDER_UP, floor);
          new_orders = 1;
        }
    }
  }

  /* Check down */
  for (int floor = 1; floor < N_FLOORS; floor++) {
    if (elev_get_button_signal(BUTTON_CALL_DOWN, floor)) {
        if (!check_order(ORDER_DOWN, floor)) {
          add_order(ORDER_DOWN, floor);
          new_orders = 1;
        }
    }
  }

  /* Check car buttons */
  for (int floor = 0; floor < N_FLOORS; floor++) {
    if (elev_get_button_signal(BUTTON_COMMAND, floor)) {
        if (!check_order(ORDER_CAR, floor)) {
          add_order(ORDER_CAR, floor);
          new_orders = 1;
        } else {
            printf ("Order already set\n");
        }
    }
  }

  return new_orders;
}
