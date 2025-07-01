#include "hardware.h"
#include "common_structs.h"

SDL_Surface* map_bg_table[8];
SDL_Surface* npc_sprite_table[2];
TILEMAP_BLOCK TILEMAP[4];
OAM_Sprite SHADOW_OAM[MAX_OAM];
u8 controller_pad = 0;
u8 controller_pad_frame = 0;
SDL_Color nes_palette[0x40];
SDL_Color default_transparent = {0,0,0,0};
u8 global_bg_palette[4][4];
u8 global_sprite_palette[4][4];

//Set color 0 of each palette to alpha
void fix_sprite_palette(void){
    for (u8 i = 0; i < 4; i++){
        global_sprite_palette[i][0] = 0xff;
    }
}

void load_palette(u8 which, u8* data, u8 entries){
    switch(which){
        case 0:
            memcpy(global_bg_palette, data, entries);
            break;
        case 1:
            memcpy(global_sprite_palette, data, entries);
            fix_sprite_palette();
            break;
        case 2:
            if (entries < 0x10){
                memcpy(global_bg_palette, data, entries);
            } else if (entries == 0x10){
                memcpy(global_bg_palette, data, 0x10);
            } else if (entries > 0x10){
                memcpy(global_bg_palette, data, 0x10);
                memcpy(global_sprite_palette, data+0x10, entries - 0x10);
                fix_sprite_palette();
            }
            break;
        default:
            break;
    }
}

SDL_Color Color_From_Id(u8 id){
    if (id == 0xff){
        return default_transparent;
    }
    return nes_palette[id];
}

void Color_From_Palette(u8* id, SDL_Color* to){
    for (int i = 0; i < 4; i++)
        to[i] = Color_From_Id(id[i]);
}

void Palette_From_Nes(u8* id, SDL_Palette* to){
    SDL_Color new[4];
    Color_From_Palette(id, new);
    SDL_SetPaletteColors(to, new, 0, 4);
}

void update_palette(char* inpath){
    FILE *filePointer;
    u8 buffer[0xc0]; // Buffer to store data

    get_realpath(inpath);

    // Open the file in binary read mode
    filePointer = fopen(path, "rb");
    if (filePointer == NULL) {
        perror("Failed to open file");
        return;
    }

    size_t bytesRead = fread(buffer, sizeof(u8), sizeof(buffer), filePointer);

    for (size_t i = 0; i < bytesRead; i+=3) {
        u8 s = i;
        u8 color = s / 3;
        SDL_Color new = {buffer[s], buffer[s+1], buffer[s+2], 255};
        nes_palette[color] = new;
    }

    // Close the file
    fclose(filePointer);
}

void CheckInput(void){
    bool quit = false;

    u8 old_keys = controller_pad;

    SDL_Event e;
    // Get available event
    while(SDL_PollEvent(&e))
    {
        // User requests quit
        if(e.type == SDL_QUIT) {
            quit = true;
            break;
        } else if(e.key.repeat == 0){
            if(e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;

                    case SDLK_RIGHT:
                        controller_pad |= PAD_RIGHT;
                        break;
                    case SDLK_LEFT:
                        controller_pad |= PAD_LEFT;
                        break;
                    case SDLK_DOWN:
                        controller_pad |= PAD_DOWN;
                        break;
                    case SDLK_UP:
                        controller_pad |= PAD_UP;
                        break;
                    case SDLK_RETURN:
                        controller_pad |= PAD_START;
                        break;
                    case SDLK_SPACE:
                        controller_pad |= PAD_SELECT;
                        break;
                    case SDLK_z:
                        controller_pad |= PAD_B;
                        break;
                    case SDLK_x:
                        controller_pad |= PAD_A;
                        break;
                }
            } else if(e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_RIGHT:
                        controller_pad &= (u8) ~PAD_RIGHT;
                        break;
                    case SDLK_LEFT:
                        controller_pad &= (u8) ~PAD_LEFT;
                        break;
                    case SDLK_DOWN:
                        controller_pad &= (u8) ~PAD_DOWN;
                        break;
                    case SDLK_UP:
                        controller_pad &= (u8) ~PAD_UP;
                        break;
                    case SDLK_RETURN:
                        controller_pad &= (u8) ~PAD_START;
                        break;
                    case SDLK_SPACE:
                        controller_pad &= (u8) ~PAD_SELECT;
                        break;
                    case SDLK_z:
                        controller_pad &= (u8) ~PAD_B;
                        break;
                    case SDLK_x:
                        controller_pad &= (u8) ~PAD_A;
                        break;

                }
            }
        }
    }

    //eval controller_pad_frame
    for (u8 key = 1; key > 0; key <<= 1){
        bool old_state = (old_keys & key) > 0;
        bool new_state = (controller_pad & key) > 0;
        bool pressed = !(old_state) && new_state;
        bool released = old_state && !(new_state);
        bool held = old_state == new_state;
        // pressed since last frame
        if (pressed){
            controller_pad_frame |= key;
        }
        // released since last frame
        // held since last frame
        else if(released || held) {
            controller_pad_frame &= (u8) ~key;
        }
    }

    if (quit){
        main_quit = true;
    }

}

//NOTE: SDL2 does not support flipping surfaces. gulp
//rendercopyex should be fine for now...
void OAM_Draw(SDL_Renderer *renderer){
    SDL_Palette* current_palette[4];
    for(u8 i = 0; i < 4; i++){
        current_palette[i] = SDL_AllocPalette(4);
        Palette_From_Nes(global_sprite_palette[i], current_palette[i]);
    }

    //emulate OAM
    OAM_Sprite* current_oam = &SHADOW_OAM[0];
    for (int i = 0; i < MAX_OAM; i++, current_oam++){
        int tile_x = current_oam->tile % 0x10; //ppu stuff
        int tile_y = current_oam->tile / 0x10;
        SDL_Surface* use_sheet = npc_sprite_table[0];

        if (tile_y >= 0x8){
            use_sheet = npc_sprite_table[1];
            tile_y -= 8;
        }
        tile_x *= 8; //ppu stuff
        tile_y *= 8;

        SDL_Rect in_rect = {tile_x,tile_y,8,8};
        SDL_Rect out_rect = {current_oam->x,current_oam->y,8,8};

        int use_palette = current_oam->attributes & 3;

        SDL_SetSurfacePalette(use_sheet, current_palette[use_palette]);

        int flipargs = SDL_FLIP_NONE;
        if (current_oam->attributes & 0x40){
            flipargs |= SDL_FLIP_HORIZONTAL;
        }
        if (current_oam->attributes & 0x80){
            flipargs |= SDL_FLIP_VERTICAL;
        }

        //sprite must be retrieved again after updating palette
        SDL_Texture* sprite = texture_from_surface(use_sheet, renderer);
        if (sprite){
            //Make a target texture to render to
            SDL_RenderCopyEx(renderer, sprite, &in_rect, &out_rect, 0, NULL, flipargs);
        }
        SDL_DestroyTexture(sprite);
    }
}