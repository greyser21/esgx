#include "../include/esgx_window.h"
#include <stdio.h>
#include <stdlib.h>

SDL_Surface *esgx_screen = NULL;

static int _width  = 0;
static int _height = 0;

int esgx_window_init(int width, int height, const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "[ESGX] SDL_Init error: %s\n", SDL_GetError());
        return -1;
    }

    esgx_screen = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!esgx_screen) {
        fprintf(stderr, "[ESGX] SDL_SetVideoMode error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_WM_SetCaption(title, NULL);
    _width  = width;
    _height = height;

    printf("[ESGX] Fenêtre %dx%d créée : \"%s\"\n", width, height, title);
    return 0;
}

void esgx_window_clear(Uint8 r, Uint8 g, Uint8 b)
{
    if (!esgx_screen) return;
    SDL_FillRect(esgx_screen, NULL,
                 SDL_MapRGB(esgx_screen->format, r, g, b));
}

void esgx_window_flip(void)
{
    if (!esgx_screen) return;
    SDL_Flip(esgx_screen);
}

void esgx_window_quit(void)
{
    /* esgx_screen est géré par SDL, pas besoin de SDL_FreeSurface */
    esgx_screen = NULL;
    SDL_Quit();
    printf("[ESGX] Fenêtre fermée.\n");
}

int esgx_window_width(void)  { return _width;  }
int esgx_window_height(void) { return _height; }