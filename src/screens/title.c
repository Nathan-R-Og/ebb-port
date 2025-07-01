#include "types.h"
#include "sdl.h"
#include "hardware.h"
#include "data/sprites.h"
#include "common_structs.h"

u8 Title_Palette_Old[8][4] = {
    {0x0f, 0x28, 0x30, 0x18},
    {0x0f, 0x21, 0x30, 0x12},
    {0x0f, 0x16, 0x30, 0x12},
    {0x0f, 0x3a, 0x30, 0x12},

    {0xff, 0x21, 0x30, 0x12},
    {0xff, 0x21, 0x30, 0x12},
    {0xff, 0x21, 0x30, 0x12},
    {0xff, 0x21, 0x30, 0x12}
};
u8 Title_Palette[8][4] = {
    {0x0f, 0x21, 0x30, 0x16},
    {0x0f, 0x21, 0x30, 0x16},
    {0x0f, 0x21, 0x30, 0x16},
    {0x0f, 0x21, 0x30, 0x16},

    {0xff, 0x21, 0x30, 0x12},
    {0xff, 0x21, 0x30, 0x12},
    {0xff, 0x21, 0x30, 0x12},
    {0xff, 0x21, 0x30, 0x12}
};
void switch_game_mode(u8 to);
extern SDL_Palette* generic_grayscale_sdl_palette;
extern SDL_Surface* global_bg;
extern u8 palette_queue[8][4];
extern u8 palette_backup[8][4];
u8 screen_inc = 0;
u8 earth_anim = 0;
long long last_earth;
extern SpriteDef SPRITEDEF_EARTH[];
extern SpriteTile SPRITE_EARTH[];
extern PRE_oam shadow_something[32];
void Draw_Tilemap(SDL_Surface* insurface, TILEMAP_BLOCK* tm, SDL_Renderer *renderer);
void OAM_from_PREoam(void);


u8 t_x, t_y, t_ox, t_oy;
TILEMAP_BLOCK* current_tilemap;

void Write_to_TileMap(u8 tile, u8 x, u8 y, TILEMAP_BLOCK* tm){
    tm->tiles[x + (y * 0x20)] = tile;
}
void add_tile(u8 tile){
    Write_to_TileMap(tile, t_x, t_y, current_tilemap);
    t_x++;
    if (t_x >= 0x20){
        t_x = 0;
        t_y++;
    }
}
void add_tiles(u8* tiles, u8 count){
    for(u8 i = 0; i < count; i++){
        add_tile(tiles[i]);
    }
}
void repeatTile(u8 tile, u8 count){
    for(u8 i = 0; i < count; i++){
        add_tile(tile);
    }
}
void do_newline(void){
    t_y++;
    t_x = t_ox;
}
void do_goto(u8 x, u8 y){
    t_x = t_ox = x;
    t_y = t_oy = y+1;
}

void do_produced_by_tiles(void){
    current_tilemap = &TILEMAP[0];
    //produced by
    do_goto(13, 11);
    for (u8 i = 0; i < 4; i++) add_tile(0xc8+i);
    for (u8 i = 0; i < 3; i++) add_tile(0xcd+i);
    do_newline();
    //tail of p
    do_goto(13, 12);
    add_tile(0xD8);
    do_newline();
    //tail of y
    do_goto(19, 12);
    add_tile(0xDF);
    do_newline();

    //line
    do_goto(13, 13);
    repeatTile(0xCC, 19);
    do_newline();

    //Nintendo
    do_goto(13, 15);
    for (u8 i = 0; i < 6; i++) add_tile(0xe3+i);
}

void do_presented_by_tiles(void){
    current_tilemap = &TILEMAP[0];
    //presented by
    do_goto(13, 11);
    for (u8 i = 0; i < 6; i++) add_tile(0xd9 + i);
    for (u8 i = 0; i < 2; i++) add_tile(0xCE + i);
    do_newline();
    //tail of p
    do_goto(13, 12);
    add_tile(0xD8);
    do_newline();
    //tail of y
    do_goto(20, 12);
    add_tile(0xDF);
    do_newline();

    //line
    do_goto(0, 13);
    repeatTile(0xCC, 21);
    do_newline();

    //SHIGESATO ITOI
    do_goto(8, 15);
    for (u8 i = 0; i < 13; i++) add_tile(0xf3 + i);
}

void do_title_screen_tiles(void){
    current_tilemap = &TILEMAP[0];
    //EARTHBOUND logo
    do_goto(8, 7);
    for (u8 i = 0; i < 13; i++) add_tile(0x90 + i);
    do_newline();
    for (u8 i = 0; i < 13; i++) add_tile(0xa0 + i);
    do_newline();
    for (u8 i = 0; i < 0x10; i++) add_tile(0xb0 + i);
    do_newline();
    for (u8 i = 0; i < 0x10; i++) add_tile(0xc0 + i);
    do_newline();
    for (u8 i = 0; i < 15; i++) add_tile(0xd0 + i);
    do_newline();
    for (u8 i = 0; i < 15; i++) add_tile(0xe0 + i);
    do_newline();
    for (u8 i = 0; i < 15; i++) add_tile(0xf0 + i);
    do_newline();

    //c 1989/1990
    //SHIGESATO ITOI / NINTENDO
    do_goto(7, 23);
    for (u8 i = 0; i < 5; i++) add_tile(0x43 + i);
    add_tile(0x70);
    for (u8 i = 0; i < 7; i++) add_tile(0x69 + i);
    for (u8 i = 0; i < 5; i++) add_tile(0x53 + i);
}



