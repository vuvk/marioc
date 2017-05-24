#pragma once

#include "SDL2/SDL.h"

#include "list.h"
#include "level.h"
#include "physObj.h"


typedef enum
{
    ctPlayer,
    ctMushroom,
    ctGoomba,
    ctCoopa
} ECreatureType;


typedef struct
{
    ECreatureType creatureType;
    int16 health;

    SPhysObject* physBody;

    char xDir;           /* -1 влево 1 вправо*/
    float moveSpeed;     /* скорость движения максимальная */
    float accelSpeed;    /* ускорение */

    /* текстуры */
    SDL_Texture** textures;
    uint16 texCount;

    /* анимация */
    short startFrame, endFrame;
    short curFrame;
    float animSpeed;    /* чем меньше, тем быстрее анимация! */
    float animDelay;    /* здесь идет наращение времени до смены кадра */
} SCreature;

/* набор всех созданий */
SList* creatures;

/* имитация конструктора */
SCreature* CreatureCreate (ECreatureType creatureType,
                           short health,
                           float x, float y,
                           ubyte w, ubyte h,
                           float moveSpeed,
                           SDL_Texture** textures,
                           unsigned short texCount,
                           float animSpeed);
/* имитация деструктора */
void CreatureDestroy (SCreature** creature);
void CreatureClearAll ();

void CreatureGetDamage (SCreature* creature, int damage);

void CreatureUpdateState (SCreature* creature);

/* обработка физики и движения */
void CreatureAddImpulse (SCreature* creature, float x, float y);
void CreatureUpdatePhysics (SCreature* creature);
bool CreatureContainsPoint (SCreature* creature, float x, float y);
bool CreatureIsCollisionCreature (SCreature* c1, SCreature* c2);
bool CreatureIsCollisionLevelObject (SCreature* c1, SLevelObject* l2);
/* обработка анимации */
void CreatureSetFrameRange (SCreature* creature, int startFrame, int endFrame);
void CreatureUpdateAnimation (SCreature* creature);
/* геттеры */
void CreatureGetSdlRect (SCreature* creature, SDL_Rect* rect);
SDL_Texture* CreatureGetTexture (SCreature* creature, int numFrame);
/* AI */
void CreatureUpdateAI (SCreature* creature);

void CreaturesUpdateAndRender();
