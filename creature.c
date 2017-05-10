#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "vector2d.h"
#include "texture.h"
#include "sound.h"
#include "creature.h"
#include "corpse.h"
#include "player.h"


SCreature* CreatureCreate (ECreatureType creatureType,
                           short health,
                           float x, float y,
                           byte w, int h,
                           float moveSpeed,
                           SDL_Texture** textures,
                           unsigned short texCount,
                           float animSpeed)
{
    SCreature* creature = (SCreature*) malloc(sizeof(SCreature));

    creature->creatureType = creatureType;
    creature->health = health;

    /* search place for body in physObjects array */
    register unsigned short i = 0;
    while ((physObjects[i] != NULL) && (i < MAX_PHYSOBJECTS_COUNT))
        i++;
    physObjects[i] = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_ALL);
    creature->physBodyIndex = i;

    creature->xDir = 0;

    creature->moveSpeed = moveSpeed;
    creature->accelSpeed = 20.0f;

    creature->textures = textures;
    creature->texCount = texCount;

    creature->animDelay = 0.0f;
    creature->animSpeed = animSpeed;
    creature->startFrame = 0;
    creature->endFrame = texCount - 1;
    creature->curFrame = 0;


    return creature;
}

void CreatureDestroy (SCreature** creature)
{
    if (creature == NULL || *creature == NULL)
        return;

    PhysObjectDestroy (&(physObjects[(*creature)->physBodyIndex]));

    free (*creature);
    *creature = NULL;
}

void CreatureClearAll ()
{
    register unsigned short int i;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        CreatureDestroy (&(creatures[i]));
    }

    creaturesCount = 0;
}

void CreatureGetDamage (SCreature* creature, int damage)
{
    if (creature == NULL)
        return;

    creature->health -= damage;
}

void CreatureUpdateState (SCreature* creature)
{
    if (creature == NULL)
        return;

    /* самовыпилиться, если за границей уровня */
    SPhysObject* physBody = physObjects[creature->physBodyIndex];
    short xPos = (short)((physBody->center.x) / BLOCK_SIZE);
    short yPos = (short)((physBody->center.y) / BLOCK_SIZE);
    if (xPos < 0 || xPos >= LEVEL_WIDTH ||
        yPos < 0 || yPos >= LEVEL_HEIGHT)
    {
        CreatureGetDamage (creature, 100);
        return;
    }

    /* ограничение скорости движения физического тела */
    if (abs(physBody->impulse.x) > creature->moveSpeed)
        physBody->impulse.x = (creature->moveSpeed)*(creature->xDir);
}

void CreatureAddImpulse (SCreature* creature, float x, float y)
{
    if (creature == NULL)
        return;

    PhysObjectAddImpulse (physObjects[creature->physBodyIndex], x, y);
}

bool CreatureContainsPoint (SCreature* creature, float x, float y)
{
    if (creature == NULL)
        return false;

    return PhysObjectContainsPoint(physObjects[creature->physBodyIndex], x, y);
}

bool CreatureIsCollisionCreature (SCreature* c1, SCreature* c2)
{
    if (c1 == NULL || c2 == NULL)
        return false;

    return (PhysObjectIsCollisionPhysObject(physObjects[c1->physBodyIndex], physObjects[c2->physBodyIndex]));
}

bool CreatureIsCollisionLevelObject (SCreature* c1, SLevelObject* l2)
{
    if (c1 == NULL || l2 == NULL)
        return false;

    return (PhysObjectIsCollisionLevelObject(physObjects[c1->physBodyIndex], l2));
}

static bool IsPlaceFreeForCreature (float x, float y,
                                    bool checkAll,
                                    SLevelObject** obstacleLevelObject,
                                    SCreature** obstacleCreature)
{
    /* check elements of level */
    short xPos = (short)x / BLOCK_SIZE;
    short yPos = (short)y / BLOCK_SIZE;
    if (xPos < 0 || xPos >= LEVEL_WIDTH ||
        yPos < 0 || yPos >= LEVEL_HEIGHT)
        return false;

    SLevelObject* levelObject = level[yPos][xPos];
    if ((levelObject != NULL) && (levelObject->isSolid))
    {
        *obstacleLevelObject = levelObject;
        return false;
    }

    /* check creatures... */
    if (checkAll)
    {
        SCreature* creature;
        register unsigned short int i;
        for (i = 0; i < MAX_CREATURES_COUNT; i++)
        {
            creature = creatures[i];
            if (creature != NULL)
            {
                SPhysObject* physBody = physObjects [creature->physBodyIndex];
                if (physBody == NULL || physBody->collisionFlag < PHYSOBJ_COLLISION_WITH_ALL)
                    return true;

                if (CreatureContainsPoint(creature, x, y))
                {
                    *obstacleCreature = creature;
                    return false;
                }
            }
        }
    }

    /* nobody here ;) */
    return true;
}

