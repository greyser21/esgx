#ifndef ESGX_DRAW_H
#define ESGX_DRAW_H

#include <SDL/SDL.h>

/* ---- Callback de dessin personnalisé ---- */
/*
 * Un "Dessin" peut être :
 *   1. Une image SDL_Surface (BMP chargé)
 *   2. Une fonction de dessin C (callback)
 *   3. Une Animation (voir esgx_anim.h) — géré via ESGX_DrawType
 */
typedef enum {
    ESGX_DRAW_IMAGE,    /* SDL_Surface chargée depuis un fichier */
    ESGX_DRAW_FUNC,     /* Fonction de dessin personnalisée */
    ESGX_DRAW_SHAPE,    /* Forme géométrique simple */
    ESGX_DRAW_ANIM      /* Animation (séquence de frames) */
} ESGX_DrawType;

/* Types de formes géométriques */
typedef enum {
    ESGX_SHAPE_RECT,
    ESGX_SHAPE_CIRCLE,
    ESGX_SHAPE_LINE
} ESGX_ShapeType;

/* Données pour une forme géométrique */
typedef struct {
    ESGX_ShapeType type;
    int w, h;           /* largeur / hauteur (rect) ou rayon*2 (cercle) */
    int x2, y2;         /* point final (ligne) */
    Uint8 r, g, b;      /* couleur */
    int filled;         /* 1 = rempli */
} ESGX_Shape;

/* Transformations applicables à l'image */
typedef struct {
    double scale_x;     /* Homothétie X  (1.0 = normal) */
    double scale_y;     /* Homothétie Y  (1.0 = normal) */
    double angle;       /* Rotation en degrés */
    int    flip_h;      /* Réflexion horizontale (0/1) */
    int    flip_v;      /* Réflexion verticale   (0/1) */
    int    offset_x;    /* Translation locale X */
    int    offset_y;    /* Translation locale Y */
} ESGX_Transform;

/* Forward-declaration de Animation (défini dans esgx_anim.h) */
struct ESGX_Animation;

/* Callback de dessin : reçoit la surface cible et la position */
typedef void (*ESGX_DrawFunc)(SDL_Surface *screen, int x, int y, void *userdata);

/* ---- Structure principale Dessin ---- */
typedef struct {
    ESGX_DrawType type;

    union {
        SDL_Surface        *image;  /* ESGX_DRAW_IMAGE */
        struct {
            ESGX_DrawFunc   func;
            void           *userdata;
        } custom;                   /* ESGX_DRAW_FUNC  */
        ESGX_Shape          shape;  /* ESGX_DRAW_SHAPE */
        struct ESGX_Animation *anim;/* ESGX_DRAW_ANIM  */
    } data;

    ESGX_Transform transform;       /* Transformations à appliquer */
} ESGX_Draw;

/* ---- Constructeurs ---- */

/* Crée un Dessin depuis un fichier BMP */
ESGX_Draw *esgx_draw_from_image(const char *path);

/* Crée un Dessin depuis un callback C */
ESGX_Draw *esgx_draw_from_func(ESGX_DrawFunc func, void *userdata);

/* Crée un Dessin rectangle */
ESGX_Draw *esgx_draw_rect(int w, int h, Uint8 r, Uint8 g, Uint8 b, int filled);

/* Crée un Dessin cercle */
ESGX_Draw *esgx_draw_circle(int radius, Uint8 r, Uint8 g, Uint8 b, int filled);

/* Crée un Dessin depuis une Animation (prend ownership) */
ESGX_Draw *esgx_draw_from_anim(struct ESGX_Animation *anim);

/* ---- Transformations ---- */

void esgx_draw_set_scale(ESGX_Draw *d, double sx, double sy);
void esgx_draw_set_angle(ESGX_Draw *d, double angle);
void esgx_draw_set_flip(ESGX_Draw *d, int flip_h, int flip_v);
void esgx_draw_set_offset(ESGX_Draw *d, int ox, int oy);
void esgx_draw_reset_transform(ESGX_Draw *d);

/* ---- Rendu ---- */

/*
 * Dessine le Dessin à la position (x, y) sur l'écran.
 * Applique les transformations définies.
 */
void esgx_draw_render(ESGX_Draw *d, SDL_Surface *screen, int x, int y);

/* ---- Primitives bas niveau (utilitaires internes & callbacks) ---- */

void esgx_put_pixel(SDL_Surface *s, int x, int y, Uint8 r, Uint8 g, Uint8 b);
void esgx_draw_rect_raw(SDL_Surface *s, int x, int y, int w, int h,
                        Uint8 r, Uint8 g, Uint8 b, int filled);
void esgx_draw_circle_raw(SDL_Surface *s, int cx, int cy, int radius,
                          Uint8 r, Uint8 g, Uint8 b, int filled);
void esgx_draw_line_raw(SDL_Surface *s, int x1, int y1, int x2, int y2,
                        Uint8 r, Uint8 g, Uint8 b);

/* ---- Libération ---- */
void esgx_draw_free(ESGX_Draw *d);

#endif /* ESGX_DRAW_H */