#include <stdlib.h>
#include <stdio.h>

#include "defines.h"
#include "additions.h"
#include "vector2d.h"
#include "list.h"
#include "texture.h"
#include "sound.h"
#include "creature.h"
#include "corpse.h"
#include "lump.h"
#include "player.h"

SCreature* CreatureCreate (ECreatureType creatureType,
                           short health,
                           float x, float y,
                           ubyte w, ubyte h,
                           float moveSpeed,
                           SDL_Texture** textures,
                           unsigned short texCount,
                           float animSpeed)
{
    SCreature* creature = (SCreature*) malloc(sizeof(SCreature));

    creature->creatureType = creatureType;
    creature->health = health;

    creature->physBody = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_ALL);
    ListAddElement (physObjects, creature->physBody);

    creature->xDir = 0;
    creature->moveSpeed = moveSpeed;
    creature->accelSpeed = BLOCK_SIZE * 0.65f;

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

    if ((*creature)->physBody != NULL)
        ListDeleteElementByValue (physObjects, (*creature)->physBody);

    free (*creature);
    *creature = NULL;
}

void CreatureClearAll ()
{
    for (SListElement* element = creatures->first; element; element = element->next)
    {
        if (element->value != NULL)
            CreatureDestroy((SCreature**) &element->value);
    }

    ListClear (creatures);
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
    SPhysObject* physBody = creature->physBody;
    if (physBody == NULL)
    {
        CreatureGetDamage (creature, 100);
        return;
    }

    short xPos = (short)((physBody->center.x) / BLOCK_SIZE);
    short yPos = (short)((physBody->center.y) / BLOCK_SIZE);
    if (xPos < 0 || xPos > (LEVEL_WIDTH - 1) ||
        yPos < 0 || yPos > (LEVEL_HEIGHT - 1))
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
    if (creature == NULL || creature->physBody == NULL)
        return;

    PhysObjectAddImpulse (creature->physBody, x, y);
}

bool CreatureContainsPoint (SCreature* creature, float x, float y)
{
    if (creature == NULL || creature->physBody == NULL)
        return false;

    return PhysObjectContainsPoint (creature->physBody, x, y);
}

bool CreatureIsCollisionCreature (SCreature* c1, SCreature* c2)
{
    if (c1 == NULL || c2 == NULL)
        return false;

    SPhysObject* physBody1 = c1->physBody;
    SPhysObject* physBody2 = c2->physBody;

    if (physBody1 != NULL && physBody2 != NULL)
        return (PhysObjectIsCollisionPhysObject(physBody1, physBody2));
    else
        return false;
}

bool CreatureIsCollisionLevelObject (SCreature* c1, SLevelObject* l2)
{
    if (c1 == NULL || c1->physBody == NULL || l2 == NULL)
        return false;

    return (PhysObjectIsCollisionLevelObject (c1->physBody, l2));
}

bool IsPlaceFreeForCreature (float x, float y,
                             bool checkAll,
                             SLevelObject** obstacleLevelObject,
                             SCreature** obstacleCreature)
{
    /* check elements of level */
    short xPos = (short)x / BLOCK_SIZE;
    short yPos = (short)y / BLOCK_SIZE;
    if (xPos < 0 || xPos > (LEVEL_WIDTH - 1) ||
        yPos < 0 || yPos > (LEVEL_HEIGHT - 1))
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
        for (SListElement* element = creatures->first; element; element = element->next)
        {
            creature = (SCreature*) element->value;

            if (creature != NULL)
            {
                if (creature->physBody != NULL && creature->physBody->collisionFlag == PHYSOBJ_COLLISION_WITH_ALL)
                if (CreatureContainsPoint (creature, x, y))
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
    if (creature == NULL || creature->physBody == NULL)
        return;

    SPhysObject* physBody = creature->physBody;

    SLevelObject* obstacleLevelObject;
    SCreature* obstacleCreature;
    SVector2f testPosition;
    short xPos;
    short yPos;
    uint16 i;
    bool isCollided;     /* было ли уже взаимодействие */

    /* если преграда перед существом (крайняя точка существа + impulse.x), то туда нельзя идти... */
    /* проверяем три точки: у головы, у центра, у ног по Y до первого взаимодействия */
    isCollided = false;
    testPosition.x = physBody->center.x;
    testPosition.y = physBody->pos.y + 1.0f;
    if (physBody->impulse.x < -EPSILON)
        testPosition.x = physBody->pos.x - 8.0f;
    if (physBody->impulse.x >  EPSILON)
        testPosition.x = physBody->pos.x + physBody->w + 8.0f;

    for (i = 0; ((i < 3) && (!isCollided)); i++)
    {
        /* level edges check  */
        xPos = (short)testPosition.x / BLOCK_SIZE;
        yPos = (short)testPosition.y / BLOCK_SIZE;
        if (xPos < 0 || xPos > (LEVEL_WIDTH - 1) ||
            yPos < 0 || yPos > (LEVEL_HEIGHT - 1))
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
                    if ((obstacleCreature == player) && (playerCanDamaged))
                    {
                        CreatureGetDamage (player, 1);
                        playerCanDamaged = false;
                    }

                    //physBody->impulse.x *= -1;
                    physBody->impulse.x = 0.0f;
                    creature->xDir *= -1;
                }
                else
                {
                    if ((obstacleCreature != NULL) && (playerCanDamaged))
                    {
                        CreatureGetDamage (player, 1);
                        playerCanDamaged = false;
                    }
                }

                isCollided = true;
            }
        }

        testPosition.y += physBody->halfH - 1;
    }


    /* если преграда под или над существом, то движение невозможно */
    /* проверяем три точки: у крайней левой точки, у центра, у правого края по X до первого взаимодействия */
    isCollided = false;
    testPosition.x = physBody->pos.x + 1.0f;
    testPosition.y = physBody->center.y;
    if (physBody->impulse.y < -EPSILON)
        testPosition.y = physBody->pos.y - 8.0f;
    if (physBody->impulse.y >  EPSILON)
        testPosition.y = physBody->pos.y + physBody->h + 8.0f;

    for (i = 0; ((i < 3) && (!isCollided)); i++)
    {
        /* level edges check  */
        xPos = (short)testPosition.x / BLOCK_SIZE;
        yPos = (short)testPosition.y / BLOCK_SIZE;
        if (xPos < 0 || xPos > (LEVEL_WIDTH - 1) ||
            yPos < 0 || yPos > (LEVEL_HEIGHT - 1))
        {
            CreatureGetDamage (creature, 100);
            return;
        }

        obstacleLevelObject = NULL;
        obstacleCreature = NULL;
        if (!IsPlaceFreeForCreature (testPosition.x, testPosition.y, true, &obstacleLevelObject, &obstacleCreature))
        {
            /* obstacle is creature */
            if (obstacleCreature != NULL)
            {
                if (creature != obstacleCreature)
                {
                    /* jump, if obstacle is creature under you */
                    if (obstacleCreature->physBody->center.y >= (physBody->pos.y + physBody->h))
                    {
                        //creature->pos.y -= 1.0f;
                        physBody->impulse.y = 0.0f;
                        CreatureAddImpulse (creature, 0.0f, -3.5f);

                        /* kick enemie's ass! */
                        CreatureGetDamage (obstacleCreature, 1);

                        SoundPlay (sndKick, 0);
                    }
                }

                isCollided = true;
            }
        }

        testPosition.x += physBody->halfW - 1;
    }
}

