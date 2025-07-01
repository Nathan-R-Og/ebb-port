#ifndef DATA_SPRITES_H
#define DATA_SPRITES_H
#include "types.h"

#define SPRITETILE_NULL {-1, -1, 0, 0, 0, 0, 0, -1}

typedef struct SpriteTile {
    u8 x;
    u8 y;
    u8 palette; //2 bits
    u8 unkArg1; //3 bits
    bool order;
    bool flipX;
    bool flipY;
    u8 index;
} SpriteTile;

typedef struct SpriteDef {
    SpriteTile* tiles;
    u8 ppu_offset;
    u8 p1; // 2 bits. palette in slot 0
    u8 p2; // 2 bits. palette in slot 1
    bool unkArg1;
    u8 unkArg2; //3 bits
} SpriteDef;
#endif // DATA_SPRITES_H
