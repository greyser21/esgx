#ifndef ESGX_EVENT_H
#define ESGX_EVENT_H

#include <SDL/SDL.h>

/* ---- Types de touches clavier simplifiés ---- */
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

/* ---- Boutons souris ---- */
typedef enum {
    ESGX_MOUSE_LEFT   = 1,
    ESGX_MOUSE_MIDDLE = 2,
    ESGX_MOUSE_RIGHT  = 3
} ESGX_MouseButton;

/* ---- Structure événement ESGX ---- */
typedef struct {
    /* Fenêtre */
    int quit;           /* 1 si l'utilisateur a fermé la fenêtre */

    /* Clavier */
    int        key_down;   /* 1 si une touche vient d'être pressée */
    int        key_up;     /* 1 si une touche vient d'être relâchée */
    ESGX_KeyCode key;      /* Touche concernée */
    SDL_keysym raw_key;    /* Accès brut SDL si besoin */

    /* Souris - position */
    int mouse_x;
    int mouse_y;

    /* Souris - boutons */
    int mouse_button_down; /* 1 si un bouton vient d'être pressé */
    int mouse_button_up;   /* 1 si un bouton vient d'être relâché */
    ESGX_MouseButton mouse_button;

    /* Etat courant des touches (tableau) */
    Uint8 *keys;           /* Pointeur vers SDL_GetKeyState */
} ESGX_Event;

/*
 * Pompe les événements SDL et remplit la structure ESGX_Event.
 * À appeler une fois par frame dans la boucle principale.
 * Retourne 1 tant que la fenêtre est ouverte, 0 si quit.
 */
int esgx_event_poll(ESGX_Event *ev);

/*
 * Retourne 1 si la touche SDL_KEY_xxx est actuellement enfoncée.
 */
int esgx_key_pressed(ESGX_Event *ev, SDLKey k);

#endif /* ESGX_EVENT_H */