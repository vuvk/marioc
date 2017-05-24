#pragma once

#include "SDL2/SDL.h"

#include "defines.h"
#include "list.h"
//#include "vector2d.h"
#include "physObj.h"


typedef struct
{
    SPhysObject* physBody;

    float timeToRemove;     /* time before destroy */
    float _timeToRemove;    /* delay */

    SDL_Texture* texture;  /* */
} SCorpse;

/* all corpses here */
SList* corpses;

/* constructor|destructor */
SCorpse* CorpseCreate (float x, float y, ubyte w, ubyte h, float timeToRemove, SDL_Texture* texture);
void CorpseDestroy (SCorpse** corpse);
void CorpseClearAll ();

void CorpseGetSdlRect (SCorpse* corpse, SDL_Rect* rect);
SDL_Texture* CorpseGetTexture (SCorpse* corpse);

void CorpsesUpdateAndRender ();
