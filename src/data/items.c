#include <stdio.h>
#include <stdbool.h>
#include "types.h"


char INAME_BIG_BAG[] = "Big Bag";
char INAME_PHONE_CARD[] = "Phone Card";
char INAME_CRUMBS[] = "Crumbs";
char INAME_REPEL_RING[] = "Repel Ring";
char INAME_BUTTERKNIFE[] = "ButterKnife";
char INAME_SURV_KNIFE[] = "Surv.Knife";
char INAME_SWORD[] = "Sword";
char INAME_KATANA[] = "Katana";
char INAME_STUN_GUN[] = "Stun Gun";
char INAME_AIR_GUN[] = "Air Gun";
char INAME_PLASTIC_BAT[] = "Plastic Bat";
char INAME_WOODEN_BAT[] = "Wooden Bat";
char INAME_ALUMINUMBAT[] = "AluminumBat";
char INAME_HANKS_BAT[] = "Hank's Bat";
char INAME_FRYING_PAN[] = "Frying Pan";
char INAME_NONSTICKPAN[] = "NonstickPan";
char INAME_IRONSKILLET[] = "IronSkillet";
char INAME_SLINGSHOT[] = "Slingshot";
char INAME_BOOMERANG[] = "Boomerang";
char INAME_INSECTICIDE[] = "Insecticide";
char INAME_SUPER_SPRAY[] = "Super Spray";
char INAME_FLEA_BAG[] = "Flea Bag";
char INAME_WORDSOLOVE[] = "WordsO'Love";
char INAME_SWEAR_WORDS[] = "Swear Words";
char INAME_STKYMACHINE[] = "StkyMachine";
char INAME_FLASHDARK[] = "Flashdark";
char INAME_STONEORIGIN[] = "StoneOrigin";
char INAME_POISNNEEDLE[] = "PoisnNeedle";
char INAME_FL_THROWER[] = "Fl Thrower";
char INAME_BOMB[] = "Bomb";
char INAME_SUPER_BOMB[] = "Super Bomb";
char INAME_LASER_BEAM[] = "Laser Beam";
char INAME_PLASMA_BEAM[] = "Plasma Beam";
char INAME_ROPE[] = "Rope";
char INAME_PEACE_COIN[] = "Peace Coin";
char INAME_PROTECTCOIN[] = "ProtectCoin";
char INAME_MAGIC_COIN[] = "Magic Coin";
char INAME_BRASS_RING[] = "Brass Ring";
char INAME_SILVER_RING[] = "Silver Ring";
char INAME_GOLD_RING[] = "Gold Ring";
char INAME_H2O_PENDANT[] = "H2o Pendant";
char INAME_FIREPENDANT[] = "FirePendant";
char INAME_EARTHPENDNT[] = "EarthPendnt";
char INAME_SEA_PENDANT[] = "Sea Pendant";
char INAME_ORANGEJUICE[] = "OrangeJuice";
char INAME_FRENCHFRIES[] = "FrenchFries";
char INAME_MAGIC_HERB[] = "Magic Herb";
char INAME_HAMBURGER[] = "Hamburger";
char INAME_SPROTSDRINK[] = "SportsDrink";
char INAME_LIFEUPCREAM[] = "LifeUpCream";
char INAME_ASTHMASPRAY[] = "AsthmaSpray";
char INAME_ANTIDOTE[] = "Antidote";
char INAME_MOUTHWASH[] = "Mouthwash";
char INAME_BERRY_TOFU[] = "berry Tofu";
char INAME_BREAD[] = "Bread";
char INAME_NOBLE_SEED[] = "Noble Seed";
char INAME_PSI_STONE[] = "PSI Stone";
char INAME_MAGICRIBBON[] = "MagicRibbon";
char INAME_MAGIC_CANDY[] = "Magic Candy";
char INAME_QUICKCAPSUL[] = "QuickCapsul";
char INAME_WISDOM_CAPS[] = "Wisdom Caps";
char INAME_PHYSICALCAP[] = "PhysicalCap";
char INAME_FORCECAPSUL[] = "ForceCapsul";
char INAME_FIGHTCAPSUL[] = "FightCapsul";
char INAME_BASEMENTKEY[] = "BasementKey";
char INAME_ZOO_KEY[] = "Zoo Key";
char INAME_GHOST_KEY[] = "Ghost Key";
char INAME_GGFS_DIARY[] = "GGF's Diary";
char INAME_PASS[] = "Pass";
char INAME_TICKET[] = "Ticket";
char INAME_CANARYCHICK[] = "CanaryChick";
char INAME_BOTTLROCKET[] = "BottlRocket";
char INAME_HAT[] = "Hat";
char INAME_DENTURES[] = "Dentures";
char INAME_TICKET_STUB[] = "Ticket Stub";
char INAME_IC_CHIP[] = "IC-chip";
char INAME_OCARINA[] = "Ocarina";
char INAME_FRANKLNBDGE[] = "FranklnBdge";
char INAME_FRNDSHPRING[] = "FrndshpRing";
char INAME_ONYX_HOOK[] = "Onyx Hook";
char INAME_LAST_WEAPON[] = "Last Weapon";
char INAME_RULER[] = "Ruler";
char INAME_CASH_CARD[] = "Cash Card";
char INAME_RED_WEED[] = "Red Weed";
char INAME_BULLHORN[] = "Bullhorn";
char INAME_MAP[] = "Map";
char INAME_DEBUG[] = "Debug";



