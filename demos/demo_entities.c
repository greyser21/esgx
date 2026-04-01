#include "../include/esgx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define WIN_W      800
#define WIN_H      600
#define NB_ENTITIES 15

typedef struct {
    double wander_angle;
    double wander_speed;
    int    color_r, color_g, color_b;
    int    radius;
} EntityData;

static void wander_update(ESGX_Entity *self, void *userdata)
{
    EntityData *d = (EntityData *)userdata;

    d->wander_angle += ((rand() % 60) - 30) * 0.05;

    self->vx = cos(d->wander_angle) * d->wander_speed;
    self->vy = sin(d->wander_angle) * d->wander_speed;

    esgx_entity_bounce(self, WIN_W, WIN_H);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    ESGX_Event    ev       = {0};
    ESGX_Entity  *entities[NB_ENTITIES];
    ESGX_Draw    *draws[NB_ENTITIES];
    EntityData    data[NB_ENTITIES];
    int i;

    srand((unsigned int)time(NULL));

    if (esgx_window_init(WIN_W, WIN_H, "ESGX Demo - Entités autonomes") != 0)
        return 1;

    for (i = 0; i < NB_ENTITIES; i++) {
        int r = rand() % 256;
        int g = rand() % 256;
        int b = rand() % 256;
        int radius = 10 + rand() % 25;

        data[i].color_r     = r;
        data[i].color_g     = g;
        data[i].color_b     = b;
        data[i].radius      = radius;
        data[i].wander_angle = (rand() % 360) * M_PI / 180.0;
        data[i].wander_speed = 1.5 + (rand() % 30) * 0.1;

        draws[i] = esgx_draw_circle(radius,
                                    (Uint8)r, (Uint8)g, (Uint8)b, 1);

        double x = (double)(rand() % (WIN_W - radius * 2));
        double y = (double)(rand() % (WIN_H - radius * 2));
        entities[i] = esgx_entity_create(x, y, draws[i]);

        entities[i]->update   = wander_update;
        entities[i]->userdata = &data[i];
    }

    printf("=== Demo Entités autonomes ===\n");
    printf("%d entités se promènent. Échap pour quitter.\n", NB_ENTITIES);

    while (esgx_event_poll(&ev)) {
        esgx_entities_update_all(entities, NB_ENTITIES);

        esgx_window_clear(20, 20, 40);

        int gx, gy;
        for (gx = 0; gx < WIN_W; gx += 80)
            esgx_draw_line_raw(esgx_screen, gx, 0, gx, WIN_H, 30, 30, 50);
        for (gy = 0; gy < WIN_H; gy += 80)
            esgx_draw_line_raw(esgx_screen, 0, gy, WIN_W, gy, 30, 30, 50);

        esgx_entities_draw_all(entities, NB_ENTITIES, esgx_screen);

        int j;
        for (i = 0; i < NB_ENTITIES; i++) {
            for (j = i + 1; j < NB_ENTITIES; j++) {
                double dx = entities[i]->x - entities[j]->x;
                double dy = entities[i]->y - entities[j]->y;
                double dist = sqrt(dx * dx + dy * dy);
                if (dist < 120.0) {
                    Uint8 alpha = (Uint8)(255 * (1.0 - dist / 120.0));
                    esgx_draw_line_raw(esgx_screen,
                                       (int)(entities[i]->x + data[i].radius),
                                       (int)(entities[i]->y + data[i].radius),
                                       (int)(entities[j]->x + data[j].radius),
                                       (int)(entities[j]->y + data[j].radius),
                                       alpha, alpha, alpha);
                }
            }
        }

        esgx_window_flip();
        SDL_Delay(16);
    }

    for (i = 0; i < NB_ENTITIES; i++) {
        esgx_entity_free(entities[i]);
        esgx_draw_free(draws[i]);
    }
    esgx_window_quit();
    return 0;
}