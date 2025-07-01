#include "main.h"
#include "game.h"

// Define screen dimensions
#define SCREEN_WIDTH    240
#define SCREEN_HEIGHT   224

char GENERIC_STR_NULL[1] = {0};
char* basepath;
char path[256]; //generic path memory
bool main_quit = false; // Event loop exit flag

void get_realpath(char* in){
    memset(path, 0, sizeof(path));
    snprintf(path, sizeof(path), in, basepath);
}

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

            basepath = SDL_GetBasePath();

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

void Utils_setBackgroundColor(SDL_Renderer *renderer, SDL_Color color)
{
    // Initialize renderer color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Clear screen
    SDL_RenderClear(renderer);
}

long long Utils_time()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

void Utils_randInit()
{
    srand(time(NULL));
}

int Utils_rand(int min, int max)
{
    return ( rand() % (max + 1) ) + min;
}

bool Utils_equalColors(SDL_Color color1, SDL_Color color2)
{
    //return color1.r == color2.r && color1.g == color2.g && color1.b == color2.b && color1.a == color2.a;
    return *((Sint32 *) &color1) == *((Sint32 *) &color2);
}

SDL_Surface* surface_from_path(char* path){
    SDL_Surface* loadedSurface = IMG_Load(path);
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
        return NULL;
    }
    return loadedSurface;
}

SDL_Texture* texture_from_surface(SDL_Surface* surface, SDL_Renderer *renderer){
    return SDL_CreateTextureFromSurface(renderer, surface);
}

SDL_Texture* texture_from_path(char* path, SDL_Renderer *renderer){
    SDL_Texture* loadedTexture = IMG_LoadTexture(renderer, path);
    if( loadedTexture == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
        return NULL;
    }
    return loadedTexture;
}