void CreatureUpdatePhysics (SCreature* creature)
{
    if (creature == NULL)
        return;

    SPhysObject* physBody = physObjects[creature->physBodyIndex];
    SLevelObject* obstacleLevelObject;
    SCreature* obstacleCreature;
    SVector2f testPosition;
    short xPos;
    short yPos;

    /* если преграда перед существом (крайняя точка существа + impulse.x), то туда нельзя идти... */
    //testPosition.x = physBody->center.x + (physBody->halfW)*(creature->xDir) + (physBody->impulse.x * 2.0f);
    //testPosition.x = physBody->center.x + (physBody->w)*(creature->xDir);
    if (physBody->impulse.x < EPSILON)
        testPosition.x = physBody->pos.x - 8.0f;
    else
        testPosition.x = physBody->pos.x + physBody->w + 8.0f;
    testPosition.y = physBody->center.y;
    /* level edges check  */
    xPos = (short)testPosition.x / BLOCK_SIZE;
    yPos = (short)testPosition.y / BLOCK_SIZE;
    if (xPos < 0 || xPos > LEVEL_WIDTH ||
        yPos < 0 || yPos > LEVEL_HEIGHT)
    {
        CreatureGetDamage (creature, 100);
        return;
    }

    obstacleLevelObject = NULL;
    obstacleCreature = NULL;
    if (!IsPlaceFreeForCreature (testPosition.x, testPosition.y, true, &obstacleLevelObject, &obstacleCreature))
    {
        if (creature != obstacleCreature)
        {
            if (creature != player)
            {
                if (obstacleCreature == player)
                    CreatureGetDamage (player, 1);

                physBody->impulse.x = 0.0f;
                creature->xDir *= -1;
            }
            else
            {
                if (obstacleCreature != NULL)
                    CreatureGetDamage (player, 1);
            }
        }
    }
    /* если преграда под или над существом, то движение невозможно */
    testPosition.x = physBody->center.x;
    //testPosition.y = physBody->center.y;
    if (physBody->impulse.y < EPSILON)
        testPosition.y = physBody->pos.y - 8.0f;
    else
        testPosition.y = physBody->pos.y + physBody->h + 8.0f;
    /* level edges check  */
    xPos = (short)testPosition.x / BLOCK_SIZE;
    yPos = (short)testPosition.y / BLOCK_SIZE;
    if (xPos < 0 || xPos >= LEVEL_WIDTH ||
        yPos < 0 || yPos >= LEVEL_HEIGHT)
    {
        CreatureGetDamage (creature, 100);
        return;
    }

    obstacleLevelObject = NULL;
    obstacleCreature = NULL;
    if (!IsPlaceFreeForCreature (testPosition.x, testPosition.y, true, &obstacleLevelObject, &obstacleCreature))
    {
        //physBody->impulse.y = 0.0f;

        /* obstacle is block */
        if (obstacleLevelObject != NULL)
        {
            //creature->impulse.y = 0.0f;
        }

        /* obstacle is creature */
        if (obstacleCreature != NULL)
        {
            if (creature != obstacleCreature)
            {
                /* you are enemy? */
                /*if (creature != player)
                {
                    /* kick player's ass!!
                    if (obstacleCreature == player)
                        CreatureGetDamage (player, 1);
                        //printf ("Kick player's ass!\n");
                }*/

                /* jump, if obstacle is creature under you */
                if (physObjects[obstacleCreature->physBodyIndex]->center.y >= (physBody->pos.y + physBody->h))
                {
                    //creature->pos.y -= 1.0f;
                    physBody->impulse.y = 0.0f;
                    CreatureAddImpulse (creature, 0.0f, -3.5f);

                    /* kick enemie's ass! */
                    CreatureGetDamage (obstacleCreature, 1);

                    Mix_PlayChannel (-1, sndKick, 0);
                }
            }
        }
    }
}

