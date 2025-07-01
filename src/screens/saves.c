#include "types.h"
#include "sdl.h"
#include "hardware.h"
#include "data/sprites.h"
#include "data/objects.h"
#include <string.h>

//top left of the map is 0, $80. why?
typedef struct doorArgDef{
    u8 music;
    u16 targetX;
    Direction direction;
    u16 targetY;
} doorArgDef;

typedef struct Character {
    u8 unk1;
    u8 status;
    u8 unk2;
    u16 max_hp;
    u16 max_pp;
    u16 offense;
    u16 defense;
    u8 fight;
    u8 speed;
    u8 wisdom;
    u8 strength;
    u8 force;
    u8 level;
    u32 exp; //u24
    u16 hp;
    u16 pp;
    void* name_pointer; // Deprecated. literally why would i need this
    u8 unk3[4];
    SpriteDef* sprite;
    u8 items[8];
    u8 weapon;
    u8 coin;
    u8 ring;
    u8 pendant;
    doorArgDef breadcrumb_data;
    bool psi_table[64];
    char name[8];
} Character;

//initial character data
//us blanks out initial names
//jp has temp names
extern SpriteDef NINTEN_DOWN1;
Character default_party_data[] = {
    {
    0,0,0,30,8,5,5,5,5,5,5,5,1,0,30,8,NULL,
    {0,0,8,0x64},
    &NINTEN_DOWN1,
    {0x6E,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x00,0x00,0x00,0x00,

    #ifdef VER_JP
    {0, 0, DIR_UP, 0},
    #else
    {0x12, 0x21, DIR_LEFT, 0xBE},
    #endif

    {
    0,1,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    },

    #ifdef VER_JP
    //Boy A
    {0,0,0,0,0,0,0,0,},
    #else
    {0,0,0,0,0,0,0,0,},
    #endif
    },


    {
    0,0,0,26,12,1,3,1,3,7,3,8,1,0,26,12,NULL,
    {0,0,8,0x64},
    NULL,
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x00,0x00,0x00,0x00,

    #ifdef VER_JP
    {0, 0, DIR_UP, 0},
    #else
    {0x12, 0x21, DIR_LEFT, 0xBE},
    #endif

    {
    0,1,0,0,0,0,0,0, 0,1,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    },

    #ifdef VER_JP
    //Girl B
    {0,0,0,0,0,0,0,0,},
    #else
    {0,0,0,0,0,0,0,0,},
    #endif
    },

    {
    0,0,0,28,0,4,2,4,2,8,4,3,1,0,28,0,NULL,
    {0,0,8,0x64},
    NULL,
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x00,0x00,0x00,0x00,

    #ifdef VER_JP
    {0, 0, DIR_UP, 0},
    #else
    {0x12, 0x21, DIR_LEFT, 0xBE},
    #endif

    {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    },

    #ifdef VER_JP
    //Boy C
    {0,0,0,0,0,0,0,0,},
    #else
    {0,0,0,0,0,0,0,0,},
    #endif
    },

    {
    0,0,0,134,0,86,86,86,86,19,57,38,18,3600,134,0,NULL,
    {0x00,0x00,0x08,0x64},
    NULL,
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x00,0x00,0x00,0x00,

    #ifdef VER_JP
    {0, 0, DIR_UP, 0},
    #else
    {0x12, 0x21, DIR_LEFT, 0xBE},
    #endif

    {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    },

    #ifdef VER_JP
    //Boy D
    {0,0,0,0,0,0,0,0,},
    #else
    {0,0,0,0,0,0,0,0,},
    #endif
    },

    {
    0,0,0,32,0,9,9,9,9,2,6,4,1,0,32,0,NULL,
    {0x00,0x00,0x08,0x64},
    NULL,
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x00,0x00,0x00,0x00,

    #ifdef VER_JP
    {0, 0, DIR_UP, 0},
    #else
    {0x12, 0x21, DIR_LEFT, 0xBE},
    #endif

    {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    },

    #ifdef VER_JP
    //Pippi
    {0,0,0,0,0,0,0,0,},
    #else
    "Pippi  ",
    #endif
    },

    {
    0,0,0,999,0,999,999,255,255,255,255,255,99,1000000,999,0,NULL,
    {0x00,0x00,0x08,0x3a},
    NULL,
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x00,0x00,0x00,0x00,

    #ifdef VER_JP
    {0, 0, DIR_UP, 0},
    #else
    {0x12, 0x21, DIR_LEFT, 0xBE},
    #endif

    {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    },

    #ifdef VER_JP
    //EVE
    {0,0,0,0,0,0,0,0,},
    #else
    "EVE    ",
    #endif
    },

    {
    0,0,0,50,0,30,30,30,30,30,30,30,99,1000000,30,0,NULL,
    {0x00,0x00,0x08,0x64},
    NULL,
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    0x00,0x00,0x00,0x00,

    #ifdef VER_JP
    {0, 0, DIR_UP, 0},
    #else
    {0x12, 0x21, DIR_LEFT, 0xBE},
    #endif

    {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
    },

    #ifdef VER_JP
    //Flying Man
    {0,0,0,0,0,0,0,0,},
    #else
    "FlynMan",
    #endif
    },
};


typedef struct save {
    u16 save_checksum;
    u8 save_slot;
    u8 save_slot_state;
    u16 xpos_music;
    u16 ypos_direction;
    u8 party_members[4];
    u16 save_coordinates;
    u16 save_coordinates_y;
    u16 wallet_money;
    u32 bank_money; //u24
    u32 dad_money; //u24
    // message speed - stores no. of frames to lag after printing a battle msg
    // anything not named speed 5 (1 frame lag) is unbearably slow
    u8 battle_message_speed;
    #ifdef VER_JP
    // 25
    #else
    // 33
    #endif
    u8 repel_counter;
    u8 unk1[4];
    //flags? higher nybble is used for the train blockage graphics, at least
    u8 flags_maybe;
    u8 big_bag_uses;
    char player_name[17];
    u8 unk2[11];
    u8 save_ram_unk[4];
    Character character_data[7];
    bool story_flags[0x89*8];
    char favorite_food[12];
    u8 unk3[0x1b];
    u8 storage_items[0x20];
    u8 unk4[0x30];
} save;

save save_slots[3];
save current_save;


extern SDL_Surface* global_bg;
extern u8 palette_queue[8][4];
extern u8 palette_backup[8][4];
extern SDL_Palette* generic_grayscale_sdl_palette;
extern SDL_Palette* generic_transparent_sdl_palette;


u8 Block_Palette[4][4] = {
    {0x0F,0x0F,0x30,0x30},
    {0x0F,0x3A,0x10,0x20},
    {0x0F,0x3A,0x25,0x1A},
    {0x0F,0x3A,0x30,0x12}
};

//top
#define uibox_tl 0xDB
#define uibox_t 0xDC
#define uibox_tr 0xDD
#define uibox_tc 0xFE //version cut off (save menus and whatnot)

//middle
//(For real this time)
#define uibox_l_r 0xDE
#define uibox_r_r 0xDF
//middle
//(problem area)
#define uibox_l uibox_l_r //is actually $DE
#define uibox_r uibox_r_r //is actually $DF

//bottom
#define uibox_bl 0xFB
#define uibox_b 0xFC
#define uibox_br 0xFD

#define radial_empty 0x94
#define radial_filled 0x95

void switch_game_mode(u8 to);

SDL_Surface* save_slot_gfx[3];
extern u8 default_sprite_palette[4][4];
extern void (*fade_handler)(void);
void start_fade(void);
void simulate_fades(void);
u8 dest;

SDL_Texture* global_bg_blockt[3];
SDL_Rect dest_block = {4*8, 4*8, 0, 0};
u8 chosen_slot = 0;
u8 sub_option = 0;
SDL_Surface* cursor;
SDL_Surface* cursor2;
long long last_cursor;
bool cursor_show;
SDL_Rect cursor_rect;
u8 menu_layer;
SDL_Color bg_color = {0,0,0,255};

void blit_tile(SDL_Surface* from, SDL_Surface* to, u8 tile, u8 to_x, u8 to_y){
    u8 x = tile % 0x10;
    u8 y = tile / 0x10;
    SDL_Rect get = {x*8, y*8, 8, 8};
    SDL_Rect place = {to_x*8, to_y*8, 8, 8};
    SDL_BlitSurface(from, &get, to, &place);
}

char lvl_template[] = "Lvl";
char blank_save_template[] = "Start up";
char existing_save_template[] = " Continue  Copy Erase ";
char save_blanker[] = "GAME(1)";
u8 level = 99;

char* eval_charmap(char* string);

void do_save_slot(SDL_Surface* block, u8 slot){
    save* mine = &save_slots[slot];
    slot = slot;
    u8 block_width = (block->w/8);
    //construct
    u8 m_x = 0;
    u8 m_y = 0;

    blit_tile(global_bg, block, uibox_tl, m_x, m_y);
    blit_tile(global_bg, block, uibox_t, m_x+1, m_y);
    blit_tile(global_bg, block, uibox_tc, m_x+2, m_y);
    m_x = 3;

    char* str_buffer = eval_charmap(mine->character_data[0].name);
    if (str_buffer[0] != 0){
        while(*str_buffer != 0){
            blit_tile(global_bg, block, *str_buffer+0x80, m_x, m_y);
            m_x++;
            str_buffer++;
        }
        while (m_x < 3+8){
            blit_tile(global_bg, block, 0, m_x, m_y);
            m_x++;
        }
        str_buffer = lvl_template;
        while(*str_buffer != 0){
            blit_tile(global_bg, block, *str_buffer+0x80, m_x, m_y);
            m_x++;
            str_buffer++;
        }
        char str_level[] = "  ";
        snprintf(str_level, sizeof(str_level), "%i", level);
        str_buffer = eval_charmap(str_level);
        while(*str_buffer != 0){
            blit_tile(global_bg, block, *str_buffer, m_x, m_y);
            m_x++;
            str_buffer++;
        }
        while(m_x < block_width-1){
            blit_tile(global_bg, block, uibox_t, m_x, m_y);
            m_x++;
        }
        blit_tile(global_bg, block, uibox_tr, m_x, m_y);
        m_y++;
        m_x = 0;

        //spacer??
        blit_tile(global_bg, block, uibox_l, 0, m_y);
        blit_tile(global_bg, block, uibox_r, block_width-1, m_y);
        m_y++;

        //continue block
        blit_tile(global_bg, block, uibox_l, m_x, m_y);
        m_x = 1;
        str_buffer = eval_charmap(existing_save_template);
        while(*str_buffer != 0){
            blit_tile(global_bg, block, *str_buffer, m_x, m_y);
            m_x++;
            str_buffer++;
        }
        while (m_x < block_width-1){
            blit_tile(global_bg, block, 0, m_x, m_y);
            m_x++;
        }
        blit_tile(global_bg, block, uibox_r, m_x, m_y);
    } else {
        //blank file
        char usar[] = " ";
        sprintf(usar, "%d", slot+1);
        save_blanker[5] = usar[0];
        str_buffer = eval_charmap(save_blanker);
        while(*str_buffer != 0){
            blit_tile(global_bg, block, *str_buffer, m_x, m_y);
            m_x++;
            str_buffer++;
        }
        while(m_x < block_width-1){
            blit_tile(global_bg, block, uibox_t, m_x, m_y);
            m_x++;
        }
        blit_tile(global_bg, block, uibox_tr, m_x, m_y);
        m_y++;
        m_x = 0;

        //spacer??
        blit_tile(global_bg, block, uibox_l, 0, m_y);
        blit_tile(global_bg, block, uibox_r, block_width-1, m_y);
        m_y++;

        //continue block
        blit_tile(global_bg, block, uibox_l, m_x, m_y);
        m_x = 1;
        while (m_x < 7){
            blit_tile(global_bg, block, 0, m_x, m_y);
            m_x++;
        }
        str_buffer = eval_charmap(blank_save_template);
        while(*str_buffer != 0){
            blit_tile(global_bg, block, *str_buffer, m_x, m_y);
            m_x++;
            str_buffer++;
        }
        while (m_x < block_width-1){
            blit_tile(global_bg, block, 0, m_x, m_y);
            m_x++;
        }
        blit_tile(global_bg, block, uibox_r, m_x, m_y);
    }



    m_y++;
    m_x = 0;
    blit_tile(global_bg, block, uibox_bl, m_x, m_y);
    m_x++;
    while(m_x < block_width-1){
        blit_tile(global_bg, block, uibox_b, m_x, m_y);
        m_x++;
    }
    blit_tile(global_bg, block, uibox_br, m_x, m_y);

}

char alphabet_chart[] =
    "ABCDEFG HIJKLMN\n"
    "OPQRSTU VWXYZ.'\n"
    "abcdefg hijklmn\n"
    "opqrstu vwxyz-:\n";

void do_namings_panel(SDL_Surface* block){
    u8 block_width = (block->w/8);

    //construct
    u8 m_x = 2;
    u8 m_y = 0;

    //question panel
    blit_tile(global_bg, block, uibox_tl, m_x, m_y);
    m_x++;
    while(m_x < block_width-3){
        blit_tile(global_bg, block, uibox_t, m_x, m_y);
        m_x++;
    }
    blit_tile(global_bg, block, uibox_tr, m_x, m_y);
    m_x=2;
    m_y++;
    for(u8 i = 0; i<5; i++){
        blit_tile(global_bg, block, uibox_l, m_x, m_y);
        m_x++;
        while(m_x < block_width-3){
            blit_tile(global_bg, block, 0, m_x, m_y);
            m_x++;
        }
        blit_tile(global_bg, block, uibox_r, m_x, m_y);
        m_x=2;
        m_y++;
    }
    blit_tile(global_bg, block, uibox_bl, m_x, m_y);
    m_x++;
    while(m_x < block_width-3){
        blit_tile(global_bg, block, uibox_b, m_x, m_y);
        m_x++;
    }
    blit_tile(global_bg, block, uibox_br, m_x, m_y);


    //answer panel
    m_x = 0;
    blit_tile(global_bg, block, uibox_tl, m_x, m_y);
    m_x++;
    blit_tile(global_bg, block, uibox_t, m_x, m_y);
    m_x = block_width - 2;
    blit_tile(global_bg, block, uibox_t, m_x, m_y);
    m_x++;
    blit_tile(global_bg, block, uibox_tr, m_x, m_y);

    m_x=0;
    m_y++;
    for(u8 i = 0; i<17; i++){
        blit_tile(global_bg, block, uibox_l, m_x, m_y);
        m_x++;
        while(m_x < block_width-1){
            blit_tile(global_bg, block, 0, m_x, m_y);
            m_x++;
        }
        blit_tile(global_bg, block, uibox_r, m_x, m_y);
        m_x=0;
        m_y++;
    }
    blit_tile(global_bg, block, uibox_bl, m_x, m_y);
    m_x++;
    while(m_x < block_width-1){
        blit_tile(global_bg, block, uibox_b, m_x, m_y);
        m_x++;
    }
    blit_tile(global_bg, block, uibox_br, m_x, m_y);

    m_x = 4;
    m_y = 2;
    u8 o_x = m_x;

    char use[] = "What is this\nboy's name?";
    char* str_buffer = eval_charmap(use);
    while(*str_buffer != 0){
        if (*str_buffer == '\n'){
            m_x = o_x;
            m_y += 2;
        } else {
            blit_tile(global_bg, block, *str_buffer, m_x, m_y);
            m_x++;
        }
        str_buffer++;
    }

    m_x = 3;
    m_y = 12;
    o_x = m_x;

    char use2[0x100] = "";
    strcpy(use2, alphabet_chart);
    char footer[] =
    "  ◆Back  ◆End\n"
    "   ◆Previous";
    strcat(use2, footer);

    str_buffer = eval_charmap(use2);
    while(*str_buffer != 0){
        if (*str_buffer == '\n'){
            m_x = o_x;
            m_y += 2;
        } else {
            blit_tile(global_bg, block, *str_buffer, m_x, m_y);
            m_x++;
        }
        str_buffer++;
    }
}

void fade_handler_saves(void){
    for(u16 tile = 0; tile < 0x3c0; tile++){
        TILEMAP[0].tiles[tile] = 0;
    }
    menu_layer = dest;
    switch(menu_layer){
        case 0:
            break;
        case 1:
            break;
    }

    last_cursor = 0;
}

SDL_Surface* naming_panel_temp;
void Block_Init(void){
    fade_handler = &fade_handler_saves;

    SDL_SetSurfacePalette(global_bg, generic_grayscale_sdl_palette);

    get_realpath("%simage/ui.png");
    SDL_Surface* use_bg = surface_from_path(path);
    SDL_SetSurfacePalette(use_bg, generic_grayscale_sdl_palette);
    SDL_BlitSurface(use_bg, NULL, global_bg, NULL);
    SDL_FreeSurface(use_bg);

    //oam
    char* oam_tables[] = {
        "%simage/characters1.png",
        "%simage/characters10.png"
    };
    for (u8 i = 0; i < 2; i++){
        get_realpath(oam_tables[i]);
        use_bg = surface_from_path(path);
        //SDL_SetSurfacePalette(use_bg, generic_transparent_sdl_palette);
        npc_sprite_table[i] = use_bg;
    }

    memcpy(palette_backup, Block_Palette, 0x10); //BackupPalette
    memcpy(palette_backup[4], default_sprite_palette, 0x10); //BackupPalette

    for (u8 i = 0; i < sizeof(save_slot_gfx) / sizeof(SDL_Surface*); i++){
        save_slot_gfx[i] = SDL_CreateRGBSurfaceWithFormat(0, 24*8, 4*8, 8, SDL_PIXELFORMAT_INDEX8);
        SDL_SetSurfacePalette(save_slot_gfx[i], generic_grayscale_sdl_palette);
        SDL_Palette* oopsies = SDL_AllocPalette(4);
        Palette_From_Nes(palette_backup[0], oopsies);
        SDL_SetSurfacePalette(save_slot_gfx[i], oopsies);
        SDL_FreePalette(oopsies);

        do_save_slot(save_slot_gfx[i], i);
    }

    cursor = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 8, SDL_PIXELFORMAT_INDEX8);//SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 8, SDL_PIXELFORMAT_INDEX8);
    SDL_SetSurfacePalette(cursor, generic_transparent_sdl_palette);
    //this is REQUIRED on a new surface creation. IMG_load does this automatically.
    //geeee !!!! if only something told me that :DDDDDDDD
    SDL_SetColorKey(cursor, true, 0);
    blit_tile(npc_sprite_table[0], cursor, 0x3a, 0, 0);

    cursor2 = SDL_CreateRGBSurfaceWithFormat(0, 8, 8, 8, SDL_PIXELFORMAT_INDEX8);
    SDL_SetSurfacePalette(cursor2, generic_transparent_sdl_palette);
    SDL_SetColorKey(cursor2, true, 0);
    blit_tile(npc_sprite_table[0], cursor2, 1, 0, 0);

    naming_panel_temp = SDL_CreateRGBSurfaceWithFormat(0, 21*8, 26*8, 8, SDL_PIXELFORMAT_INDEX8);
    SDL_SetSurfacePalette(naming_panel_temp, generic_grayscale_sdl_palette);
    do_namings_panel(naming_panel_temp);
}

