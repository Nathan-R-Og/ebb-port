#ifndef DATA_ENEMIES_H
#define DATA_ENEMIES_H
#include "types.h"
//the only difference between jp and us stats is whether or not
//unused slots have blank names
//enemy graphics are divided between every 0x400 bytes.
//its exactly the same size as a tileset. uses the same system? probably
//therefore, some are shared. will be noted accordingly (with _A_)
//these numbers are just n = (ADDR - CHR_START) / 0x400
// (same as tilesets)
typedef enum ENEMY_GRAPHICS {
    EGFX_BIGWOODOH = 0x20,
    EGFX_DRAGON,
    EGFX_R703,
    EGFX_ELEPHANT,
    EGFX_GIEGUE,
    EGFX_BIG_FOOT,
    EGFX_TRUCK_A_GROUCHO,
    EGFX_ROBOT_A_FOUREYES,
    EGFX_BORG_A_UFO,
    EGFX_GARGOYLE_A_BAT,
    EGFX_GABILAN_A_SPIDER,
    EGFX_BEAR_A_FLY,
    EGFX_BISON_A_ENERGY,
    EGFX_TIGER_A_SKUNK,
    EGFX_FISH_A_CAR,
    EGFX_ARMOR_A_TEDDY,
    EGFX_WOODOH_A_MAGIC_SNAIL,
    EGFX_BARBOT_A_CROCODILE,
    EGFX_NANCY_A_GORILLA,
    EGFX_STARMAN_A_TITANIAN,
    EGFX_MOOK_A_SHROUDLEY,
    EGFX_FUGITIVE_A_EAGLE,
    EGFX_ULLRICH_A_DOG,
    EGFX_TEDDY_A_SEAGULL,
    EGFX_CEREBRUM_A_PILLOW_A_DOLL_A_LAMP,
    EGFX_HIPPIE_A_WALLY_A_FIREBALL,
    EGFX_BAGLADY_A_CENTIPEDE_A_GHOST,
    EGFX_DUSTBALL_A_BBGANG_A_BOMBER,
    EGFX_GANGZOMBIE_A_ZOMBIE_A_STARMANJR,
    EGFX_COUGAR_A_SNAKE_A_SCORPION,
    EGFX_HYENA_A_CROW_A_RAT,
    EGFX_DRDISTORTO_A_BETADOLL_A_ROPE,
} ENEMY_GRAPHICS;

#define ENEMY_FLAGS_NONE 0
#define ENEMY_FLAGS_SPRAYS (1 << 0)
#define ENEMY_FLAGS_R_PK_BEAM (1 << 1)
#define ENEMY_FLAGS_R_DEFDOWN_CONFSLEEP (1 << 2)
#define ENEMY_FLAGS_UNK (1 << 3)
#define ENEMY_FLAGS_R_PK_THUNDER (1 << 4)
#define ENEMY_FLAGS_R_PK_FREEZE (1 << 5)
#define ENEMY_FLAGS_R_PK_FIRE (1 << 6)
#define ENEMY_FLAGS_GOD (1 << 7)

typedef struct enemyDef {
    u8 unk0;
    u8 initStatus;
    u8 flags;
    //ggggsshhhhhhhhhh
    u16 health;
    u8 subpal;
    u8 gtile;
    //ggggggpppppppppp
    u16 pp;
    u8 gfx;
    //aaafffoooooooooo
    u16 offense;
    u8 finalAction;
    u8 altitude;
    //uuummmdddddddddd
    u16 defense;
    u8 messageDefeat;
    u8 unkParam;
    u8 fight;
    u8 speed;
    u8 wisdom;
    u8 strength;
    u8 force;
    u8 battleAction[8];
    char* name;
    u16 exp;
    u16 money;
    u8 item;
    ENEMY_GRAPHICS graphic;
} enemyDef;

//https://datacrystal.tcrf.net/wiki/EarthBound_Beginnings/Enemy_data
//enemy health can range from 0-$3ff (inclusive)
//enemy pp can range from 0-$3ff (inclusive)
//enemy offense can range from 0-$3ff (inclusive)
//enemy defense can range from 0-$3ff (inclusive)
#endif // DATA_ENEMIES_H