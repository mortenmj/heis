#ifndef __INCLUDE_UI_H__
#define __INCLUDE_UI_H__

#include <stdbool.h>

typedef enum {
    ORDER_UP,
    ORDER_DOWN,
    ORDER_CAR,
	N_ORDER_TYPES
} order_type_t;

typedef enum {
    UP,
    DOWN,
    NONE,
    N_DIRS
} direction_t;

typedef struct {
    order_type_t type;
    int floor;
} order_t;

bool ui_add_order (order_type_t type,
    int floor);
bool ui_check_order (order_type_t type,
    int floor);
int ui_get_nearest_order (order_type_t type,
    int floor);
int ui_get_nearest_order_in_direction (order_type_t type,
    direction_t dir,
    int floor);
bool ui_remove_order (order_type_t type,
    int floor);
void ui_clear_orders (void);
bool ui_check_buttons (void);

#endif /* __INCLUDE_UI_H__ */