u8 letter_selector = 0;
u8 current_name = 0;
u8 current_letter = 0;
char name_buffer[128] = "";
void name_entry(SDL_Renderer* renderer){
    SDL_Palette* current_palette[8];
    for(u8 i = 0; i < 8; i++){
        current_palette[i] = SDL_AllocPalette(4);
        if (i < 4){
            Palette_From_Nes(global_bg_palette[i], current_palette[i]);
        } else {
            Palette_From_Nes(global_sprite_palette[i%4], current_palette[i]);
        }
    }

    SDL_SetSurfacePalette(naming_panel_temp, current_palette[0]);
    SDL_Texture* use_texture = texture_from_surface(naming_panel_temp, renderer);
    dest_block.w = 21*8;
    dest_block.h = 26*8;
    dest_block.x = 5*8;
    dest_block.y = 2*8;
    SDL_RenderCopy(renderer, use_texture, NULL, &dest_block);
    SDL_DestroyTexture(use_texture);

    cursor_rect = dest_block;
    cursor_rect.w = 8;
    cursor_rect.h = 8;
    cursor_rect.x = (8+(letter_selector%0x10))*8;
    cursor_rect.y = (14+((letter_selector/0x10)*2))*8;

    if (Utils_time() - last_cursor > 300){
        cursor_show = !cursor_show;
        last_cursor = Utils_time();
    }
    if (cursor_show){
        SDL_SetSurfacePalette(cursor2, current_palette[4]);
        use_texture = texture_from_surface(cursor2, renderer);
        SDL_RenderCopy(renderer, use_texture, NULL, &cursor_rect);
        SDL_DestroyTexture(use_texture);
    }


    char* getc_current;
    u8 gcc_size = 0;
    switch(current_name){
        default:
            getc_current = save_slots[chosen_slot].character_data[current_name].name;
            gcc_size = sizeof(save_slots[chosen_slot].character_data[current_name].name);
            break;
    }
    memset(name_buffer, '?', gcc_size-1);
    for(u8 i = 0; *getc_current != 0 && i < gcc_size; i++, getc_current++){
        name_buffer[i] = *getc_current;
    }

    //this is also insanely hacky. mostly just because idgaf rn
    u8 z_x = 12;
    u8 z_y = 11;
    SDL_Rect placer = {0,0,8,8};
    SDL_Rect hi = {0,0,8,8};
    char* str_buffer = eval_charmap(name_buffer);
    use_texture = texture_from_surface(global_bg, renderer);
    //this is basically just blittile
    while(*str_buffer != 0){
        placer.x = z_x * 8;
        placer.y = z_y * 8;
        u8 tiler = (u8) *str_buffer;
        hi.x = (tiler % 0x10) * 8;
        hi.y = (tiler / 0x10) * 8;
        SDL_RenderCopy(renderer, use_texture, &hi, &placer);
        z_x++;
        str_buffer++;
    }
    SDL_DestroyTexture(use_texture);


    for(u8 i = 0; i < 8; i++){
        SDL_FreePalette(current_palette[i]);
    }

    //this is insanely hacky. please make an actual choicer system LOL
    u8 actual_size = sizeof(alphabet_chart) - 0x11;
    if (letter_selector >= actual_size){
        if ((letter_selector & 0xF0) == 0x30){ //last actual row
            if (controller_pad_frame & PAD_DOWN){
                if ((letter_selector & 0x0F) < 6){
                    letter_selector = 0x42;
                } else {
                    letter_selector = 0x49;
                }
            }
            else if (controller_pad_frame & PAD_UP){
                letter_selector -= 0x10;
            }
            else if (controller_pad_frame & PAD_RIGHT){
                if ((letter_selector & 0xf) < 14){
                    letter_selector++;
                }
            }
            else if (controller_pad_frame & PAD_LEFT){
                if ((letter_selector & 0xf) > 0){
                    letter_selector--;
                }
            }
        } else if ((letter_selector & 0xF0) == 0x40){ //back previous
            if (controller_pad_frame & PAD_DOWN){
                letter_selector = 0x53;
            }
            else if (controller_pad_frame & PAD_UP){
                letter_selector -= 0x10;
            }
            else if (controller_pad_frame & PAD_RIGHT){
                letter_selector = 0x49;
            }
            else if (controller_pad_frame & PAD_LEFT){
                letter_selector = 0x42;
            }
        } else if ((letter_selector & 0xF0) == 0x50){ //back previous
            if (controller_pad_frame & PAD_UP){
                letter_selector = 0x42;
            }
        }
    } else {
        if (controller_pad_frame & PAD_DOWN){
            letter_selector += 0x10;
        }
        else if (controller_pad_frame & PAD_UP){
            letter_selector -= 0x10;
        }
        else if (controller_pad_frame & PAD_RIGHT){
            if ((letter_selector & 0xf) < 14){
                letter_selector++;
            }
        }
        else if (controller_pad_frame & PAD_LEFT){
            if ((letter_selector & 0xf) > 0){
                letter_selector--;
            }
        }
    }

    if (controller_pad_frame & PAD_A){
        if (letter_selector == 0x42){
            //back (do the same thing as PAD_B)
        }
        else if (letter_selector == 0x49){
            //end (do the same thing as PAD_START)
        }
        else if (letter_selector == 0x53){
            //previous
            if (current_name == 0){
                sub_option = 0;
                dest = 0;
                start_fade();
            } else {
                current_name--;
            }
        } else {
            //actually input letter
            char get = alphabet_chart[letter_selector];
            char* get_current;
            u8 gc_size = 0;
            switch(current_name){
                default:
                    get_current = save_slots[chosen_slot].character_data[current_name].name;
                    gc_size = sizeof(save_slots[chosen_slot].character_data[current_name].name);
                    break;
            }
            get_current[current_letter] = get;
            current_letter++;
            if (current_letter >= gc_size-1){
                current_letter = gc_size;
            }
            printf("%s\n", get_current);
            printf("%i\n", gc_size);
        }
    }

    if (controller_pad_frame & PAD_START){
        letter_selector = 0;
    }

    if (controller_pad){
        cursor_show = true;
        last_cursor = Utils_time();
    }

}

