#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "sdl.h"
// Define screen dimensions
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   224

// Define MAX and MIN macros
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#define COLOR_BREAKUP(COLOR)    COLOR.r, COLOR.g, COLOR.b, COLOR.a

extern char GENERIC_STR_NULL[1];
extern char* basepath;
extern char path[256]; //generic path memory
extern bool main_quit; // Event loop exit flag

void get_realpath(char* in);
//int main(int argc, char* argv[]);
void Utils_setBackgroundColor(SDL_Renderer *renderer, SDL_Color color);
long long Utils_time();
void Utils_randInit();
int Utils_rand(int min, int max);
bool Utils_equalColors(SDL_Color color1, SDL_Color color2);
SDL_Surface* surface_from_path(char* path);
SDL_Texture* texture_from_surface(SDL_Surface* surface, SDL_Renderer *renderer);
SDL_Texture* texture_from_path(char* path, SDL_Renderer *renderer);

#endif // MAIN_H
