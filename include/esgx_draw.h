#ifndef ESGX_DRAW_H
#define ESGX_DRAW_H

#include <SDL/SDL.h>

typedef enum
{
    ESGX_DRAW_IMAGE,
    ESGX_DRAW_FUNC,
    ESGX_DRAW_SHAPE,
    ESGX_DRAW_ANIM
} ESGX_DrawType;

typedef enum
{
    ESGX_SHAPE_RECT,
    ESGX_SHAPE_CIRCLE,
    ESGX_SHAPE_LINE
} ESGX_ShapeType;

typedef struct
{
    ESGX_ShapeType type;
    int w, h;
    int x2, y2;
    Uint8 r, g, b;
    int filled;
} ESGX_Shape;

typedef struct
{
    double scale_x;
    double scale_y;
    double angle;
    int flip_h;
    int flip_v;
    int offset_x;
    int offset_y;
} ESGX_Transform;

struct ESGX_Animation;

typedef void (*ESGX_DrawFunc)(SDL_Surface *screen, int x, int y, void *userdata);

typedef struct
{
    ESGX_DrawType type;

    union
    {
        SDL_Surface *image;
        struct
        {
            ESGX_DrawFunc func;
            void *userdata;
        } custom;
        ESGX_Shape shape;
        struct ESGX_Animation *anim;
    } data;

    ESGX_Transform transform;
} ESGX_Draw;

ESGX_Draw *esgx_draw_from_image(const char *path);

ESGX_Draw *esgx_draw_from_func(ESGX_DrawFunc func, void *userdata);

ESGX_Draw *esgx_draw_rect(int w, int h, Uint8 r, Uint8 g, Uint8 b, int filled);

ESGX_Draw *esgx_draw_circle(int radius, Uint8 r, Uint8 g, Uint8 b, int filled);

ESGX_Draw *esgx_draw_from_anim(struct ESGX_Animation *anim);

void esgx_draw_set_scale(ESGX_Draw *d, double sx, double sy);
void esgx_draw_set_angle(ESGX_Draw *d, double angle);
void esgx_draw_set_flip(ESGX_Draw *d, int flip_h, int flip_v);
void esgx_draw_set_offset(ESGX_Draw *d, int ox, int oy);
void esgx_draw_reset_transform(ESGX_Draw *d);

void esgx_draw_render(ESGX_Draw *d, SDL_Surface *screen, int x, int y);

void esgx_put_pixel(SDL_Surface *s, int x, int y, Uint8 r, Uint8 g, Uint8 b);
void esgx_draw_rect_raw(SDL_Surface *s, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, int filled);
void esgx_draw_circle_raw(SDL_Surface *s, int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, int filled);
void esgx_draw_line_raw(SDL_Surface *s, int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b);

void esgx_draw_free(ESGX_Draw *d);

#endif