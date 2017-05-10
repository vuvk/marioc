#pragma once

#include "SDL2/SDL.h"

#include "defines.h"
#include "vector2d.h"
#include "physObj.h"


typedef struct
{
    //SVector2f pos;          /* position */
    //int w, h;               /* width-height */

    unsigned short int physBodyIndex;

    float timeToRemove;     /* time before destroy */
    float _timeToRemove;    /* delay */

    SDL_Texture** texture;  /* */
} SCorpse;

/* all corpses here */
SCorpse* corpses[MAX_CREATURES_COUNT];

/* constructor|destructor */
SCorpse* CorpseCreate (float x, float y, int w, int h, float timeToRemove, SDL_Texture** texture);
void CorpseDestroy (SCorpse** corpse);
void CorpseClearAll ();

void CorpseGetSdlRect (SCorpse* corpse, SDL_Rect* rect);
SDL_Texture* CorpseGetTexture (SCorpse* corpse);

void CorpsesUpdate ();
