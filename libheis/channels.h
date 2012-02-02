// Channel definitions for elevator control using LibComedi
//
// 2006, Martin Korsgaard
#ifndef __INCLUDE_CHANNELS_H__
#define __INCLUDE_CHANNELS_H__

//in port 4
#define PORT4          3
#define OBSTRUCTION    (0x300+23)
#define STOP           (0x300+22)
#define FLOOR1_COMMAND (0x300+21)
#define FLOOR2_COMMAND (0x300+20)
#define FLOOR3_COMMAND (0x300+19)
#define FLOOR4_COMMAND (0x300+18)
#define FLOOR1_UP      (0x300+17)
#define FLOOR2_UP      (0x300+16)

//in port 1
#define PORT1          2
#define FLOOR2_DOWN    (0x200+0)
#define FLOOR3_UP      (0x200+1)
#define FLOOR3_DOWN    (0x200+2)
#define FLOOR4_DOWN    (0x200+3)
#define SENSOR1        (0x200+4)
#define SENSOR2        (0x200+5)
#define SENSOR3        (0x200+6)
#define SENSOR4        (0x200+7)

//out port 3
#define PORT3          3
#define MOTORDIR       (0x300+15)
#define LIGHT_STOP     (0x300+14)
#define LIGHT1_COMMAND (0x300+13)
#define LIGHT2_COMMAND (0x300+12)
#define LIGHT3_COMMAND (0x300+11)
#define LIGHT4_COMMAND (0x300+10)
#define LIGHT1_UP      (0x300+9)
#define LIGHT2_UP      (0x300+8)

//out port 2
#define PORT2 3
#define LIGHT2_DOWN    (0x300+7)
#define LIGHT3_UP      (0x300+6)
#define LIGHT3_DOWN    (0x300+5)
#define LIGHT4_DOWN    (0x300+4)
#define DOOR_OPEN      (0x300+3)
#define FLOOR_IND2     (0x300+1)
#define FLOOR_IND1     (0x300+0)

//out port 0
#define PORT0          1
#define MOTOR          (0x100+0)

//non-existing ports (to achieve macro consistency)
#define FLOOR1_DOWN    -1
#define FLOOR4_UP      -1
#define LIGHT1_DOWN    -1
#define LIGHT4_UP      -1


#endif //#ifndef __INCLUDE_CHANNELS_H__
