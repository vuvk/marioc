#pragma once

#include "SDL2/SDL.h"

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
    short health;

    unsigned short physBodyIndex;
    //SPhysObject* physBody;
    /*SVector2f pos;
    SVector2f center;
    int w, h;
    int halfW, halfH;
    SVector2f impulse;*/

    char xDir;           /* -1 ����� 1 ������*/
    float moveSpeed;     /* �������� �������� ������������ */
    float accelSpeed;    /* ��������� */

    /*float friction;
    bool isGrounded;*/

    /* �������� */
    SDL_Texture** textures;
    unsigned short texCount;

    /* �������� */
    short startFrame, endFrame;
    short curFrame;
    float animSpeed;    /* ��� ������, ��� ������� ��������! */
    float animDelay;    /* ����� ���� ��������� ������� �� ����� ����� */
} SCreature;

/* ����� ���� �������� */
/* �������� �� ������! */
SCreature* creatures [MAX_CREATURES_COUNT];
unsigned short creaturesCount;

/* �������� ������������ */
SCreature* CreatureCreate (ECreatureType creatureType,
                           short health,
                           float x, float y,
                           byte w, int h,
                           float moveSpeed,
                           SDL_Texture** textures,
                           unsigned short texCount,
                           float animSpeed);
/* �������� ����������� */
void CreatureDestroy (SCreature** creature);
void CreatureClearAll ();

void CreatureGetDamage (SCreature* creature, int damage);

void CreatureUpdateState (SCreature* creature);

/* ��������� ������ � �������� */
void CreatureAddImpulse (SCreature* creature, float x, float y);
void CreatureUpdatePhysics (SCreature* creature);
bool CreatureContainsPoint (SCreature* creature, float x, float y);
bool CreatureIsCollisionCreature (SCreature* c1, SCreature* c2);
bool CreatureIsCollisionLevelObject (SCreature* c1, SLevelObject* l2);
/* ��������� �������� */
void CreatureSetFrameRange (SCreature* creature, int startFrame, int endFrame);
void CreatureUpdateAnimation (SCreature* creature);
/* ������� */
void CreatureGetSdlRect (SCreature* creature, SDL_Rect* rect);
SDL_Texture* CreatureGetTexture (SCreature* creature, int numFrame);
/* AI */
void CreatureUpdateAI (SCreature* creature);

/*static bool IsPlaceFree (float x, float y,
                         bool checkAll,
                         SLevelObject** obstacleLevelObject,
                         SCreature** obstacleCreature);*/

void CreaturesUpdate();
