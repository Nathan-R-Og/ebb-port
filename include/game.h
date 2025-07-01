#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdbool.h>
#include "sdl.h"
#include "main.h"
#include "types.h"
#include "data/sprites.h"
#include "data/objects.h"
#include "hardware.h"

bool Game_start(SDL_Renderer *renderer);

#endif // GAME_H
