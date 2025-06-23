#include <stdio.h>
#include <stdbool.h>
#include "types.h"

extern char STATUS_FAINTD[];

char PSI_TELEPATHY[] = "Telepathy";
char PSI_TELEPORT[] = "Teleport";
char PSI_LIFEUP_ALPHA[] = "LifeUp α";
char PSI_LIFEUP_BETA[] = "LifeUp β";
char PSI_LIFEUP_GAMMA[] = "LifeUp γ";
char PSI_LIFEUP_PI[] = "LifeUp π";
char PSI_LIFEUP_OMEGA[] = "LifeUp Ω";
char PSI_HEALING_ALPHA[] = "Healing α";
char PSI_HEALING_BETA[] = "Healing β";
char PSI_HEALING_GAMMA[] = "Healing γ";
char PSI_HEALING_PI[] = "Healing π";
char PSI_SUPRHEALING[] = "SuprHealing";
char PSI_PSISHIELD_ALPHA[] = "PSIShield α";
char PSI_PSISHIELD_BETA[] = "PSIShield β";
char PSI_POWERSHIELD[] = "PowerShield";
char PSI_BRAINSHOCK[] = "BrainShock";
char PSI_BRAINCYCLON[] = "BrainCyclon";
char PSI_HYPNOSIS[] = "Hypnosis";
char PSI_PARALYSIS[] = "Paralysis";
char PSI_DARKNESS[] = "Darkness";
char PSI_PSI_MAGNET[] = "PSI-Magnet";
char PSI_SHIELD_OFF[] = "Shield-Off";
char PSI_PSI_BLOCK[] = "PSI-Block";
char PSI_OFFENSEUP[] = "OffenseUp";
char PSI_DEFENSEUP_ALPHA[] = "DefenseUp α";
char PSI_DEFENSEUP_BETA[] = "DefenseUp β";
char PSI_QUICKUP[] = "QuickUp";
char PSI_DEF_DOWN_ALPHA[] = "Def.Down α";
char PSI_DEF_DOWN_BETA[] = "Def.Down β";
char PSI_4TH_D_SLIP[] = "4th-D Slip";
char PSI_PK_FREEZE_ALPHA[] = "PK Freeze α";
char PSI_PK_FREEZE_BETA[] = "PK Freeze β";
char PSI_PK_FREEZE_GAMMA[] = "PK Freeze γ";
char PSI_PK_FREEZE_OMEGA[] = "PK Freeze Ω";
char PSI_PK_FIRE_ALPHA[] = "PK Fire α";
char PSI_PK_FIRE_BETA[] = "PK Fire β";
char PSI_PK_FIRE_GAMMA[] = "PK Fire γ";
char PSI_PK_FIRE_OMEGA[] = "PK Fire Ω";
char PSI_PK_BEAM_ALPHA[] = "PK Beam α";
char PSI_PK_BEAM_BETA[] = "PK Beam β";
char PSI_PK_BEAM_GAMMA[] = "PK Beam γ";
char PSI_PK_BEAM_OMEGA[] = "PK Beam Ω";
char PSI_PK_THUNDERALPHA[] = "PK Thunderα";
char PSI_PK_THUNDERBETA[] = "PK Thunderβ";
char PSI_PK_THUNDERGAMMA[] = "PK Thunderγ";

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


