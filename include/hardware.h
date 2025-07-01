#ifndef HARDWARE_H
#define HARDWARE_H

#include "types.h"
#include "main.h"
#include "sdl.h"
#include "common_structs.h"

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

extern SDL_Surface* map_bg_table[8];
extern SDL_Surface* npc_sprite_table[2];
extern TILEMAP_BLOCK TILEMAP[4];
#define MAX_OAM 64
extern OAM_Sprite SHADOW_OAM[MAX_OAM];
extern u8 controller_pad;
extern u8 controller_pad_frame;
extern SDL_Color nes_palette[0x40];
extern SDL_Color default_transparent;
extern u8 global_bg_palette[4][4];
extern u8 global_sprite_palette[4][4];

//Set color 0 of each palette to alpha
void fix_sprite_palette(void);
void load_palette(u8 which, u8* data, u8 entries);
SDL_Color Color_From_Id(u8 id);
void Color_From_Palette(u8* id, SDL_Color* to);
void Palette_From_Nes(u8* id, SDL_Palette* to);
void update_palette(char* inpath);
void CheckInput(void);
void OAM_Draw(SDL_Renderer *renderer);
#endif