void save_select(SDL_Renderer* renderer){
    SDL_Palette* current_palette[8];
    for(u8 i = 0; i < 8; i++){
        current_palette[i] = SDL_AllocPalette(4);
        if (i < 4){
            Palette_From_Nes(global_bg_palette[i], current_palette[i]);
        } else {
            Palette_From_Nes(global_sprite_palette[i%4], current_palette[i]);
        }
    }


    /*printf("%i\n", default_sprite_palette[0][0]);
    printf("%i\n", default_sprite_palette[0][1]);
    printf("%i\n", default_sprite_palette[0][2]);
    printf("%i\n", default_sprite_palette[0][3]);
    printf("%i\n", global_sprite_palette[0][0]);
    printf("%i\n", global_sprite_palette[0][1]);
    printf("%i\n", global_sprite_palette[0][2]);
    printf("%i\n", global_sprite_palette[0][3]);
    printf("funny\n");*/


    for (u8 i = 0; i < sizeof(save_slot_gfx) / sizeof(SDL_Surface*); i++){
        SDL_SetSurfacePalette(save_slot_gfx[i], current_palette[0]);

        SDL_Texture* use_texture = texture_from_surface(save_slot_gfx[i], renderer);
        dest_block.w = save_slot_gfx[i]->w;
        dest_block.h = save_slot_gfx[i]->h;
        dest_block.y = (4*8)+(i*6*8);
        if (i == chosen_slot){
            if (save_slots[i].character_data[0].name[0] == 0){
                cursor_rect = dest_block;
                cursor_rect.w = 8;
                cursor_rect.h = 8;
                cursor_rect.x += 6*8;
                cursor_rect.y += 2*8;
            }
            cursor_rect = dest_block;
            cursor_rect.w = 8;
            cursor_rect.h = 8;
            cursor_rect.x += 6*8;
            cursor_rect.y += 2*8;
        }
        SDL_RenderCopy(renderer, use_texture, NULL, &dest_block);
        SDL_DestroyTexture(use_texture);
    }

    if (Utils_time() - last_cursor > 300){
        cursor_show = !cursor_show;
        last_cursor = Utils_time();
    }
    if (cursor_show){
        SDL_SetSurfacePalette(cursor, current_palette[4]);
        SDL_Texture* use_cursor = texture_from_surface(cursor, renderer);
        SDL_RenderCopy(renderer, use_cursor, NULL, &cursor_rect);
        SDL_DestroyTexture(use_cursor);
    }

    for(u8 i = 0; i < 8; i++){
        SDL_FreePalette(current_palette[i]);
    }


    if (controller_pad_frame & PAD_UP && controller_pad_frame & PAD_DOWN){
    }
    else if (controller_pad_frame & PAD_DOWN){
        chosen_slot++;
        if (chosen_slot > 2){
            chosen_slot = 2;
        }
    } else if (controller_pad_frame & PAD_UP){
        chosen_slot--;
        if (chosen_slot > 2){
            chosen_slot = 0;
        }
    }

    if (controller_pad){
        cursor_show = true;
        last_cursor = Utils_time();
    }

    if (controller_pad_frame & (PAD_A | PAD_START)){
        sub_option = 0;
        dest = 1;
        start_fade();
    }
}

void Block_Process(SDL_Renderer* renderer){
    Utils_setBackgroundColor(renderer, bg_color);

    switch(menu_layer){
        case 0:
            save_select(renderer);
            break;
        case 1:
            name_entry(renderer);
            break;
    }

    simulate_fades();

}
