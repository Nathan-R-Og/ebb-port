#include "types.h"
#include "sdl.h"
#include "hardware.h"
#include "data/objects.h"
#include "hardware.h"
#include "data/sprites.h"
#include "main.h"

extern u8 palette_queue[8][4];
extern u8 palette_backup[8][4];

u8 default_sprite_palette[4][4] = {
    {0x0f, 0x0f, 0x00, 0x30},
    {0x0f, 0x0f, 0x16, 0x37},
    {0x0f, 0x0f, 0x24, 0x37},
    {0x0f, 0x0f, 0x12, 0x37},
};

//animation shit
int speed = 1;
bool running = false;
bool walkbool = true;
long long walktime = 0;
long long actual_walk_time = 0;
bool walking = false;
bool can_control = true;

extern SpriteDef NINTEN_DOWN1;
extern SpriteDef NINTEN_DOWN2;
Entity Player = {5,5,DIR_DOWN,&NINTEN_DOWN1,0,0};
Direction walk_dir = DIR_IN_PLACE;
void OAM_from_entity(Entity* object, int start);

void debug_print_inputs(SDL_Renderer *renderer){
    char new[9];
    char chew[9];

    //debug
    u8 i;
    u8 key;
    for (key = 1, i = 0; key > 0; key <<= 1, i++){
        new[i] = (controller_pad & key) > 0 ? '1' : '0';
    }

    for (key = 1, i = 0; key > 0; key <<= 1, i++){
        chew[i] = (controller_pad_frame & key) > 0 ? '1' : '0';
    }

    // Show message
    stringRGBA(renderer, 5, 20, new, 0x9a, 0x9a, 0x9a, 0x9a);

    stringRGBA(renderer, 5, 32, chew, 0x9a, 0x9a, 0x9a, 0x9a);

}


void fade_handler_game(void){
    memcpy(&palette_backup[4], &default_sprite_palette, 0x10); //BackupPalette
}

extern void (*fade_handler)(void);
void start_fade(void);
void simulate_fades(void);
extern SDL_Palette* generic_grayscale_sdl_palette;
void Normal_Game_Init(void){
    fade_handler = &fade_handler_game;
    SDL_Surface* use_bg;
    //oam
    char* oam_tables[] = {
        "%simage/characters1.png",
        "%simage/characters2.png"
    };
    for (u8 i = 0; i < 2; i++){
        get_realpath(oam_tables[i]);
        use_bg = surface_from_path(path);
        SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
        npc_sprite_table[i] = use_bg;
    }

    load_palette(1, (u8*) &default_sprite_palette, sizeof(default_sprite_palette) / sizeof(u8));
    memcpy(palette_queue, global_bg_palette, 0x10); //BackupPalette
    memcpy(palette_queue[4], global_sprite_palette, 0x10); //BackupPalette
    memcpy(palette_backup, palette_queue, 0x20); //BackupPalette
}

SDL_Color funny = {0x3a, 0x3a, 0x3a, 255};
void Normal_Game(SDL_Renderer *renderer){
    Utils_setBackgroundColor(renderer, funny);

    if (controller_pad_frame & PAD_A){
        start_fade();
    }

    //if distance met, reset all relevant values
    if (actual_walk_time >= 16){
        actual_walk_time = 0;
        walking = false;
        walk_dir = DIR_IN_PLACE;
    }

    //if resting, allow reangle/move
    if (walk_dir == DIR_IN_PLACE){
        running = (controller_pad & PAD_B) > 0;
        speed = running ? 4 : 1;


        if (controller_pad & PAD_RIGHT){
            walking = true;
            if (controller_pad & PAD_UP){
                walk_dir = DIR_UP_RIGHT;
            }
            else if (controller_pad & PAD_DOWN){
                walk_dir = DIR_DOWN_RIGHT;
            }
            else {
                walk_dir = DIR_RIGHT;
            }
        }
        else if (controller_pad & PAD_LEFT){
            walking = true;
            if (controller_pad & PAD_UP){
                walk_dir = DIR_UP_LEFT;
            }
            else if (controller_pad & PAD_DOWN){
                walk_dir = DIR_DOWN_LEFT;
            }
            else {
                walk_dir = DIR_LEFT;
            }
        }
        else if (controller_pad & PAD_DOWN){
            walking = true;
            walk_dir = DIR_DOWN;
        }
        else if (controller_pad & PAD_UP){
            walking = true;
            walk_dir = DIR_UP;
        }
    }

    if (walking){
        actual_walk_time += speed;
        bool move_right = walk_dir == DIR_RIGHT || walk_dir == DIR_DOWN_RIGHT || walk_dir == DIR_UP_RIGHT;
        bool move_left = walk_dir == DIR_LEFT || walk_dir == DIR_DOWN_LEFT || walk_dir == DIR_UP_LEFT;
        bool move_up = walk_dir == DIR_UP || walk_dir == DIR_UP_LEFT || walk_dir == DIR_UP_RIGHT;
        bool move_down = walk_dir == DIR_DOWN || walk_dir == DIR_DOWN_LEFT || walk_dir == DIR_DOWN_RIGHT;
        if (move_right){
            Player.real_x += speed;
            if (Player.real_x >= 16){
                Player.x++;
                Player.real_x -= 16;
            }
        }
        if (move_left){
            Player.real_x -= speed;
            if (Player.real_x <= -16){
                Player.x--;
                Player.real_x += 16;
            }
        }
        if (move_down){
            Player.real_y += speed;
            if (Player.real_y >= 16){
                Player.y++;
                Player.real_y -= 16;
            }
        }
        if (move_up){
            Player.real_y -= speed;
            if (Player.real_y <= -16){
                Player.y--;
                Player.real_y += 16;
            }
        }
    }

    debug_print_inputs(renderer);

    int animSpeed = running ? 60 : 120;

    //animate walk
    if (Utils_time() - walktime >= animSpeed){
        walkbool = !walkbool;
        if (walkbool){
            Player.sprite = &NINTEN_DOWN1;
        } else {
            Player.sprite = &NINTEN_DOWN2;
        }

        walktime = Utils_time();
    }

    //get entity and generate oam from it
    OAM_from_entity(&Player, 4);

    simulate_fades();
}