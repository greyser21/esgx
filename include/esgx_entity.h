#ifndef ESGX_ENTITY_H
#define ESGX_ENTITY_H

#include "esgx_draw.h"

/*
 * Structure Entity :
 * Associe un Dessin à une position dans la fenêtre.
 * Fournit des fonctions pour la mettre à jour et la dessiner.
 */
typedef struct ESGX_Entity {
    double      x, y;       /* Position (flottant pour mouvements fluides) */
    double      vx, vy;     /* Vitesse (pixels/frame)                       */
    ESGX_Draw  *draw;       /* Dessin associé                               */
    int         visible;    /* 1 = affiché, 0 = caché                       */
    int         active;     /* 1 = mis à jour, 0 = gelé                     */

    /* Dimensions de la bounding box (calculées depuis le Dessin) */
    int         w, h;

    /* Callback de mise à jour personnalisée */
    void (*update)(struct ESGX_Entity *self, void *userdata);
    void *userdata;
} ESGX_Entity;

/* ---- Création / Destruction ---- */

/*
 * Crée une entité à la position (x, y) avec le Dessin donné.
 * L'entité ne prend pas ownership du Dessin.
 */
ESGX_Entity *esgx_entity_create(double x, double y, ESGX_Draw *draw);

/*
 * Libère l'entité (pas le Dessin associé).
 */
void esgx_entity_free(ESGX_Entity *entity);

/* ---- Mise à jour ---- */

/*
 * Applique la vitesse à la position.
 * Si update callback est défini, l'appelle aussi.
 */
void esgx_entity_update(ESGX_Entity *entity);

/*
 * Rebondit l'entité sur les bords de la fenêtre.
 */
void esgx_entity_bounce(ESGX_Entity *entity, int win_w, int win_h);

/*
 * Déplace l'entité vers une cible (x_target, y_target) à la vitesse donnée.
 * Retourne 1 si la cible est atteinte.
 */
int esgx_entity_move_toward(ESGX_Entity *entity,
                             double x_target, double y_target,
                             double speed);

/* ---- Rendu ---- */

/*
 * Dessine l'entité sur l'écran (si visible).
 * Met également à jour l'animation interne si le Dessin est de type ANIM.
 */
void esgx_entity_draw(ESGX_Entity *entity, SDL_Surface *screen);

/* ---- Accès / mutateurs ---- */

void esgx_entity_set_pos(ESGX_Entity *e, double x, double y);
void esgx_entity_set_vel(ESGX_Entity *e, double vx, double vy);
void esgx_entity_set_draw(ESGX_Entity *e, ESGX_Draw *draw);

/* ---- Gestion de listes d'entités ---- */

/*
 * Met à jour et dessine toutes les entités d'un tableau (size éléments).
 */
void esgx_entities_update_all(ESGX_Entity **entities, int size);
void esgx_entities_draw_all(ESGX_Entity **entities, int size,
                             SDL_Surface *screen);

#endif /* ESGX_ENTITY_H */