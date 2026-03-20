#ifndef ESGX_ANIM_H
#define ESGX_ANIM_H

#include <SDL/SDL.h>

/*
 * Structure Animation :
 * Contient une séquence de frames SDL_Surface*.
 * Le rendu avance automatiquement en fonction du temps (delay_ms).
 */
typedef struct ESGX_Animation {
    SDL_Surface **frames;   /* Tableau de frames */
    int          frame_count;
    int          current;   /* Index de la frame courante */
    Uint32       delay_ms;  /* Délai entre deux frames en millisecondes */
    Uint32       last_tick; /* Timestamp SDL du dernier changement */
    int          loop;      /* 1 = boucle infinie, 0 = s'arrête à la fin */
} ESGX_Animation;

/*
 * Charge une animation depuis un spritesheet (une seule image).
 * path       : chemin du fichier BMP
 * frame_w    : largeur d'une frame
 * frame_h    : hauteur d'une frame
 * frame_count: nombre de frames (disposées en ligne)
 * delay_ms   : délai entre chaque frame
 */
ESGX_Animation *esgx_anim_from_sheet(const char *path,
                                      int frame_w, int frame_h,
                                      int frame_count, Uint32 delay_ms);

/*
 * Assemble une animation depuis plusieurs fichiers BMP distincts.
 * paths[]    : tableau de chemins (frame_count entrées)
 * frame_count: nombre de fichiers
 * delay_ms   : délai entre chaque frame
 */
ESGX_Animation *esgx_anim_from_files(const char **paths,
                                      int frame_count, Uint32 delay_ms);

/*
 * Crée une animation depuis un tableau de SDL_Surface* déjà chargées.
 * L'animation prend ownership des surfaces.
 */
ESGX_Animation *esgx_anim_from_surfaces(SDL_Surface **frames,
                                         int frame_count, Uint32 delay_ms);

/*
 * Met à jour l'animation selon le temps courant (SDL_GetTicks).
 * À appeler chaque frame avant le rendu.
 */
void esgx_anim_update(ESGX_Animation *anim);

/*
 * Retourne la frame courante (SDL_Surface*).
 */
SDL_Surface *esgx_anim_current_frame(ESGX_Animation *anim);

/*
 * Réinitialise l'animation à la première frame.
 */
void esgx_anim_reset(ESGX_Animation *anim);

/*
 * Active / désactive la boucle.
 */
void esgx_anim_set_loop(ESGX_Animation *anim, int loop);

/*
 * Retourne 1 si l'animation est terminée (loop=0 et dernière frame atteinte).
 */
int esgx_anim_finished(ESGX_Animation *anim);

/*
 * Libère l'animation et toutes ses frames.
 */
void esgx_anim_free(ESGX_Animation *anim);

#endif /* ESGX_ANIM_H */