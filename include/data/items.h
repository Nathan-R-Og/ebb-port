#ifndef DATA_ITEMS_H
#define DATA_ITEMS_H
#include "types.h"
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
#endif // DATA_ITEMS_H
