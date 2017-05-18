#pragma once

#include "SDL2/SDL.h"

#include "defines.h"
#include "level.h"
#include "creature.h"


typedef enum
{
    stMushroom,
    stFlower,
    stStar
} ESurpriseType;

typedef struct
{
    ESurpriseType surpriseType;

    unsigned short physBodyIndex;

    char xDir;           /* -1 влево 1 вправо*/
    float moveSpeed;     /* скорость движения максимальная */
    float accelSpeed;    /* ускорение */

    /* текстуры */
    SDL_Texture* texture;
} SSurprise;


/* набор всех сюрпризов/бонусов */
SSurprise* surprises [MAX_SURPRISES_COUNT];

/* имитация конструктора */
SSurprise* SurpriseCreate (ESurpriseType surpriseType,
                           float x, float y,
                           ubyte w, ubyte h,
                           float moveSpeed,
                           SDL_Texture* texture);
/* имитация деструктора */
void SurpriseDestroy (SSurprise** surprise);
void SurpriseClearAll ();

void SurpriseUpdateState (SSurprise* surprise);

/* обработка физики и движения */
void SurpriseAddImpulse (SSurprise* surprise, float x, float y);
//void SurpriseUpdatePhysics (SSurprise* surprise);
//bool SurpriseContainsPoint (SSurprise* surprise, float x, float y);
bool SurpriseIsCollisionPhysObject (SSurprise* s1, SPhysObject* o2);
bool SurpriseIsCollisionCreature (SSurprise* s1, SCreature* c2);
bool SurpriseIsCollisionLevelObject (SSurprise* s1, SLevelObject* l2);
/* геттеры */
void SurpriseGetSdlRect (SSurprise* surprise, SDL_Rect* rect);
SDL_Texture* SurpriseGetTexture (SSurprise* surprise);
/* AI */
void SurpriseUpdateAI (SSurprise* surprise);

void SurprisesUpdateAndRender();
