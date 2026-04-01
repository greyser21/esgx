#ifndef ESGX_WINDOW_H
#define ESGX_WINDOW_H

#include <SDL/SDL.h>

extern SDL_Surface *esgx_screen;

int  esgx_window_init(int width, int height, const char *title);

void esgx_window_clear(Uint8 r, Uint8 g, Uint8 b);

void esgx_window_flip(void);

void esgx_window_quit(void);

int esgx_window_width(void);
int esgx_window_height(void);

#endif