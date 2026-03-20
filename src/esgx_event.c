#include "../include/esgx_event.h"
#include <string.h>

static ESGX_KeyCode sdlkey_to_esgx(SDLKey k)
{
    switch (k) {
        case SDLK_UP:     return ESGX_KEY_UP;
        case SDLK_DOWN:   return ESGX_KEY_DOWN;
        case SDLK_LEFT:   return ESGX_KEY_LEFT;
        case SDLK_RIGHT:  return ESGX_KEY_RIGHT;
        case SDLK_ESCAPE: return ESGX_KEY_ESCAPE;
        case SDLK_SPACE:  return ESGX_KEY_SPACE;
        case SDLK_RETURN: return ESGX_KEY_ENTER;
        default:          return ESGX_KEY_OTHER;
    }
}

int esgx_event_poll(ESGX_Event *ev)
{
    SDL_Event sdl_ev;
    if (!ev) return 0;

    ev->key_down          = 0;
    ev->key_up            = 0;
    ev->key               = ESGX_KEY_NONE;
    ev->mouse_button_down = 0;
    ev->mouse_button_up   = 0;

    SDL_GetMouseState(&ev->mouse_x, &ev->mouse_y);
    ev->keys = SDL_GetKeyState(NULL);

    while (SDL_PollEvent(&sdl_ev)) {
        switch (sdl_ev.type) {
            case SDL_QUIT:
                ev->quit = 1;
                return 0;
            case SDL_KEYDOWN:
                ev->key_down = 1;
                ev->key      = sdlkey_to_esgx(sdl_ev.key.keysym.sym);
                ev->raw_key  = sdl_ev.key.keysym;
                if (sdl_ev.key.keysym.sym == SDLK_ESCAPE) {
                    ev->quit = 1;
                    return 0;
                }
                break;
            case SDL_KEYUP:
                ev->key_up  = 1;
                ev->key     = sdlkey_to_esgx(sdl_ev.key.keysym.sym);
                ev->raw_key = sdl_ev.key.keysym;
                break;
            case SDL_MOUSEBUTTONDOWN:
                ev->mouse_button_down = 1;
                ev->mouse_button      = (ESGX_MouseButton)sdl_ev.button.button;
                ev->mouse_x           = sdl_ev.button.x;
                ev->mouse_y           = sdl_ev.button.y;
                break;
            case SDL_MOUSEBUTTONUP:
                ev->mouse_button_up = 1;
                ev->mouse_button    = (ESGX_MouseButton)sdl_ev.button.button;
                ev->mouse_x         = sdl_ev.button.x;
                ev->mouse_y         = sdl_ev.button.y;
                break;
            case SDL_MOUSEMOTION:
                ev->mouse_x = sdl_ev.motion.x;
                ev->mouse_y = sdl_ev.motion.y;
                break;
            default:
                break;
        }
    }
    return 1;
}

int esgx_key_pressed(ESGX_Event *ev, SDLKey k)
{
    if (!ev || !ev->keys) return 0;
    return ev->keys[k] != 0;
}
