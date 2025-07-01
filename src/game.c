#include "game.h"
#include "common_structs.h"

SDL_Surface* global_bg;
PRE_oam shadow_something[32];

SDL_Rect screenRect = {0,0,0x20*8,0x20*8};
SDL_Surface* screen;
SDL_Texture* global_bg_t;
u8 game_mode = 0;
bool emulate_oam = true;

SDL_Color generic_grayscale_sdl_colors[4] = {
    {0x00,0x00,0x00,255},
    {0x66,0x66,0x66,255},
    {0xad,0xad,0xad,255},
    {0xff,0xfe,0xff,255},
};
SDL_Palette* generic_grayscale_sdl_palette;
SDL_Color generic_transparent_sdl_colors[4] = {
    {0x00,0x00,0x00,0},
    {0x00,0x00,0x00,255},
    {0x66,0x66,0x66,255},
    {0xff,0xfe,0xff,255},
};
SDL_Palette* generic_transparent_sdl_palette;


void OAM_from_SpriteDef(SpriteDef* in, u8 start, u8 x_off, u8 yoff){
    SpriteTile* currentTile = in->tiles;
    if (!currentTile){
        return;
    }
    while (true) {

        //end
        if (currentTile->x == (u8)-1 && currentTile->y == (u8)-1 && currentTile->index == (u8)-1){
            break;
        }

        //calc tile
        SHADOW_OAM[start].tile = currentTile->index + in->ppu_offset;

        SHADOW_OAM[start].x = currentTile->x + x_off;
        SHADOW_OAM[start].y = currentTile->y + yoff;

        u8 use_palette = 0;
        if (currentTile->palette == 0){
            use_palette = in->p1;
        }
        else if (currentTile->palette == 1){
            use_palette = in->p2;
        }
        SHADOW_OAM[start].attributes = (currentTile->flipY << 7) | (currentTile->flipX << 6) | (currentTile->order << 5) | use_palette;

        currentTile++;
        start++;
    }
}

void OAM_from_entity(Entity* object, int start){
    if (!object){
        return;
    }
    SpriteDef* currentSprite = object->sprite;
    if (!currentSprite){
        return;
    }
    u8 yeah = (object->x * 16) + object->real_x;
    u8 yeah2 = (object->y * 16) + object->real_y;
    OAM_from_SpriteDef(currentSprite, start, yeah, yeah2);

}

void Draw_Tilemap(SDL_Surface* insurface, TILEMAP_BLOCK* tm, SDL_Renderer *renderer){
    u8 x, y;
    SDL_Palette* current_palette[4];
    for(u8 i = 0; i < 4; i++){
        current_palette[i] = SDL_AllocPalette(4);
        Palette_From_Nes(global_bg_palette[i], current_palette[i]);
    }

    for (y=0;y < 0x20; y++){
        for (x=0;x < 0x20; x++){
            u8 value = tm->tiles[x + (y * 0x20)];

            u8 attr_x = x / 2;
            u8 attr_y = y / 2;
            u8 attr = tm->attr[attr_x + (attr_y * 2)];
            u8 attr_x_side = x - (attr_x * 2);
            u8 attr_y_side = y - (attr_y * 2);
            u8 attr_bit = attr_x_side + (attr_y_side * 2);
            attr = 3 & (attr >> (attr_bit * 2));

            SDL_SetSurfacePalette(insurface, current_palette[attr]);

            u8 tx = value % 0x10;
            u8 ty = value / 0x10;
            SDL_Rect get_tile = {tx * 8, ty * 8, 8, 8};
            SDL_Rect put_tile = {x * 8, y * 8, 8, 8};
            SDL_BlitSurface(insurface, &get_tile, screen, &put_tile);

        }
    }
    SDL_DestroyTexture(global_bg_t);
    global_bg_t = texture_from_surface(screen, renderer);
    SDL_RenderCopy(renderer, global_bg_t, NULL, &screenRect);
    for (u8 i = 0; i < 4; i++){
        SDL_FreePalette(current_palette[i]);
    }

}

void OAM_from_PREoam(void){
    PRE_oam* current = shadow_something;
    for(u8 i = 0; i < 32; i++, current++){
        if (current->tiles == 0){
            continue;
        }
        OAM_from_SpriteDef(current->sprite, current->start_index, current->x, current->y+1);
    }
}

void Title_Init(void);
void Block_Init(void);
void Normal_Game_Init(void);
void switch_game_mode(u8 to){
    //do inits
    switch (to){
        case 0:
            Title_Init();
            break;
        case 1:
            Block_Init();
            break;
        case 2:
            Normal_Game_Init();
            break;
    }
    game_mode = to;
}

void Title_Process(SDL_Renderer *renderer);
void Block_Process(SDL_Renderer *renderer);
void Normal_Game(SDL_Renderer *renderer);
bool Game_start(SDL_Renderer *renderer) {
    // Initialize framerate manager : 30 FPS
    FPSmanager fpsmanager;
    SDL_initFramerate(&fpsmanager);
    SDL_setFramerate(&fpsmanager, 60);

    //setup generic sdl grayscale palette (for bgs at least)
    generic_grayscale_sdl_palette = SDL_AllocPalette(4);
    SDL_SetPaletteColors(generic_grayscale_sdl_palette, generic_grayscale_sdl_colors, 0, 4);

    //setup generic sdl grayscale palette (for sprites at least)
    generic_transparent_sdl_palette = SDL_AllocPalette(4);
    SDL_SetPaletteColors(generic_transparent_sdl_palette, generic_transparent_sdl_colors, 0, 4);

    //setup bg ppu
    global_bg = SDL_CreateRGBSurfaceWithFormat(0, 0x10*8, 0x10*8, 32, SDL_PIXELFORMAT_INDEX8);
    SDL_SetSurfacePalette(global_bg, generic_grayscale_sdl_palette);

    //setup bg screen ppu
    screen = SDL_CreateRGBSurface(0, screenRect.w, screenRect.h, 32, 0, 0, 0, 0);

    //load palette file
    update_palette("%spalettes/nes.pal");

    //do inits
    switch_game_mode(game_mode);

    // Event loop
    while(!main_quit)
    {
        CheckInput();

        if (main_quit){
            break;
        }
        switch (game_mode){
            case 0:
                Title_Process(renderer);
                break;
            case 1:
                Block_Process(renderer);
                break;
            case 2:
                Normal_Game(renderer);
                break;
        }

        if (emulate_oam){
            OAM_Draw(renderer);
        }
        // Update screen
        SDL_RenderPresent(renderer);
        // Delay
        SDL_framerateDelay(&fpsmanager);
    }
    for (u8 i = 0; i < 2; i++){
        SDL_FreeSurface(npc_sprite_table[i]);
    }
    SDL_FreePalette(generic_grayscale_sdl_palette);
    SDL_FreeSurface(global_bg);
    SDL_FreeSurface(screen);

    return true;
}

