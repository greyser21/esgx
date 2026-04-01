#include "../include/esgx.h"
#include <stdlib.h>
#include <string.h>

#define WIN_W       900
#define WIN_H       650
#define MAX_SHAPES  256

typedef struct { Uint8 r, g, b; const char *name; } Color;

static Color palette[] = {
    { 255, 80,  80,  "Rouge"  },
    {  80, 255,  80, "Vert"   },
    {  80, 120, 255, "Bleu"   },
    { 255, 220,  50, "Jaune"  },
    { 200,  80, 255, "Violet" }
};
#define NB_COLORS 5

typedef enum { MODE_RECT, MODE_CIRCLE } DrawMode;

typedef struct {
    DrawMode mode;
    int x, y, size;
    Color color;
} PlacedShape;

static void draw_hud(SDL_Surface *screen, int color_idx, DrawMode mode, int shape_count)
{
    esgx_draw_rect_raw(screen, 0, WIN_H - 80, WIN_W, 80, 20, 20, 20, 1);
    esgx_draw_line_raw(screen, 0, WIN_H - 80, WIN_W, WIN_H - 80, 80, 80, 80);

    int i;
    for (i = 0; i < NB_COLORS; i++) {
        int px = 10 + i * 45;
        int py = WIN_H - 65;
        esgx_draw_rect_raw(screen, px, py, 35, 30,
                           palette[i].r, palette[i].g, palette[i].b, 1);
        if (i == color_idx)
            esgx_draw_rect_raw(screen, px - 2, py - 2, 39, 34, 255, 255, 255, 0);
    }

    int mx = 250, my = WIN_H - 65;
    esgx_draw_rect_raw(screen, mx, my, 100, 30, 50, 50, 70, 1);
    if (mode == MODE_RECT)
        esgx_draw_rect_raw(screen, mx + 10, my + 8, 30, 15,
                           palette[color_idx].r, palette[color_idx].g, palette[color_idx].b, 1);
    else
        esgx_draw_circle_raw(screen, mx + 50, my + 15, 12,
                             palette[color_idx].r, palette[color_idx].g, palette[color_idx].b, 1);

    int cx = 400, cy = WIN_H - 65;
    esgx_draw_rect_raw(screen, cx, cy, 200, 30, 40, 40, 40, 1);
    int bars = shape_count < 20 ? shape_count : 20;
    for (i = 0; i < bars; i++)
        esgx_draw_rect_raw(screen, cx + 5 + i * 9, cy + 5, 6, 20, 100, 200, 100, 1);

    int bx = 620, by = WIN_H - 65;
    esgx_draw_rect_raw(screen, bx, by, 120, 30, 140, 40, 40, 1);
    esgx_draw_line_raw(screen, bx + 50, by + 8,  bx + 65, by + 22, 255, 255, 255);
    esgx_draw_line_raw(screen, bx + 65, by + 8,  bx + 50, by + 22, 255, 255, 255);
}

static void draw_ghost(SDL_Surface *screen, int mx, int my,
                       DrawMode mode, int size, Color c)
{
    Uint8 gr = c.r / 2, gg = c.g / 2, gb = c.b / 2;
    if (mode == MODE_RECT)
        esgx_draw_rect_raw(screen, mx - size/2, my - size/2, size, size, gr, gg, gb, 0);
    else
        esgx_draw_circle_raw(screen, mx, my, size/2, gr, gg, gb, 0);
}

static void draw_shape(SDL_Surface *screen, PlacedShape *s)
{
    if (s->mode == MODE_RECT)
        esgx_draw_rect_raw(screen, s->x - s->size/2, s->y - s->size/2,
                           s->size, s->size, s->color.r, s->color.g, s->color.b, 1);
    else
        esgx_draw_circle_raw(screen, s->x, s->y, s->size/2,
                             s->color.r, s->color.g, s->color.b, 1);
}

int main(int argc, char *argv[])
{
    (void)argc; /* non utilisé */
    (void)argv; /* non utilisé */
    
    ESGX_Event   ev          = {0};
    PlacedShape  shapes[MAX_SHAPES];
    int          shape_count = 0;
    int          color_idx   = 0;
    DrawMode     mode        = MODE_RECT;
    int          size        = 60;

    if (esgx_window_init(WIN_W, WIN_H, "ESGX Demo - Peintre") != 0)
        return 1;

    while (esgx_event_poll(&ev)) {

        if (ev.key_down) {
            if (ev.raw_key.unicode == '+') {
                if (size < 200) size += 10;
            } else if (ev.raw_key.unicode == '-') {
                if (size > 10) size -= 10;
            } else {
                switch (ev.raw_key.sym) {
                    case SDLK_r:        mode = MODE_RECT;                          break;
                    case SDLK_c:        mode = MODE_CIRCLE;                        break;
                    case SDLK_1:        color_idx = 0;                             break;
                    case SDLK_2:        color_idx = 1;                             break;
                    case SDLK_3:        color_idx = 2;                             break;
                    case SDLK_4:        color_idx = 3;                             break;
                    case SDLK_5:        color_idx = 4;                             break;
                    case SDLK_DELETE:
                    case SDLK_BACKSPACE: if (shape_count > 0) shape_count--;      break;
                    default: break;
                }
            }
        }

        if (ev.mouse_button_down && ev.mouse_button == ESGX_MOUSE_LEFT) {
            if (ev.mouse_y < WIN_H - 80 && shape_count < MAX_SHAPES) {
                PlacedShape *s = &shapes[shape_count++];
                s->mode  = mode;
                s->x     = ev.mouse_x;
                s->y     = ev.mouse_y;
                s->size  = size;
                s->color = palette[color_idx];
            }
        }

        esgx_window_clear(245, 245, 240);

        int i;
        for (i = 0; i < shape_count; i++)
            draw_shape(esgx_screen, &shapes[i]);

        if (ev.mouse_y < WIN_H - 80)
            draw_ghost(esgx_screen, ev.mouse_x, ev.mouse_y,
                       mode, size, palette[color_idx]);

        draw_hud(esgx_screen, color_idx, mode, shape_count);
        esgx_window_flip();
        SDL_Delay(16);
    }

    esgx_window_quit();
    return 0;
}
