#ifndef HARDWARE_H
#define HARDWARE_H

#define PAD_A (1 << 7)
#define PAD_B (1 << 6)
#define PAD_SELECT (1 << 5)
#define PAD_START (1 << 4)
#define PAD_UP (1 << 3)
#define PAD_DOWN (1 << 2)
#define PAD_LEFT (1 << 1)
#define PAD_RIGHT 1

typedef struct TILEMAP_BLOCK {
    u8 tiles[0x3c0];
    u8 attr[0x40];
} TILEMAP_BLOCK;

#endif