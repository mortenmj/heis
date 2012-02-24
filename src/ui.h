#ifndef __INCLUDE_CONTROL_H__
#define __INCLUDE_CONTROL_H__

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

void ui_add_order (order_type_t type, int floor);
int ui_check_order (order_type_t type, int floor);
int ui_get_nearest_order (order_type_t type, int floor);
int ui_get_nearest_order_in_direction (order_type_t type, direction_t dir, int floor);
void ui_remove_order (order_type_t type, int floor);
void ui_clear_orders (void);
void ui_print_orders (void);
int ui_check_buttons (void);

#endif /* __INCLUDE_CONTROL_H__ */
