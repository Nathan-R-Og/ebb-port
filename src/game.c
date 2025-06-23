#include "game.h"
#include "hardware.h"

char GENERIC_STR_NULL[1] = {0};
char* basepath;
char path[256]; //generic path memory
// Event loop exit flag
bool main_quit = false;
extern SDL_Surface* npc_sprite_table[2];

u8 palette_queue[8][4];
u8 palette_backup[8][4];

extern u8 global_bg_palette[4][4];
extern u8 global_sprite_palette[4][4];

void get_realpath(char* in){
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), in, basepath);
}

void Darken(void){
    u8 color_count = 0x1f; //amount of colors - 1
    while (color_count < 0x20){
        u8 palette = color_count / 4;
        u8 color = color_count % 4;
        u8 current_color = palette_queue[palette][color];
        current_color -= 0x10;
        bool isalpha = palette > 3 && color == 0;
        if (current_color >= 0x40){
            current_color = 0xf;
        }
        if (isalpha){
            current_color = 0xff;
        }
        palette_queue[palette][color] = current_color;
        color_count--;
    }
}

void Lighten(void){
    u8 color_count = 0x1f; //amount of colors - 1
    while (color_count < 0x20){
        u8 palette = color_count / 4;
        u8 color = color_count % 4;
        u8 current_color = palette_queue[palette][color];
        current_color += 0x10;
        bool isalpha = palette > 3 && color == 0;
        if (current_color >= palette_backup[palette][color]){
            current_color = palette_backup[palette][color];
        }
        if (isalpha){
            current_color = 0xff;
        }
        palette_queue[palette][color] = current_color;
        color_count--;
    }
}

SpriteDef NINTEN_DOWN1;
SpriteDef NINTEN_DOWN2;
Entity Player = {5,5,DIR_DOWN,&NINTEN_DOWN1,0,0};

//animation shit
bool can_control = true;
long long actual_walk_time = 0;
bool walking = false;
int speed = 1;
bool running = false;
Direction walk_dir = DIR_IN_PLACE;
bool walkbool = true;
long long walktime = 0;

extern OAM_Sprite SHADOW_OAM[];
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

extern u8 controller_pad;
extern u8 controller_pad_frame;
void CheckInput(void);

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
    stringRGBA(renderer, 5, 20, new, COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

    stringRGBA(renderer, 5, 32, chew, COLOR_LIGHT_GRAY.r, COLOR_LIGHT_GRAY.g, COLOR_LIGHT_GRAY.b, COLOR_LIGHT_GRAY.a);

}

void Normal_Game(SDL_Renderer *renderer){
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

    // Set background color
    Utils_setBackgroundColor(renderer, COLOR_DARK_GRAY);

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
}

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

u8 default_sprite_palette[4][4] = {
    {0x0f, 0x0f, 0x00, 0x30},
    {0x0f, 0x0f, 0x16, 0x37},
    {0x0f, 0x0f, 0x24, 0x37},
    {0x0f, 0x0f, 0x12, 0x37},
};

#define TILECODE_stopText 0
#define TILECODE_newLine 1
#define TILECODE_waitThenOverwrite 2
#define TILECODE_pauseText 3
#define TILECODE_t_nop 5

#define TILECODE_goto(ta) 0x4,ta
#define TILECODE_set_pos(tx, ty) 0x20,tx,ty
#define TILECODE_print_string(ta) 0x21,ta
#define TILECODE_repeatTile(ta, tb) 0x22,ta,tb
#define TILECODE_print_number(ta, tb, tc) 0x23,ta,tb,tc


u8 produced_by_tiles[] = {
    //produced by
    TILECODE_set_pos(13, 11),
    0xC8,0xC9,0xCA,0xCB,0xCD,0xCE,0xCF,
    TILECODE_newLine,
    //tail of p
    TILECODE_set_pos(13, 12),
    0xD8,
    TILECODE_newLine,
    //tail of y
    TILECODE_set_pos(19, 12),
    0xDF,
    TILECODE_newLine,

    //line
    TILECODE_set_pos(13, 13),
    TILECODE_repeatTile(0xCC, 19),
    TILECODE_newLine,

    //Nintendo
    TILECODE_set_pos(13, 15),
    0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,
    TILECODE_stopText,
};

u8 presented_by_tiles[] = {
    //presented by
    TILECODE_set_pos(13, 11),
    0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xCE,0xCF,
    TILECODE_newLine,
    //tail of p
    TILECODE_set_pos(13, 12),
    0xD8,
    TILECODE_newLine,
    //tail of y
    TILECODE_set_pos(20, 12),
    0xDF,
    TILECODE_newLine,

    //line
    TILECODE_set_pos(0, 13),
    TILECODE_repeatTile(0xCC, 21),
    TILECODE_newLine,

    //SHIGESATO ITOI
    TILECODE_set_pos(8, 15),
    0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF,
    TILECODE_stopText,
};