void CreatureUpdateAI (SCreature* creature)
{
    if (creature == NULL || creature->physBody == NULL)
        return;

    if (creature->moveSpeed != 0.0f)
    {
        /* random direction */
        if (creature->xDir == 0)
        {
            creature->xDir = rand() % 2;
            if (creature->xDir == 0)
                creature->xDir = -1;
        }

        /* move... */
        if (abs (creature->physBody->impulse.x) < EPSILON)
            CreatureAddImpulse (creature, creature->xDir, 0.0f);
        CreatureAddImpulse (creature, (creature->accelSpeed)*(creature->xDir)*deltaTime, 0.0f);
    }
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
        Swaps (&(creature->startFrame), &(creature->endFrame));

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
    if (creature == NULL || creature->physBody == NULL || rect == NULL)
        return;

    SPhysObject* physBody = creature->physBody;

    rect->x = (int)(physBody->pos.x - cameraPos.x);
    rect->y = (int)(physBody->pos.y - cameraPos.y);
    rect->w = physBody->w;
    rect->h = physBody->h;
}

SDL_Texture* CreatureGetTexture (SCreature* creature, int numFrame)
{
    SDL_Texture** ptr = creature->textures;
    ptr += numFrame;
    return *ptr;
}

void CreaturesUpdateAndRender ()
{
    if (creatures->first == NULL)
        return;

    SCreature* creature;
    SListElement* element = creatures->first;
    while (element != NULL)
    {
        creature = NULL;
        if (element)
            creature = (SCreature*) element->value;

        if (creature != NULL)
        {
            CreatureUpdateState (creature);
            CreatureUpdatePhysics (creature);
            CreatureUpdateAnimation (creature);

            if (creature != player)
                CreatureUpdateAI (creature);

            // render!
            SDL_Rect rect;
            CreatureGetSdlRect (creature, &rect);
            // if rect in screen range
            if ((rect.x + rect.w) > 0 &&
                (rect.x <= WINDOW_WIDTH))
                EngineRenderImage (CreatureGetTexture (creature, creature->curFrame), &rect, (creature->xDir < 0));

            if (creature->health <= 0)
            {
                SPhysObject* physBody = creature->physBody;
                if (physBody != NULL)
                {
                    // create blood!
                    LumpCreateSeveral (physBody->center.x, physBody->center.y,
                                       BLOCK_SIZE / 4.0f, BLOCK_SIZE / 8.0f,
                                       5.0f,
                                       true,
                                       miscTextures[1],
                                       50);

                    // now create corpse
                    SCorpse* corpse = CorpseCreate(physBody->pos.x, physBody->pos.y,
                                                   physBody->w,     physBody->h,
                                                   15.0f,
                                                   NULL);

                    switch (creature->creatureType)
                    {
                        case ctPlayer :
                        {
                            corpse->texture = playerTextures[6];

                            physBody = corpse->physBody;
                            physBody->w = BLOCK_SIZE;
                            physBody->h = BLOCK_SIZE;
                            physBody->collisionFlag = PHYSOBJ_NO_COLLISION;
                            physBody->impulse.x = 0.0f;
                            physBody->impulse.y = 0.0f;
                            physBody->pos.y -= 3.0f;
                            PhysObjectAddImpulse(physBody, 0.0f, -5.0f);

                            SoundPlay (sndMarioDie, 0);

                            break;
                        }

                        case ctGoomba :
                        {
                            corpse->texture = goombaTextures[2];

                            physBody = corpse->physBody;
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

                    ListAddElement (corpses, corpse);
                }

                if (creature == player)
                    player = NULL;


                SListElement* nextElement = element->next;
                ListDeleteElementByValue (physObjects, creature->physBody);
                ListDeleteElementByValue (creatures, creature);
                element = nextElement;

                continue;
            }
        }

        element = element->next;
    }
}
