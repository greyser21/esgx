#ifndef ESGX_ENTITY_H
#define ESGX_ENTITY_H

#include "esgx_draw.h"

typedef struct ESGX_Entity
{
    double x, y;
    double vx, vy;
    ESGX_Draw *draw;
    int visible;
    int active;

    int w, h;

    void (*update)(struct ESGX_Entity *self, void *userdata);
    void *userdata;
} ESGX_Entity;

ESGX_Entity *esgx_entity_create(double x, double y, ESGX_Draw *draw);

void esgx_entity_free(ESGX_Entity *entity);

void esgx_entity_update(ESGX_Entity *entity);

void esgx_entity_bounce(ESGX_Entity *entity, int win_w, int win_h);

int esgx_entity_move_toward(ESGX_Entity *entity, double x_target, double y_target, double speed);

void esgx_entity_draw(ESGX_Entity *entity, SDL_Surface *screen);

void esgx_entity_set_pos(ESGX_Entity *e, double x, double y);
void esgx_entity_set_vel(ESGX_Entity *e, double vx, double vy);
void esgx_entity_set_draw(ESGX_Entity *e, ESGX_Draw *draw);

void esgx_entities_update_all(ESGX_Entity **entities, int size);
void esgx_entities_draw_all(ESGX_Entity **entities, int size,
                            SDL_Surface *screen);

#endif