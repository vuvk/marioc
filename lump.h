#pragma once

#include "SDL2/SDL.h"

#include "list.h"
#include "physObj.h"


typedef struct
{
    SPhysObject* physBody;

    bool isSpinned;         /* вращается? */
    float angle;

    float timeToRemove;     /* time before destroy */
    float _timeToRemove;    /* delay */

    SDL_Texture* texture;
} SLump;


/* all lumps here */
SList* lumps;

/* constructor|destructor */
SLump* LumpCreate (float x, float y,
                   int w, int h,
                   float timeToRemove,
                   bool isSpinned,
                   SDL_Texture* texture);
void LumpCreateSeveral (float x, float y,
                        int w, int h,
                        float timeToRemove,
                        bool isSpinned,
                        SDL_Texture* texture,
                        const uint32 count);
void LumpDestroy (SLump** lump);
void LumpClearAll ();

void LumpGetSdlRect (SLump* lump, SDL_Rect* rect);
SDL_Texture* LumpGetTexture (SLump* lump);

void LumpsUpdateAndRender ();

