#include <stdio.h>
#include <stdbool.h>
#include "types.h"

char CHOICER_FLIGHTPLANSA[] = "FlightplanA";
char CHOICER_FLIGHTPLANSB[] = "FlightplanB";
char CHOICER_FLIGHTPLANSC[] = "FlightplanC";
char CHOICER_HINTS1[] = "Hint 1";
char CHOICER_HINTS2[] = "Hint 2";
char CHOICER_HINTS3[] = "Hint 3";
char CHOICER_SCIENTIST1[] = "Real Rocket";
char CHOICER_SCIENTIST2[] = "TimeMachine";

//teleport locales
char TELEPORT_MYHOME[] = "MyHome";
char TELEPORT_PODUNK[] = "Podunk";
char TELEPORT_MERRYSVILLE[] = "Merrysville";
char TELEPORT_REINDEER[] = "Reindeer";
char TELEPORT_SPOOKANE[] = "Spookane";
char TELEPORT_SNOWMAN[] = "Snowman";
char TELEPORT_YOUNGTOWN[] = "Youngtown";
char TELEPORT_ELLAY[] = "Ellay";
char TELEPORT_UNION[] = "Union";

//only changes here is that in jp, there are unused choicers
//4 options all of which say "POISON"
//very ominous


//not JUST teleport defs, by the way!
//name, (posX << 6)| music, (posY << 6)| startDirection, msgPrice

typedef struct choicerDef{
    char* name;
    u8 music;
    u16 posX;
    Direction direction;
    u16 posY;
    u16 msgPrice;
} choicerDef;

//ACTUAL teleport defs
choicerDef CHOICERS[] = {
{TELEPORT_MYHOME,6,0xCE,6,0x145,0},
{TELEPORT_PODUNK,6,0xAB,2,0x1A7,0},
{TELEPORT_MERRYSVILLE,6,0x14B,2,0x1CF,0},
{TELEPORT_REINDEER,6,0x1CA,6,0x3BF,0},
{TELEPORT_SPOOKANE,6,0x2DD,2,0x364,0},
{TELEPORT_SNOWMAN,10,0x398,4,0x3BE,0},
{TELEPORT_YOUNGTOWN,0x30,0x2E2,6,0x1D3,0},
{TELEPORT_ELLAY,6,0x32A,6,0x12E,0},

{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},

//trainstuff
//union station
{TELEPORT_REINDEER,0x14,0x13E,5,0x14F,16},
{TELEPORT_SPOOKANE,0x14,0x13E,5,0x14F,25},
{TELEPORT_SNOWMAN,0x14,0x13E,5,0x14F,36},
//reindeer station
{TELEPORT_UNION,0x14,0x205,6,0x33F,16},
//spookane station
{TELEPORT_UNION,0x14,0x2AD,6,0x33F,25},
//snowman station
{TELEPORT_UNION,0x14,0x35B,6,0x3DF,36},
//reindeer station 2
{TELEPORT_SPOOKANE,0x14,0x206,2,0x33F,9},
{TELEPORT_SNOWMAN,0x14,0x206,2,0x33F,20},
//spookane station 2
{TELEPORT_REINDEER,0x14,0x2AD,6,0x33F,9},
{TELEPORT_SNOWMAN,0x14,0x2AE,2,0x33F,11},
//snowman station 2
{TELEPORT_REINDEER,0x14,0x35B,6,0x3DF,20},
{TELEPORT_SPOOKANE,0x14,0x35B,6,0x3DF,11},

//flight plan choicers
{CHOICER_FLIGHTPLANSA,0,0,0,0,5},
{CHOICER_FLIGHTPLANSB,0,0,0,0,10},
{CHOICER_FLIGHTPLANSC,0,0,0,0,15},

//hint choicers
{CHOICER_HINTS1,0,0,0,0,0},
{CHOICER_HINTS2,0,0,0,0,0},
{CHOICER_HINTS3,0,0,0,0,0},

//somehow this is the only change? pleasantly surprised
#ifdef VER_JP
{CHOICER_UNK1,0,0,0,0,0},
{CHOICER_UNK2,0,0,0,0,0},
{CHOICER_UNK3,0,0,0,0,0},
{CHOICER_UNK4,0,0,0,0,0},
#else
{CHOICER_SCIENTIST1,0,0,0,0,0},
{CHOICER_SCIENTIST1,0,0,0,0,0},
{CHOICER_SCIENTIST1,0,0,0,0,0},
{CHOICER_SCIENTIST1,0,0,0,0,0},
#endif

//scientist gag choicers
{CHOICER_SCIENTIST1,0,0,0,0,3485},
{CHOICER_SCIENTIST2,0,0,0,0,2775},

{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},
{GENERIC_STR_NULL,0,0,0,0,0},

};