/*
 * demo_turtle.c
 * Démo : une tortue animée qui suit le curseur de la souris.
 *
 * La "tortue" est une entité dont le Dessin est une Animation.
 * Si aucun spritesheet n'est disponible, on dessine une tortue
 * via un callback C pour montrer aussi ESGX_DRAW_FUNC.
 *
 * Compilation (depuis esgx/) :
 *   make demo_turtle
 */

#include "../include/esgx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIN_W 800
#define WIN_H 600
#define TURTLE_SPEED 3.0

/* -------- Callback de dessin : tortue dessinée en C -------- */
/*
 * Dessine une tortue simplifiée centrée en (x, y).
 * userdata contient un pointeur vers l'angle de rotation courant.
 */
static void draw_turtle_func(SDL_Surface *screen, int x, int y, void *userdata)
{
    double angle = userdata ? *(double *)userdata : 0.0;
    (void)angle; /* la rotation est appliquée via la transform ESGX */

    /* Corps (ellipse approx via cercle) */
    esgx_draw_circle_raw(screen, x + 24, y + 24, 20, 34, 139, 34, 1);   /* vert foncé */
    esgx_draw_circle_raw(screen, x + 24, y + 24, 18, 0,  180, 0,  1);   /* vert clair */

    /* Tête */
    esgx_draw_circle_raw(screen, x + 24, y + 4, 8, 34, 139, 34, 1);

    /* Pattes */
    esgx_draw_circle_raw(screen, x + 4,  y + 12, 6, 34, 139, 34, 1); /* G haut */
    esgx_draw_circle_raw(screen, x + 44, y + 12, 6, 34, 139, 34, 1); /* D haut */
    esgx_draw_circle_raw(screen, x + 4,  y + 36, 6, 34, 139, 34, 1); /* G bas  */
    esgx_draw_circle_raw(screen, x + 44, y + 36, 6, 34, 139, 34, 1); /* D bas  */

    /* Queue */
    esgx_draw_circle_raw(screen, x + 24, y + 46, 5, 34, 139, 34, 1);

    /* Dessin de la carapace (lignes) */
    esgx_draw_line_raw(screen, x + 14, y + 24, x + 34, y + 24, 0, 100, 0);
    esgx_draw_line_raw(screen, x + 24, y + 14, x + 24, y + 34, 0, 100, 0);
    esgx_draw_line_raw(screen, x + 14, y + 14, x + 34, y + 34, 0, 100, 0);
    esgx_draw_line_raw(screen, x + 34, y + 14, x + 14, y + 34, 0, 100, 0);
}

int main(void)
{
    ESGX_Event ev = {0};
    double turtle_angle = 0.0;

    /* Init fenêtre */
    if (esgx_window_init(WIN_W, WIN_H, "ESGX Demo - Tortue") != 0)
        return 1;

    /* Création du dessin tortue (callback) */
    ESGX_Draw *turtle_draw = esgx_draw_from_func(draw_turtle_func, &turtle_angle);

    /* Entité tortue : position initiale au centre */
    ESGX_Entity *turtle = esgx_entity_create(WIN_W / 2.0 - 24,
                                              WIN_H / 2.0 - 24,
                                              turtle_draw);

    printf("=== Demo Tortue ===\n");
    printf("La tortue suit votre curseur. Appuyez sur Échap pour quitter.\n");

    /* Boucle principale */
    while (esgx_event_poll(&ev)) {
        /* --- Mise à jour --- */
        double target_x = ev.mouse_x - 24.0; /* centrer la tortue sur le curseur */
        double target_y = ev.mouse_y - 24.0;

        double dx = target_x - turtle->x;
        double dy = target_y - turtle->y;
        double dist = sqrt(dx * dx + dy * dy);

        if (dist > 4.0) {
            /* Calculer l'angle de direction pour orienter la tortue */
            turtle_angle = atan2(dy, dx) * 180.0 / M_PI + 90.0;
            esgx_draw_set_angle(turtle_draw, turtle_angle);

            esgx_entity_move_toward(turtle, target_x, target_y, TURTLE_SPEED);
        }

        /* --- Rendu --- */
        esgx_window_clear(135, 206, 235); /* ciel bleu */

        /* Sol */
        esgx_draw_rect_raw(esgx_screen, 0, WIN_H - 60, WIN_W, 60, 34, 139, 34, 1);

        /* Curseur souris visuel */
        esgx_draw_circle_raw(esgx_screen, ev.mouse_x, ev.mouse_y, 5,
                             255, 0, 0, 1);

        /* Tortue */
        esgx_entity_draw(turtle, esgx_screen);

        esgx_window_flip();
        SDL_Delay(16); /* ~60 FPS */
    }

    /* Nettoyage */
    esgx_entity_free(turtle);
    esgx_draw_free(turtle_draw);
    esgx_window_quit();
    return 0;
}