#include "../include/esgx_draw.h"
#include "../include/esgx_anim.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* ============================================================
 *  Utilitaires bas niveau
 * ============================================================ */

void esgx_put_pixel(SDL_Surface *s, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    if (x < 0 || y < 0 || x >= s->w || y >= s->h) return;

    Uint32 color = SDL_MapRGB(s->format, r, g, b);
    Uint8 *p     = (Uint8 *)s->pixels + y * s->pitch + x * s->format->BytesPerPixel;

    switch (s->format->BytesPerPixel) {
        case 1: *p = (Uint8)color; break;
        case 2: *(Uint16 *)p = (Uint16)color; break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (color >> 16) & 0xFF;
                p[1] = (color >>  8) & 0xFF;
                p[2] =  color        & 0xFF;
            } else {
                p[0] =  color        & 0xFF;
                p[1] = (color >>  8) & 0xFF;
                p[2] = (color >> 16) & 0xFF;
            }
            break;
        case 4: *(Uint32 *)p = color; break;
    }
}

void esgx_draw_rect_raw(SDL_Surface *s, int x, int y, int w, int h,
                         Uint8 r, Uint8 g, Uint8 b, int filled)
{
    if (filled) {
        SDL_Rect rect = { (Sint16)x, (Sint16)y, (Uint16)w, (Uint16)h };
        SDL_FillRect(s, &rect, SDL_MapRGB(s->format, r, g, b));
    } else {
        /* Bords */
        int i;
        for (i = x; i < x + w; i++) {
            esgx_put_pixel(s, i, y,         r, g, b);
            esgx_put_pixel(s, i, y + h - 1, r, g, b);
        }
        for (i = y; i < y + h; i++) {
            esgx_put_pixel(s, x,         i, r, g, b);
            esgx_put_pixel(s, x + w - 1, i, r, g, b);
        }
    }
}

void esgx_draw_circle_raw(SDL_Surface *s, int cx, int cy, int radius,
                           Uint8 r, Uint8 g, Uint8 b, int filled)
{
    /* Algorithme de Bresenham */
    int x = 0, y = radius;
    int d = 3 - 2 * radius;

    while (x <= y) {
        if (filled) {
            int i;
            for (i = cx - x; i <= cx + x; i++) {
                esgx_put_pixel(s, i, cy + y, r, g, b);
                esgx_put_pixel(s, i, cy - y, r, g, b);
            }
            for (i = cx - y; i <= cx + y; i++) {
                esgx_put_pixel(s, i, cy + x, r, g, b);
                esgx_put_pixel(s, i, cy - x, r, g, b);
            }
        } else {
            esgx_put_pixel(s, cx + x, cy + y, r, g, b);
            esgx_put_pixel(s, cx - x, cy + y, r, g, b);
            esgx_put_pixel(s, cx + x, cy - y, r, g, b);
            esgx_put_pixel(s, cx - x, cy - y, r, g, b);
            esgx_put_pixel(s, cx + y, cy + x, r, g, b);
            esgx_put_pixel(s, cx - y, cy + x, r, g, b);
            esgx_put_pixel(s, cx + y, cy - x, r, g, b);
            esgx_put_pixel(s, cx - y, cy - x, r, g, b);
        }
        if (d < 0) d += 4 * x + 6;
        else { d += 4 * (x - y) + 10; y--; }
        x++;
    }
}

void esgx_draw_line_raw(SDL_Surface *s, int x1, int y1, int x2, int y2,
                         Uint8 r, Uint8 g, Uint8 b)
{
    /* Algorithme de Bresenham */
    int dx = abs(x2 - x1), sx = (x1 < x2) ? 1 : -1;
    int dy = abs(y2 - y1), sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy, e2;

    while (1) {
        esgx_put_pixel(s, x1, y1, r, g, b);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 <  dx) { err += dx; y1 += sy; }
    }
}

/* ============================================================
 *  Transform par défaut
 * ============================================================ */

static ESGX_Transform transform_default(void)
{
    ESGX_Transform t;
    t.scale_x  = 1.0;
    t.scale_y  = 1.0;
    t.angle    = 0.0;
    t.flip_h   = 0;
    t.flip_v   = 0;
    t.offset_x = 0;
    t.offset_y = 0;
    return t;
}

/* ============================================================
 *  Constructeurs
 * ============================================================ */

ESGX_Draw *esgx_draw_from_image(const char *path)
{
    SDL_Surface *raw = SDL_LoadBMP(path);
    if (!raw) {
        fprintf(stderr, "[ESGX] Impossible de charger '%s': %s\n",
                path, SDL_GetError());
        return NULL;
    }
    /* Optimisation : convertir au format de l'écran */
    SDL_Surface *opt = SDL_DisplayFormat(raw);
    SDL_FreeSurface(raw);
    if (!opt) opt = raw; /* fallback si DisplayFormat échoue */

    ESGX_Draw *d = (ESGX_Draw *)malloc(sizeof(ESGX_Draw));
    if (!d) { SDL_FreeSurface(opt); return NULL; }

    d->type       = ESGX_DRAW_IMAGE;
    d->data.image = opt;
    d->transform  = transform_default();
    return d;
}

