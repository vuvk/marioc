#pragma once

#include "SDL2/SDL.h"

#include "defines.h"
#include "list.h"
#include "level.h"
#include "physObj.h"
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

    SPhysObject* physBody;

    char xDir;           /* -1 ����� 1 ������*/
    float moveSpeed;     /* �������� �������� ������������ */
    float accelSpeed;    /* ��������� */

    /* �������� */
    SDL_Texture* texture;
} SSurprise;


/* ����� ���� ���������/������� */
SList* surprises;

/* �������� ������������ */
SSurprise* SurpriseCreate (ESurpriseType surpriseType,
                           float x, float y,
                           ubyte w, ubyte h,
                           float moveSpeed,
                           SDL_Texture* texture);
/* �������� ����������� */
void SurpriseDestroy (SSurprise** surprise);
void SurpriseClearAll ();

/* ��������� ������ � �������� */
void SurpriseAddImpulse (SSurprise* surprise, float x, float y);
bool SurpriseIsCollisionPhysObject (SSurprise* s1, SPhysObject* o2);
bool SurpriseIsCollisionCreature (SSurprise* s1, SCreature* c2);
bool SurpriseIsCollisionLevelObject (SSurprise* s1, SLevelObject* l2);
/* ������� */
void SurpriseGetSdlRect (SSurprise* surprise, SDL_Rect* rect);
SDL_Texture* SurpriseGetTexture (SSurprise* surprise);
/* AI */
void SurpriseUpdateAI (SSurprise* surprise);

void SurprisesUpdateAndRender();
