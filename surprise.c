#include "engine.h"
#include "sound.h"
#include "physObj.h"
#include "surprise.h"
#include "player.h"

SSurprise* SurpriseCreate (ESurpriseType surpriseType,
                           float x, float y,
                           ubyte w, ubyte h,
                           float moveSpeed,
                           SDL_Texture* texture)
{
    SSurprise* surprise = (SSurprise*) malloc (sizeof (SSurprise));

    surprise->surpriseType = surpriseType;
    surprise->moveSpeed = moveSpeed;
    surprise->accelSpeed = 20.0f;
    surprise->texture = texture;
    surprise->xDir = 0;

    /* search place for body in physObjects array */
    uint16 i = 0;
    while ((physObjects[i] != NULL) && (i < MAX_PHYSOBJECTS_COUNT))
        i++;
    physObjects[i] = PhysObjectCreate (x, y, w, h, PHYSOBJ_COLLISION_WITH_ALL);
    surprise->physBodyIndex = i;

    return surprise;
}

void SurpriseDestroy (SSurprise** surprise)
{
    if (surprise == NULL || *surprise == NULL)
        return;

    SPhysObject* physBody = physObjects [(*surprise)->physBodyIndex];
    if (physBody != NULL)
        PhysObjectDestroy(&(physObjects [(*surprise)->physBodyIndex]));

    free (*surprise);
    *surprise = NULL;
}

void SurpriseClearAll ()
{
    for (uint16 i = 0; i < MAX_SURPRISES_COUNT; i++)
    {
        SurpriseDestroy (&(surprises[i]));
    }
}

void SurpriseUpdateState (SSurprise* surprise)
{
    if (surprise == NULL)
        return;

    SPhysObject* physBody = physObjects[surprise->physBodyIndex];
    if (physBody == NULL)
    {
        for (uint16 i = 0; i < MAX_SURPRISES_COUNT; i++)
            if (surprise == surprises[i])
            {
                SurpriseDestroy (&(surprises[i]));
                return;
            }
    }

    /* ограничение скорости движения физического тела */
    if (abs(physBody->impulse.x) > surprise->moveSpeed)
        physBody->impulse.x = (surprise->moveSpeed)*(surprise->xDir);
}

void SurpriseAddImpulse (SSurprise* surprise, float x, float y)
{
    if (surprise == NULL)
        return;

    SPhysObject* physBody = physObjects[surprise->physBodyIndex];
    if (physBody != NULL)
        PhysObjectAddImpulse(physBody, x, y);
}

bool IsPlaceFreeForSurprise (float x, float y,
                             SLevelObject** obstacleLevelObject)
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

    return true;
}

void SurpriseUpdatePhysics (SSurprise* surprise)
{
    if ((surprise == NULL) || (surprise->moveSpeed == 0.0f))
        return;

    SPhysObject* physBody = physObjects[surprise->physBodyIndex];
    SLevelObject* obstacleLevelObject;
    SVector2f testPosition;
    bool isCollided;     /* было ли уже взаимодействие */

    /* если преграда впереди (крайняя точка + impulse.x), то туда нельзя идти... */
    /* проверяем три точки: у головы, у центра, у ног по Y до первого взаимодействия */
    isCollided = false;
    testPosition.x = physBody->center.x;
    testPosition.y = physBody->pos.y + 1.0f;
    if (physBody->impulse.x < -EPSILON)
        testPosition.x = physBody->pos.x - 8.0f;
    if (physBody->impulse.x >  EPSILON)
        testPosition.x = physBody->pos.x + physBody->w + 8.0f;

    for (uint16 i = 0; ((i < 3) && (!isCollided)); i++)
    {
        obstacleLevelObject = NULL;
        if ((!IsPlaceFreeForSurprise (testPosition.x, testPosition.y, &obstacleLevelObject))  &&
            (obstacleLevelObject != NULL))
        {
            //physBody->impulse.x *= -1;
            physBody->impulse.x = 0.0f;
            surprise->xDir *= -1;

            isCollided = true;
        }

        testPosition.y += physBody->halfH - 1;
    }
}