//the main table of items to use
//the only real differences between jp and us is the name stuff again
//and the repel ring, obviously

#define ITEMUSE_NINTEN 1
#define ITEMUSE_ANA (1 << 1)
#define ITEMUSE_LLOYD (1 << 2)
#define ITEMUSE_TEDDY (1 << 3)
//guesses????
#define ITEMUSE_PIPPI (1 << 4)
#define ITEMUSE_EVE (1 << 5)
#define ITEMUSE_FLYINGMAN 0
//
#define ITEMUSE_ALL ITEMUSE_NINTEN | ITEMUSE_ANA | ITEMUSE_LLOYD | ITEMUSE_TEDDY | ITEMUSE_PIPPI | ITEMUSE_EVE | ITEMUSE_FLYINGMAN

typedef struct itemDef {
    char* name;
    u8 usableBy;
    bool edible;
    bool keyitem;
    u8 effectValue;
    u8 armorType;
    u8 overworldAction;
    u8 battleAction;
    u16 msgPrice;
} itemDef;

itemDef Item_Data[] = {
{GENERIC_STR_NULL, 0, 0, 0, 0, 0, 0, 0, 0},
{INAME_BIG_BAG, ITEMUSE_ALL, 0, 0, 0, 0, 0x10, 0x75, 0},
{INAME_PHONE_CARD, ITEMUSE_ALL, 0, 0, 0, 0, 0x07, 0x00, 50},
{INAME_CRUMBS, ITEMUSE_ALL, 0, 0, 0, 0, 0x1F, 0x00, 0},
#ifdef VER_JP
{INAME_UNKITEM4, 0, 0, 0, 0, 0, 0x00, 0x00, 40},
{INAME_UNKITEM5, 0, 0, 0, 0, 0, 0x00, 0x00, 50},
{INAME_UNKITEM6, 0, 0, 0, 0, 0, 0x00, 0x00, 60},
{INAME_UNKITEM7, 0, 0, 0, 0, 0, 0x00, 0x00, 70},
{INAME_UNKITEM8, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM9, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_REPEL_RING, ITEMUSE_ALL, 0, 0, 0, 0, 0x08, 0x00, 160},
{INAME_BUTTERKNIFE, 0, 0, 0, 0, 0, 0x00, 0x00, 50},
{INAME_BUTTERKNIFE, 0, 0, 0, 0, 0, 0x00, 0x00, 60},
{INAME_BUTTERKNIFE, 0, 0, 0, 0, 0, 0x00, 0x00, 70},
{INAME_BUTTERKNIFE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_BUTTERKNIFE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_BUTTERKNIFE, ITEMUSE_TEDDY, 0, 0, 15, 0, 0x02, 0x00, 580},
{INAME_SURV_KNIFE, ITEMUSE_TEDDY, 0, 0, 38, 0, 0x02, 0x00, 1200},
{INAME_SWORD, ITEMUSE_TEDDY, 0, 0, 46, 0, 0x02, 0x00, 1280},
{INAME_KATANA, ITEMUSE_TEDDY, 0, 0, 58, 0, 0x02, 0x00, 1360},
{INAME_STUN_GUN, ITEMUSE_LLOYD, 0, 0, 15, 0, 0x02, 0x00, 300},
{INAME_AIR_GUN, ITEMUSE_LLOYD, 0, 0, 42, 0, 0x02, 0x00, 1400},
{INAME_PLASTIC_BAT, ITEMUSE_NINTEN, 0, 0, 3, 0, 0x02, 0x00, 80},
{INAME_WOODEN_BAT, ITEMUSE_NINTEN, 0, 0, 12, 0, 0x02, 0x00, 500},
{INAME_ALUMINUMBAT, ITEMUSE_NINTEN, 0, 0, 30, 0, 0x02, 0x00, 1000},
{INAME_HANKS_BAT, ITEMUSE_NINTEN, 0, 0, 48, 0, 0x02, 0x00, 1400},
{INAME_FRYING_PAN, ITEMUSE_ANA, 0, 0, 8, 0, 0x02, 0x00, 300},
{INAME_NONSTICKPAN, ITEMUSE_ANA, 0, 0, 16, 0, 0x02, 0x00, 700},
{INAME_IRONSKILLET, ITEMUSE_ANA, 0, 0, 36, 0, 0x02, 0x00, 1120},
{INAME_SLINGSHOT, ITEMUSE_ALL, 0, 0, 7, 0, 0x02, 0x00, 120},
{INAME_BOOMERANG, ITEMUSE_ALL, 0, 0, 32, 0, 0x02, 0x00, 1100},
{INAME_INSECTICIDE, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x16, 300},
{INAME_SUPER_SPRAY, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x77, 1500},
{INAME_FLEA_BAG, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x60, 0},
{INAME_WORDSOLOVE, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x78, 0},
{INAME_SWEAR_WORDS, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x79, 0},
{INAME_STKYMACHINE, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x6E, 3200},
{INAME_FLASHDARK, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x6D, 0},
{INAME_STONEORIGIN, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x3E, 0},
{INAME_POISNNEEDLE, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x3D, 0},
{INAME_FL_THROWER, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x67, 0},

//these items had no price originally},
//also might as well include the unused item},
#ifdef VER_JP
{INAME_BOMB, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x10, 0},
{INAME_SUPER_BOMB, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x11, 0},
{INAME_LASER_BEAM, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x69, 0},
{INAME_PLASMA_BEAM, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x6A, 0},
{INAME_UNKITEM27, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_BOMB, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x10, 280},
{INAME_SUPER_BOMB, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x11, 1800},
{INAME_LASER_BEAM, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x69, 760},
{INAME_PLASMA_BEAM, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x6A, 1300},
{INAME_ROPE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_ROPE, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x47, 600},

#ifdef VER_JP
{INAME_UNKITEM29, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM2A, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM2B, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM2C, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_PEACE_COIN, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_PEACE_COIN, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_PEACE_COIN, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_PEACE_COIN, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_PEACE_COIN, ITEMUSE_ALL, 0, 0, 5, 1, 0x02, 0x00, 260},
{INAME_PROTECTCOIN, ITEMUSE_ALL, 0, 0, 11, 1, 0x02, 0x00, 648},
{INAME_MAGIC_COIN, ITEMUSE_ALL, 0, 0, 20, 1, 0x02, 0x00, 1200},
{INAME_BRASS_RING, ITEMUSE_ALL, 0, 0, 8, 2, 0x02, 0x00, 460},
{INAME_SILVER_RING, ITEMUSE_ALL, 0, 0, 14, 2, 0x02, 0x00, 825},
{INAME_GOLD_RING, ITEMUSE_ALL, 0, 0, 28, 2, 0x02, 0x00, 1510},
{INAME_H2O_PENDANT, ITEMUSE_ALL, 0, 0, 32, 3, 0x02, 0x00, 700},
{INAME_FIREPENDANT, ITEMUSE_ALL, 0, 0, 16, 3, 0x02, 0x00, 700},
{INAME_EARTHPENDNT, ITEMUSE_ALL, 0, 0, 8, 3, 0x02, 0x00, 700},
{INAME_SEA_PENDANT, ITEMUSE_ALL, 0, 0, 56, 3, 0x02, 0x00, 2860},

#ifdef VER_JP
{INAME_UNKITEM37, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM38, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM39, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM3A, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM3B, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_ORANGEJUICE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_ORANGEJUICE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_ORANGEJUICE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_ORANGEJUICE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_ORANGEJUICE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_ORANGEJUICE, ITEMUSE_ALL, 1, 0, 0, 0, 0x0A, 0x71, 5},
{INAME_FRENCHFRIES, ITEMUSE_ALL, 1, 0, 0, 0, 0x0B, 0x72, 15},
{INAME_MAGIC_HERB, ITEMUSE_ALL, 1, 0, 0, 0, 0x0C, 0x73, 30},
{INAME_HAMBURGER, ITEMUSE_ALL, 1, 0, 0, 0, 0x0D, 0x7A, 25},
{INAME_SPROTSDRINK, ITEMUSE_ALL, 1, 0, 0, 0, 0x0E, 0x74, 75},
{INAME_LIFEUPCREAM, ITEMUSE_ALL, 0, 0, 0, 0, 0x0F, 0x7B, 194},
{INAME_ASTHMASPRAY, ITEMUSE_NINTEN, 0, 0, 0, 0, 0x00, 0x76, 148},
{INAME_ANTIDOTE, ITEMUSE_ALL, 1, 0, 0, 0, 0x11, 0x7C, 20},
{INAME_MOUTHWASH, ITEMUSE_ALL, 0, 0, 0, 0, 0x12, 0x00, 175},
{INAME_BERRY_TOFU, ITEMUSE_ALL, 1, 0, 0, 0, 0x06, 0x00, 975},

#ifdef VER_JP
{INAME_UNKITEM46, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_BREAD, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_BREAD, ITEMUSE_ALL, 1, 0, 0, 0, 0x05, 0x84, 30},
{INAME_NOBLE_SEED, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x5F, 0},
{INAME_PSI_STONE, ITEMUSE_NINTEN | ITEMUSE_ANA, 0, 0, 0, 0, 0x13, 0x2F, 0},

#ifdef VER_JP
{INAME_UNKITEM4A, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_MAGICRIBBON, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_MAGICRIBBON, ITEMUSE_ANA, 0, 0, 0, 0, 0x14, 0x00, 0},
{INAME_MAGIC_CANDY, ITEMUSE_LLOYD, 1, 0, 0, 0, 0x15, 0x00, 0},

#ifdef VER_JP
{INAME_UNKITEM4D, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_QUICKCAPSUL, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_QUICKCAPSUL, ITEMUSE_ALL, 1, 0, 0, 0, 0x16, 0x00, 0},
{INAME_WISDOM_CAPS, ITEMUSE_ALL, 1, 0, 0, 0, 0x17, 0x00, 0},
{INAME_PHYSICALCAP, ITEMUSE_ALL, 1, 0, 0, 0, 0x18, 0x00, 0},
{INAME_FORCECAPSUL, ITEMUSE_ALL, 1, 0, 0, 0, 0x19, 0x00, 0},
{INAME_FIGHTCAPSUL, ITEMUSE_ALL, 1, 0, 0, 0, 0x1A, 0x00, 0},

#ifdef VER_JP
{INAME_UNKITEM53, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM54, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_BASEMENTKEY, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_BASEMENTKEY, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_BASEMENTKEY, ITEMUSE_ALL, 0, 1, 0, 0, 0x01, 0x00, 0},
{INAME_ZOO_KEY, ITEMUSE_ALL, 0, 0, 0, 0, 0x01, 0x00, 0},
{INAME_GHOST_KEY, ITEMUSE_ALL, 0, 1, 0, 0, 0x01, 0x00, 0},
{INAME_GGFS_DIARY, ITEMUSE_ALL, 0, 1, 0, 0, 0x1B, 0x00, 0},
{INAME_PASS, ITEMUSE_ALL, 0, 1, 0, 0, 0x01, 0x00, 0},
{INAME_TICKET, ITEMUSE_ALL, 0, 0, 0, 0, 0x01, 0x00, 350},

#ifdef VER_JP
{INAME_UNKITEM5B, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM5C, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM5D, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM5E, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_CANARYCHICK, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_CANARYCHICK, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_CANARYCHICK, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_CANARYCHICK, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_CANARYCHICK, ITEMUSE_ALL, 0, 1, 0, 0, 0x01, 0x00, 85},

#ifdef VER_JP
{INAME_UNKITEM60, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_BOTTLROCKET, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_BOTTLROCKET, ITEMUSE_LLOYD, 0, 0, 0, 0, 0x00, 0x7D, 0},
{INAME_HAT, ITEMUSE_ALL, 0, 1, 0, 0, 0x01, 0x00, 0},
{INAME_DENTURES, ITEMUSE_ALL, 0, 1, 0, 0, 0x01, 0x00, 0},
{INAME_TICKET_STUB, ITEMUSE_ALL, 0, 0, 0, 0, 0x01, 0x00, 0},
{INAME_IC_CHIP, ITEMUSE_LLOYD, 0, 1, 0, 0, 0x01, 0x00, 0},
{INAME_OCARINA, ITEMUSE_ALL, 0, 1, 0, 0, 0x23, 0x00, 0},

#ifdef VER_JP
{INAME_UNKITEM67, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_FRANKLNBDGE, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_FRANKLNBDGE, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_FRNDSHPRING, 0, 0, 0, 0, 0, 0x00, 0x00, 0},

#ifdef VER_JP
{INAME_UNKITEM6A, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_ONYX_HOOK, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_ONYX_HOOK, ITEMUSE_ALL, 0, 1, 0, 0, 0x1E, 0x00, 0},
{INAME_LAST_WEAPON, ITEMUSE_ALL, 0, 0, 0, 0, 0x20, 0x00, 1048},
{INAME_RULER, ITEMUSE_ALL, 0, 0, 0, 0, 0x21, 0x00, 22},
{INAME_CASH_CARD, ITEMUSE_ALL, 0, 1, 0, 0, 0x01, 0x00, 0},
{INAME_RED_WEED, ITEMUSE_ALL, 0, 0, 0, 0, 0x01, 0x00, 0},
{INAME_BULLHORN, ITEMUSE_ALL, 0, 0, 0, 0, 0x00, 0x3C, 0},
{INAME_MAP, ITEMUSE_ALL, 0, 1, 0, 0, 0x22, 0x00, 0},

#ifdef VER_JP
{INAME_UNKITEM72, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM73, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM74, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM75, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM76, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM77, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM78, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM79, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM7A, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM7B, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM7C, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM7D, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_UNKITEM7E, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#else
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
{INAME_DEBUG, 0, 0, 0, 0, 0, 0x00, 0x00, 0},
#endif

{INAME_DEBUG, ITEMUSE_ALL, 0, 1, 0, 0, 0x09, 0x00, 0},
};
