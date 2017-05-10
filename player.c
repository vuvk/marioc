#include "player.h"
#include "physObj.h"


void PlayerUpdate (SCreature* player)
{
    if (player == NULL)
        return;

    SPhysObject* physBody = physObjects [player->physBodyIndex];
    if (physBody == NULL)
        return;

    SLevelObject* levelObject;
    if (physBody->impulse.y < -EPSILON)
    if (!IsPlaceFree (physBody->center.x, physBody->pos.y - 1.0f, false, &levelObject, NULL))
    {
        if (levelObject != NULL)
        {
            switch (levelObject->levelObjectType)
            {
                case lotBrick :
                {
                    levelObject->pos.y = levelObject->startPos.y - (BLOCK_SIZE >> 2);
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