bool SurpriseIsCollisionPhysObject (SSurprise* s1, SPhysObject* o2)
{
    if (s1 == NULL || o2 == NULL)
        return false;

    SPhysObject* physBody = physObjects[s1->physBodyIndex];
    if (physBody != NULL)
        return (PhysObjectIsCollisionPhysObject (physBody, o2));
    else
        return false;
}

bool SurpriseIsCollisionCreature (SSurprise* s1, SCreature* c2)
{
    if (s1 == NULL || c2 == NULL)
        return false;

    SPhysObject* physBody1 = physObjects[s1->physBodyIndex];
    SPhysObject* physBody2 = physObjects[c2->physBodyIndex];
    if (physBody1 != NULL && physBody2 != NULL)
        return (PhysObjectIsCollisionPhysObject (physBody1, physBody2));
    else
        return false;
}

bool SurpriseIsCollisionLevelObject (SSurprise* s1, SLevelObject* l2)
{
    if (s1 == NULL || l2 == NULL)
        return false;

    SPhysObject* physBody = physObjects[s1->physBodyIndex];
    if (physBody != NULL)
        return (PhysObjectIsCollisionLevelObject (physBody, l2));
    else
        return false;
}

void SurpriseGetSdlRect (SSurprise* surprise, SDL_Rect* rect)
{
    if (surprise == NULL || rect == NULL)
        return;

    SPhysObject* physBody = physObjects[surprise->physBodyIndex];
    if (physBody == NULL)
        return;

    rect->x = (int)(physBody->pos.x - cameraPos.x);
    rect->y = (int)(physBody->pos.y - cameraPos.y);
    rect->w = physBody->w;
    rect->h = physBody->h;
}

SDL_Texture* SurpriseGetTexture (SSurprise* surprise)
{
    if (surprise == NULL)
        return NULL;

    return surprise->texture;
}

void SurpriseUpdateAI (SSurprise* surprise)
{
    if (surprise == NULL)
        return;

    if (surprise->moveSpeed != 0.0f)
    {
        /* random direction */
        if (surprise->xDir == 0)
        {
            surprise->xDir = rand() % 2;
            if (surprise->xDir == 0)
                surprise->xDir = -1;
        }

        /* move... */
        if (abs (physObjects[surprise->physBodyIndex]->impulse.x) < EPSILON)
            SurpriseAddImpulse (surprise, surprise->xDir, 0.0f);
        SurpriseAddImpulse (surprise, (surprise->accelSpeed)*(surprise->xDir)*deltaTime, 0.0f);
    }
}

void SurprisesUpdateAndRender ()
{
    SSurprise* surprise = NULL;
    for (uint16 i = 0; i < MAX_SURPRISES_COUNT; i++)
    {
        surprise = surprises[i];

        if (surprise != NULL)
        {
            SurpriseUpdateState (surprise);
            SurpriseUpdatePhysics (surprise);
            SurpriseUpdateAI (surprise);

            /* player eat surprise? */
            if (player != NULL)
            if (SurpriseIsCollisionCreature (surprise, player))
            {
                switch (surprise->surpriseType)
                {
                    case stMushroom :
                    {
                        if (player->health < 2)
                            player->health ++;

                        SoundPlay (sndPowerUp, 0);
                    }

                    default :
                        break;
                }

                SurpriseDestroy(&(surprises[i]));

                continue;
            }


            /* now draw */
            if (surprise->texture != NULL)
            {
                SDL_Rect rect;
                SurpriseGetSdlRect(surprise, &rect);
                /* if rect in screen range */
                if ((rect.x + rect.w) > 0 &&
                    (rect.x <= WINDOW_WIDTH))
                    EngineRenderImage (surprise->texture, &rect, false);
            }
        }
    }
}