u8 title_screen_tiles[] = {
    //EARTHBOUND logo
    TILECODE_set_pos(8, 7),
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,
    TILECODE_newLine,
    0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,
    TILECODE_newLine,
    0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF,
    TILECODE_newLine,
    0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,
    TILECODE_newLine,
    0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,
    TILECODE_newLine,
    0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,
    TILECODE_newLine,
    0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,
    TILECODE_newLine,

    //c 1989/1990
    //SHIGESATO ITOI / NINTENDO
    TILECODE_set_pos(7, 23),
    0x43,0x44,0x45,0x46,0x47,0x70,
    0x69,0x6A,0x6B,0x6C,0x6D,0x6E,0x6F,0x53,0x54,0x55,0x56,0x57,
    TILECODE_stopText,
};


bool pallettte = false;
void update_palette(char* path);
void load_palette(u8 which, u8* data, u8 entries);
extern char INAME_PSI_STONE[];

SDL_Surface* global_bg;

void Write_to_TileMap(u8 tile, u8 x, u8 y, TILEMAP_BLOCK* tm){
    tm->tiles[x + (y * 0x20)] = tile;
}

extern TILEMAP_BLOCK TILEMAP[4];
void Eval_Tilecodes(u8* codes){
    u8 x, y;
    u8 o_x, o_y;
    u8 tile, times;
    while(true){
        switch(*codes){
            case 0: //stopText
                codes++;
                return;
            case 1: //newLine
                y++;
                x = o_x;
                codes++;
                break;
            //case 4: //goto
                //codes = (u8*) *(++codes);
                //break;
            case 0x20: //set_pos
                x = o_x = *(++codes);
                y = o_y = *(++codes) + 1;
                codes++;
                break;
            //case 0x21: //print_string
                //char* string = *(++codes);
                //break;
            case 0x22: //repeatTile
                tile = *(++codes);
                times = *(++codes);
                while (times > 0){
                    Write_to_TileMap(tile, x, y, &TILEMAP[0]);
                    times--;
                    x++;
                    if (x >= 0x20){
                        x = 0;
                        y++;
                    }
                }
                codes++;
                break;
            //case 0x23: //print_number
                //char* string = *(++codes);
                //break;
            default:
                tile = *codes++;
                Write_to_TileMap(tile, x, y, &TILEMAP[0]);
                x++;
                if (x >= 0x20){
                    x = 0;
                    y++;
                }
                break;
        }
    }
}

SDL_Color generic_grayscale_sdl_colors[4] = {
    {0,0,0,255},
    {0x66,0x66,0x66,255},
    {0xad,0xad,0xad,255},
    {0xff,0xfe,0xff,255},
};
SDL_Palette* generic_grayscale_sdl_palette;

void Title_Init(SDL_Renderer *renderer){
    renderer = renderer;
    global_bg = SDL_CreateRGBSurfaceWithFormat(0, 0x10*8, 0x10*8, 32, SDL_PIXELFORMAT_INDEX8);
    get_realpath("%simage/title.png");
    SDL_Surface* use_bg = surface_from_path(path);
    SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
    SDL_SetSurfacePalette(global_bg, generic_grayscale_sdl_palette);
    SDL_Rect temp = {0,64,0,0};
    SDL_BlitSurface(use_bg, NULL, global_bg, &temp);

    get_realpath("%simage/title_earth.png");
    use_bg = surface_from_path(path);
    SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
    npc_sprite_table[0] = use_bg;

    get_realpath("%simage/characters10.png");
    use_bg = surface_from_path(path);
    SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
    npc_sprite_table[1] = use_bg;

    Eval_Tilecodes(produced_by_tiles);
    for (u8 i = 0; i < 0x40; i++){
        TILEMAP[0].attr[i] = (2 << 6) | (2 << 4) | (2 << 2) | 2;
        //br | bl | tr | tl
    }

    load_palette(2, (u8*) &Title_Palette_Old, sizeof(Title_Palette_Old) / sizeof(u8));
    memcpy(palette_queue, global_bg_palette, 0x10); //BackupPalette
    memcpy(palette_queue[4], global_sprite_palette, 0x10); //BackupPalette
    memcpy(palette_backup, palette_queue, 0x20); //BackupPalette

}
void Palette_From_Nes(u8* id, SDL_Palette* to);
void Draw_Tilemap(SDL_Surface* insurface, TILEMAP_BLOCK* tm, SDL_Renderer *renderer){
    u8 x, y;
    SDL_Rect screenRect = {0,0,0x20*8,0x20*8};
    SDL_Surface* screen = SDL_CreateRGBSurface(0, screenRect.w, screenRect.h, 32, 0, 0, 0, 0);
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
    SDL_Texture* global_bg_t = texture_from_surface(screen, renderer);
    SDL_RenderCopy(renderer, global_bg_t, NULL, &screenRect);

}

u8 screen_inc = 0;
long long last_fade;
u8 fade_i = 0;
u8 fade_way = 0;
u16 true_delay = 30;


typedef struct PRE_oam{
    u8 tiles;
    u8 start_index;
    u8 x;
    u8 y;
    void* something;
    SpriteDef* sprite;
}PRE_oam;

PRE_oam shadow_something[32];


