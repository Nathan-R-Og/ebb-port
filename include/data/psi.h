#ifndef DATA_PSI_H
#define DATA_PSI_H
#include "types.h"
//psi data
//TODO: what does arg5 do???????
//NOTE- battleEffect uses BATTLE_ACTION_POINTERS as an array.
//(or at least i think it does)
//also i only made a macro out of this because i dont wanna seperate one word and six bytes
typedef struct psiDef {
    char* name;
    u8 nintenLevel;
    u8 anaLevel;
    u8 overworldEffect;
    u8 battleEffect;
    u8 arg5;
    u8 cost;
} psiDef;
extern char STATUS_FAINTD[];
#endif // DATA_PSI_H
