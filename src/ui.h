#ifndef __INCLUDE_CONTROL_H__
#define __INCLUDE_CONTROL_H__

#define N_BUTTONS 2

int ui_check_buttons(void);

typedef enum order_type {
    ORDER_UP = 0,
    ORDER_DOWN = 1,
    ORDER_CAR = 2
} order_type_t;

#endif /* __INCLUDE_CONTROL_H__ */
