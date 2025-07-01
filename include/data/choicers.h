#ifndef DATA_CHOICERS_H
#define DATA_CHOICERS_H
#include "types.h"
#include "data/objects.h"
//not JUST teleport defs, by the way!
//name, (posX << 6)| music, (posY << 6)| startDirection, msgPrice
typedef struct choicerDef{
    char* name;
    u8 music;
    u16 posX;
    Direction direction;
    u16 posY;
    u16 msgPrice;
} choicerDef;
#endif // DATA_CHOICERS_H