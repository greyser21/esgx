#ifndef ESGX_WINDOW_H
#define ESGX_WINDOW_H

#include <SDL/SDL.h>

/* Surface principale de la fenêtre */
extern SDL_Surface *esgx_screen;

/*
 * Initialise SDL et crée une fenêtre.
 * width, height : dimensions en pixels
 * title        : titre de la fenêtre
 * Retourne 0 en cas de succès, -1 en cas d'erreur.
 */
int  esgx_window_init(int width, int height, const char *title);

/*
 * Efface l'écran avec la couleur donnée (r, g, b).
 */
void esgx_window_clear(Uint8 r, Uint8 g, Uint8 b);

/*
 * Affiche le contenu de l'écran (flip).
 */
void esgx_window_flip(void);

/*
 * Libère les ressources SDL et ferme la fenêtre.
 */
void esgx_window_quit(void);

/*
 * Retourne la largeur / hauteur de la fenêtre.
 */
int esgx_window_width(void);
int esgx_window_height(void);

#endif /* ESGX_WINDOW_H */