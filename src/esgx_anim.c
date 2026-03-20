#include "../include/esgx_anim.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static ESGX_Animation *anim_alloc(int frame_count, Uint32 delay_ms)
{
    ESGX_Animation *a = (ESGX_Animation *)malloc(sizeof(ESGX_Animation));
    if (!a) return NULL;
    a->frames      = (SDL_Surface **)calloc(frame_count, sizeof(SDL_Surface *));
    if (!a->frames) { free(a); return NULL; }
    a->frame_count = frame_count;
    a->current     = 0;
    a->delay_ms    = delay_ms;
    a->last_tick   = SDL_GetTicks();
    a->loop        = 1;
    return a;
}

ESGX_Animation *esgx_anim_from_sheet(const char *path,
                                      int frame_w, int frame_h,
                                      int frame_count, Uint32 delay_ms)
{
    SDL_Surface *sheet = SDL_LoadBMP(path);
    if (!sheet) {
        fprintf(stderr, "[ESGX] Impossible de charger spritesheet '%s': %s\n",
                path, SDL_GetError());
        return NULL;
    }
    ESGX_Animation *a = anim_alloc(frame_count, delay_ms);
    if (!a) { SDL_FreeSurface(sheet); return NULL; }
    int i;
    for (i = 0; i < frame_count; i++) {
        SDL_Surface *frame = SDL_CreateRGBSurface(SDL_SWSURFACE,
                                                   frame_w, frame_h,
                                                   sheet->format->BitsPerPixel,
                                                   sheet->format->Rmask,
                                                   sheet->format->Gmask,
                                                   sheet->format->Bmask,
                                                   sheet->format->Amask);
        if (!frame) continue;
        SDL_Rect src_rect = { (Sint16)(i * frame_w), 0,
                              (Uint16)frame_w, (Uint16)frame_h };
        SDL_BlitSurface(sheet, &src_rect, frame, NULL);
        a->frames[i] = SDL_DisplayFormat(frame);
        SDL_FreeSurface(frame);
    }
    SDL_FreeSurface(sheet);
    return a;
}

ESGX_Animation *esgx_anim_from_files(const char **paths,
                                      int frame_count, Uint32 delay_ms)
{
    ESGX_Animation *a = anim_alloc(frame_count, delay_ms);
    if (!a) return NULL;
    int i;
    for (i = 0; i < frame_count; i++) {
        SDL_Surface *raw = SDL_LoadBMP(paths[i]);
        if (!raw) { a->frames[i] = NULL; continue; }
        a->frames[i] = SDL_DisplayFormat(raw);
        SDL_FreeSurface(raw);
    }
    return a;
}

ESGX_Animation *esgx_anim_from_surfaces(SDL_Surface **frames,
                                         int frame_count, Uint32 delay_ms)
{
    ESGX_Animation *a = anim_alloc(frame_count, delay_ms);
    if (!a) return NULL;
    int i;
    for (i = 0; i < frame_count; i++)
        a->frames[i] = frames[i];
    return a;
}

void esgx_anim_update(ESGX_Animation *anim)
{
    if (!anim || anim->frame_count <= 1) return;
    if (!anim->loop && anim->current == anim->frame_count - 1) return;
    Uint32 now = SDL_GetTicks();
    if (now - anim->last_tick >= anim->delay_ms) {
        anim->current++;
        if (anim->current >= anim->frame_count)
            anim->current = anim->loop ? 0 : anim->frame_count - 1;
        anim->last_tick = now;
    }
}

SDL_Surface *esgx_anim_current_frame(ESGX_Animation *anim)
{
    if (!anim || anim->frame_count == 0) return NULL;
    return anim->frames[anim->current];
}

void esgx_anim_reset(ESGX_Animation *anim)
{
    if (!anim) return;
    anim->current   = 0;
    anim->last_tick = SDL_GetTicks();
}

void esgx_anim_set_loop(ESGX_Animation *anim, int loop)
{
    if (!anim) return;
    anim->loop = loop;
}

int esgx_anim_finished(ESGX_Animation *anim)
{
    if (!anim) return 1;
    return (!anim->loop && anim->current == anim->frame_count - 1);
}

void esgx_anim_free(ESGX_Animation *anim)
{
    if (!anim) return;
    int i;
    for (i = 0; i < anim->frame_count; i++)
        if (anim->frames[i]) SDL_FreeSurface(anim->frames[i]);
    free(anim->frames);
    free(anim);
}
