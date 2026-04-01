#ifndef ESGX_ANIM_H
#define ESGX_ANIM_H

#include <SDL/SDL.h>

typedef struct ESGX_Animation
{
    SDL_Surface **frames;
    int frame_count;
    int current;
    Uint32 delay_ms;
    Uint32 last_tick;
    int loop;
} ESGX_Animation;

ESGX_Animation *esgx_anim_from_sheet(const char *path, int frame_w, int frame_h, int frame_count, Uint32 delay_ms);

ESGX_Animation *esgx_anim_from_files(const char **paths, int frame_count, Uint32 delay_ms);

ESGX_Animation *esgx_anim_from_surfaces(SDL_Surface **frames, int frame_count, Uint32 delay_ms);

void esgx_anim_update(ESGX_Animation *anim);

SDL_Surface *esgx_anim_current_frame(ESGX_Animation *anim);

void esgx_anim_reset(ESGX_Animation *anim);

void esgx_anim_set_loop(ESGX_Animation *anim, int loop);

int esgx_anim_finished(ESGX_Animation *anim);

void esgx_anim_free(ESGX_Animation *anim);

#endif