#ifndef DATA_OBJECTS_H
#define DATA_OBJECTS_H
#include "types.h"
#include "sprites.h"
typedef enum Direction {
    DIR_UP = 0,
    DIR_UP_RIGHT,
    DIR_RIGHT,
    DIR_DOWN_RIGHT,
    DIR_DOWN,
    DIR_DOWN_LEFT,
    DIR_LEFT,
    DIR_UP_LEFT,
    DIR_IN_PLACE,
} Direction;

typedef struct Entity {
    u16 x;
    u16 y;
    Direction direction;
    SpriteDef* sprite;
    int real_x;
    int real_y;
} Entity;
#endif // DATA_OBJECTS_H