ESGX_Draw *esgx_draw_from_func(ESGX_DrawFunc func, void *userdata)
{
    ESGX_Draw *d = (ESGX_Draw *)malloc(sizeof(ESGX_Draw));
    if (!d) return NULL;
    d->type               = ESGX_DRAW_FUNC;
    d->data.custom.func   = func;
    d->data.custom.userdata = userdata;
    d->transform          = transform_default();
    return d;
}

ESGX_Draw *esgx_draw_rect(int w, int h, Uint8 r, Uint8 g, Uint8 b, int filled)
{
    ESGX_Draw *d = (ESGX_Draw *)malloc(sizeof(ESGX_Draw));
    if (!d) return NULL;
    d->type            = ESGX_DRAW_SHAPE;
    d->data.shape.type = ESGX_SHAPE_RECT;
    d->data.shape.w    = w;
    d->data.shape.h    = h;
    d->data.shape.r    = r;
    d->data.shape.g    = g;
    d->data.shape.b    = b;
    d->data.shape.filled = filled;
    d->transform       = transform_default();
    return d;
}

ESGX_Draw *esgx_draw_circle(int radius, Uint8 r, Uint8 g, Uint8 b, int filled)
{
    ESGX_Draw *d = (ESGX_Draw *)malloc(sizeof(ESGX_Draw));
    if (!d) return NULL;
    d->type            = ESGX_DRAW_SHAPE;
    d->data.shape.type = ESGX_SHAPE_CIRCLE;
    d->data.shape.w    = radius;  /* on stocke le rayon dans w */
    d->data.shape.h    = radius;
    d->data.shape.r    = r;
    d->data.shape.g    = g;
    d->data.shape.b    = b;
    d->data.shape.filled = filled;
    d->transform       = transform_default();
    return d;
}

ESGX_Draw *esgx_draw_from_anim(struct ESGX_Animation *anim)
{
    ESGX_Draw *d = (ESGX_Draw *)malloc(sizeof(ESGX_Draw));
    if (!d) return NULL;
    d->type       = ESGX_DRAW_ANIM;
    d->data.anim  = anim;
    d->transform  = transform_default();
    return d;
}

/* ============================================================
 *  Transformations
 * ============================================================ */

void esgx_draw_set_scale(ESGX_Draw *d, double sx, double sy)
{
    if (!d) return;
    d->transform.scale_x = sx;
    d->transform.scale_y = sy;
}

void esgx_draw_set_angle(ESGX_Draw *d, double angle)
{
    if (!d) return;
    d->transform.angle = angle;
}

void esgx_draw_set_flip(ESGX_Draw *d, int flip_h, int flip_v)
{
    if (!d) return;
    d->transform.flip_h = flip_h;
    d->transform.flip_v = flip_v;
}

void esgx_draw_set_offset(ESGX_Draw *d, int ox, int oy)
{
    if (!d) return;
    d->transform.offset_x = ox;
    d->transform.offset_y = oy;
}

void esgx_draw_reset_transform(ESGX_Draw *d)
{
    if (!d) return;
    d->transform = transform_default();
}

/* ============================================================
 *  Application des transformations sur une SDL_Surface
 *  (scale + flip + rotation par rotation pixel-by-pixel)
 * ============================================================ */

/*
 * Applique scale, flip et rotation sur 'src' et retourne
 * une nouvelle SDL_Surface transformée.
 * L'appelant est responsable de libérer la surface retournée.
 */
static SDL_Surface *apply_transform(SDL_Surface *src, ESGX_Transform *t)
{
    if (!src) return NULL;

    /* 1. Calcul des dimensions finales après scale */
    int new_w = (int)(src->w * fabs(t->scale_x));
    int new_h = (int)(src->h * fabs(t->scale_y));
    if (new_w < 1) new_w = 1;
    if (new_h < 1) new_h = 1;

    /* 2. Rotation : agrandir la surface pour contenir l'image tournée */
    double rad   = t->angle * M_PI / 180.0;
    double cosA  = fabs(cos(rad));
    double sinA  = fabs(sin(rad));
    int rot_w    = (int)(new_w * cosA + new_h * sinA) + 1;
    int rot_h    = (int)(new_w * sinA + new_h * cosA) + 1;

    SDL_Surface *dst = SDL_CreateRGBSurface(SDL_SWSURFACE, rot_w, rot_h,
                                             src->format->BitsPerPixel,
                                             src->format->Rmask,
                                             src->format->Gmask,
                                             src->format->Bmask,
                                             src->format->Amask);
    if (!dst) return NULL;

    /* Couleur de remplissage transparente / noire */
    SDL_FillRect(dst, NULL, SDL_MapRGB(dst->format, 0, 0, 0));

    /* 3. Transformation pixel par pixel */
    double cx_dst = rot_w / 2.0;
    double cy_dst = rot_h / 2.0;
    double cx_src = new_w / 2.0;
    double cy_src = new_h / 2.0;

    int dx, dy;
    for (dy = 0; dy < rot_h; dy++) {
        for (dx = 0; dx < rot_w; dx++) {
            /* Coordonnées relatives au centre destination */
            double rx = dx - cx_dst;
            double ry = dy - cy_dst;

            /* Rotation inverse */
            double sx_r = rx * cos(-rad) - ry * sin(-rad) + cx_src;
            double sy_r = rx * sin(-rad) + ry * cos(-rad) + cy_src;

            /* Retour aux coordonnées sources (avant scale) */
            int sx_orig = (int)(sx_r / fabs(t->scale_x));
            int sy_orig = (int)(sy_r / fabs(t->scale_y));

            /* Flip */
            if (t->flip_h) sx_orig = src->w - 1 - sx_orig;
            if (t->flip_v) sy_orig = src->h - 1 - sy_orig;

            /* Vérification des bornes */
            if (sx_orig < 0 || sx_orig >= src->w ||
                sy_orig < 0 || sy_orig >= src->h)
                continue;

            /* Copie du pixel */
            Uint8 *sp = (Uint8 *)src->pixels
                        + sy_orig * src->pitch
                        + sx_orig * src->format->BytesPerPixel;
            Uint8 *dp = (Uint8 *)dst->pixels
                        + dy * dst->pitch
                        + dx * dst->format->BytesPerPixel;

            memcpy(dp, sp, src->format->BytesPerPixel);
        }
    }

    return dst;
}

