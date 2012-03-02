#include "config.h"

#include "ui.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <libheis/elev.h>

#include "car.h"
#include "debug.h"

/* Arrays for pressed buttons. We store when a button is pushed in, and set the order when it is released */ 
bool buttons[N_ORDER_TYPES][N_FLOORS] = { 0 };

/* Arrays for elevator orders. When an order is in effect the corresponding variable is true, otherwise it is false */
bool orders[N_ORDER_TYPES][N_FLOORS] = { 0 };


/**
 * ui_add_order:
 * @order_type: an #order_type_t 
 * @floor: the order's floor number
 *
 * Add an order to the order queue. The order is only added if it is within bounds, and not already set.
 *
 * Returns: true if a new order was set, or false.
 *
 */
bool
ui_add_order (order_type_t type,
    int floor)
{
  DEBUG(("Order added. Type %i, floor %i\n", type, floor));

  if (type == ORDER_UP && floor == N_FLOORS-1)
    return false;

  if (type == ORDER_DOWN && floor == 0)
    return false;

  if (ui_check_order(type, floor))
    return false;

  elev_set_button_lamp(type, floor, 1);
  orders[type][floor] = 1;

  return true;
}


/**
 * ui_check_order:
 * @order_type: an #order_type_t 
 * @floor: the order's floor number
 *
 * Check if an order is registered.
 *
 * Returns: true if the spesified order is set, or false.
 */
bool
ui_check_order (order_type_t type,
    int floor)
{
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
int 
ui_get_nearest_order (order_type_t type,
    int floor)
{
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
int
ui_get_nearest_order_in_direction (order_type_t type,
    direction_t dir,
    int floor)
{
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
 * Returns: true if an order was removed, or false.
 *
 */
bool
ui_remove_order (order_type_t type,
    int floor)
{
    if (type == ORDER_UP && floor == N_FLOORS-1)
      return false;

    if (type == ORDER_DOWN && floor == 0)
      return false;

    if (!ui_check_order(type, floor))
      return false;

    elev_set_button_lamp(type, floor, 0);
    orders[type][floor] = 0;

    return true;
}


/**
 * ui_clear_orders:
 *
 * Removes all orders from the order queue.
 *
 */
void
ui_clear_orders (void)
{
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
 * ui_check_buttons:
 *
 * Checks all UI buttons and updates the order queue.
 *
 * Returns: true if new orders are present, or false.
 *
 */
bool
ui_check_buttons (void)
{
    int new_orders = 0;

    /* Check order buttons */
    for (int button_type = 0; button_type < N_BUTTONS; button_type++) {
        for (int floor = 0; floor < N_FLOORS; floor++) {

            /* Ignore buttons that don't exist */
            if (button_type == BUTTON_CALL_DOWN && floor == 0 || button_type == BUTTON_CALL_UP && floor == 3)
              continue;
            
            /* Button presses are registered when the button is released.
               We set a button as active when we register that it is pressed,
               and we register an order if a previously activated button is no longer pressed.
             */
            if (elev_get_button_signal(button_type, floor)) {
                buttons[button_type][floor] = true;
            } else {
                if (buttons[button_type][floor]) {
                    if (!ui_check_order(button_type, floor)) {
                        ui_add_order(button_type, floor);
                        new_orders = 1;
                    }

                    buttons[button_type][floor] = false;
                }
            }
        }
    }

    return new_orders;
}
