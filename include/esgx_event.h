#ifndef ESGX_EVENT_H
#define ESGX_EVENT_H

#include <SDL/SDL.h>

typedef enum {
    ESGX_KEY_NONE = 0,
    ESGX_KEY_UP,
    ESGX_KEY_DOWN,
    ESGX_KEY_LEFT,
    ESGX_KEY_RIGHT,
    ESGX_KEY_ESCAPE,
    ESGX_KEY_SPACE,
    ESGX_KEY_ENTER,
    ESGX_KEY_OTHER
} ESGX_KeyCode;

typedef enum {
    ESGX_MOUSE_LEFT   = 1,
    ESGX_MOUSE_MIDDLE = 2,
    ESGX_MOUSE_RIGHT  = 3
} ESGX_MouseButton;

typedef struct {
    int quit;

    int        key_down;
    int        key_up;
    ESGX_KeyCode key;
    SDL_keysym raw_key;

    int mouse_x;
    int mouse_y;

    int mouse_button_down;
    int mouse_button_up;
    ESGX_MouseButton mouse_button;

    Uint8 *keys;
} ESGX_Event;

int esgx_event_poll(ESGX_Event *ev);

int esgx_key_pressed(ESGX_Event *ev, SDLKey k);

#endif