psiDef PSI_Data[] = {
{    GENERIC_STR_NULL,-1,-1,0x00,0x00,0x00,0},
{       PSI_TELEPATHY, 0, 0,0x25,0x00,0x00,1},
{        PSI_TELEPORT,-1,-1,0x26,0x00,0x00,3},

//leftovers 1
#ifdef VER_JP
{            PSI_UNK3,-1,-1,0x00,0x00,0x00,0},
{            PSI_UNK4,-1,-1,0x00,0x00,0x00,0},
{            PSI_UNK5,-1,-1,0x00,0x00,0x00,0},
{            PSI_UNK6,-1,-1,0x00,0x00,0x00,0},
{            PSI_UNK7,-1,-1,0x00,0x00,0x00,0},
#else
{    PSI_LIFEUP_ALPHA,-1,-1,0x00,0x00,0x00,0},
{    PSI_LIFEUP_ALPHA,-1,-1,0x00,0x00,0x00,0},
{    PSI_LIFEUP_ALPHA,-1,-1,0x00,0x00,0x00,0},
{    PSI_LIFEUP_ALPHA,-1,-1,0x00,0x00,0x00,0},
{    PSI_LIFEUP_ALPHA,-1,-1,0x00,0x00,0x00,0},
#endif

{    PSI_LIFEUP_ALPHA, 2,-1,0x27,0x29,0x00,3},
{     PSI_LIFEUP_BETA,28, 0,0x28,0x2A,0x00,5},
{    PSI_LIFEUP_GAMMA,32,11,0x29,0x2B,0x00,9},
{       PSI_LIFEUP_PI,37,21,0x00,0x2C,0x00,17},
{    PSI_LIFEUP_OMEGA,-1,33,0x00,0x2D,0x00,48},

//leftovers 2
#ifdef VER_JP
{            PSI_UNKD,-1,-1,0x00,0x00,0x00,0},
{            PSI_UNKE,-1,-1,0x00,0x00,0x00,0},
{            PSI_UNKF,-1,-1,0x00,0x00,0x00,0},
#else
{   PSI_HEALING_ALPHA,-1,-1,0x00,0x00,0x00,0},
{   PSI_HEALING_ALPHA,-1,-1,0x00,0x00,0x00,0},
{   PSI_HEALING_ALPHA,-1,-1,0x00,0x00,0x00,0},
#endif

{   PSI_HEALING_ALPHA, 4, 5,0x2C,0x5B,0x00,3},
{    PSI_HEALING_BETA,15,12,0x00,0x63,0x00,6},
{   PSI_HEALING_GAMMA,18,13,0x2A,0x86,0x00,6},
{      PSI_HEALING_PI,21,14,0x00,0x64,0x00,6},
{     PSI_SUPRHEALING,34,17,0x2D,0x65,0x00,36},
{ PSI_PSISHIELD_ALPHA,12, 3,0x00,0x38,0x00,4},
{  PSI_PSISHIELD_BETA,29, 8,0x00,0x39,0x00,9},
{     PSI_POWERSHIELD,33,23,0x00,0x37,0x00,9},
{      PSI_BRAINSHOCK,-1, 1,0x00,0x42,0x00,5},
{     PSI_BRAINCYCLON,-1,25,0x00,0x43,0x00,12},
{        PSI_HYPNOSIS,-1, 8,0x00,0x44,0x00,5},
{       PSI_PARALYSIS,-1,14,0x00,0x45,0x00,8},
{        PSI_DARKNESS,-1,19,0x00,0x31,0x00,10},
{      PSI_PSI_MAGNET,-1, 6,0x00,0x30,0x00,0},
{      PSI_SHIELD_OFF,-1,18,0x00,0x66,0x00,8},
{       PSI_PSI_BLOCK,-1, 7,0x00,0x36,0x00,7},
{       PSI_OFFENSEUP,17,-1,0x00,0x1B,0x00,6},
{ PSI_DEFENSEUP_ALPHA, 6,-1,0x00,0x1D,0x00,3},
{  PSI_DEFENSEUP_BETA,23,-1,0x00,0x1E,0x00,8},
{         PSI_QUICKUP,13,-1,0x00,0x1F,0x00,3},
{  PSI_DEF_DOWN_ALPHA, 8,-1,0x00,0x25,0x00,3},
{   PSI_DEF_DOWN_BETA,26,-1,0x00,0x82,0x00,9},
{      PSI_4TH_D_SLIP,10,-1,0x00,0x5C,0x00,16},
{        PSI_HYPNOSIS, 3,-1,0x00,0x44,0x00,5},
{ PSI_PK_FREEZE_ALPHA,-1, 0,0x00,0x0C,0x00,3},
{  PSI_PK_FREEZE_BETA,-1, 6,0x00,0x0D,0x00,5},
{ PSI_PK_FREEZE_GAMMA,-1,20,0x00,0x0F,0x00,8},
{ PSI_PK_FREEZE_OMEGA,-1,28,0x00,0x0E,0x00,13},
{   PSI_PK_FIRE_ALPHA,-1,16,0x00,0x09,0x00,8},
{    PSI_PK_FIRE_BETA,-1,26,0x00,0x0A,0x00,12},
{   PSI_PK_FIRE_GAMMA,-1,30,0x00,0x0B,0x00,14},
{   PSI_PK_FIRE_OMEGA,-1,34,0x00,0x85,0x00,30},
{   PSI_PK_BEAM_ALPHA,-1, 4,0x00,0x12,0x00,4},
{    PSI_PK_BEAM_BETA,-1,10,0x00,0x13,0x00,7},
{   PSI_PK_BEAM_GAMMA,-1,24,0x00,0x15,0x00,12},
{   PSI_PK_BEAM_OMEGA,-1,32,0x00,0x14,0x00,24},
{ PSI_PK_THUNDERALPHA,-1, 2,0x00,0x17,0x00,3},
{  PSI_PK_THUNDERBETA,-1, 8,0x00,0x18,0x00,6},
{ PSI_PK_THUNDERGAMMA,-1,27,0x00,0x83,0x00,12},

//leftovers 3
#ifdef VER_JP
{           PSI_UNK37,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK38,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK39,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK3A,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK3B,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK3C,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK3D,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK3E,-1,-1,0x00,0x00,0x00,0},
{           PSI_UNK3F,-1,-1,0x00,0x00,0x00,0},
#else
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
{       STATUS_FAINTD,-1,-1,0x00,0x00,0x00,0},
#endif

};