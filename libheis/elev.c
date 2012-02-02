// Wrapper for libComedi Elevator control.
// These functions provides an interface to the elevators in the real time lab
//
// 2007, Martin Korsgaard

//
// YOU DO NOT NEED TO EDIT THIS FILE
//

#include "channels.h"
#include "elev.h"
#include "io.h"

#include <assert.h>
#include <stdlib.h>

// Matrix of lamp channels indexed by floor and button type. Excuse ugly macro.
#define LCM_SET(f) {LIGHT##f##_UP, LIGHT##f##_DOWN, LIGHT##f##_COMMAND}
static const int lamp_channel_matrix[N_FLOORS][N_BUTTONS] =
  {LCM_SET(1), LCM_SET(2), LCM_SET(3), LCM_SET(4)};


// Matrix of elevator button signal channels indexed by floor and button type.
#define SCM_SET(f) { FLOOR##f##_UP, FLOOR##f##_DOWN, FLOOR##f##_COMMAND }
static const int button_channel_matrix[N_FLOORS][N_BUTTONS] =
  {SCM_SET(1), SCM_SET(2), SCM_SET(3), SCM_SET(4)};



void elev_set_speed(int speed)
{
    // In order to sharply stop the elevator, the direction bit is toggled
    // before setting speed to zero.
    static int last_speed = 0;
    
    // If to start (speed > 0)
    if (speed > 0)
        io_clear_bit(MOTORDIR);
    else if (speed < 0)
        io_set_bit(MOTORDIR);

    // If to stop (speed == 0)
    else if (last_speed < 0)
        io_clear_bit(MOTORDIR);
    else if (last_speed > 0)
        io_set_bit(MOTORDIR);

    last_speed = speed ;

    // Write new setting to motor.
    io_write_analog(MOTOR, 2048 + 4*abs(speed));
}



void elev_set_door_open_lamp(int value)
{
    if (value)
        io_set_bit(DOOR_OPEN);
    else
        io_clear_bit(DOOR_OPEN);
}
 


int elev_get_obstruction_signal(void)
{
    return io_read_bit(OBSTRUCTION);
}



int elev_get_stop_signal(void)
{
    return io_read_bit(STOP);
}



void elev_set_stop_lamp(int value)
{
    if (value)
        io_set_bit(LIGHT_STOP);
    else
        io_clear_bit(LIGHT_STOP);
}



int elev_get_floor_sensor_signal(void)
{
    if (io_read_bit(SENSOR1))
        return 0;
    else if (io_read_bit(SENSOR2))
        return 1;
    else if (io_read_bit(SENSOR3))
        return 2;
    else if (io_read_bit(SENSOR4))
        return 3;
    else
        return -1;
}



void elev_set_floor_indicator(int floor)
{
	// assert crashes the program deliberately if it's condition does not hold,
	// and prints an informative error message. Useful for debugging.
    assert(floor >= 0);
    assert(floor < N_FLOORS);

    if (floor & 0x02)
        io_set_bit(FLOOR_IND1);
    else
        io_clear_bit(FLOOR_IND1);
        
    if (floor & 0x01)
        io_set_bit(FLOOR_IND2);
    else
        io_clear_bit(FLOOR_IND2);
}



int elev_get_button_signal(elev_button_type_t button, int floor)
{
	// assert crashes the program deliberately if it's condition does not hold,
	// and prints an informative error message. Useful for debugging.
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS-1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button ==
            BUTTON_COMMAND);

    if (io_read_bit(button_channel_matrix[floor][button]))
        return 1;
    else
        return 0;
}



void elev_set_button_lamp(elev_button_type_t button, int floor, int value)
{
	// assert crashes the program deliberately if it's condition does not hold,
	// and prints an informative error message. Useful for debugging.
    assert(floor >= 0);
    assert(floor < N_FLOORS);
    assert(!(button == BUTTON_CALL_UP && floor == N_FLOORS-1));
    assert(!(button == BUTTON_CALL_DOWN && floor == 0));
    assert(button == BUTTON_CALL_UP || button == BUTTON_CALL_DOWN || button ==
            BUTTON_COMMAND);

    if (value == 1)
        io_set_bit(lamp_channel_matrix[floor][button]);
    else
        io_clear_bit(lamp_channel_matrix[floor][button]);        
}



int elev_init(void)
{
    int i;

    // Init hardware
    if (!io_init())
        return 0;

    // Zero all floor button lamps
    for (i = 0; i < N_FLOORS; ++i) {
        if (i != 0)
            elev_set_button_lamp(BUTTON_CALL_DOWN, i, 0);

        if (i != N_FLOORS-1)
            elev_set_button_lamp(BUTTON_CALL_UP, i, 0);

        elev_set_button_lamp(BUTTON_COMMAND, i, 0);
    }

    // Clear stop lamp, door open lamp, and set floor indicator to ground floor.
    elev_set_stop_lamp(0);
    elev_set_door_open_lamp(0);
    elev_set_floor_indicator(0);

    // Return success.
    return 1;
}



