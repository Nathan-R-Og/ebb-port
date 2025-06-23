#ifndef TYPES_H
#define TYPES_H
#include <stdio.h>
#include <stdbool.h>

typedef unsigned char			u8;	/* unsigned  8-bit */
typedef unsigned short			u16;	/* unsigned 16-bit */
typedef unsigned long			u32;	/* unsigned 32-bit */
typedef unsigned long long		u64;	/* unsigned 64-bit */

typedef signed char			s8;	/* signed  8-bit */
typedef short				s16;	/* signed 16-bit */
typedef long				s32;	/* signed 32-bit */
typedef long long			s64;	/* signed 64-bit */

typedef float				f32;	/* single prec floating point */
typedef double				f64;	/* double prec floating point */

extern char GENERIC_STR_NULL[];



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

typedef struct Entity {
    u16 x;
    u16 y;
    Direction direction;
    SpriteDef* sprite;
    int real_x;
    int real_y;
} Entity;

#endif