void CreatureUpdateAI (SCreature* creature)
{
    if (creature == NULL)
        return;

    /* random direction */
    if (creature->xDir == 0)
    {
        creature->xDir = rand() % 2;
        if (creature->xDir == 0)
            creature->xDir = -1;
    }

    /* move... */
    if (abs (physObjects[creature->physBodyIndex]->impulse.x) < EPSILON)
        CreatureAddImpulse (creature, creature->xDir, 0.0f);
    CreatureAddImpulse (creature, (creature->accelSpeed)*(creature->xDir)*deltaTime, 0.0f);
}

void CreatureSetFrameRange (SCreature* creature, int startFrame, int endFrame)
{
    if (creature == NULL)
        return;

    creature->startFrame = startFrame;
    creature->endFrame = endFrame;
}

void CreatureUpdateAnimation (SCreature* creature)
{
    if (creature == NULL)
        return;

    /* всяческие проверки диапазона анимации */
    if (creature->startFrame > creature->endFrame)
        Swapi (&(creature->startFrame), &(creature->endFrame));

    if (creature->startFrame < 0)
        creature->startFrame = 0;

    if (creature->endFrame > creature->texCount - 1)
        creature->endFrame = creature->texCount - 1;

    /* кадр один */
    if (creature->startFrame == creature->endFrame)
    {
        if (creature->curFrame != creature->startFrame)
            creature->curFrame = creature->startFrame;
        return;
    }


    creature->animDelay += deltaTime;
    if (creature->animDelay >= creature->animSpeed)
    {
        creature->animDelay -= creature->animSpeed;
        creature->curFrame++;
    }

    if (creature->curFrame > creature->endFrame)
        creature->curFrame = creature->startFrame;
}

void CreatureGetSdlRect (SCreature* creature, SDL_Rect* rect)
{
    if (creature != NULL && rect != NULL)
    {
        rect->x = (int)(physObjects[creature->physBodyIndex]->pos.x);
        rect->y = (int)(physObjects[creature->physBodyIndex]->pos.y);
        rect->w = physObjects[creature->physBodyIndex]->w;
        rect->h = physObjects[creature->physBodyIndex]->h;
    }
}

SDL_Texture* CreatureGetTexture (SCreature* creature, int numFrame)
{
    SDL_Texture** p = (SDL_Texture**) creature->textures;
    p += numFrame;
    return *p;
}

void CreaturesUpdate()
{
    register unsigned short int i;
    SCreature* creature = NULL;
    for (i = 0; i < MAX_CREATURES_COUNT; i++)
    {
        creature = creatures[i];

        if (creature != NULL)
        {
            CreatureUpdateState (creature);

            if (creature->health <= 0)
            {
                SPhysObject* physBody = physObjects[creature->physBodyIndex];
                if (physBody != NULL)
                {
                    SCorpse* corpse = CorpseCreate(physBody->pos.x, physBody->pos.y,
                                                   physBody->w,     physBody->h,
                                                   3.0f,
                                                   NULL);

                    switch (creature->creatureType)
                    {
                        case ctPlayer :
                        {
                            corpse->texture = &(playerTextures[6]);

                            physBody = physObjects[corpse->physBodyIndex];
                            physBody->collisionFlag = PHYSOBJ_NO_COLLISION;
                            physBody->impulse.x = 0.0f;
                            physBody->impulse.y = 0.0f;
                            physBody->pos.y -= 3.0f;
                            PhysObjectAddImpulse(physBody, 0.0f, -5.0f);

                            Mix_PlayChannel (-1, sndMarioDie, 0);

                            break;
                        }

                        case ctGoomba :
                        {
                            corpse->texture = &(goombaTextures[2]);

                            physBody = physObjects[corpse->physBodyIndex];
                            physBody->collisionFlag = PHYSOBJ_COLLISION_WITH_LEVEL;
                            physBody->impulse.x = 0.0f;
                            physBody->impulse.y = 0.0f;

                            break;
                        }

                        default :
                        {
                            break;
                        }
                    }

                    corpses[i] = corpse;
                }

                if (creature == player)
                    player = NULL;

                CreatureDestroy (&(creatures[i]));
                continue;
            }

            CreatureUpdatePhysics (creature);
            CreatureUpdateAnimation (creature);

            if (creature != player)
                CreatureUpdateAI (creature);
        }
    }
}
