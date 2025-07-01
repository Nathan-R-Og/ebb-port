#ifndef COMMON_STRUCTS
#define COMMON_STRUCTS

#include "types.h"
#include "data/sprites.h"

typedef struct OAM_Sprite {
    u8 y;
    u8 tile;
    u8 attributes;
    /*
    76543210
    ||||||||
    ||||||++- Palette (0 to 3) of sprite
    |||+++--- Unimplemented (read 0)
    ||+------ Priority (0: in front of background; 1: behind background)
    |+------- Flip sprite horizontally
    +-------- Flip sprite vertically
    */
    u8 x;
} OAM_Sprite;

typedef struct PRE_oam{
    u8 tiles;
    u8 start_index;
    u8 x;
    u8 y;
    void* something;
    SpriteDef* sprite;
}PRE_oam;

#endif //COMMON_STRUCTS