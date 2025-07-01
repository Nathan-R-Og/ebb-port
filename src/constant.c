#include "types.h"
#include "sdl.h"
#include "hardware.h"


long long last_fade;
u8 fade_i = 0;
u8 fade_way = 0;
u16 true_delay = 30;
u8 palette_queue[8][4];
u8 palette_backup[8][4];

void (*fade_handler)(void);

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

void start_fade(void){
    fade_i = 1;
    fade_way = 1;
}

void simulate_fades(void){
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
                if (fade_i == 8 && fade_handler){
                    fade_way = 0;
                    (*fade_handler)();
                    //set to lowest form of palette_backup
                    for (u8 i = 0; i < 0x20; i++){
                        u8 palette = i / 4;
                        u8 color = i % 4;
                        palette_queue[palette][color] = palette_backup[palette][color] & 0x0f;
                    }
                }
                else if (fade_i == 10){
                    fade_way = 2;
                }
                fade_i++;
            }
        }
    }
}

typedef struct mapping{
    char character[4];
    u8 tile;
} mapping;

mapping mappings[] = {
    {" ", 0xA0},
    {"!", 0xA1},
    {"?", 0xA2},
    {"$", 0xA4},
    {"\"", 0xA6},
    {"'", 0xA7},
    {"(", 0xA8},
    {")", 0xA9},
    {":", 0xAA},
    {";", 0xAB},
    {",", 0xAC},
    {"-", 0xAD},
    {".", 0xAE},
    {"/", 0xAF},
    {"0", 0xB0},
    {"1", 0xB1},
    {"2", 0xB2},
    {"3", 0xB3},
    {"4", 0xB4},
    {"5", 0xB5},
    {"6", 0xB6},
    {"7", 0xB7},
    {"8", 0xB8},
    {"9", 0xB9},
    {"α", 0xBB},
    {"β", 0xBC},
    {"γ", 0xBD},
    {"π", 0xBE},
    {"Ω", 0xBF},
    {"◆", 0xC0},
    {"A", 0xC1},
    {"B", 0xC2},
    {"C", 0xC3},
    {"D", 0xC4},
    {"E", 0xC5},
    {"F", 0xC6},
    {"G", 0xC7},
    {"H", 0xC8},
    {"I", 0xC9},
    {"J", 0xCA},
    {"K", 0xCB},
    {"L", 0xCC},
    {"M", 0xCD},
    {"N", 0xCE},
    {"O", 0xCF},
    {"P", 0xD0},
    {"Q", 0xD1},
    {"R", 0xD2},
    {"S", 0xD3},
    {"T", 0xD4},
    {"U", 0xD5},
    {"V", 0xD6},
    {"W", 0xD7},
    {"X", 0xD8},
    {"Y", 0xD9},
    {"Z", 0xDA},
    {"a", 0xE1},
    {"b", 0xE2},
    {"c", 0xE3},
    {"d", 0xE4},
    {"e", 0xE5},
    {"f", 0xE6},
    {"g", 0xE7},
    {"h", 0xE8},
    {"i", 0xE9},
    {"j", 0xEA},
    {"k", 0xEB},
    {"l", 0xEC},
    {"m", 0xED},
    {"n", 0xEE},
    {"o", 0xEF},
    {"p", 0xf0},
    {"q", 0xf1},
    {"r", 0xf2},
    {"s", 0xf3},
    {"t", 0xf4},
    {"u", 0xf5},
    {"v", 0xf6},
    {"w", 0xf7},
    {"x", 0xf8},
    {"y", 0xf9},
    {"z", 0xfA},
};
char string_temper[0x400];

char* eval_charmap(char* string){
    memset(string_temper, 0, sizeof(string_temper));
    //char alpha[] = "α";
    u64 string_i = 0;
    while (*string != 0){
        if (*string == '\n'){
            string_temper[string_i] = '\n';
            string++;
            string_i++;
            continue;
        }
        char test[5] = {0,0,0,0,0};
        bool found = false;
        for (u8 i = 0; i < 4; i++){
            test[i] = *string;
            mapping* m_i = mappings;
            for(u16 x = 0; x < sizeof(mappings) / sizeof(mapping); x++){
                if(strcmp(test,m_i->character) == 0) {
                    string_temper[string_i] = m_i->tile;
                    found = true;
                    break;
                }
                m_i++;
            }
            if (found){
                break;
            }
            string++;
        }
        string++;
        string_i++;
    }
    return string_temper;
}