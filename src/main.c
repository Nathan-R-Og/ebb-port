#include <stdio.h>
#include <stdbool.h>

#include <stdio.h>

#include "sdl.h"
#include "game.h"

// Define screen dimensions
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   224


int main(int argc, char* argv[])
{

    // Unused argc, argv
    (void) argc;
    (void) argv;

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not be initialized!\n"
                        "SDL_Error: %s\n", SDL_GetError());
        return 0;
    }

    #if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        printf("SDL can not disable compositor bypass!\n");
        return 0;
    }
    #endif

    if (0 != SDL_Init(SDL_INIT_AUDIO)) {
        SDL_Log ("SDL_Init failed: %s\n", SDL_GetError ());
        return 0;
    }

    // Create window
    SDL_Window *window = SDL_CreateWindow("EBB",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if(!window)
    {
        fprintf(stderr, "Window could not be created!\n"
                        "SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        // Create renderer
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(!renderer)
        {
            fprintf(stderr, "Renderer could not be created!\n"
                            "SDL_Error: %s\n", SDL_GetError());
        }
        else
        {

            SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
            SDL_RenderSetIntegerScale(renderer, true);
            // Start the game
            Game_start(renderer);

            // Destroy renderer
            SDL_DestroyRenderer(renderer);

        }

        // Destroy window
        SDL_DestroyWindow(window);

    }

    // Quit SDL
    SDL_Quit();

    return 0;
}
