#include "../include/esgx_entity.h"
#include "../include/esgx_anim.h"
#include <stdlib.h>
#include <math.h>

static void entity_compute_size(ESGX_Entity *e)
{
    if (!e->draw) { e->w = 0; e->h = 0; return; }

    switch (e->draw->type) {
        case ESGX_DRAW_IMAGE:
            if (e->draw->data.image) {
                e->w = e->draw->data.image->w;
                e->h = e->draw->data.image->h;
            }
            break;
        case ESGX_DRAW_SHAPE:
            if (e->draw->data.shape.type == ESGX_SHAPE_CIRCLE) {
                e->w = e->draw->data.shape.w * 2;
                e->h = e->draw->data.shape.h * 2;
            } else {
                e->w = e->draw->data.shape.w;
                e->h = e->draw->data.shape.h;
            }
            break;
        case ESGX_DRAW_ANIM:
            if (e->draw->data.anim) {
                SDL_Surface *f = esgx_anim_current_frame(e->draw->data.anim);
                if (f) { e->w = f->w; e->h = f->h; }
            }
            break;
        default:
            e->w = 32; e->h = 32;
            break;
    }
}

ESGX_Entity *esgx_entity_create(double x, double y, ESGX_Draw *draw)
{
    ESGX_Entity *e = (ESGX_Entity *)malloc(sizeof(ESGX_Entity));
    if (!e) return NULL;

    e->x        = x;
    e->y        = y;
    e->vx       = 0.0;
    e->vy       = 0.0;
    e->draw     = draw;
    e->visible  = 1;
    e->active   = 1;
    e->update   = NULL;
    e->userdata = NULL;

    entity_compute_size(e);
    return e;
}

void esgx_entity_free(ESGX_Entity *entity)
{
    free(entity);
}

void esgx_entity_update(ESGX_Entity *entity)
{
    if (!entity || !entity->active) return;

    entity->x += entity->vx;
    entity->y += entity->vy;

    if (entity->update)
        entity->update(entity, entity->userdata);
}

void esgx_entity_bounce(ESGX_Entity *entity, int win_w, int win_h)
{
    if (!entity) return;

    if (entity->x < 0) {
        entity->x  = 0;
        entity->vx = -entity->vx;
    }
    if (entity->y < 0) {
        entity->y  = 0;
        entity->vy = -entity->vy;
    }
    if (entity->x + entity->w > win_w) {
        entity->x  = win_w - entity->w;
        entity->vx = -entity->vx;
    }
    if (entity->y + entity->h > win_h) {
        entity->y  = win_h - entity->h;
        entity->vy = -entity->vy;
    }
}

int esgx_entity_move_toward(ESGX_Entity *entity,
                              double x_target, double y_target,
                              double speed)
{
    if (!entity) return 1;

    double dx   = x_target - entity->x;
    double dy   = y_target - entity->y;
    double dist = sqrt(dx * dx + dy * dy);

    if (dist <= speed) {
        entity->x  = x_target;
        entity->y  = y_target;
        entity->vx = 0.0;
        entity->vy = 0.0;
        return 1;
    }

    entity->vx = (dx / dist) * speed;
    entity->vy = (dy / dist) * speed;
    entity->x += entity->vx;
    entity->y += entity->vy;
    return 0;
}

void esgx_entity_draw(ESGX_Entity *entity, SDL_Surface *screen)
{
    if (!entity || !entity->visible || !entity->draw) return;
    esgx_draw_render(entity->draw, screen, (int)entity->x, (int)entity->y);
}

void esgx_entity_set_pos(ESGX_Entity *e, double x, double y)
{
    if (!e) return;
    e->x = x; e->y = y;
}

void esgx_entity_set_vel(ESGX_Entity *e, double vx, double vy)
{
    if (!e) return;
    e->vx = vx; e->vy = vy;
}

void esgx_entity_set_draw(ESGX_Entity *e, ESGX_Draw *draw)
{
    if (!e) return;
    e->draw = draw;
    entity_compute_size(e);
}

void esgx_entities_update_all(ESGX_Entity **entities, int size)
{
    int i;
    for (i = 0; i < size; i++)
        if (entities[i]) esgx_entity_update(entities[i]);
}

void esgx_entities_draw_all(ESGX_Entity **entities, int size,
                              SDL_Surface *screen)
{
    int i;
    for (i = 0; i < size; i++)
        if (entities[i]) esgx_entity_draw(entities[i], screen);
}