void OAM_from_PREoam(void){
    PRE_oam* current = shadow_something;
    for(u8 i = 0; i < 32; i++, current++){
        if (current->tiles == 0){
            continue;
        }
        OAM_from_SpriteDef(current->sprite, current->start_index, current->x, current->y+1);
    }
}

u8 earth_anim = 0;
extern SpriteDef SPRITEDEF_EARTH[];
extern SpriteTile SPRITE_EARTH[];
long long last_earth;
void Title_Process(SDL_Renderer *renderer){
    Utils_setBackgroundColor(renderer, COLOR_DARK_GRAY);

    Draw_Tilemap(global_bg, &TILEMAP[0], renderer);
    //OAM_from_entity(&Player, 4);

    if (controller_pad_frame & PAD_A){
        fade_i = 1;
        fade_way = 1;
    }

    if (fade_i > 0 && fade_i < 16){
        if (fade_i % 2 == 1){
            if (fade_way == 1){
                Darken();
            } else if (fade_way == 2){
                Lighten();
            }
            memcpy(global_bg_palette, palette_queue, 0x10); //BackupPalette
            memcpy(global_sprite_palette, palette_queue[4], 0x10); //BackupPalette
            last_fade = Utils_time();
            fade_i++;
        } else {
            if (Utils_time() - last_fade > true_delay){
                if (fade_i == 8){
                    fade_way = 0;

                    for(u16 tile = 0; tile < 0x3c0; tile++){
                        TILEMAP[0].tiles[tile] = 0;
                    }

                    SDL_Rect sheet1 = {0,0,0,0};
                    SDL_Rect sheet2 = {0,64,0,0};
                    switch(screen_inc){
                        case 0:
                            Eval_Tilecodes(presented_by_tiles);
                            break;
                        case 1:
                            get_realpath("%simage/title.png");
                            SDL_Surface* sheet1_png = surface_from_path(path);
                            SDL_SetSurfacePalette(global_bg, generic_grayscale_sdl_palette);
                            SDL_SetSurfacePalette(sheet1_png, generic_grayscale_sdl_palette);
                            SDL_BlitSurface(sheet1_png, NULL, global_bg, &sheet1);
                            get_realpath("%simage/earth_bound.png");
                            sheet1_png = surface_from_path(path);
                            SDL_SetSurfacePalette(sheet1_png, generic_grayscale_sdl_palette);
                            SDL_BlitSurface(sheet1_png, NULL, global_bg, &sheet2);

                            //get stuff set up for the earth animation
                            earth_anim = 0;
                            last_earth = Utils_time();

                            PRE_oam earth = {16, 4, 88, 87, NULL, &SPRITEDEF_EARTH[0]};
                            shadow_something[28] = earth;

                            Eval_Tilecodes(title_screen_tiles);

                            memcpy(palette_backup, Title_Palette, 0x20); //BackupPalette
                            break;
                        case 2:
                            get_realpath("%simage/title.png");
                            SDL_Surface* use_bg = surface_from_path(path);
                            SDL_SetSurfacePalette(global_bg, generic_grayscale_sdl_palette);
                            SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
                            SDL_BlitSurface(use_bg, NULL, global_bg, &sheet2);

                            shadow_something[28].tiles = 0;
                            OAM_Sprite nope = {0,0,0,0};
                            for (int i = 0; i < 64; i++){
                                SHADOW_OAM[i] = nope;
                            }

                            Eval_Tilecodes(produced_by_tiles);

                            memcpy(palette_backup, Title_Palette_Old, 0x20); //BackupPalette
                            screen_inc = 0xff;
                            break;
                    }

                    //set to lowest form of palette_backup
                    for (u8 i = 0; i < 0x20; i++){
                        u8 palette = i / 4;
                        u8 color = i % 4;
                        palette_queue[palette][color] = palette_backup[palette][color] & 0x0f;
                    }

                    screen_inc++;
                }
                else if (fade_i == 10){
                    fade_way = 2;
                }
                fade_i++;
            }
        }
    }

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

void OAM_Draw(SDL_Renderer *renderer);
bool Game_start(SDL_Renderer *renderer) {
    basepath = SDL_GetBasePath();

    update_palette("%spalettes/nes.pal");
    load_palette(1, (u8*) &default_sprite_palette, sizeof(default_sprite_palette) / sizeof(u8));

    generic_grayscale_sdl_palette = SDL_AllocPalette(4);
    SDL_SetPaletteColors(generic_grayscale_sdl_palette, generic_grayscale_sdl_colors, 0, 4);

    // Initialize framerate manager : 30 FPS
    FPSmanager fpsmanager;
    SDL_initFramerate(&fpsmanager);
    SDL_setFramerate(&fpsmanager, 60);

    Title_Init(renderer);

    // Event loop
    while(!main_quit)
    {
        CheckInput();

        if (main_quit){
            break;
        }
        Title_Process(renderer);

        OAM_Draw(renderer);
        // Update screen
        SDL_RenderPresent(renderer);
        // Delay
        SDL_framerateDelay(&fpsmanager);


    }

    return true;
}

