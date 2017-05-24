#include <stdlib.h>

#include "defines.h"
#include "texture.h"
#include "sound.h"
#include "level.h"
#include "physObj.h"
#include "corpse.h"
#include "lump.h"
#include "surprise.h"
#include "player.h"

/* time before player can get damage */
float delayDamage;

void PlayerUpdate ()
{
    if (player == NULL || player->physBody == NULL)
        return;

    SPhysObject* physBody = player->physBody;

    if (playerPrevHealth <= 0)
        playerPrevHealth = player->health;

    //if (playerCanDamaged)
    if ((playerPrevHealth > player->health) && (player->health > 0))
    {
        playerCanDamaged = false;
        delayDamage = 0.0f;

        TextureArraySetColor (player->textures, 255, 0, 0, player->texCount);
    }

    /* wait time before player can get damage again */
    if (!playerCanDamaged)
    {
        if (delayDamage < 1.0f)
            delayDamage += deltaTime;
        else
        {
            playerCanDamaged = true;
            delayDamage = 0.0f;

            TextureArraySetColor (player->textures, 255, 255, 255, player->texCount);
        }
    }

    if (player->health == 1)
    {
        physBody->h = BLOCK_SIZE;
        if (playerPrevHealth > 1)
            physBody->pos.y += BLOCK_SIZE;
    }
    if (player->health == 2)
        physBody->h = BLOCK_SIZE << 1;

    SLevelObject* levelObject;
    if (physBody->impulse.y < -EPSILON)
    if (!IsPlaceFree (physBody->center.x, physBody->pos.y - 1.0f, false, &levelObject, NULL))
    {
        if (levelObject != NULL)
        {
            switch (levelObject->levelObjectType)
            {
                /* кирпичик */
                case lotBrick :
                {
                    if (player->health < 2)
                    {
                        levelObject->pos.y = levelObject->startPos.y - (BLOCK_SIZE >> 2);
                        physBody->pos.y = levelObject->startPos.y + BLOCK_SIZE;

                        SoundPlay (sndBump, 0);
                    }
                    else
                    {
                        /* create garbage */
                        LumpCreateSeveral (levelObject->center.x, levelObject->center.y,
                                           BLOCK_SIZE / 2, BLOCK_SIZE / 4,
                                           5.0f,
                                           true,
                                           miscTextures[0],
                                           4);
                        LumpCreateSeveral (levelObject->center.x, levelObject->center.y,
                                           BLOCK_SIZE / 8, BLOCK_SIZE / 8,
                                           5.0f,
                                           true,
                                           miscTextures[2],
                                           40);

                        /* delete block */
                        for (uint16 j = 0; j < LEVEL_HEIGHT; j++)
                            for (uint16 i = 0; i < LEVEL_WIDTH; i++)
                            {
                                if (level[j][i] == levelObject)
                                {
                                    LevelObjectDestroy (&(level[j][i]));
                                    levelObject = NULL;
                                    break;
                                }
                            }

                        SoundPlay (sndBreakBlock, 0);
                    }

                    physBody->impulse.y = 0.0f;

                    break;
                }

                /* сюрпризики */
                case lotCoinBox :
                case lotMushroomBox :
                {
                    if (levelObject->levelObjectType == lotCoinBox)
                    {

                    }
                    if (levelObject->levelObjectType == lotMushroomBox)
                    {
                        SSurprise* surprise = SurpriseCreate (stMushroom,
                                                              levelObject->pos.x, levelObject->pos.y - BLOCK_SIZE,
                                                              BLOCK_SIZE, BLOCK_SIZE,
                                                              1.5f,
                                                              surpriseTextures[0]);
                        surprise->physBody->collisionFlag = PHYSOBJ_COLLISION_WITH_LEVEL;
                        ListAddElement(surprises, surprise);

                    }

                    levelObject->pos.y = levelObject->startPos.y - (BLOCK_SIZE >> 2);
                    levelObject->levelObjectType = lotSurpriseBlockUsed;
                    levelObject->isStatic = true;
                    levelObject->texIndex = 11;

                    physBody->impulse.y = 0.0f;
                    physBody->pos.y = levelObject->startPos.y + BLOCK_SIZE;

                    break;
                }

                default :
                {
                    break;
                }
            }
        }
    }

    playerPrevHealth = player->health;


    /* set position of camera */
    /* X */
    cameraPos.x = physBody->center.x - (WINDOW_WIDTH >> 1);
    if (cameraPos.x < 0.0f)
        cameraPos.x = 0.0f;
    uint16 maxCameraPosX = LEVEL_WIDTH * BLOCK_SIZE - (WINDOW_WIDTH >> 1);
    if (cameraPos.x > maxCameraPosX)
        cameraPos.x = maxCameraPosX;
    /* Y */
    cameraPos.y = physBody->center.y - (WINDOW_HEIGHT >> 1);
    if (cameraPos.y < 0.0f)
        cameraPos.y = 0.0f;
    uint16 maxCameraPosY = LEVEL_HEIGHT * BLOCK_SIZE - WINDOW_HEIGHT;
    if (cameraPos.y > maxCameraPosY)
        cameraPos.y = maxCameraPosY;
}

void PlayerUpdateFrames ()
{
    if (player == NULL || player->physBody == NULL)
        return;

    SPhysObject* physBody = player->physBody;

    /* jump? */
    if (!physBody->isGrounded)
    {
        if (player->health < 2)
            CreatureSetFrameRange (player, 5, 5);
        else
            CreatureSetFrameRange (player, 12, 12);
    }
    else
    {
        /* движется? */
        if (abs (physBody->impulse.x) > EPSILON)
        {
            /* анимация торможения */
            if ((moveL && (physBody->impulse.x > 0.0f)) ||
                (moveR && (physBody->impulse.x < 0.0f)))
            {
                if (player->health < 2)
                    CreatureSetFrameRange (player, 4, 4);
                else
                    CreatureSetFrameRange (player, 11, 11);

            }
            else
            {
                /* walk */
                if (player->health < 2)
                    CreatureSetFrameRange (player, 1, 3);
                else
                    CreatureSetFrameRange (player, 8, 10);

            }
        }
        else
        {
            /* staying */
            if (player->health < 2)
                CreatureSetFrameRange (player, 0, 0);
            else
                CreatureSetFrameRange (player, 7, 7);
        }
    }
}