void fade_handler_title(){
    for(u16 tile = 0; tile < 0x3c0; tile++){
        TILEMAP[0].tiles[tile] = 0;
    }

    SDL_Rect sheet1 = {0,0,0,0};
    SDL_Rect sheet2 = {0,64,0,0};
    switch(screen_inc){
        case 0:
            do_presented_by_tiles();
            break;
        case 1:
            get_realpath("%simage/title.png");
            SDL_Surface* sheet1_png = surface_from_path(path);
            SDL_SetSurfacePalette(global_bg, generic_grayscale_sdl_palette);
            SDL_SetSurfacePalette(sheet1_png, generic_grayscale_sdl_palette);
            SDL_BlitSurface(sheet1_png, NULL, global_bg, &sheet1);
            SDL_FreeSurface(sheet1_png);
            get_realpath("%simage/earth_bound.png");
            sheet1_png = surface_from_path(path);
            SDL_SetSurfacePalette(sheet1_png, generic_grayscale_sdl_palette);
            SDL_BlitSurface(sheet1_png, NULL, global_bg, &sheet2);
            SDL_FreeSurface(sheet1_png);

            //get stuff set up for the earth animation
            earth_anim = 0;
            last_earth = Utils_time();

            PRE_oam earth = {16, 4, 88, 87, NULL, &SPRITEDEF_EARTH[0]};
            shadow_something[28] = earth;

            do_title_screen_tiles();

            memcpy(palette_backup, Title_Palette, 0x20); //BackupPalette
            break;
        case 2:
            memset(shadow_something, 0, sizeof(shadow_something));
            memset(SHADOW_OAM, 0, sizeof(SHADOW_OAM));


            switch_game_mode(1);
            return;
            get_realpath("%simage/title.png");
            SDL_Surface* use_bg = surface_from_path(path);
            SDL_SetSurfacePalette(global_bg, generic_grayscale_sdl_palette);
            SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
            SDL_BlitSurface(use_bg, NULL, global_bg, &sheet2);
            SDL_FreeSurface(use_bg);

            shadow_something[28].tiles = 0;
            OAM_Sprite nope = {0,0,0,0};
            for (int i = 0; i < 64; i++){
                SHADOW_OAM[i] = nope;
            }

            do_produced_by_tiles();

            memcpy(palette_backup, Title_Palette_Old, 0x20); //BackupPalette
            screen_inc = 0xff;
            break;
    }

    screen_inc++;
}

extern void (*fade_handler)(void);
void start_fade(void);
void simulate_fades(void);

void Title_Init(void){
    fade_handler = &fade_handler_title;

    get_realpath("%simage/title.png");
    SDL_Surface* use_bg = surface_from_path(path);
    SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
    SDL_Rect temp = {0,64,0,0};
    SDL_BlitSurface(use_bg, NULL, global_bg, &temp);
    SDL_FreeSurface(use_bg);


    //oam
    char* oam_tables[] = {
        "%simage/title_earth.png",
        "%simage/characters10.png"
    };
    for (u8 i = 0; i < 2; i++){
        get_realpath(oam_tables[i]);
        use_bg = surface_from_path(path);
        SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
        npc_sprite_table[i] = use_bg;
    }

    //tilemap
    do_produced_by_tiles();
    //attr
    for (u8 i = 0; i < 0x40; i++){
        TILEMAP[0].attr[i] = (2 << 6) | (2 << 4) | (2 << 2) | 2;
        //br | bl | tr | tl
    }

    load_palette(2, (u8*) &Title_Palette_Old, sizeof(Title_Palette_Old) / sizeof(u8));
    memcpy(palette_queue, global_bg_palette, 0x10); //BackupPalette
    memcpy(palette_queue[4], global_sprite_palette, 0x10); //BackupPalette
    memcpy(palette_backup, palette_queue, 0x20); //BackupPalette
}

void Title_Process(SDL_Renderer *renderer){
    SDL_Color bg = {0,0,0,255};
    Utils_setBackgroundColor(renderer, bg);
    Draw_Tilemap(global_bg, &TILEMAP[0], renderer);
    //OAM_from_entity(&Player, 4);

    if (controller_pad_frame & (PAD_A | PAD_START)){
        start_fade();
    }

    simulate_fades();

    if (shadow_something[28].tiles == 16){
        if (Utils_time() - last_earth > 150){
            shadow_something[28].sprite = &SPRITEDEF_EARTH[earth_anim];
            earth_anim++;
            if (earth_anim >= 7){
                earth_anim = 0;
            }
            last_earth = Utils_time();
        }
        OAM_from_PREoam();
    }

}