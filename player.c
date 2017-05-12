#include <stdlib.h>

#include "defines.h"
#include "texture.h"
#include "sound.h"
#include "physObj.h"
#include "corpse.h"
#include "lump.h"
#include "player.h"


void PlayerUpdate (SCreature* player)
{
    if (player == NULL)
        return;

    SPhysObject* physBody = physObjects [player->physBodyIndex];
    if (physBody == NULL)
        return;

    if (player->health == 1)
        physBody->h = BLOCK_SIZE;
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
                    }
                    else
                    {
                        register short int i, j;

                        /* create garbage */
                        LumpCreateSeveral (levelObject->center.x, levelObject->center.y,
                                           16, 8,
                                           5.0f,
                                           true,
                                           &(miscTextures[0]),
                                           4);

                        /* delete block */
                        for (j = 0; j < LEVEL_HEIGHT; j++)
                            for (i = 0; i < LEVEL_WIDTH; i++)
                            {
                                if (level[j][i] == levelObject)
                                {
                                    LevelObjectDestroy (&(level[j][i]));
                                    levelObject = NULL;
                                    break;
                                }
                            }

                        Mix_PlayChannel (-1, sndBreakBlock, false);
                    }

                    physBody->impulse.y = 0.0f;

                    break;
                }

                /* сюрпризики */
                case lotCoinBox :
                case lotMushroomBox :
                {
                    levelObject->pos.y = levelObject->startPos.y - (BLOCK_SIZE >> 2);
                    levelObject->levelObjectType = lotSurpriseBlockUsed;
                    levelObject->texIndex = 4;

                    physBody->impulse.y = 0.0f;

                    break;
                }

                default :
                {
                    break;
                }
            }
        }
    }
}


void PlayerUpdateFrames (SCreature* player)
{
    if (player == NULL)
        return;

    /* прыгает? */
    SPhysObject* physBody = physObjects[player->physBodyIndex];
    if (physBody == NULL)
        return;

    if (!physBody->isGrounded)
    {
        CreatureSetFrameRange (player, 5, 5);
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
                CreatureSetFrameRange (player, 4, 4);
            }
            else
            {
                /* ходьба */
                CreatureSetFrameRange (player, 1, 3);
            }
        }
        else
        {
            CreatureSetFrameRange (player, 0, 0);
        }
    }
}