/* ============================================================
 *  Rendu principal
 * ============================================================ */

void esgx_draw_render(ESGX_Draw *d, SDL_Surface *screen, int x, int y)
{
    if (!d || !screen) return;

    ESGX_Transform *t = &d->transform;
    x += t->offset_x;
    y += t->offset_y;

    switch (d->type) {

        case ESGX_DRAW_IMAGE: {
            SDL_Surface *src = d->data.image;
            int trivial = (t->scale_x == 1.0 && t->scale_y == 1.0 &&
                           t->angle   == 0.0  &&
                           t->flip_h  == 0    && t->flip_v == 0);
            if (trivial) {
                SDL_Rect dst_rect = { (Sint16)x, (Sint16)y, 0, 0 };
                SDL_BlitSurface(src, NULL, screen, &dst_rect);
            } else {
                SDL_Surface *transformed = apply_transform(src, t);
                if (transformed) {
                    SDL_Rect dst_rect = { (Sint16)x, (Sint16)y, 0, 0 };
                    SDL_BlitSurface(transformed, NULL, screen, &dst_rect);
                    SDL_FreeSurface(transformed);
                }
            }
            break;
        }

        case ESGX_DRAW_FUNC:
            if (d->data.custom.func)
                d->data.custom.func(screen, x, y, d->data.custom.userdata);
            break;

        case ESGX_DRAW_SHAPE: {
            ESGX_Shape *sh = &d->data.shape;
            int sw = (int)(sh->w * t->scale_x);
            int sh2 = (int)(sh->h * t->scale_y);
            switch (sh->type) {
                case ESGX_SHAPE_RECT:
                    esgx_draw_rect_raw(screen, x, y, sw, sh2,
                                       sh->r, sh->g, sh->b, sh->filled);
                    break;
                case ESGX_SHAPE_CIRCLE:
                    esgx_draw_circle_raw(screen, x, y, (int)(sh->w * t->scale_x),
                                         sh->r, sh->g, sh->b, sh->filled);
                    break;
                case ESGX_SHAPE_LINE:
                    esgx_draw_line_raw(screen, x, y, sh->x2, sh->y2,
                                       sh->r, sh->g, sh->b);
                    break;
            }
            break;
        }

        case ESGX_DRAW_ANIM: {
            if (!d->data.anim) break;
            esgx_anim_update(d->data.anim);
            SDL_Surface *frame = esgx_anim_current_frame(d->data.anim);
            if (frame) {
                int trivial = (t->scale_x == 1.0 && t->scale_y == 1.0 &&
                               t->angle   == 0.0  &&
                               t->flip_h  == 0    && t->flip_v == 0);
                if (trivial) {
                    SDL_Rect dst_rect = { (Sint16)x, (Sint16)y, 0, 0 };
                    SDL_BlitSurface(frame, NULL, screen, &dst_rect);
                } else {
                    SDL_Surface *transformed = apply_transform(frame, t);
                    if (transformed) {
                        SDL_Rect dst_rect = { (Sint16)x, (Sint16)y, 0, 0 };
                        SDL_BlitSurface(transformed, NULL, screen, &dst_rect);
                        SDL_FreeSurface(transformed);
                    }
                }
            }
            break;
        }
    }
}

/* ============================================================
 *  Libération
 * ============================================================ */

void esgx_draw_free(ESGX_Draw *d)
{
    if (!d) return;
    switch (d->type) {
        case ESGX_DRAW_IMAGE:
            if (d->data.image) SDL_FreeSurface(d->data.image);
            break;
        case ESGX_DRAW_ANIM:
            if (d->data.anim) esgx_anim_free(d->data.anim);
            break;
        default:
            break;
    }
    free(d);
}